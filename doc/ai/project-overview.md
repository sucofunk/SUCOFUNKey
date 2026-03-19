# SUCOFUNKey Project Documentation

## Project Overview

SUCOFUNKey is firmware for **Beatmaker's Sketchbook**, a hardware sampler/sequencer built on the **Teensy 4.1** microcontroller with an **ILI9341 2.4" TFT display** (320×240 RGB565). The project is developed using **PlatformIO**.

**Website:** https://www.sucofunk.com  
**Author:** Marc Berendes (marc@sucofunk.com)  
**License:** GPL v3

---

## Hardware Platform

- **MCU:** Teensy 4.1 (ARM Cortex-M7, 600MHz)
- **Display:** ILI9341 2.4" TFT (320×240, RGB565 color)
- **Storage:** SD card for samples, songs, and configuration
- **Audio:** Teensy Audio Library with external memory (PSRAM)
- **Input:** Custom keyboard matrix with I2C expanders, faders, encoders
- **MIDI:** USB MIDI and hardware MIDI I/O

---

## Directory Structure

```
SUCOFUNKey/
├── platformio.ini          # PlatformIO configuration
├── src/
│   ├── main.cpp            # Entry point, context switching, main loop
│   ├── context/            # Application contexts (modes/screens)
│   │   ├── arrange/        # Arrangement mode
│   │   ├── check/          # System check
│   │   ├── home/           # Home screen
│   │   ├── live/           # Live performance mode
│   │   ├── recorder/       # Audio recording
│   │   ├── sampler/        # Sample editing
│   │   ├── sequencer/      # Main sequencer (Play, Selection, Snippets, SongStructure, Swing, Zoom)
│   │   ├── settings/       # Configuration settings
│   │   ├── synth/          # Wavetable synthesizer
│   │   └── synthcopy/      # Synth preset copy
│   ├── gui/
│   │   ├── Screen.cpp/h    # Main screen abstraction (wraps ILI9341_t3n)
│   │   ├── fonts/          # Adafruit GFX fonts
│   │   └── screens/        # Screen implementations for each context
│   │       └── components/ # Reusable UI components (menus, keyboard, etc.)
│   ├── hardware/
│   │   ├── Configuration.h # ConfigurationValues struct, version management
│   │   ├── Sucofunkey.cpp/h # Hardware abstraction (keys, LEDs, faders, I2C)
│   │   └── ILI9341/        # Modified ILI9341_t3n display driver
│   └── helper/
│       ├── AudioResources.cpp/h  # Audio playback objects
│       ├── FSIO.cpp/h            # File system I/O, songs, library
│       ├── SampleFSIO.cpp/h      # Sample loading/saving
│       ├── DebugPrint.cpp/h      # Conditional debug output
│       ├── screen-streaming/     # USB screen mirroring feature
│       └── audio-extensions/     # Custom audio objects (play_sd_raw_suco, synth_wavetable_suco)
├── tools/
│   └── screen-stream/      # Web app for screen streaming
├── doc/
│   ├── manual.md           # User manual
│   └── ai/                 # AI documentation
└── built/                  # Pre-built firmware binaries
```

---

## Architecture

### Context System

The application uses a **context-based architecture** where each mode (Home, Sequencer, Sampler, etc.) is a separate context class. The main loop in `main.cpp` handles:

1. Hardware input polling (`keyboard.scanI2C()`)
2. Event routing to active context
3. Audio processing
4. MIDI handling
5. Screen streaming (if enabled)

### Key Classes

| Class | Purpose |
|-------|---------|
| `Sucofunkey` | Hardware abstraction - keys, LEDs, faders, encoders |
| `Screen` | Display abstraction wrapping ILI9341_t3n |
| `Configuration` | Persistent settings (ConfigurationValues struct) |
| `FSIO` | Song/library file management |
| `SampleFSIO` | Sample data handling with PSRAM |
| `SongStructure` | Sequencer data model (patterns, blocks, samples) |
| `Play` | Sequencer playback engine |

### Configuration System

Settings are stored in `ConfigurationValues` struct in `Configuration.h`:

```cpp
struct ConfigurationValues {
    byte configVersion;           // Must match CONFIGURATION_VERSION
    byte midiChannelPlay;
    byte midiChannelPiano;
    // ... other settings ...
    boolean enableScreenStreaming; // Screen streaming toggle
};
```

**Version management:** When adding new fields:
1. Increment `CONFIGURATION_VERSION` in `Configuration.h`
2. Add field to `ConfigurationValues` struct
3. Set default in `resetConfiguration()`
4. Update `FSIO::loadConfiguration()` version check
5. Add UI in `Settings.cpp` / `SettingsScreen.cpp`

---

