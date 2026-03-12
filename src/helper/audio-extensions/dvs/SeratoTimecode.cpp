#include "SeratoTimecode.h"

SeratoTimecode::SeratoTimecode(AudioResources& audio) 
    : _audio(audio) {
}

void SeratoTimecode::begin() {    
    // Enable the record queues
    _audio.queueL.begin();
    _audio.queueR.begin();
    
    // Initialize timing
    _lastValidCrossingTime = millis();
}

void SeratoTimecode::update() {
    // Skip if disabled - queues are already stopped
    if (!_enabled) {
        return;
    }

    // Process all available audio buffers
    // Each buffer is 128 samples (AUDIO_BLOCK_SAMPLES)
    
    while (_audio.queueL.available() > 0 && _audio.queueR.available() > 0) {
        // Get pointers to the sample buffers
        int16_t* bufferL = _audio.queueL.readBuffer();
        int16_t* bufferR = _audio.queueR.readBuffer();
        
        if (bufferL && bufferR) {
            processBuffer(bufferL, bufferR);
        }
        
        // Release the buffers back to the audio library
        _audio.queueL.freeBuffer();
        _audio.queueR.freeBuffer();
    }
    
    // Check for timeout (no valid signal)
    if (millis() - _lastValidCrossingTime > TIMEOUT_MS) {
        // No valid zero crossings detected recently - vinyl is stopped
        _smoothedSpeed = _smoothedSpeed * 0.8f;  // Faster decay towards zero
        if (fabsf(_smoothedSpeed) < MIN_SPEED_THRESHOLD) {
            _smoothedSpeed = 0.0f;
        }
        // Reset validation state
        _validCrossingStreak = 0;
        _crossingCount = 0;
        _samplesPerCrossingAccum = 0;
        _lastCrossingInterval = 0;
    }
}

void SeratoTimecode::processBuffer(int16_t* bufferL, int16_t* bufferR) {
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        processSample(bufferL[i], bufferR[i]);
        _totalSamplesProcessed++;
    }
}

void SeratoTimecode::processSample(int16_t sampleL, int16_t sampleR) {
    // Track for debug
    _lastSampleL = sampleL;
    _lastSampleR = sampleR;
    
    // Update DC offset tracking (exponential moving average)
    // This adapts to any DC bias in the signal
    _left.dcOffset += DC_FILTER_COEFF * (sampleL - _left.dcOffset);
    _right.dcOffset += DC_FILTER_COEFF * (sampleR - _right.dcOffset);
    
    // Calculate sample relative to DC offset
    float relativeL = sampleL - _left.dcOffset;
    float relativeR = sampleR - _right.dcOffset;
    
    // Track peak amplitude (for signal strength validation)
    float absL = fabsf(relativeL);
    float absR = fabsf(relativeR);
    _peakAmplitudeL = _peakAmplitudeL * 0.999f + absL * 0.001f;  // Slow tracking
    if (absL > _peakAmplitudeL) _peakAmplitudeL = _peakAmplitudeL * 0.9f + absL * 0.1f;  // Fast attack
    _peakAmplitudeR = _peakAmplitudeR * 0.999f + absR * 0.001f;
    if (absR > _peakAmplitudeR) _peakAmplitudeR = _peakAmplitudeR * 0.9f + absR * 0.1f;
    
    bool leftCrossed = false;
    bool rightCrossed = false;
    
    // Left channel zero crossing detection with hysteresis
    // Compare against the tracked center point, not absolute zero
    if (!_left.positive && relativeL > ZERO_CROSS_THRESHOLD) {
        // Crossed from negative to positive
        _left.positive = true;
        leftCrossed = true;
    } else if (_left.positive && relativeL < -ZERO_CROSS_THRESHOLD) {
        // Crossed from positive to negative
        _left.positive = false;
        leftCrossed = true;
    }
    
    // Right channel zero crossing detection with hysteresis
    if (!_right.positive && relativeR > ZERO_CROSS_THRESHOLD) {
        _right.positive = true;
        rightCrossed = true;
    } else if (_right.positive && relativeR < -ZERO_CROSS_THRESHOLD) {
        _right.positive = false;
        rightCrossed = true;
    }
    
    // Process crossings
    if (leftCrossed || rightCrossed) {
        onZeroCrossing(leftCrossed, rightCrossed, _totalSamplesProcessed);
    }
}

