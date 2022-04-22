#include "SongStructure.h"

SongStructure::SongStructure() { };

// ToDo:  - save to sd card OK
//        - load from sd card OK
//        - snippets
//        - implement shift


// returns the closest pointer to the given position. it always returns the predecessor, even if a successor is closer, as change the pointed list at the predecessor.
SongStructure::closestPointer SongStructure::_getClosestPointerIndex(uint8_t channel, uint16_t position) {
    SongStructure::closestPointer closePointer;

    uint16_t blockIndex = _getBlockIndex(position);
    uint8_t blockOffset = _getBlockOffset(position);

    uint16_t startPointerIndex = _blocks[blockIndex].startPointer[channel];

    if (startPointerIndex == 0) {
        // if the start pointer is 0, there is nothing on this channel in the whole block
        closePointer.samplePointerIndex = 0;
        closePointer.type = NONE;

        return closePointer;
    }

    uint16_t lastPointer = 0;
    uint16_t nextPointer = startPointerIndex;

    while (true) {
        if (_samplePointers[nextPointer].columnOffset == blockOffset) {
            // there is already a pointer pointing at this position
            closePointer.samplePointerIndex = 0;
            closePointer.type = TAKEN;
            return closePointer;
        }
        
        // the pointer is pointing ahead of the position, ..
        if (_samplePointers[nextPointer].columnOffset > blockOffset) { 
            if (lastPointer == 0) {
                // .. but it might be the first in the block, so it is a successor of the position
                closePointer.type = SUCCESSOR;
                closePointer.samplePointerIndex = nextPointer;
                return closePointer;
            } else {
                // nextPosition will be ahead, so we return the last pointer, as a predecessor has a higher priority than a successor
                closePointer.type = PREDECESSOR;
                closePointer.samplePointerIndex = lastPointer;
                return closePointer;
            }
        }

        if (_samplePointers[nextPointer].columnOffset < blockOffset) {
            lastPointer = nextPointer;

            if (_samplePointers[nextPointer].successor == 0) {
                // end of pointerstructure reached and closest pointer is the match
                closePointer.type = PREDECESSOR;
                closePointer.samplePointerIndex = nextPointer;                
                return closePointer;
            } else {
                // take another round in traversing the list - we have not reached the position
                nextPointer = _samplePointers[nextPointer].successor;
            }
        }
    }

    return closePointer;
};


// tests, if a position is empty
boolean SongStructure::isSomethingAt(uint8_t channel, uint16_t position) {
    uint16_t blockIndex = _getBlockIndex(position);
    uint8_t blockOffset = _getBlockOffset(position);

    uint16_t startPointerIndex = _blocks[blockIndex].startPointer[channel];

    if (startPointerIndex == 0) return false; // if the start pointer is 0, there is nothing on this channel in the whole block

    uint16_t nextPointer = startPointerIndex;

    while (true) {
        if (_samplePointers[nextPointer].columnOffset == blockOffset) return true; // there is a pointer pointing at this position
        if (_samplePointers[nextPointer].columnOffset > blockOffset) return false; // the pointer is pointing ahead of the position

        if (_samplePointers[nextPointer].columnOffset < blockOffset) {
            if (_samplePointers[nextPointer].successor == 0) {
                // end of pointerstructure reached
                return false;
            } else {
                // test successor in next iteration
                nextPointer = _samplePointers[nextPointer].successor;
            }
        }
    }
    return false;
};

uint16_t SongStructure::_getBlockIndex(uint16_t position) {
    return position / BLOCKSIZE;
};

// returns offset from the block start where the position is located in
uint16_t SongStructure::_getBlockOffset(uint16_t position) {
    uint16_t b = _getBlockIndex(position);
    return (b > 0) ? position-(b*BLOCKSIZE) : position;
};


// retrieves the samplePointer to the given position
uint16_t SongStructure::getPositionPointerIndex(uint8_t channel, uint16_t position) {
    uint16_t blockIndex = _getBlockIndex(position);
    uint8_t blockOffset = _getBlockOffset(position);

    uint16_t startPointerIndex = _blocks[blockIndex].startPointer[channel];

    if (startPointerIndex == 0) return 0; // if the start pointer is 0, there is nothing on this channel in the whole block

    uint16_t nextPointer = startPointerIndex;
    boolean cont = true;

    while (cont) {
        if (_samplePointers[nextPointer].columnOffset == blockOffset) return nextPointer; // got the position!
        if (_samplePointers[nextPointer].columnOffset > blockOffset) return 0; // the pointer is pointing ahead of the position and the position was skipped

        if (_samplePointers[nextPointer].columnOffset < blockOffset) {
            if (_samplePointers[nextPointer].successor == 0) {
                // end of pointerstructure reached
                return 0;
            } else {
                // test successor in next iteration
                nextPointer = _samplePointers[nextPointer].successor;
            }
        }
    }

    return 0;
};


// returns samplePointer of the given position
SongStructure::samplePointerStruct SongStructure::getPosition(uint8_t channel, uint16_t position) {
    return _samplePointers[getPositionPointerIndex(channel, position)];
};


SongStructure::sampleStruct SongStructure::getSampleFromBucketId(uint16_t sampleBucketIndex) {
    return _sampleBucket[sampleBucketIndex];
};


