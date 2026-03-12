#ifndef SERATO_TIMECODE_H
#define SERATO_TIMECODE_H

#include <Arduino.h>
#include "../../AudioResources.h"

/**
 * SeratoTimecode - Decodes Serato timecode vinyl signal
 * 
 * Analyzes the 1kHz quadrature signal from Serato control vinyl
 * to determine playback speed and direction.
 * 
 * Usage:
 *   AudioResources audio;
 *   SeratoTimecode timecode(audio);
 *   
 *   void setup() {
 *     timecode.begin();
 *   }
 *   
 *   void loop() {
 *     timecode.update();  // Call regularly to process buffered audio
 *     float speed = timecode.getSpeed();
 *     bool forward = timecode.isForward();
 *   }
 */
class SeratoTimecode {
public:
    /**
     * Constructor
     * @param audio Reference to AudioResources containing queues and codec
     */
    SeratoTimecode(AudioResources& audio);
    
    /**
     * Initialize the timecode decoder
     * Configures SGTL5000 for line-in and enables record queues
     * Call this in setup() after AudioMemory() is allocated
     */
    void begin();
    
    /**
     * Process any buffered audio samples
     * Call this regularly from loop() - handles variable timing
     * Processes all accumulated samples since last call
     */
    void update();
    
    /**
     * Get current playback speed
     * @return Speed multiplier (1.0 = normal speed, 0.5 = half speed, etc.)
     *         Negative values indicate reverse playback
     */
    float getSpeed();
    
    /**
     * Get current playback speed as percentage
     * @return Speed percentage (100.0 = normal speed)
     *         Negative values indicate reverse playback
     */
    float getSpeedPercent();
    
    /**
     * Get current playback speed scaled 0-5
     * @return 0 when stopped, 1 at normal speed, up to 5 at 500% speed
     *         Always positive (use isForward/isReverse for direction)
     */
    float getSpeedScaled();
    
    /**
     * Check if playing forward
     * @return true if direction is forward (or stopped)
     */
    bool isForward();
    
    /**
     * Check if playing reverse
     * @return true if direction is reverse
     */
    bool isReverse();
    
    /**
     * Check if vinyl is moving
     * @return true if speed is above minimum threshold
     */
    bool isPlaying();
    
    /**
     * Set speed smoothing factor
     * Higher values = smoother but slower response
     * @param factor Smoothing factor (0.0 to 0.99, default 0.85)
     */
    void setSmoothingFactor(float factor);
    
    /**
     * Set direction change hysteresis
     * Number of consistent direction readings before changing reported direction
     * @param count Hysteresis count (default 3)
     */
    void setDirectionHysteresis(int count);
    
    /**
     * Set line input gain
     * @param gain Gain in dB (0 to 15, default 5)
     */
    void setInputGain(int gain);

    
    /**
     * Enable or disable timecode processing
     * When disabled, queues are stopped to prevent buffer overflow
     * @param enabled true to enable, false to disable
     */
    void setEnabled(bool enabled);
    
    /**
     * Check if timecode processing is enabled
     * @return true if enabled
     */
    bool isEnabled();
    
    /**
     * Pause timecode processing (stops queues, preserves state)
     */
    void pause();
    
    /**
     * Resume timecode processing (restarts queues)
     */
    void resume();

    // Debug methods
    int getBuffersAvailable();      // How many audio buffers are queued
    int getCrossingCount();         // Total zero crossings detected
    int16_t getLastSampleL();       // Last sample value (left)
    int16_t getLastSampleR();       // Last sample value (right)
    float getInstantSpeed();        // Unsmoothed speed

private:
    AudioResources& _audio;
    
    // Configuration
    static constexpr float TIMECODE_FREQUENCY = 1000.0f;  // Serato uses 1kHz
    static constexpr float SAMPLE_RATE = 44100.0f;
    static constexpr float SAMPLES_PER_CYCLE = SAMPLE_RATE / TIMECODE_FREQUENCY;  // ~44.1
    static constexpr int16_t ZERO_CROSS_THRESHOLD = 50;   // Hysteresis for noise immunity
    static constexpr float MIN_SPEED_THRESHOLD = 0.08f;   // Below this = stopped (raised more)
    static constexpr uint32_t TIMEOUT_MS = 80;            // No signal timeout (faster decay)
    
    // Signal validation - reject noise that doesn't look like 1kHz tone
    // At 1x speed: ~22 samples between crossings
    // At 4x speed: ~5.5 samples between crossings  
    // At 0.1x speed: ~220 samples between crossings
    static constexpr float MIN_SAMPLES_PER_CROSSING = 4.0f;    // Max ~5x speed
    static constexpr float MAX_SAMPLES_PER_CROSSING = 300.0f;  // Min ~0.07x speed (tightened)
    static constexpr int MIN_VALID_CROSSINGS = 16;             // Need 16 valid crossings (raised)
    
    // Amplitude validation - reject weak signals (noise)
    static constexpr float MIN_AMPLITUDE = 150.0f;             // Minimum peak amplitude to accept
    static constexpr float TIMING_VARIANCE_MAX = 0.4f;         // Max allowed variance in crossing timing (40%)
    
    // Smoothing parameters
    float _smoothingFactor = 0.85f;
    int _directionHysteresis = 3;
    
    // Zero crossing state for each channel
    struct ChannelState {
        bool positive = false;        // Current polarity (above/below center)
        int32_t lastCrossingSample = 0;  // Sample count at last crossing
        float dcOffset = 0.0f;        // Tracked DC offset (center point)
    };
    
    ChannelState _left;
    ChannelState _right;
    
    // DC offset tracking filter coefficient (smaller = slower adaptation)
    static constexpr float DC_FILTER_COEFF = 0.001f;
    
    // Timing and measurement
    uint32_t _totalSamplesProcessed = 0;
    uint32_t _lastValidCrossingTime = 0;   // millis() of last VALID crossing (for timeout)
    
    // Speed calculation
    float _instantaneousSpeed = 0.0f;
    float _smoothedSpeed = 0.0f;
    float _samplesPerCrossingAccum = 0.0f;
    int _crossingCount = 0;
    int _validCrossingStreak = 0;  // Consecutive valid crossings (for noise rejection)
    float _lastCrossingInterval = 0.0f;  // For timing consistency check
    
    // Amplitude tracking
    float _peakAmplitudeL = 0.0f;
    float _peakAmplitudeR = 0.0f;
    
    // Direction calculation
    bool _instantaneousForward = true;
    bool _reportedForward = true;
    int _directionConsistentCount = 0;
    
    // Debug tracking
    int _totalCrossings = 0;
    int16_t _lastSampleL = 0;
    int16_t _lastSampleR = 0;
    
    // Enabled state
    bool _enabled = true;

    // Internal methods
    void processBuffer(int16_t* bufferL, int16_t* bufferR);
    void processSample(int16_t sampleL, int16_t sampleR);
    void onZeroCrossing(bool leftCrossed, bool rightCrossed, uint32_t sampleIndex);
    void updateSpeed(float samplesPerCrossing);
    void updateDirection(bool forward);
};

#endif // SERATO_TIMECODE_H