## Screen Streaming Feature

Real-time USB screen mirroring to browser for screencasts.

### Components

- `src/helper/screen-streaming/ScreenStreaming.cpp` - Intercepts draw calls, sends binary commands
- `src/helper/DebugPrint.cpp` - Suppresses Serial.print when streaming enabled
- `tools/screen-stream/index.html` - WebSerial-based viewer (Chrome/Edge)

### Protocol

Binary commands over USB Serial with sync markers (`FE 00 00 FE 00`) every 15 commands.

### Debug Output

Use `DebugPrint::print()` / `DebugPrint::println()` instead of `Serial.print()` to avoid interfering with streaming protocol. The helper checks `Configuration::configurationValues.enableScreenStreaming` at runtime.

---

## Sequencer Data Model

### SongStructure

The sequencer uses a block-based sparse storage system:

- **Blocks:** Divide timeline into segments, each with start pointers per channel
- **SamplePointers:** Linked list entries pointing to sample/event data
- **SampleBucket:** Actual sample trigger data (sample number, velocity, pitch, etc.)
- **ParameterChangeSampleBucket:** Automation data
- **MidiNoteBucket:** MIDI note events

### Key Methods

```cpp
SongStructure::setSample(channel, position, sampleStruct)
SongStructure::getPosition(channel, position)
SongStructure::removePosition(channel, position)
SongStructure::copyOrMoveSelection(selection, offsetChannel, offsetPosition, deleteAfterOperation)
```

---

## Display System

### Screen Class

Wraps `ILI9341_t3n` with higher-level methods:

```cpp
screen.fillArea(area, color)
screen.drawTextInArea(area, position, wrap, size, bold, color, text)
screen.drawLine(x1, y1, x2, y2, color)
// etc.
```

### Fonts

Three Adafruit GFX fonts converted from TTF:
- `OxygenMono_Regular8pt7b` - Monospace
- `BaiJamjuree_Medium5pt7b` - Small text
- `BaiJamjureeRegularMonoDigits8pt7b` - Monospace digits

---

## Audio System

Uses Teensy Audio Library with custom extensions:

- `play_sd_raw_suco` - SD card raw audio playback
- `play_memory_suco` - PSRAM sample playback
- `synth_wavetable_suco` - Wavetable synthesizer

Samples are loaded into PSRAM (`_extmemArray`) for low-latency playback.

---

## File Formats

### Song Directory Structure

```
/SONGS/<songname>/
├── PATTERN/
│   ├── META.DAT        # MetaInfos struct
│   ├── BLOCKS.DAT      # Block array
│   ├── SMPLPTR.DAT     # Sample pointers
│   ├── SMPLBKT.DAT     # Sample bucket
│   ├── SPSBKT.DAT      # Parameter changes
│   ├── MIDINBKT.DAT    # MIDI notes
│   └── SNIPPETS.DAT    # Selection snippets
└── SAMPLES/
    ├── B1S01.RAW       # Bank 1, Sample 1
    ├── B1S01.NFO       # Sample info
    └── ...
```

### Configuration

`/CONFIG.DAT` - Binary dump of `ConfigurationValues` struct

---

## Common Patterns

### Adding a New Setting

1. Add field to `ConfigurationValues` in `Configuration.h`
2. Increment `CONFIGURATION_VERSION`
3. Set default in `Configuration::resetConfiguration()`
4. Update version check in `FSIO::loadConfiguration()`
5. Add getter/setter if needed
6. Add UI in `Settings.cpp` (handleEvent) and `SettingsScreen.cpp` (display)

### Adding Debug Output

```cpp
#include "../helper/DebugPrint.h"

DebugPrint::print("Value: ");
DebugPrint::println(someValue);
```

### Screen Drawing

```cpp
_screen->fillArea(_screen->AREA_CONTENT, _screen->C_BLACK);
_screen->drawTextInArea(_screen->AREA_CONTENT, 
    _screen->TEXTPOSITION_HCENTER_VCENTER, 
    true, _screen->TEXTSIZE_MEDIUM, false, 
    _screen->C_WHITE, "Hello");
```

---

## Build & Upload

```bash
# Build
platformio run

# Upload to Teensy
platformio run --target upload

# Clean
platformio run --target clean
```

---

## Tips for AI Sessions

1. **Configuration changes** require version increment and FSIO update
2. **Serial.print** → Use `DebugPrint` for streaming compatibility
3. **Screen commands** go through `Screen` class, not direct ILI9341
4. **Context switching** handled in `main.cpp` main loop
5. **Sample data** lives in PSRAM via `SampleFSIO`
6. **Settings UI** split between `Settings.cpp` (logic) and `SettingsScreen.cpp` (display)