SongStructure::parameterChangeSampleStruct SongStructure::getParameterChangeFromBucketId(uint16_t parameterChangeBucketIndex) {
    return _parameterChangeSampleBucket[parameterChangeBucketIndex];
};

boolean SongStructure::setSample(uint8_t channel, uint16_t position, sampleStruct sample) {

    uint16_t blockIndex = _getBlockIndex(position);
    uint8_t blockOffset = _getBlockOffset(position);

    uint16_t sbi = _getNextSampleBucketIndex();
    _sampleBucket[sbi].baseMidiNote = sample.baseMidiNote;
    _sampleBucket[sbi].pitchedNote = sample.pitchedNote;
    _sampleBucket[sbi].probability = sample.probability;
    _sampleBucket[sbi].sampleNumber = sample.sampleNumber;
    _sampleBucket[sbi].stereoPosition = sample.stereoPosition;
    _sampleBucket[sbi].velocity = sample.velocity;

    uint16_t startPointerIndex = _blocks[blockIndex].startPointer[channel];

    // bucket has no content in this channel yet.. create first pointer
    if (startPointerIndex == 0) {
        uint16_t spi = _getNextSamplePointerIndex();
        _samplePointers[spi].columnOffset = blockOffset;
        _samplePointers[spi].successor = 0;
        _samplePointers[spi].type = SAMPLE;
        _samplePointers[spi].typeIndex = sbi;

        _blocks[blockIndex].startPointer[channel] = spi;
        setCurrentPosition(channel, position, true);
        return true;
    }

    uint16_t lastPointer = 0;
    uint16_t nextPointer = startPointerIndex;
    boolean cont = true;

    while (cont) {
        // position is already taken, overwrite with new sample
        if (_samplePointers[nextPointer].columnOffset == blockOffset) {
            _clearPositionBucket(nextPointer);
        
            _samplePointers[nextPointer].type = SAMPLE;
            _samplePointers[nextPointer].typeIndex = sbi;

            setCurrentPosition(channel, position, true);            
            return true;
        }

        if (_samplePointers[nextPointer].columnOffset > blockOffset) {
             // the pointer is pointing ahead of the position and the position is free. insert sample in between

            uint16_t spi = _getNextSamplePointerIndex();
            _samplePointers[spi].columnOffset = blockOffset;
            _samplePointers[spi].successor = nextPointer;
            _samplePointers[spi].type = SAMPLE;
            _samplePointers[spi].typeIndex = sbi;

            if (lastPointer == 0) {
                // the new pointer is the first in the block
                 _blocks[blockIndex].startPointer[channel] = spi;
            } else {
                // the new pointer has a predecessor
                // change successor to the new sample
                _samplePointers[lastPointer].successor = spi;
            }
            
            setCurrentPosition(channel, position, true);
            return true;
        }

        if (_samplePointers[nextPointer].columnOffset < blockOffset) {
            if (_samplePointers[nextPointer].successor == 0) {
                // end of pointerstructure reached
                // append new samplePointer

                uint16_t spi = _getNextSamplePointerIndex();
                _samplePointers[spi].columnOffset = blockOffset;
                _samplePointers[spi].successor = 0;
                _samplePointers[spi].type = SAMPLE;
                _samplePointers[spi].typeIndex = sbi;

                _samplePointers[nextPointer].successor = spi;

                setCurrentPosition(channel, position, true);
                return true;               
            } else {
                // test successor in next iteration
                lastPointer = nextPointer;
                nextPointer = _samplePointers[nextPointer].successor;
            }
        }
    }
    
    return false; // should never be reached!
};


boolean SongStructure::setParameterChange(uint8_t channel, uint16_t position, parameterChangeSampleStruct parameterChange) {

    uint16_t blockIndex = _getBlockIndex(position);
    uint8_t blockOffset = _getBlockOffset(position);

    // create a new parameter change entry in the bucket
    uint16_t pcbi = _getNextParameterChangeBucketIndex();

    _parameterChangeSampleBucket[pcbi].stereoPosition = parameterChange.stereoPosition;
    _parameterChangeSampleBucket[pcbi].velocity = parameterChange.velocity;

    uint16_t startPointerIndex = _blocks[blockIndex].startPointer[channel];

    // bucket has no content in this channel yet.. create first pointer
    if (startPointerIndex == 0) {
        uint16_t spi = _getNextSamplePointerIndex();
        _samplePointers[spi].columnOffset = blockOffset;
        _samplePointers[spi].successor = 0;
        _samplePointers[spi].type = PARAMETER_CHANGE_SAMPLE;
        _samplePointers[spi].typeIndex = pcbi;

        _blocks[blockIndex].startPointer[channel] = spi;
        setCurrentPosition(channel, position, true);
        return true;
    }

    uint16_t lastPointer = 0;
    uint16_t nextPointer = startPointerIndex;
    boolean cont = true;

    while (cont) {
        // position is already taken, overwrite with new parameter change
        if (_samplePointers[nextPointer].columnOffset == blockOffset) {
            _clearPositionBucket(nextPointer);
            
            _samplePointers[nextPointer].type = PARAMETER_CHANGE_SAMPLE;
            _samplePointers[nextPointer].typeIndex = pcbi;

            setCurrentPosition(channel, position, true);
            return true;
        }

        if (_samplePointers[nextPointer].columnOffset > blockOffset) {
             // the pointer is pointing ahead of the position and the position is free. insert sample in between

            uint16_t spi = _getNextSamplePointerIndex();
            _samplePointers[spi].columnOffset = blockOffset;
            _samplePointers[spi].successor = nextPointer;
            _samplePointers[spi].type = PARAMETER_CHANGE_SAMPLE;
            _samplePointers[spi].typeIndex = pcbi;

            if (lastPointer == 0) {
                // the new pointer is the first in the block
                 _blocks[blockIndex].startPointer[channel] = spi;
            } else {
                // the new pointer has a predecessor
                // change successor to the new sample
                _samplePointers[lastPointer].successor = spi;
            }
            
            setCurrentPosition(channel, position, true);
            return true;
        }

        if (_samplePointers[nextPointer].columnOffset < blockOffset) {
            if (_samplePointers[nextPointer].successor == 0) {
                // end of pointerstructure reached
                // append new samplePointer

                uint16_t spi = _getNextSamplePointerIndex();
                _samplePointers[spi].columnOffset = blockOffset;
                _samplePointers[spi].successor = 0;
                _samplePointers[spi].type = PARAMETER_CHANGE_SAMPLE;
                _samplePointers[spi].typeIndex = pcbi;

                _samplePointers[nextPointer].successor = spi;

                setCurrentPosition(channel, position, true);
                return true;               
            } else {
                // test successor in next iteration
                lastPointer = nextPointer;
                nextPointer = _samplePointers[nextPointer].successor;
            }
        }
    }
    
    return false; // should never be reached!
};