void SeratoTimecode::onZeroCrossing(bool leftCrossed, bool rightCrossed, uint32_t sampleIndex) {
    _totalCrossings++;  // Debug counter only
    
    // Direction detection only when we have a valid signal (enough valid crossings)
    // This prevents noise from changing direction when stopped
    if (_validCrossingStreak >= MIN_VALID_CROSSINGS) {
        // Direction detection using quadrature phase relationship
        if (leftCrossed && !rightCrossed) {
            bool forward = (_left.positive != _right.positive);
            updateDirection(forward);
        } else if (rightCrossed && !leftCrossed) {
            bool forward = (_left.positive == _right.positive);
            updateDirection(forward);
        }
    }
    
    // Speed calculation based on left channel crossings (primary)
    if (leftCrossed) {
        if (_left.lastCrossingSample > 0) {
            uint32_t samplesSinceLast = sampleIndex - _left.lastCrossingSample;
            float interval = (float)samplesSinceLast;
            
            // Validate crossing timing - reject if outside expected range for timecode
            bool timingValid = (interval >= MIN_SAMPLES_PER_CROSSING && 
                                interval <= MAX_SAMPLES_PER_CROSSING);
            
            // Validate amplitude - reject weak signals (motor noise is usually weak)
            bool amplitudeValid = (_peakAmplitudeL >= MIN_AMPLITUDE && 
                                   _peakAmplitudeR >= MIN_AMPLITUDE);
            
            // Validate timing consistency - intervals should be similar
            bool consistencyValid = true;
            if (_lastCrossingInterval > 0) {
                float ratio = interval / _lastCrossingInterval;
                if (ratio < (1.0f - TIMING_VARIANCE_MAX) || ratio > (1.0f + TIMING_VARIANCE_MAX)) {
                    consistencyValid = false;  // Timing too erratic
                }
            }
            
            if (timingValid && amplitudeValid && consistencyValid) {
                // Valid crossing - accumulate for averaging
                _samplesPerCrossingAccum += interval;
                _crossingCount++;
                _validCrossingStreak++;
                _lastCrossingInterval = interval;
                _lastValidCrossingTime = millis();  // Update timeout timer
                
                // Update speed after enough valid crossings
                if (_crossingCount >= 4 && _validCrossingStreak >= MIN_VALID_CROSSINGS) {
                    float avgSamplesPerCrossing = _samplesPerCrossingAccum / _crossingCount;
                    updateSpeed(avgSamplesPerCrossing);
                    _samplesPerCrossingAccum = 0;
                    _crossingCount = 0;
                }
            } else {
                // Invalid - likely noise, reset streak
                _validCrossingStreak = 0;
                _samplesPerCrossingAccum = 0;
                _crossingCount = 0;
                _lastCrossingInterval = 0;
            }
        }
        _left.lastCrossingSample = sampleIndex;
    }
}

