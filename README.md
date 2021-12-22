# Beatmaker's sketchbook a.k.a. SUCOFUNKey
[Teensy](https://www.pjrc.com/store/teensy41.html) based Open Source Sample Sequencer. Beatmaker's sketchbook is the name of the device, SUCOFUNKey is the name of the firmware. SUCOFUNK is the brand name.

Beatmaker's sketchbook is an open source alternative to different commercial electronic music devices like the [MPC](https://en.wikipedia.org/wiki/Akai_MPC) or [OP-1](https://en.wikipedia.org/wiki/Teenage_Engineering_OP-1).

The goal is to enable everyone interested in the topic to easily build and customize its own device without spending a lot of money.
As sampling is an essential part of the Hip-Hop culture, SUCOFUNKey might be an option for amateur beat makers. It is battery powered, includes a stereo speaker and can be used everywhere.

The PCB is designed without any SMD components and can be soldered with a standard soldering iron from a local hardware store. Difficult to solder SMD components are pluggable.

Check out [@sucofunkey on Instagram](https://www.instagram.com/sucofunkey/) for a visual impression of the device and progress.
You can sign up to the newsletter on [www.sucofunk.com](https://sucofunk.com), check some background infos, contact me and stay up to date.

If you are interested in participating, a PCB or in the project in general, just send me a message. I spend a lot of time into the project without any return yet. Due to this I am working besides the project. If you want to support the project and help to keep the costs (office/workshop rent, hosting, etc.) as low as possible and speed the project up, feel free to donate.


## Hardware
Beatmaker's sketchbook houses:
- [Teensy 4.1](https://www.pjrc.com/store/teensy41.html) microcontroller
- 24 Keys to map a two octave keyboard
- 12 functional keys (play, stop, record, ...)
- 4 endless rotary encoders
- 10mm fader
- [2 inch LCD display](https://www.waveshare.com/product/displays/lcd-oled/lcd-oled-3/2inch-lcd-module.htm)
- 32 LEDs
- built in microphone 
- built in stereo speakers, driven by a small [amplifier](https://www.adafruit.com/product/987)
- headphone output
- volume control
- 3.5mm line-in and line-out
- MIDI-in and MIDI-out
- built-in LiPo battery with [charger](https://www.adafruit.com/product/1944)


## Firmware
The firmware is a VSCode project, using the [PlatformIO](https://platformio.org/) extension.


## Status

SUCOFUNKey is work in progress. Here is a shourt overview of what is already implemented and the next steps, including a roadmap of the project.
So far the firmware is in a state where you can sample and use the samples within the sequencer to create a pattern. You can already spend a lot of time playing around.

### Hardware
- Hardware works properly and PCBs are available -> sale will start mid january
- Prototyped case with 3D printed buttons and laser-cut frontplate is finished

### Base functionality
  - startup sequence with SD-Card check
  - loading projects
  - creating projects
  - File structure per project on SD-Card (card can be loaded on a PC to edit/change samples)
  - loading samples to PSRAM (EXTMEM) for faster and polyphonic playback (at least 10 samples at once)
  - GUI system
  - Waveform buffering for ad hoc drawing of samples to the screen
  - Mapping note keys to a keyboard to enter names, etc.
  
### Sampler
  - record from microphone
  - record from line-in
  - resampling
  - resampling with microphone or line-in
  - trim sample with encoders and/or fader
  - save samples to sample slot (stored on SD-card)
  - adjust sample volume
  
### Sequencer
  - Samples can be set on a 8 x (2..256) grid, one track is not limited to one sampler
  - chromatic pitching
  - volume adjustment per note (changeable while sample is playing)
  - panning per note (changeable while sample is playing)
  - stop note/sample at any grid position
  - probability for each note/sample/position on the grid
  - variable pattern length
  - automatic pattern resolution scaling
  - playback speed with 0.5 BPM resolution

## Next steps
Currently I am creating almost everything by myself. So the next steps might take a while, hopefully not too long.

### Off device & Hardware
- Publish circuit diagram
- Create DIY tutorial with pictures for soldering parts
- Publish files to print your own case
- Create user manual/video tutorial
- Improve the website

### On device / Firmware
- Option to give samples a name instead of the standard numbers (1..72)
- Set a base note for a sample e.g. C4 to map chromatic pitching to note keys
- Optimize menu structure and create a main menu
- Sample library (Filesystem) integration to use samples on multiple projects
- Save/Load pattern to/from SD-card
- Memory management to store multiple pattern in memory
- Implementation Song mode (chaining multiple pattern to a song)
- Move settings from code to a configuration menu within the device
- More features for the sampler (fade in/out)

## Roadmap
- Effects channels
- USB Audio
- File system integration to a computer via USB to load/save samples from/to device
- Add MIDI to the sequencer to control external sound devices
- Live mode with MIDI Integration
- Synthesizer