boolean SongStructure::setNoteOff(uint8_t channel, uint16_t position) {
    
    uint16_t blockIndex = _getBlockIndex(position);
    uint8_t blockOffset = _getBlockOffset(position);

    uint16_t startPointerIndex = _blocks[blockIndex].startPointer[channel];

    // bucket has no content in this channel yet.. create first pointer
    if (startPointerIndex == 0) {
        uint16_t spi = _getNextSamplePointerIndex();
        _samplePointers[spi].columnOffset = blockOffset;
        _samplePointers[spi].successor = 0;
        _samplePointers[spi].type = NOTE_OFF;
        _samplePointers[spi].typeIndex = 0;

        _blocks[blockIndex].startPointer[channel] = spi;
        return true;
    }

    uint16_t lastPointer = 0;
    uint16_t nextPointer = startPointerIndex;
    boolean cont = true;

    while (cont) {
        // position is already taken, overwrite with new parameter change
        if (_samplePointers[nextPointer].columnOffset == blockOffset) {
            _clearPositionBucket(nextPointer);
            
            _samplePointers[nextPointer].type = NOTE_OFF;
            _samplePointers[nextPointer].typeIndex = 0;

            return true;
        }

        if (_samplePointers[nextPointer].columnOffset > blockOffset) {
             // the pointer is pointing ahead of the position and the position is free. insert sample in between

            uint16_t spi = _getNextSamplePointerIndex();
            _samplePointers[spi].columnOffset = blockOffset;
            _samplePointers[spi].successor = nextPointer;
            _samplePointers[spi].type = NOTE_OFF;
            _samplePointers[spi].typeIndex = 0;

            if (lastPointer == 0) {
                // the new pointer is the first in the block
                 _blocks[blockIndex].startPointer[channel] = spi;
            } else {
                // the new pointer has a predecessor
                // change successor to the new sample
                _samplePointers[lastPointer].successor = spi;
            }
             return true;
        }

        if (_samplePointers[nextPointer].columnOffset < blockOffset) {
            if (_samplePointers[nextPointer].successor == 0) {
                // end of pointerstructure reached
                // append new samplePointer

                uint16_t spi = _getNextSamplePointerIndex();
                _samplePointers[spi].columnOffset = blockOffset;
                _samplePointers[spi].successor = 0;
                _samplePointers[spi].type = NOTE_OFF;
                _samplePointers[spi].typeIndex = 0;

                _samplePointers[nextPointer].successor = spi;

                return true;               
            } else {
                // test successor in next iteration
                lastPointer = nextPointer;
                nextPointer = _samplePointers[nextPointer].successor;
            }
        }
    }
    
    return false; // should never be reached!    
};



void SongStructure::_clearPositionBucket(uint16_t samplePointerIndex) {
    switch(_samplePointers[samplePointerIndex].type) {
        case SAMPLE:
            _meta.reuseableSampleBucketIndex = _samplePointers[samplePointerIndex].typeIndex;
            break;

        case PARAMETER_CHANGE_SAMPLE:
            _meta.reuseableParameterChangeBucketIndex = _samplePointers[samplePointerIndex].typeIndex;
            break;

        case MIDINOTE:
            break;
        
        default:
            break;
    }

    _samplePointers[samplePointerIndex].type = UNDEFINED;
    _samplePointers[samplePointerIndex].typeIndex = 0;
};


