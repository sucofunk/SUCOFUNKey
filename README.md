# SUCOFUNKey - the firmware for beatmaker's sketchbook

[Teensy](https://www.pjrc.com/store/teensy41.html) based Open Source Sample & Sequencer. 

Beatmaker's sketchbook is an open source alternative to different commercial electronic music devices like the [MPC](https://en.wikipedia.org/wiki/Akai_MPC) or [OP-1](https://en.wikipedia.org/wiki/Teenage_Engineering_OP-1).

The goal is to enable everyone interested in the topic to easily build and customize its own device without spending a lot of money.
As sampling is an essential part of the Hip-Hop culture, SUCOFUNKey might be an option for amateur beat makers. It is battery powered, includes a stereo speaker and can be used everywhere.

The PCB is designed without any SMD components and can be soldered with a standard soldering iron from a local hardware store. Difficult to solder SMD components are pluggable.

Check out [@sucofunkey on Instagram](https://www.instagram.com/sucofunkey/) for a visual impression of the device and progress.
You can sign up to the newsletter on [www.sucofunk.com](https://sucofunk.com), check some background infos, contact me and stay up to date.

To see the implemented features, read the [user manual](doc/manual.md).

If you are interested in participating, just send me a message. I spend a lot of time into the project without any return yet. If you want to support the project and help to keep the costs (office/workshop rent, hosting, etc.) as low as possible and speed the project up, I would be happy when you donate via Paypal. If you spend more than 25€, your name will be listed on the supporter screen in the device. Leave our name or nickname in the comment field of the donation process. [Donate](https://www.paypal.com/donate/?hosted_button_id=H9K23YHV95UXC)


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

SUCOFUNKey is work in progress. Here is a short overview of what is already implemented and the next steps, including a roadmap of the project.
The main Features - Sampling & Sequencing - are working properly.
Read the [user manual](doc/manual.md) for details.

### Hardware
- is ready and can be purchased at [www.sucofunk.com](https://sucofunk.com)

### Base functionality
  - startup sequence with SD-Card check
  - loading projects
  - creating projects
  - File structure per project on SD-Card (card can be loaded on a PC to edit/change samples)
  - Global sample library on SD card
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
  - Samples can be set on a 8 x (2..256) grid, one track is not limited to one sample
  - chromatic pitching
  - volume adjustment per note (changeable while sample is playing)
  - panning per note (changeable while sample is playing)
  - stop note/sample at any grid position
  - probability for each note/sample/position on the grid
  - variable pattern length
  - automatic pattern resolution scaling
  - playback speed with 0.5 BPM resolution
  - Triggering external MIDI gear with channel, note, velocity and NOTE OFF

### MIDI integration
  - Samples can be played at every time
  - Up to 8 samples at the same time with polyphonic aftertouch

## Next steps
Currently I am creating almost everything by myself. So the next steps might take a while, hopefully not too long.

### Off device & Hardware
- <del>Publish circuit diagram</del>
- <del>Create DIY tutorial with pictures for soldering parts</del>
- <del>Publish files to print your own case<del>
- Create user manual/video tutorial
- <del>Improve the website</del>

### On device / Firmware
- Option to give samples a name instead of the standard numbers (1..72)
- Set a base note for a sample e.g. C4 to map chromatic pitching to note keys
- <del>Optimize menu structure and create a main menu</del>
- <del>Sample library (Filesystem) integration to use samples on multiple projects</del>
- <del>Save/Load pattern to/from SD-card</del>
- <del>Memory management to store multiple pattern in memory</del>
- <del>Implementation Song mode (chaining multiple pattern to a song)</del>
- Move settings from code to a configuration menu within the device
- More features for the sampler (fade in/out)
- add feature to select and move blocks in the sequencer
- add snippets to the sequencer
- add another zoom level to the sequencer

## Roadmap
- Effects channels
- USB Audio
- File system integration to a computer via USB to load/save samples from/to device
- <del>Add MIDI to the sequencer to control external sound devices<del>
- Live mode with <del>MIDI Integration</del> and snippets
- Synthesizer
