/* ----------------------------------------------------------------------------------------------

    SUCOFUNKey

    This project is the firmware - named SUCOFUNKey - for Beatmaker's Sketchbook from SUCOFUNK. 

    For more information, check out www.sucofunk.com

    To support the development of this firmware, please donate to the project and buy hardware
    from sucofunk.com.

    Copyright 2021-2022 by Marc Berendes (marc @ sucofunk.com)
    
   ----------------------------------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.    

   ---------------------------------------------------------------------------------------------- */
   
#include "SongStructure.h"
#include "Swing.h"

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

SongStructure::midiNoteStruct SongStructure::getMidiNoteFromBucketId(uint16_t midiNoteBucketIndex) {
    return _midiNoteBucket[midiNoteBucketIndex];
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
    _sampleBucket[sbi].swing = sample.swing;
    _sampleBucket[sbi].reverse = sample.reverse;

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
    _parameterChangeSampleBucket[pcbi].pitchChange = parameterChange.pitchChange;
    _parameterChangeSampleBucket[pcbi].reverse = parameterChange.reverse;

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


boolean SongStructure::setMidiNote(uint8_t channel, uint16_t position, midiNoteStruct midiNote) {

    uint16_t blockIndex = _getBlockIndex(position);
    uint8_t blockOffset = _getBlockOffset(position);

    uint16_t mnbi = _getNextMidiNoteBucketIndex();
    _midiNoteBucket[mnbi].note = midiNote.note;
    _midiNoteBucket[mnbi].velocity = midiNote.velocity;
    _midiNoteBucket[mnbi].channel = midiNote.channel;

    uint16_t startPointerIndex = _blocks[blockIndex].startPointer[channel];

    // bucket has no content in this channel yet.. create first pointer
    if (startPointerIndex == 0) {
        uint16_t spi = _getNextSamplePointerIndex();
        _samplePointers[spi].columnOffset = blockOffset;
        _samplePointers[spi].successor = 0;
        _samplePointers[spi].type = MIDINOTE;
        _samplePointers[spi].typeIndex = mnbi;

        _blocks[blockIndex].startPointer[channel] = spi;
        setCurrentPosition(channel, position, true);
        return true;
    }

    uint16_t lastPointer = 0;
    uint16_t nextPointer = startPointerIndex;
    boolean cont = true;

    while (cont) {
        // position is already taken, overwrite with new midiNote
        if (_samplePointers[nextPointer].columnOffset == blockOffset) {
            _clearPositionBucket(nextPointer);
        
            _samplePointers[nextPointer].type = MIDINOTE;
            _samplePointers[nextPointer].typeIndex = mnbi;

            setCurrentPosition(channel, position, true);            
            return true;
        }

        if (_samplePointers[nextPointer].columnOffset > blockOffset) {
             // the pointer is pointing ahead of the position and the position is free. insert sample in between

            uint16_t spi = _getNextSamplePointerIndex();
            _samplePointers[spi].columnOffset = blockOffset;
            _samplePointers[spi].successor = nextPointer;
            _samplePointers[spi].type = MIDINOTE;
            _samplePointers[spi].typeIndex = mnbi;

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
                _samplePointers[spi].type = MIDINOTE;
                _samplePointers[spi].typeIndex = mnbi;

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
            _meta.reuseableMidiNoteBucketIndex = _samplePointers[samplePointerIndex].typeIndex;
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
        nss.swing = ss.swing;
        nss.reverse = ss.reverse;
        
        if (setSample(toChannel, toPosition, nss) == false) {
            return false;
        }
    }
        
    if (from.type == PARAMETER_CHANGE_SAMPLE) {
        parameterChangeSampleStruct ps = getParameterChangeFromBucketId(from.typeIndex);

        parameterChangeSampleStruct nps;
        nps.stereoPosition = ps.stereoPosition;
        nps.velocity = ps.velocity;
        nps.pitchChange = ps.pitchChange;
        nps.reverse = ps.reverse;

        if (setParameterChange(toChannel, toPosition, nps) == false) {
            return false;
        }            
    }


    if (from.type == MIDINOTE) {
        midiNoteStruct mns = getMidiNoteFromBucketId(from.typeIndex);
        
        midiNoteStruct nmns;
        nmns.channel = mns.channel;
        nmns.note = mns.note;
        nmns.velocity = mns.velocity;        
        
        if (setMidiNote(toChannel, toPosition, nmns) == false) {
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
            case MIDINOTE:
                if (_midiNoteBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].velocity < 126) {
                    _midiNoteBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].velocity += 2;
                }
                break;                
            default:
                break;
        }
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
            case MIDINOTE:
                if (_midiNoteBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].velocity > 1) {
                    _midiNoteBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].velocity -= 2;                    
                }
            default:
                break;
        }
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
            case MIDINOTE:
                _midiNoteBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].velocity = velocity;
                break;
            default:
                break;                
        }
    } 
};