void SongStructure::removePosition(uint8_t channel, uint16_t position) {
    // find position
    uint16_t blockIndex = _getBlockIndex(position);
    uint8_t blockOffset = _getBlockOffset(position);

    uint16_t startPointerIndex = _blocks[blockIndex].startPointer[channel];

    if (startPointerIndex == 0) return; // if the start pointer is 0, there is nothing on this channel in the whole block; should not happen!

    uint16_t lastPointer = 0;
    uint16_t nextPointer = startPointerIndex;
    boolean cont = true;

    while (cont) {
        if (_samplePointers[nextPointer].columnOffset == blockOffset) {
            // got the position!
            
            // save the successor, might be 0 if there is none
            uint16_t s = _samplePointers[nextPointer].successor;

            if (lastPointer == 0) {
                // no predecessor
                _blocks[blockIndex].startPointer[channel] = s;
            } else {
                _samplePointers[lastPointer].successor = s;
            }
            
            _clearPositionBucket(nextPointer); // clean up
            // .. and set Pointer free
            _meta.reusableSamplePointerIndex = nextPointer;

            return;
        }

        if (_samplePointers[nextPointer].columnOffset > blockOffset) {
            return; // the pointer is pointing ahead of the position and the position was skipped; should not happen!
        }

        if (_samplePointers[nextPointer].columnOffset < blockOffset) {
            if (_samplePointers[nextPointer].successor == 0) {
                // end of pointerstructure reached
                return; // should never happen!
            } else {
                // continue checking the pointer list
                lastPointer = nextPointer;
                nextPointer = _samplePointers[nextPointer].successor;
            }
        }
    }
};


void SongStructure::shiftPosition(uint8_t channel, uint16_t position, shiftAction action) {

Serial.print("SHIFT position::");
Serial.println(action);

// ToDo: implement

// Attention! check zoomLevel!!!

// shift up possible? 
//  - check if field is within matrix
//  - check if field is empty
//  - save information
//  - remove current field
//  - insert new field with saved data

// shift down possible?
//  - check if field is within matrix
//  - check if field is empty
//  - save information
//  - remove current field
//  - insert new field with saved data


// shift left possible?
//  - check if field is within matrix
//  - check if field is empty
//  - check if in the same bucket
//    - yes: change pointer list
//    - no: save field, delete field, insert new field

// shift right
//  - check if field is empty
//  - check if in the same bucket
//    - yes: change pointer list
//    - no: check if maximum buckets is reached -> nothing OR save field, delete field, insert new field

};


boolean SongStructure::copyPosition(uint8_t fromChannel, uint16_t fromPosition, uint8_t toChannel, uint16_t toPosition, boolean deleteAfterOperation) {
    // copy to an already taken position is -by design- not possible -> delete the to position first!
    if (isSomethingAt(toChannel, toPosition)) return false;

    // it is not possible to copy an empty cell -> use the delete function ;)
    if (!isSomethingAt(fromChannel, fromPosition)) return false;

    samplePointerStruct from = getPosition(fromChannel, fromPosition);

    if (from.type == SAMPLE) {
        sampleStruct ss = getSampleFromBucketId(from.typeIndex);
        
        sampleStruct nss;
        nss.baseMidiNote = ss.baseMidiNote;
        nss.pitchedNote = ss.pitchedNote;
        nss.probability = ss.probability;
        nss.sampleNumber = ss.sampleNumber;
        nss.stereoPosition = ss.stereoPosition;
        nss.velocity = ss.velocity;
        
        if (setSample(toChannel, toPosition, nss) == false) {
            return false;
        }
    }
        
    if (from.type == PARAMETER_CHANGE_SAMPLE) {
        parameterChangeSampleStruct ps = getParameterChangeFromBucketId(from.typeIndex);

        parameterChangeSampleStruct nps;
        nps.stereoPosition = ps.stereoPosition;
        nps.velocity = ps.velocity;

        if (setParameterChange(toChannel, toPosition, nps) == false) {
            return false;
        }            
    }

    if (from.type == NOTE_OFF) {
        if (setNoteOff(toChannel, toPosition) == false) {
            return false;
        };
    }

    if (deleteAfterOperation) {
        removePosition(fromChannel, fromPosition);
    }

    return true;
};

boolean SongStructure::movePosition(uint8_t fromChannel, uint16_t fromPosition, uint8_t toChannel, uint16_t toPosition) {
    // a move is a copy with deleting the original afterwards ;)
    return copyPosition(fromChannel, fromPosition, toChannel, toPosition, true);
};


void SongStructure::setCurrentPosition(uint8_t channel, uint16_t position, boolean initial) {
    // only retrieve new position data (which means traversing the lists), if the position changed
    // this will come in handy, when changing sample parameters -> some kind of caching ;)
    if (initial || channel != _currentPosition.channel || position != _currentPosition.position) {
        _currentPosition.channel = channel;
        _currentPosition.position = position;
        _currentPosition.samplePointerIndex = getPositionPointerIndex(channel, position);
    }
}



void SongStructure::increaseVelocity(uint8_t channel, uint16_t position) {
    if (_currentPosition.channel == channel && _currentPosition.position == position) {
        switch(_samplePointers[_currentPosition.samplePointerIndex].type) {
            case SAMPLE:
                if (_sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].velocity < 126) {
                    _sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].velocity += 2;
                }
                break;
            case PARAMETER_CHANGE_SAMPLE:
                if (_parameterChangeSampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].velocity < 126) {
                    _parameterChangeSampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].velocity += 2;
                }
                break;
            default:
                break;
        }
    } else {
        // ToDo: is this possible? need to check..
        Serial.println("ELSE!");
    }
};

