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

Real-time USB screen mirroring to browser for screencasts and documentation. Streams display output and hardware events (LEDs, keys, encoders, fader) over USB Serial to a WebSerial-based viewer.

### Architecture Overview

```
┌─────────────────────┐     USB Serial      ┌──────────────────────┐
│   Teensy 4.1        │────────────────────▶│   Browser (WebSerial)│
│                     │   Binary Protocol   │                      │
│  Screen.cpp         │                     │  index.html          │
│       │             │                     │       │              │
│       ▼             │                     │       ▼              │
│  ScreenStreaming    │                     │  Canvas 2D API       │
│  (intercepts draws) │                     │  + SVG Hardware View │
└─────────────────────┘                     └──────────────────────┘
```

### Source Files

| File | Purpose |
|------|---------|
| `src/helper/screen-streaming/ScreenStreaming.h` | Command opcodes, class definition |
| `src/helper/screen-streaming/ScreenStreaming.cpp` | Binary protocol implementation |
| `src/helper/DebugPrint.cpp/h` | Serial.print wrapper (suppresses when streaming) |
| `tools/screen-stream/index.html` | WebSerial viewer with SVG hardware mockup |
| `tools/screen-stream/device.svg` | Hardware device visualization |

### Compile-Time Flag

Screen streaming is conditionally compiled:

```cpp
// In Configuration.h
#define ENABLE_SCREEN_STREAMING

// Usage in code
#ifdef ENABLE_SCREEN_STREAMING
#include "helper/screen-streaming/ScreenStreaming.h"
#endif
```

### Binary Protocol

Commands are sent as binary packets over USB Serial at native USB speed.

#### Command Format

| Opcode | Name | Bytes | Format |
|--------|------|-------|--------|
| `0x01` | FILL_RECT | 11 | `01 x0L x0H y0L y0H wL wH hL hH cL cH` |
| `0x02` | LINE | 11 | `02 x0L x0H y0L y0H x1L x1H y1L y1H cL cH` |
| `0x03` | PIXEL | 7 | `03 xL xH yL yH cL cH` |
| `0x04` | HLINE | 9 | `04 xL xH yL yH wL wH cL cH` |
| `0x05` | VLINE | 9 | `05 xL xH yL yH hL hH cL cH` |
| `0x06` | CIRCLE | 10 | `06 xL xH yL yH rL rH fill cL cH` |
| `0x07` | TRIANGLE | 16 | `07 x0L x0H y0L y0H x1L x1H y1L y1H x2L x2H y2L y2H fill cL cH` |
| `0x08` | TEXT | 6+n | `08 xL xH yL yH cL cH font len text...` |
| `0x09` | BITMAP | 11+n | `09 xL xH yL yH wL wH hL hH cL cH data...` |
| `0x10` | LED_STATE | 3 | `10 ledId state` |
| `0x11` | KEY_EVENT | 5 | `11 idxL idxH pressed type` |
| `0x12` | ENCODER | 3 | `12 encoderId clockwise` |
| `0x13` | FADER | 2 | `13 percent` |
| `0xFE` | SYNC | 5 | `FE 00 00 FE 00` |
| `0xFF` | CLEAR | 3 | `FF cL cH` |

- All 16-bit values are **little-endian**
- Colors are RGB565 format
- Sync markers sent every 15 commands for recovery from byte drops

#### Sync Marker Protocol

The sync pattern `FE 00 00 FE 00` uses null bytes that cannot appear in text strings, making it unambiguous:

```cpp
// ScreenStreaming.cpp
void ScreenStreaming::doSendSync() {
    uint8_t sync[] = {CMD_SYNC, 0x00, 0x00, CMD_SYNC, 0x00};
    Serial.write(sync, 5);
    Serial.flush();
}
```

### Memory Optimization

Screen streaming uses memory-efficient techniques to minimize RAM usage:

#### FLASHMEM Functions

All `ScreenStreaming` methods use `FLASHMEM` attribute to store code in flash instead of ITCM (RAM):

```cpp
FLASHMEM void ScreenStreaming::logFillRect(...) { ... }
FLASHMEM void ScreenStreaming::logLine(...) { ... }
// etc.
```

This saves ~3.4KB of ITCM RAM and avoids ITCM block padding waste (~30KB).

#### DMAMEM Buffer

The streaming buffer is placed in RAM2 (DMAMEM) instead of primary RAM:

```cpp
// In ScreenStreaming.cpp
DMAMEM static uint8_t _streamBuffer[STREAM_BUFFER_SIZE];  // 32KB
```

### Integration Points

#### Screen Class Integration

`Screen.cpp` calls streaming methods after each draw operation:

```cpp
void Screen::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    _tft->fillRect(x, y, w, h, color);
    #ifdef ENABLE_SCREEN_STREAMING
    if (_streaming) _streaming->logFillRect(x, y, w, h, color);
    #endif
}
```

#### Hardware Event Integration

`Sucofunkey.cpp` logs hardware events:

```cpp
// LED state changes
_screenStreaming->logLedState(ledId, state);

// Key press/release
_screenStreaming->logKeyEvent(index, pressed, keyType);

// Encoder rotation
_screenStreaming->logEncoderEvent(encoderId, clockwise);

// Fader position
_screenStreaming->logFaderPosition(percent);
```

#### Main Loop Integration

```cpp
// In main.cpp loop()
#ifdef ENABLE_SCREEN_STREAMING
screenStreaming.checkSerialCommand();  // Check for viewer requests
#endif
```

### Web Viewer (`tools/screen-stream/index.html`)

Single-file HTML application using WebSerial API (Chrome/Edge only).

#### Features

- **320×240 Canvas** - Renders screen content using Canvas 2D API
- **SVG Hardware Mockup** - Shows device with interactive LEDs, keys, encoders, fader
- **Font Rendering** - Embedded GFX font bitmaps for accurate text rendering
- **Auto-Resync** - Recovers from byte drops using sync markers
- **PNG Export** - Save current screen as image

#### Hardware Visualization

The viewer includes mappings from hardware IDs to SVG elements:

```javascript
const LED_MAP = {
    7: 'led-play',
    52: 'led-record',
    // ... etc
};

const KEY_MAP = {
    18: 'key-fn',
    17: 'key-menu',
    // ... etc
};
```

#### Usage

1. Enable screen streaming in device settings
2. Open `tools/screen-stream/index.html` in Chrome/Edge
3. Click "Connect" and select the Teensy serial port
4. Device screen mirrors in real-time with hardware state

### Debug Output Compatibility

Use `DebugPrint` instead of `Serial.print` to avoid corrupting the binary stream:

```cpp
#include "../helper/DebugPrint.h"

// Instead of: Serial.print("Debug: "); Serial.println(value);
DebugPrint::print("Debug: ");
DebugPrint::println(value);
```

`DebugPrint` checks `Configuration::configurationValues.enableScreenStreaming` and suppresses output when streaming is active.

### Configuration

Screen streaming is toggled via the Settings menu:

1. **Settings screen** - Toggle "Screen Streaming" on/off
2. Stored in `ConfigurationValues.enableScreenStreaming`
3. Requires device restart to take effect

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