void SeratoTimecode::updateSpeed(float samplesPerCrossing) {
    // At 1x speed: 1000 Hz signal at 44100 Hz sample rate
    // = 44.1 samples per cycle = 22.05 samples per zero crossing (2 crossings per cycle)
    constexpr float SAMPLES_PER_CROSSING_1X = SAMPLES_PER_CYCLE / 2.0f;
    
    // Speed is inverse of samples per crossing
    // More samples between crossings = slower speed
    if (samplesPerCrossing > 0) {
        _instantaneousSpeed = SAMPLES_PER_CROSSING_1X / samplesPerCrossing;
        
        // Clamp to reasonable range
        if (_instantaneousSpeed > 4.0f) _instantaneousSpeed = 4.0f;
        
        // Apply smoothing (exponential moving average)
        // On first reading, initialize directly instead of smoothing from zero
        if (_smoothedSpeed < MIN_SPEED_THRESHOLD) {
            _smoothedSpeed = _instantaneousSpeed;  // Initialize directly
        } else {
            _smoothedSpeed = (_smoothingFactor * _smoothedSpeed) + 
                             ((1.0f - _smoothingFactor) * _instantaneousSpeed);
        }
    }
}

void SeratoTimecode::updateDirection(bool forward) {
    _instantaneousForward = forward;
    
    // Hysteresis: only change reported direction after consistent readings
    if (forward == _reportedForward) {
        _directionConsistentCount = 0;
    } else {
        _directionConsistentCount++;
        if (_directionConsistentCount >= _directionHysteresis) {
            _reportedForward = forward;
            _directionConsistentCount = 0;
        }
    }
}

float SeratoTimecode::getSpeed() {
    if (!isPlaying()) {
        return 0.0f;
    }
    return _reportedForward ? _smoothedSpeed : -_smoothedSpeed;
}

float SeratoTimecode::getSpeedPercent() {
    return getSpeed() * 100.0f;
}

float SeratoTimecode::getSpeedScaled() {
    if (!isPlaying()) {
        return 0.0f;
    }
    // Return absolute speed (0-5 scale, where 1 = normal speed)
    // Clamped to max 5.0
    float absSpeed = fabsf(_smoothedSpeed);
    return (absSpeed > 5.0f) ? 5.0f : absSpeed;
}

bool SeratoTimecode::isForward() {
    return _reportedForward;
}

bool SeratoTimecode::isReverse() {
    return !_reportedForward;
}

bool SeratoTimecode::isPlaying() {
    return fabsf(_smoothedSpeed) >= MIN_SPEED_THRESHOLD;
}

void SeratoTimecode::setSmoothingFactor(float factor) {
    _smoothingFactor = constrain(factor, 0.0f, 0.99f);
}

void SeratoTimecode::setDirectionHysteresis(int count) {
    _directionHysteresis = max(1, count);
}

void SeratoTimecode::setInputGain(int gain) {
//    _audio.sgtl5000.lineInLevel(constrain(gain, 0, 15));
}

int SeratoTimecode::getBuffersAvailable() {
    return _audio.queueL.available();
}

int SeratoTimecode::getCrossingCount() {
    return _totalCrossings;
}

int16_t SeratoTimecode::getLastSampleL() {
    return _lastSampleL;
}

int16_t SeratoTimecode::getLastSampleR() {
    return _lastSampleR;
}

float SeratoTimecode::getInstantSpeed() {
    return _instantaneousSpeed;
}

void SeratoTimecode::setEnabled(bool enabled) {
    if (enabled && !_enabled) {
        resume();
    } else if (!enabled && _enabled) {
        pause();
    }
}

bool SeratoTimecode::isEnabled() {
    return _enabled;
}

void SeratoTimecode::pause() {
    if (_enabled) {
        _enabled = false;
        // Stop the queues - this prevents buffer overflow
        _audio.queueL.end();
        _audio.queueR.end();
        // Clear any remaining buffers
        _audio.queueL.clear();
        _audio.queueR.clear();
        // Reset speed to stopped
        _smoothedSpeed = 0.0f;
        _instantaneousSpeed = 0.0f;
        _validCrossingStreak = 0;
    }
}

void SeratoTimecode::resume() {
    if (!_enabled) {
        _enabled = true;
        // Restart the queues
        _audio.queueL.begin();
        _audio.queueR.begin();
        // Reset timing
        _lastValidCrossingTime = millis();
    }
}