void SongStructure::decreaseVelocity(uint8_t channel, uint16_t position) {
    if (_currentPosition.channel == channel && _currentPosition.position == position) {
        switch(_samplePointers[_currentPosition.samplePointerIndex].type) {
            case SAMPLE:
                if (_sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].velocity > 1) {
                    _sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].velocity -= 2;
                }
                break;
            case PARAMETER_CHANGE_SAMPLE:
                if (_parameterChangeSampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].velocity > 1) {
                    _parameterChangeSampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].velocity -= 2;
                }
                break;
            default:
                break;
        }
    } else {
        // ToDo: is this possible? need to check..
    }
};

void SongStructure::setVelocity(uint8_t channel, uint16_t position, byte velocity) {
    if (_currentPosition.channel == channel && _currentPosition.position == position) {
        switch(_samplePointers[_currentPosition.samplePointerIndex].type) {
            case SAMPLE:
                _sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].velocity = velocity;
                break;
            case PARAMETER_CHANGE_SAMPLE:
                _parameterChangeSampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].velocity = velocity;
                break;
            default:
                break;
        }
    } else {
        // ToDo: select position and change velocity -> needed when loading a songstructure?
    }
};

void SongStructure::stereoPositionTickLeft(uint8_t channel, uint16_t position) {
    if (_currentPosition.channel == channel && _currentPosition.position == position) {
        switch(_samplePointers[_currentPosition.samplePointerIndex].type) {
            case SAMPLE:
                if (_sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].stereoPosition > 1) {
                    _sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].stereoPosition -= 2;
                }
                break;
            case PARAMETER_CHANGE_SAMPLE:
                if (_parameterChangeSampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].stereoPosition > 1) {
                    _parameterChangeSampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].stereoPosition -= 2;
                }
                break;
            default:
                break;
        }
    } else {
        // ToDo: is this possible? need to check..
    }
};

void SongStructure::stereoPositionTickRight(uint8_t channel, uint16_t position) {
    if (_currentPosition.channel == channel && _currentPosition.position == position) {
        switch(_samplePointers[_currentPosition.samplePointerIndex].type) {
            case SAMPLE:
                if (_sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].stereoPosition < 126) {
                    _sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].stereoPosition += 2;
                }
                break;
            case PARAMETER_CHANGE_SAMPLE:
                if (_parameterChangeSampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].stereoPosition < 126) {
                    _parameterChangeSampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].stereoPosition += 2;
                }
                break;
            default:
                break;
        }
    } else {
        // ToDo: is this possible? need to check..
    }
};

void SongStructure::setStereoPosition(uint8_t channel, uint16_t position, byte stereoPosition) {
    if (_currentPosition.channel == channel && _currentPosition.position == position) {
        switch(_samplePointers[_currentPosition.samplePointerIndex].type) {
            case SAMPLE:
                _sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].stereoPosition = stereoPosition;
                break;
            case PARAMETER_CHANGE_SAMPLE:
                _parameterChangeSampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].stereoPosition = stereoPosition;
                break;
            default:
                break;
        }
    } else {
        // ToDo: select position and change panning -> needed when loading a songstructure?
    }
};

void SongStructure::increasePitchByOne(uint8_t channel, uint16_t position) {
    if (_currentPosition.channel == channel && _currentPosition.position == position) {
        switch(_samplePointers[_currentPosition.samplePointerIndex].type) {
            case SAMPLE:
                if (_sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].pitchedNote < 126) {
                    _sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].pitchedNote += 1;
                }
                break;
            default:
                break;
        }
    } else {
        // ToDo: is this possible? need to check..
    }
};

void SongStructure::decreasePitchByOne(uint8_t channel, uint16_t position) {
    if (_currentPosition.channel == channel && _currentPosition.position == position) {
        switch(_samplePointers[_currentPosition.samplePointerIndex].type) {
            case SAMPLE:
                if (_sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].pitchedNote > 1) {
                    _sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].pitchedNote -= 1;
                }
                break;
            default:
                break;
        }
    } else {
        // ToDo: is this possible? need to check..
    }
};

void SongStructure::setPitchByMidiNote(uint8_t channel, uint16_t position, byte note) {
    if (_currentPosition.channel == channel && _currentPosition.position == position) {
        switch(_samplePointers[_currentPosition.samplePointerIndex].type) {
            case SAMPLE:
                _sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].pitchedNote = note;
                break;
            default:
                break;
        }
    } else {
        // ToDo: select position and change note -> needed when loading a songstructure?
    }
};

void SongStructure::setBaseMidiNote(uint8_t channel, uint16_t position, byte note) {
    if (_currentPosition.channel == channel && _currentPosition.position == position) {
        switch(_samplePointers[_currentPosition.samplePointerIndex].type) {
            case SAMPLE:
                _sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].baseMidiNote = note;
                break;
            default:
                break;
        }
    } else {
        // ToDo: select position and change note -> needed when loading a songstructure?
    }
};

void SongStructure::increaseProbability(uint8_t channel, uint16_t position) {
    if (_currentPosition.channel == channel && _currentPosition.position == position) {
        switch(_samplePointers[_currentPosition.samplePointerIndex].type) {
            case SAMPLE:
                if (_sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].probability < 100) {
                    _sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].probability += 1;
                }
                break;
            default:
                break;
        }
    } else {
        // ToDo: is this possible? need to check..
    }
};

void SongStructure::decreaseProbability(uint8_t channel, uint16_t position) {
    if (_currentPosition.channel == channel && _currentPosition.position == position) {
        switch(_samplePointers[_currentPosition.samplePointerIndex].type) {
            case SAMPLE:
                if (_sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].probability > 1) {
                    _sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].probability -= 1;
                }
                break;
            default:
                break;
        }
    } else {
        // ToDo: is this possible? need to check..
    }
};



