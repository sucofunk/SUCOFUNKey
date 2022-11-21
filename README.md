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

SUCOFUNKey is work in progress. The main Features - Sampling & Sequencing - are working properly. Even the play mode is implemented.
Read the [user manual](doc/manual.md) for details.

### Hardware
- is ready and DIY kits can be purchased at [www.sucofunk.com](https://sucofunk.com)
- transfering PCB design to SMD to have the option for producing a larger amount of boards, if needed.

## Next steps
Currently I am creating almost everything by myself. So the next steps might take a while, hopefully not too long.

### Off device & Hardware
- <del>Create</del> Optimize user manual/video tutorial
- Fix a tiny bug in the schematics and change LED resistor values
- Find and decide on a license for hardware publishing
- Publish the gerber files and/or KiCAD Project
- <del>Publish circuit diagram</del>
- <del>Create DIY tutorial with pictures for soldering parts</del>
- <del>Publish files to print your own case<del>
- <del>Improve the website</del>

### On device / Firmware
- Implement Arrange Mode
- Option to change Songs from main menu
- Option to give samples a name instead of the standard numbers (1..72)
- Set a base note for a sample e.g. C4 to map chromatic pitching to note keys
- Move settings from code to a configuration menu within the device
- More features for the sampler (fade in/out, bitcrusher)
- add feature to <del>select and</del> move blocks in the sequencer
- fix bugs
- <del>Patterns in Sequencer</del>  
- <del>Optimize menu structure and create a main menu</del>
- <del>Sample library (Filesystem) integration to use samples on multiple projects</del>
- <del>Save/Load pattern to/from SD-card</del>
- <del>Memory management to store multiple pattern in memory</del>
- <del>Implementation Song mode (chaining multiple pattern to a song)</del>
- <del>add snippets to the sequencer</del>
- <del>add another zoom level to the sequencer</del>

## Roadmap
- Timestretching samples
- Effects channels
- USB Audio
- File system integration to a computer via USB to load/save samples from/to device
- Synthesizer (maybe.. if there is enough RAM left)
- <del>Add MIDI to the sequencer to control external sound devices<del>
- <del>Live mode with <del>MIDI Integration</del> and snippets</del>