void SongStructure::increaseMidiChannel(uint8_t channel, uint16_t position) {
    if (_currentPosition.channel == channel && _currentPosition.position == position && _samplePointers[_currentPosition.samplePointerIndex].type == MIDINOTE) {
        if (_midiNoteBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].channel < 12) {
            _midiNoteBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].channel += 1;
        }
    } 
};

void SongStructure::decreaseMidiChannel(uint8_t channel, uint16_t position) {
    if (_currentPosition.channel == channel && _currentPosition.position == position && _samplePointers[_currentPosition.samplePointerIndex].type == MIDINOTE) {
        if (_midiNoteBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].channel > 1) {
            _midiNoteBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].channel -= 1;
        }
    } 
};

void SongStructure::setMidiChannel(uint8_t channel, uint16_t position, byte midiChannel) {
    if (_currentPosition.channel == channel && _currentPosition.position == position && _samplePointers[_currentPosition.samplePointerIndex].type == MIDINOTE) {
            _midiNoteBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].channel = midiChannel;
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
            case MIDINOTE:
                if (_midiNoteBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].note < 126) {
                    _midiNoteBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].note += 1;
                }
                break;
            case PARAMETER_CHANGE_SAMPLE:
                if (_parameterChangeSampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].pitchChange < 254) {                    
                    _parameterChangeSampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].pitchChange += 1;
                }                
                break;
            default:
                break;
        }
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
            case MIDINOTE:
                if (_midiNoteBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].note > 1) {
                    _midiNoteBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].note -= 1;
                }
                break;
            case PARAMETER_CHANGE_SAMPLE:
                if (_parameterChangeSampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].pitchChange > 0) {                    
                    _parameterChangeSampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].pitchChange -= 1;
                }                
                break;                                
            default:
                break;
        }
    } 
};

void SongStructure::setPitchByMidiNote(uint8_t channel, uint16_t position, byte note) {
    if (_currentPosition.channel == channel && _currentPosition.position == position) {
        switch(_samplePointers[_currentPosition.samplePointerIndex].type) {
            case SAMPLE:
                _sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].pitchedNote = note;
                break;
            case MIDINOTE:
                _midiNoteBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].note = note;
                break;                
            default:
                break;
        }
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
            case MIDINOTE:
                if (_midiNoteBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].probability < 100) {
                    _midiNoteBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].probability += 1;
                }
                break;
            default:
                break;
        }
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
            case MIDINOTE:
                if (_midiNoteBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].probability > 1) {
                    _midiNoteBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].probability -= 1;
                }
                break;                
            default:
                break;
        }
    }
};

// probability needs to be 0..100
void SongStructure::setProbability(uint8_t channel, uint16_t position, byte probability) {
    if (_currentPosition.channel == channel && _currentPosition.position == position) {
        switch(_samplePointers[_currentPosition.samplePointerIndex].type) {
            case SAMPLE:
                    _sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].probability = probability;
                break;
            case MIDINOTE:
                    _midiNoteBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].probability = probability;
                break;                
            default:
                break;
        }
    } 
};