void SongStructure::setSongLength(uint16_t songLength) {
    _meta.songLength = songLength;
};

uint16_t SongStructure::getSongLength() {
    return _meta.songLength;
};

void SongStructure::changeSongLengthByTick(boolean increase, byte tickAmount) {
    if (increase && _meta.songLength+tickAmount < _maxSongLength) {
        _meta.songLength = _meta.songLength + tickAmount;
    } 

    // song needs at least to be 2 "ticks" long - no idea why, but how would a 1 Tick song look like? Sell it on ebay for a million?
    if (!increase && _meta.songLength > 2*tickAmount) {
        _meta.songLength = _meta.songLength - tickAmount;
    }         
};

uint16_t SongStructure::getMaxSongLength() {
    return _maxSongLength;
};

void SongStructure::setSongResolution(byte res) {
    _meta.songResolution = res;
};

byte SongStructure::getSongResolution() {
    return _meta.songResolution;
};

void SongStructure::changeSongResolutionByTick(boolean increase) {
    if (increase && _meta.songResolution < 16) {
        _meta.songResolution++;
    } 

    if (!increase && _meta.songResolution > 2) {
        _meta.songResolution--;
    }     
};



// recalculates, which samples need to be in EXTMEM and returns the array
boolean * SongStructure::getSamplesUsed()
{
    // clear the array
    for (byte i = 0; i < 72; i++) {
        samplesUsed[i] = false;
    }

    // traverse structure

    uint16_t blockCount = sizeof(_blocks)/sizeof(blockStruct);
    uint16_t nextPointer = 0;

    for (uint8_t c = 0; c < 8; c++) {        
        for (uint16_t block = 0; block < blockCount; block++) {            

            if (_blocks[block].startPointer[c] != 0) {
                nextPointer = _blocks[block].startPointer[c];

                while (nextPointer != 0) {
                    if (_samplePointers[nextPointer].type == SAMPLE) {
                        samplesUsed[_sampleBucket[_samplePointers[_blocks[block].startPointer[c]].typeIndex].sampleNumber-1] = true;
                    }
                    nextPointer = _samplePointers[nextPointer].successor;
                }
            }
        }
    }

    return samplesUsed;
};


void SongStructure::setPlayBackSpeed(float bpm) {
    if (bpm < 30.0) {
        _meta.playbackSpeed = 30.0;
        return;
    }

    if (bpm > 300.0) { 
        _meta.playbackSpeed = 300.0;
        return;
    }

    _meta.playbackSpeed = bpm;
};


float SongStructure::getPlayBackSpeed() {
    return _meta.playbackSpeed;
};



boolean SongStructure::loadFromSD(char *songPath) {
    char buff[40];
    File readFile;
    byte *bufferBlocks;

    Serial.println(songPath);

    // read _meta
    strcpy(buff, songPath);
    strcat(buff, "/PATTERN/META.DAT");

    Serial.println(buff); 

    readFile = SD.open(buff, FILE_READ);

    bufferBlocks = (byte *) &_meta;

    for (uint16_t j=0; j<sizeof(MetaInfos); j++) {
        if (readFile.available()) {
            *(bufferBlocks + j) = readFile.read();
        }
    }            

    readFile.close();



    // read _blocks
    strcpy(buff, songPath);
    strcat(buff, "/PATTERN/BLOCKS.DAT");

    Serial.println(buff); 

    readFile = SD.open(buff, FILE_READ);

    for (uint16_t i=0; i<sizeof(_blocks)/sizeof(blockStruct); i++) {
        bufferBlocks = (byte *) &_blocks[i];

        for (uint16_t j=0; j<sizeof(blockStruct); j++) {
            if (readFile.available()) {
                *(bufferBlocks + j) = readFile.read();
            }
        }            
    }

    readFile.close();
    



    // read _samplePointers
    strcpy(buff, songPath);
    strcat(buff, "/PATTERN/SMPLPTR.DAT");

    Serial.println(buff); 

    readFile = SD.open(buff, FILE_READ);

    for (uint16_t i=0; i<sizeof(_samplePointers)/sizeof(samplePointerStruct); i++) {
        bufferBlocks = (byte *) &_samplePointers[i];

        for (uint16_t j=0; j<sizeof(samplePointerStruct); j++) {
            if (readFile.available()) {
                *(bufferBlocks + j) = readFile.read();
            }
        }            
    }

    readFile.close();



    // read _sampleBucket
    strcpy(buff, songPath);
    strcat(buff, "/PATTERN/SMPLBKT.DAT");

    Serial.println(buff); 

    readFile = SD.open(buff, FILE_READ);

    for (uint16_t i=0; i<sizeof(_sampleBucket)/sizeof(sampleStruct); i++) {
        bufferBlocks = (byte *) &_sampleBucket[i];

        for (uint16_t j=0; j<sizeof(sampleStruct); j++) {
            if (readFile.available()) {
                *(bufferBlocks + j) = readFile.read();
            }
        }            
    }

    readFile.close();



    // read _parameterChangeSampleBucket
    strcpy(buff, songPath);
    strcat(buff, "/PATTERN/SPSBKT.DAT");

    Serial.println(buff); 

    readFile = SD.open(buff, FILE_READ);

    for (uint16_t i=0; i<sizeof(_parameterChangeSampleBucket)/sizeof(parameterChangeSampleStruct); i++) {
        bufferBlocks = (byte *) &_parameterChangeSampleBucket[i];

        for (uint16_t j=0; j<sizeof(parameterChangeSampleStruct); j++) {
            if (readFile.available()) {
                *(bufferBlocks + j) = readFile.read();
            }
        }            
    }

    readFile.close();


    return true;
};

