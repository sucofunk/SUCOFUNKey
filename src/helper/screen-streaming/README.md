# Screen Streaming System: Teensy 4.1 to Browser

## Overview

A real-time screen mirroring system that streams the ILI9341 2.4" TFT display (320×240 RGB565) from a Teensy 4.1 microcontroller to a web browser via USB Serial. Designed for creating screencasts and documentation.

## Architecture

```
Teensy 4.1 ──[USB Serial]──> Browser (WebSerial API)
     │                              │
     └── ScreenStreaming.cpp        └── index.html
         (intercepts draw calls)        (renders commands)
```

## Configuration

Screen streaming is controlled at runtime via the Settings menu:

**Settings → Screen Streaming: ON/OFF**

When enabled:
- Draw calls are intercepted and sent over USB Serial
- Debug output is suppressed (via DebugPrint helper) to avoid protocol interference

The setting is persisted in the configuration file on the SD card.

## Teensy Firmware Components

**Files:**
- `src/helper/screen-streaming/ScreenStreaming.h` - Header with command opcodes and font IDs
- `src/helper/screen-streaming/ScreenStreaming.cpp` - Implementation
- `src/helper/DebugPrint.h/.cpp` - Conditional debug output (silent when streaming enabled)

**Protocol:**
- Binary commands sent over USB Serial
- Each command starts with an opcode byte followed by parameters
- Sync markers (`FE 00 00 FE 00`) sent every 15 commands for stream recovery
- Initial handshake: Client sends `"STR\x01"`, Teensy responds with same marker

**Command opcodes:**

| Opcode | Command | Parameters |
|--------|---------|------------|
| 0x01 | FILL_RECT | x, y, w, h, color (RGB565) |
| 0x02 | LINE | x1, y1, x2, y2, color |
| 0x03 | PIXEL | x, y, color |
| 0x04 | HLINE | x, y, w, color |
| 0x05 | VLINE | x, y, h, color |
| 0x06 | CIRCLE | x, y, r, color, filled |
| 0x07 | TRIANGLE | x1, y1, x2, y2, x3, y3, color, filled |
| 0x08 | TEXT | x, y, fontId, color, length, chars[] |
| 0x09 | BITMAP | x, y, w, h, pixel data |
| 0xFE | SYNC | Sync marker pattern |
| 0xFF | CLEAR | color |

## Web Application

**File:** `tools/screen-stream/index.html`

**Features:**
- WebSerial API for USB communication (Chrome/Edge required)
- Canvas-based rendering matching ILI9341 commands
- Embedded GFX fonts (OxygenMono_Regular8pt, BaiJamjuree_Medium5pt, BaiJamjuree_MonoDigits8pt)
- Automatic desync detection and recovery
- Silent filtering of ASCII debug text that may leak through
- Scalable display (1x, 2x, 3x, 4x)
- Save screenshot as PNG

**UI Design:**
- Minimalist interface for clean screencasts
- Black background with 5px padding
- Slide-out hamburger menu containing:
  - Connect/Disconnect button
  - Hide/Show corner dots (alignment aids)
  - Save PNG button
  - Scale selector
- Debug logging to browser console

## Data Flow

1. **Initialization:** Browser connects via WebSerial, sends `"STR\x01"`
2. **Streaming:** Teensy intercepts ILI9341_t3n draw calls, serializes to binary commands with 200µs delay between commands
3. **Rendering:** Browser parses binary stream, executes equivalent Canvas 2D operations
4. **Sync recovery:** On detecting 3+ consecutive unknown opcodes, browser waits for next sync marker
5. **Text filtering:** Printable ASCII (0x20-0x7E) and line endings are silently skipped as debug text

## Debug Output

The codebase uses `DebugPrint::print()` and `DebugPrint::println()` instead of direct `Serial.print()` calls. This helper class checks the streaming configuration and only outputs debug text when streaming is disabled, preventing interference with the binary protocol.

## Fonts

The web app includes JavaScript implementations of 3 Adafruit GFX fonts with matching bitmap data:
- `OxygenMono_Regular8pt7b` - Monospace font
- `BaiJamjuree_Medium5pt7b` - Small Thai/Latin font  
- `BaiJamjureeRegularMonoDigits8pt7b` - Monospace digits

Font rendering uses the same glyph structure as Adafruit GFX (bitmap array, glyph metadata, kerning).

## Usage

1. Enable screen streaming in SUCOFUNKey: **Settings → Screen Streaming: ON**
2. Open `tools/screen-stream/index.html` in Chrome/Edge
3. Connect Teensy via USB
4. Click hamburger menu → Connect
5. Select the Teensy serial port
6. Display mirrors in real-time

**Note:** Debug output via Serial Monitor is suppressed while streaming is enabled. Disable streaming in Settings to restore normal Serial debugging.