void SongStructure::swingLevelUp(uint8_t channel, uint16_t position) {
    byte expression, group;
    if (_currentPosition.channel == channel && _currentPosition.position == position) {
        switch(_samplePointers[_currentPosition.samplePointerIndex].type) {
            case SAMPLE:
                    expression = _sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].swing;
                    group = _swing.getGroupFromBinarySwingExpression(expression);
                    if (group == 0) {
                        _sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].swing = _swing.levelUpInExpression(expression);
                    } else {
                        setSwingGroupLevel(group, _swing.levelUp(getSwingGroupLevel(group)));                        
                    };                    
                break;
            case MIDINOTE:
                break;    
            default:
                break;
        }
    }
};

void SongStructure::swingLevelDown(uint8_t channel, uint16_t position) {
    byte expression, group;
    if (_currentPosition.channel == channel && _currentPosition.position == position) {
        switch(_samplePointers[_currentPosition.samplePointerIndex].type) {
            case SAMPLE:
                    expression = _sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].swing;
                    group = _swing.getGroupFromBinarySwingExpression(expression);
                    if (group == 0) {
                        _sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].swing = _swing.levelDownInExpression(expression);
                    } else {
                        setSwingGroupLevel(group, _swing.levelDown(getSwingGroupLevel(group)));
                    };                    
                break;
            case MIDINOTE:
                break;                
            default:
                break;
        }
    }
};

void SongStructure::swingGroupUp(uint8_t channel, uint16_t position) {
    byte expression;

    if (_currentPosition.channel == channel && _currentPosition.position == position) {
        switch(_samplePointers[_currentPosition.samplePointerIndex].type) {
            case SAMPLE:
                    expression = _sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].swing;
                    _sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].swing = _swing.groupUpInExpression(expression);
                break;
            case MIDINOTE:
                break;                
            default:
                break;
        }
    }
};

void SongStructure::swingGroupDown(uint8_t channel, uint16_t position) {
    byte expression;
    if (_currentPosition.channel == channel && _currentPosition.position == position) {
        switch(_samplePointers[_currentPosition.samplePointerIndex].type) {
            case SAMPLE:
                    expression = _sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].swing;
                    _sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].swing = _swing.groupDownInExpression(expression);
                break;
            case MIDINOTE:
                break;                
            default:
                break;
        }
    }    
};


void SongStructure::setSwingTickMicroseconds(int microseconds) {
    _swing.setTickMicroseconds(microseconds);
};


void SongStructure::reverseSamplePlayback(uint8_t channel, uint16_t position) {
    if (_currentPosition.channel == channel && _currentPosition.position == position) {
        switch(_samplePointers[_currentPosition.samplePointerIndex].type) {
            case SAMPLE:
                    _sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].reverse = !_sampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].reverse;
                break;
            case PARAMETER_CHANGE_SAMPLE:
                    _parameterChangeSampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].reverse = !_parameterChangeSampleBucket[_samplePointers[_currentPosition.samplePointerIndex].typeIndex].reverse;
                break;                
            default:
                break;
        }
    }        
};



void SongStructure::setSongLength(uint16_t songLength) {
    _meta.songLength = songLength;
};

uint16_t SongStructure::getSongLength() {
    return _meta.songLength;
};