boolean SongStructure::saveToSD(char *songPath) {
    
    // ToDo: SD card error handling and removing the delays, which are just here for the case that the sd card hangs for a few ms

    char buff[40];
    File writeFile;
    byte *bufferBlocks;


    // write _meta -> all meta infos for this song
    strcpy(buff, songPath);
    strcat(buff, "/PATTERN/META.DAT");

    if (SD.exists(buff)) { SD.remove(buff); }
   
    Serial.println(buff);

    writeFile = SD.open(buff, FILE_WRITE);

    bufferBlocks = (byte *) &_meta;
    writeFile.write(bufferBlocks, sizeof(MetaInfos));
 
    writeFile.close();


    // write _blocks
    strcpy(buff, songPath);
    strcat(buff, "/PATTERN/BLOCKS.DAT");

    if (SD.exists(buff)) { SD.remove(buff); }
   
    Serial.println(buff); 

    writeFile = SD.open(buff, FILE_WRITE);

    for (uint16_t i=0; i<sizeof(_blocks)/sizeof(blockStruct); i++) {
        bufferBlocks = (byte *) &_blocks[i];
        writeFile.write(bufferBlocks, sizeof(_blocks[i]));
    }
 
    writeFile.close();

    delay(100); // still needed?


    // write _samplePointers
    strcpy(buff, songPath);
    strcat(buff, "/PATTERN/SMPLPTR.DAT");

    if (SD.exists(buff)) { SD.remove(buff); }

    Serial.println(buff); 

    writeFile = SD.open(buff, FILE_WRITE);

    for (uint16_t i=0; i<sizeof(_samplePointers)/sizeof(samplePointerStruct); i++) {
        bufferBlocks = (byte *) &_samplePointers[i];
        writeFile.write(bufferBlocks, sizeof(_samplePointers[i]));
    }
 
    writeFile.close();



    delay(100);

    // write _sampleBucket
    strcpy(buff, songPath);
    strcat(buff, "/PATTERN/SMPLBKT.DAT");

    if (SD.exists(buff)) { SD.remove(buff); }

    Serial.println(buff); 

    writeFile = SD.open(buff, FILE_WRITE);

    for (uint16_t i=0; i<sizeof(_sampleBucket)/sizeof(sampleStruct); i++) {
        bufferBlocks = (byte *) &_sampleBucket[i];
        writeFile.write(bufferBlocks, sizeof(_sampleBucket[i]));
    }
 
    writeFile.close();


    delay(100);    

    // write _parameterChangeSampleBucket
    strcpy(buff, songPath);
    strcat(buff, "/PATTERN/SPSBKT.DAT");

    if (SD.exists(buff)) { SD.remove(buff); }

    Serial.println(buff); 

    writeFile = SD.open(buff, FILE_WRITE);

    for (uint16_t i=0; i<sizeof(_parameterChangeSampleBucket)/sizeof(parameterChangeSampleStruct); i++) {
        bufferBlocks = (byte *) &_parameterChangeSampleBucket[i];
        writeFile.write(bufferBlocks, sizeof(_parameterChangeSampleBucket[i]));
    }
 
    writeFile.close();


    return true;
};



uint16_t SongStructure::_getNextSampleBucketIndex() {
    if (_meta.reuseableSampleBucketIndex != 0) {
        // hey, there is a free index somewhere.. it was deleted and can now be reused to have no fragmentations
        uint16_t retval = _meta.reuseableSampleBucketIndex;
        _meta.reuseableSampleBucketIndex = 0;
        return retval;
    } else {
        // ToDo: check, if array is full
        _meta.nextSampleBucketIndex++;
        return _meta.nextSampleBucketIndex;
    }
}

uint16_t SongStructure::_getNextParameterChangeBucketIndex() {
    if (_meta.reuseableParameterChangeBucketIndex != 0) {
        // hey, there is a free index somewhere.. it was deleted and can now be reused to have no fragmentations
        uint16_t retval = _meta.reuseableParameterChangeBucketIndex;
        _meta.reuseableParameterChangeBucketIndex = 0;
        return retval;
    } else {
        // ToDo: check, if array is full
        _meta.nextParameterChangeBucketIndex++;
        return _meta.nextParameterChangeBucketIndex;
    }
}

uint16_t SongStructure::_getNextSamplePointerIndex() {
    if (_meta.reusableSamplePointerIndex != 0) {
        // hey, there is a free index somewhere.. it was deleted and can now be reused to have no fragmentations
        uint16_t retval = _meta.reusableSamplePointerIndex;
        _meta.reusableSamplePointerIndex = 0;
        return retval;
    } else {
        // ToDo: check, if array is full
        _meta.nextSamplePointerIndex++;
        return _meta.nextSamplePointerIndex;
    }    
};






// -------------------------------------------------------------------------------------------------------------------
//  TESTS && DEBUGGING
// -------------------------------------------------------------------------------------------------------------------

void SongStructure::debugInfos() {

    // DEBUG Information
    Serial.println("Blocks");
    for (int i = 0; i<5; i++) {
        for (int j = 0; j<8; j++) {
            Serial.print(_blocks[i].startPointer[j]);
            Serial.print(" ");
        }
        Serial.println();
    }

    // DEBUG Information
    Serial.println("SamplePointer");
    for (int i = 0; i<20; i++) {
        Serial.print(i);
        Serial.print(" ");
        Serial.print("Offset::");
        Serial.print(_samplePointers[i].columnOffset);        
        Serial.print("  Successor::");
        Serial.print(_samplePointers[i].successor);        
        Serial.print(" Type::");
        Serial.print(_samplePointers[i].type);        
        Serial.print(" TypeIndex:: ");
        Serial.print(_samplePointers[i].typeIndex);
        Serial.println();
    }

        // DEBUG Information
    Serial.println("SampleBucket");
    for (int i = 0; i<20; i++) {
        Serial.print(i);
        Serial.print(" ");
        Serial.print("SampleNumber::");
        Serial.print(_sampleBucket[i].sampleNumber);        
        Serial.println();
    }

}


void SongStructure::test() {
    sampleStruct s;
    s.baseMidiNote = 60;
    s.pitchedNote = 64;
    s.probability = 100;
    s.sampleNumber = 1;
    s.stereoPosition = 64;
    s.velocity = 120;

    Serial.println(setSample(0, 5, s));
    Serial.println(setSample(1, 1, s));
    s.sampleNumber = 5;
    s.velocity = 25;
    Serial.println(setSample(2, 15, s));
    testLog();

    Serial.println(setSample(0, 3, s));
    testLog();

    Serial.println(setSample(0, 5, s));
    testLog();

    Serial.println(setSample(0, 0, s));
    testLog();

    s.sampleNumber = 10;
    Serial.println(setSample(5, 2222, s));
    testLog();

    testLogSample(0, 6);

    s.sampleNumber = 70;
    s.velocity = 99;
    Serial.println(setSample(0, 6, s));
    testLog();

    testLogSample(0, 6);

    Serial.println(">>> removing 0/0");

    removePosition(0, 0);
    testLogSample(0, 0);

    testLog();

    Serial.println(">>> adding new 0/0 with sample 77 and velocity 76");

    s.sampleNumber = 72;
    s.velocity = 76;
    Serial.println(setSample(0, 0, s));
    testLogSample(0, 0);

    testLog();

    testLogSample(2, 15);
    testLogSample(1, 5);
    testLogSample(1, 1);

    // -----------------------------------------------

    unsigned long startTime;
    unsigned long endTime;

    startTime = micros();
    getSamplesUsed();
    endTime = micros();

    Serial.print("Microseconds used for retrieving all Samples: ");
    Serial.println(endTime-startTime);


    Serial.println("\nSamples used:");
    for (int i=0; i<72; i++) {
        if (samplesUsed[i]) {
            Serial.print(i+1);
            Serial.print(" ");
        }
    }
    Serial.println("");

    // -----------------------------------------------
}


void SongStructure::testLog() {
    //Serial.println("Matrix content");
    for (int j=0; j<8; j++) {
        Serial.print(j);
        Serial.print(":");
        for (int i = 0; i<20; i++) {
            Serial.print((isSomethingAt(j, i) ? "x" : "-"));
        }        
        Serial.println("");
    } 

    Serial.println("");

    //Serial.println("closest Pointer");
    for (int i = 0; i<20; i++) {
        switch(_getClosestPointerIndex(0, i).type) {
            case NONE:
                Serial.print("-");   
                break;
            case TAKEN:
                Serial.print("x");   
                break;
            case PREDECESSOR:
                Serial.print("<");   
                break;
            case SUCCESSOR:
                Serial.print(">");   
                break;
        }
    }

    Serial.println("");
    //Serial.println("Pointer Types");

    for (int i = 0; i<20; i++) {
        switch(getPosition(0, i).type) {
            case UNDEFINED:
                Serial.print("u");   
                break;
            case SAMPLE:
                Serial.print("S");   
                break;
            case PARAMETER_CHANGE_SAMPLE:
                Serial.print("P");   
                break;
            case NOTE_OFF:
                Serial.print("-");   
                break;
            default:
                break;
        }
    }

    Serial.println("\n----------");
}


void SongStructure::testLogSample(uint16_t channel, uint16_t position) {
        uint16_t ti;
        Serial.print("Channel::");
        Serial.println(channel);
        Serial.print("Position::");
        Serial.println(position);

        switch(getPosition(channel, position).type) {
            case UNDEFINED:
                Serial.println("UNDEFINED");
                break;
            case SAMPLE:
                Serial.println("SAMPLE");   
                ti = getPosition(channel, position).typeIndex;
                Serial.print("SampleNumber::");
                Serial.println(_sampleBucket[ti].sampleNumber);
                Serial.print("Velocity::");
                Serial.println(_sampleBucket[ti].velocity);
                break;
            case PARAMETER_CHANGE_SAMPLE:
                Serial.println("PARAMETER_CHANGE");   
                ti = getPosition(channel, position).typeIndex;
                break;
            case NOTE_OFF:
                Serial.println("NOTE_OFF");   
                break;
            default:
                break;
        }

        Serial.println("");
};