boolean SongStructure::changeSongLengthByTick(boolean increase, byte tickAmount) {
    if (increase) {
        if (_meta.songLength+tickAmount < _maxSongLength) {
            _meta.songLength = _meta.songLength + tickAmount;
            return true;
        } else {
            // max song length reached
            return false;
        }
    } else {
        // song needs at least to be 2 "ticks" long - no idea why, but how would a 1 Tick song look like? Sell it on ebay for a million?
        if (_meta.songLength > 2*tickAmount) {
            _meta.songLength = _meta.songLength - tickAmount;
            return true;
        } else {
            return false;
        }
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


// re-initializing _meta might be an option, too. maybe the values for BPM, etc.. might get edited in the settings menue..
void SongStructure::setMetadataToDefault() {
    _meta.nextSamplePointerIndex = 0;
    _meta.reusableSamplePointerIndex = 0;
    
    _meta.nextSampleBucketIndex = 1;
    _meta.reuseableSampleBucketIndex = 0;

    _meta.nextParameterChangeBucketIndex = 1;
    _meta.reuseableParameterChangeBucketIndex = 0;

    _meta.nextMidiNoteBucketIndex = 1;
    _meta.reuseableMidiNoteBucketIndex = 0;

    _meta.songLength = 64;
    _meta.songResolution = 4;
    
    _meta.playbackSpeed = 85.0;
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
                        samplesUsed[_sampleBucket[_samplePointers[nextPointer].typeIndex].sampleNumber-1] = true;
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


byte SongStructure::getSwingGroupLevel(byte group) {
    return _meta.swingGroupLevels[group];
};

void SongStructure::setSwingGroupLevel(byte group, byte level) {
    _meta.swingGroupLevels[group] = level;
};



// ----------------------------------------------------------------------------------------------------------
// Loading and saving a song structure
// ----------------------------------------------------------------------------------------------------------

boolean SongStructure::loadFromSD(char *songPath) {
    char buff[40];
    File readFile;
    byte *bufferBlocks;

    // read _meta
    strcpy(buff, songPath);
    strcat(buff, "/PATTERN/META.DAT");

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



    // read _midiNoteBucket
    strcpy(buff, songPath);
    strcat(buff, "/PATTERN/MIDINBKT.DAT");

    readFile = SD.open(buff, FILE_READ);

    for (uint16_t i=0; i<sizeof(_midiNoteBucket)/sizeof(midiNoteStruct); i++) {
        bufferBlocks = (byte *) &_midiNoteBucket[i];

        for (uint16_t j=0; j<sizeof(midiNoteStruct); j++) {
            if (readFile.available()) {
                *(bufferBlocks + j) = readFile.read();
            }
        }            
    }

    readFile.close();


    // read _snippets
    strcpy(buff, songPath);
    strcat(buff, "/PATTERN/SNIPPETS.DAT");

    readFile = SD.open(buff, FILE_READ);

    for (uint16_t i=0; i<sizeof(_snippets)/sizeof(Selection::SelectionStruct); i++) {
        bufferBlocks = (byte *) &_snippets[i];

        for (uint16_t j=0; j<sizeof(Selection::SelectionStruct); j++) {
            if (readFile.available()) {
                *(bufferBlocks + j) = readFile.read();
            }
        }            
    }

    readFile.close();


    return true;
};




boolean SongStructure::saveMetadataToSD(char *songPath) {
    char buff[40];
    File writeFile;
    byte *bufferBlocks;

    // write _meta -> all meta infos for this song
    strcpy(buff, songPath);
    strcat(buff, "/PATTERN/META.DAT");

    if (SD.exists(buff)) { SD.remove(buff); }

    writeFile = SD.open(buff, FILE_WRITE);

    bufferBlocks = (byte *) &_meta;
    writeFile.write(bufferBlocks, sizeof(MetaInfos));
 
    writeFile.close();
}


boolean SongStructure::saveToSD(char *songPath) {
    
    // ToDo: SD card error handling and removing the delays, which are just here for the case that the sd card hangs for a few ms

    saveMetadataToSD(songPath);

    char buff[40];
    File writeFile;
    byte *bufferBlocks;
 

    // write _blocks
    strcpy(buff, songPath);
    strcat(buff, "/PATTERN/BLOCKS.DAT");

    if (SD.exists(buff)) { SD.remove(buff); }

    writeFile = SD.open(buff, FILE_WRITE);

    for (uint16_t i=0; i<sizeof(_blocks)/sizeof(blockStruct); i++) {
        bufferBlocks = (byte *) &_blocks[i];
        writeFile.write(bufferBlocks, sizeof(_blocks[i]));
    }
 
    writeFile.close();

    delay(100);


    // write _samplePointers
    strcpy(buff, songPath);
    strcat(buff, "/PATTERN/SMPLPTR.DAT");

    if (SD.exists(buff)) { SD.remove(buff); }

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

    writeFile = SD.open(buff, FILE_WRITE);

    for (uint16_t i=0; i<sizeof(_parameterChangeSampleBucket)/sizeof(parameterChangeSampleStruct); i++) {
        bufferBlocks = (byte *) &_parameterChangeSampleBucket[i];
        writeFile.write(bufferBlocks, sizeof(_parameterChangeSampleBucket[i]));
    }
 
    writeFile.close();

    delay(100);    



    // write _midiNoteBucket
    strcpy(buff, songPath);
    strcat(buff, "/PATTERN/MIDINBKT.DAT");

    if (SD.exists(buff)) { SD.remove(buff); }

    writeFile = SD.open(buff, FILE_WRITE);

    for (uint16_t i=0; i<sizeof(_midiNoteBucket)/sizeof(midiNoteStruct); i++) {
        bufferBlocks = (byte *) &_midiNoteBucket[i];
        writeFile.write(bufferBlocks, sizeof(_midiNoteBucket[i]));
    }
 
    writeFile.close();


    delay(100);

    // write _snippets
    strcpy(buff, songPath);
    strcat(buff, "/PATTERN/SNIPPETS.DAT");

    if (SD.exists(buff)) { SD.remove(buff); }

    writeFile = SD.open(buff, FILE_WRITE);

    for (uint16_t i=0; i<sizeof(_snippets)/sizeof(Selection::SelectionStruct); i++) {
        bufferBlocks = (byte *) &_snippets[i];
        writeFile.write(bufferBlocks, sizeof(_snippets[i]));
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


uint16_t SongStructure::_getNextMidiNoteBucketIndex() {
    if (_meta.reuseableMidiNoteBucketIndex != 0) {
        // hey, there is a free index somewhere.. it was deleted and can now be reused to have no fragmentations
        uint16_t retval = _meta.reuseableMidiNoteBucketIndex;
        _meta.reuseableMidiNoteBucketIndex = 0;
        return retval;
    } else {
        // ToDo: check, if array is full
        _meta.nextMidiNoteBucketIndex++;
        return _meta.nextMidiNoteBucketIndex;
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


void SongStructure::clearSelection(Selection selection, int endOffsetX) {
    clearSelection(selection.getNormalizedSelection().startY, selection.getNormalizedSelection().startX, selection.getNormalizedSelection().endY, selection.getNormalizedSelection().endX + endOffsetX);
};

void SongStructure::clearSelection(uint8_t startChannel, uint16_t startPosition, uint8_t endChannel, uint16_t endPosition) {
    for (uint16_t p=startPosition; p<=endPosition; p++) {
        for (uint8_t c=startChannel; c<=endChannel; c++) {
            removePosition(c, p);
        }
    }
};



 // 1..14
void SongStructure::saveSelectionAsSnippet(Selection selection, byte slot) {
    _snippets[slot-1].startX = selection.getNormalizedSelection().startX;
    _snippets[slot-1].endX = selection.getNormalizedSelection().endX;
    _snippets[slot-1].startY = selection.getNormalizedSelection().startY;
    _snippets[slot-1].endY = selection.getNormalizedSelection().endY;
};

// 1..14
Selection::SelectionStruct SongStructure::getSnippet(byte slot) {
    return _snippets[slot-1];
}; 

// 1..14
boolean SongStructure::isSnippetSlotFree(byte slot) {
    if (_snippets[slot-1].startX == -1) return true;
    return false;
}; 

// 1..14
boolean SongStructure::setSnippetSlotFree(byte slot) {
    if (!isSnippetSlotFree(slot)) {
        _snippets[slot-1].startX = -1;
        _snippets[slot-1].endX = -1;
        _snippets[slot-1].startY = -1;
        _snippets[slot-1].endY = -1;
        return true;
    }
    return false;
}; 


// -------------------------------------------------------------------------------------------------------------------
//  SHEETS
// -------------------------------------------------------------------------------------------------------------------


boolean SongStructure::addSheetDivider(int position) {
    int freeSheets = 0;
    int insertBefore = 0;
    int checked = 0;

    if (position == 0) return false;

    for (int i=16; i>0; i--) {
        if (_meta.sheetDividerPositions[i] == 65535) freeSheets++;
    }
    
    if (freeSheets == 1) return false; // limit is reached!

    // find position in array where to add the new sheet divider
    for (int i=1; i<17; i++) {
        if (position == _meta.sheetDividerPositions[i]) return false; // elemnt already exists

        if (position > _meta.sheetDividerPositions[i-1] && position < _meta.sheetDividerPositions[i]) {
            insertBefore = i;
            break;
        }
        checked++;
    }

    // first element in list
    if (insertBefore == 0) insertBefore = 1;

    // shift all entries from that position one to the right
    for (int i=16; i>=insertBefore; i--) {
        _meta.sheetDividerPositions[i+1] = _meta.sheetDividerPositions[i];
    }

    _meta.sheetDividerPositions[insertBefore] = position;
    
    return true;
};

void SongStructure::removeSheetDivider(int position) {
    int removePosition = 0;

    // find the array position where position is located
    for (int i=1; i<17; i++) {
        if (_meta.sheetDividerPositions[i] == position) {
            removePosition = i;
            break;
        };
    }

    if (removePosition >= 1) {
        // shift whole array right of the found position one position left
        for (int i=removePosition+1; i<17; i++) {
            _meta.sheetDividerPositions[i-1] = _meta.sheetDividerPositions[i];
        }
        _meta.sheetDividerPositions[16] = 65535;
    }
};

int SongStructure::getNextSheetDividerPosition(int position) {
    // find position in array where the element before is smaller and the element ahead is larger than the position
    for (int i=0; i<16; i++) {
        if (_meta.sheetDividerPositions[i] == position) return _meta.sheetDividerPositions[i+1];

        if (position >= _meta.sheetDividerPositions[i] && position < _meta.sheetDividerPositions[i+1]) {
            return _meta.sheetDividerPositions[i+1];
        }
    }
    return -1;
};

int SongStructure::getPreviousSheetDividerPosition(int position) {
    // find position in array where the element before is smaller and the element ahead is larger than the position
    for (int i=1; i<16; i++) {

        if (_meta.sheetDividerPositions[i] == position) return position;

        if (position > _meta.sheetDividerPositions[i-1] && position < _meta.sheetDividerPositions[i]) {
            return _meta.sheetDividerPositions[i-1];
        }
    }
    return -1;
};

int SongStructure::getSheetStartForPosition(int position) {
    for (int i=1; i<17; i++) {
        if (position == _meta.sheetDividerPositions[i]) return _meta.sheetDividerPositions[i]; // position is sheet start

        if (position > _meta.sheetDividerPositions[i-1] && position < _meta.sheetDividerPositions[i]) {
            return _meta.sheetDividerPositions[i-1];
        }
    }
    return 0;
}

int SongStructure::getSheetEndForPosition(int position) {
    int retVal = 0;

    for (int i=1; i<17; i++) {
        if (position == _meta.sheetDividerPositions[i]) retVal = _meta.sheetDividerPositions[i+1]; // position is sheet start

        if (position > _meta.sheetDividerPositions[i-1] && position < _meta.sheetDividerPositions[i]) {            
            retVal = _meta.sheetDividerPositions[i];
        }
    }
    
    if (retVal == 65535) {
        retVal = _meta.songLength-1;
    } else {
        retVal--;
    }
    return retVal;
}


int SongStructure::getSheetStartPosition(int sheet) {
    if (sheet <= 0 || sheet > getSheetCount()) return -1;
    return _meta.sheetDividerPositions[sheet-1];
};

int SongStructure::getSheetEndPosition(int sheet) {
    if (sheet <= 0 || sheet > getSheetCount()) return -1;
    return _meta.sheetDividerPositions[sheet] == 65535 ? _meta.songLength-1 : _meta.sheetDividerPositions[sheet]-1;
};

int SongStructure::getSheetCount() {
    int retVal = 0;

    for (int i=0; i<17; i++) {
        if (_meta.sheetDividerPositions[i] != 65535) {
            retVal++;
        } 
    }

    return retVal;
};

int SongStructure::arrangementGetSheetForPosition(int position) {
    if (position >= 0 && position < _meta.maxSheetsInArrangement) return _meta.arrangementSheets[position];
    return -1;
};

int SongStructure::arrangementGetRepeatForPosition(int position) {
    if (position >= 0 && position < _meta.maxSheetsInArrangement) return _meta.arrangementRepeats[position];
    return 0;
};

void SongStructure::debugSheets() {
    for (int i=0; i<17; i++) {                        
        Serial.print(i);
        Serial.print("\t");
    }
    Serial.println("");

    for (int i=0; i<17; i++) {                        
        Serial.print(_meta.sheetDividerPositions[i]);
        Serial.print("\t");        
    }
    Serial.println("");
    Serial.println("-----");

    Serial.print("next 32::");
    Serial.println(getNextSheetDividerPosition(32));    

    Serial.print("previous 32::");
    Serial.println(getPreviousSheetDividerPosition(32));    
}


// -------------------------------------------------------------------------------------------------------------------
//  Arrangement
// -------------------------------------------------------------------------------------------------------------------

boolean SongStructure::appendSheetToArrangement(int sheet) {

    // check if sheet is really a sheet and not just a number..
    if (sheet <= 0 || sheet > getSheetCount()) return false;

    // first sheet in arrangement
    if (_meta.arrangementSheets[0] == -1) {
        _meta.arrangementSheets[0] = sheet;
        return true;
    }

    for (int i=_meta.maxSheetsInArrangement-1; i>=1; i--) {
        if (_meta.arrangementSheets[i] == -1 && _meta.arrangementSheets[i-1] != -1) {
            _meta.arrangementSheets[i] = sheet;
            return true;
        }
    }    
    
    return false;
};

boolean SongStructure::removeLastSheetFromArrangement() {
    // arrangement is complete/full, remove last sheet
    if (_meta.arrangementSheets[_meta.maxSheetsInArrangement-1] != -1) {
        _meta.arrangementSheets[_meta.maxSheetsInArrangement-1] = -1;
        return true;
    }

    // arrangement is not full/compete, delete last sheet entry
    for (int i=_meta.maxSheetsInArrangement-1; i>=1; i--) {
        if (_meta.arrangementSheets[i] == -1 && _meta.arrangementSheets[i-1] != -1) {
            _meta.arrangementSheets[i-1] = -1;
            return true;
        }        
    }    

    // happens, if there is no sheet in the arrangement
    return false;
};


boolean SongStructure::arrangementHasNextSheet(int position) {
    if (position >= 0 && position < _meta.maxSheetsInArrangement && _meta.arrangementSheets[position+1] != -1) return true;
    return false;
}


int SongStructure::arrangementChangePositionRepeat(int position, boolean increase) {
    if (_meta.arrangementSheets[position] != -1) {
        if (!increase && _meta.arrangementRepeats[position] == 0) return 0;
        _meta.arrangementRepeats[position] = _meta.arrangementRepeats[position] + (increase ? 1 : -1);
        return _meta.arrangementRepeats[position];
    }
    return 0; 
};

boolean SongStructure::arrangementInsertSheetAtPosition(int position, int sheet) {
    // check if sheet is really a sheet and not just a number..
    if (sheet <= 0 || sheet > getSheetCount()) return false;

    if (position >= 0 && position < _meta.maxSheetsInArrangement) {
        _meta.arrangementSheets[position] = sheet;
        return true;
    }
    return false;
};

boolean SongStructure::arrangementRemovePosition(int position) {
    if (position >= 0 && position < _meta.maxSheetsInArrangement) {
        _meta.arrangementSheets[position] = -1;
        return true;
    }
    return false;    
};

void SongStructure::debugArrangement() {
    // keep an eye on this bug.. probably a versioning problem..
    Serial.print("maxSheets::");
    Serial.println(_meta.maxSheetsInArrangement);
    if (_meta.maxSheetsInArrangement > 10) _meta.maxSheetsInArrangement = 80;

    Serial.print("Arrangement :: ");
    for (int i=0; i<_meta.maxSheetsInArrangement; i++) {
        Serial.print(_meta.arrangementSheets[i]);
        Serial.print(" ");
    }
    Serial.println("");
}

// -------------------------------------------------------------------------------------------------------------------
//  TESTS && DEBUGGING
// -------------------------------------------------------------------------------------------------------------------

void SongStructure::debugInfos() {

    Serial.println("DEBUG INFOS");
    Serial.print("_meta.playbackspeed::");
    Serial.print(_meta.playbackSpeed);
    Serial.print("::");
    Serial.println(getPlayBackSpeed());

    // DEBUG Information
/*    Serial.println("Blocks");
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
*/
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