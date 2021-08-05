# SUCOFUNKey
[Teensy](https://www.pjrc.com/store/teensy41.html) based Open Source Sample Sequencer

SUCOFUNKey is an open source alternative to different commercial electronic music devices like the [MPC](https://en.wikipedia.org/wiki/Akai_MPC) or [OP-1](https://en.wikipedia.org/wiki/Teenage_Engineering_OP-1).

The goal is to enable everyone interested in the topic to easily build and customize its own device without spending a lot of money.
As sampling is an essential part of the Hip-Hop culture, SUCOFUNKey might be an option for amateur beat makers. It is battery powered, includes a stereo speaker and can be used everywhere.

The PCB is designed without any SMD components and can be soldered with a standard soldering iron from a local hardware store. Difficult to solder SMD components are pluggable.

Check out [@sucofunkey on Instagram](https://www.instagram.com/sucofunkey/) for a visual impression of the device and progress.


## Hardware
SUCOFUNKey houses:
- [Teensy 4.1](https://www.pjrc.com/store/teensy41.html) microcontroller
- 24 Keys to map a two octave keyboard
- 12 functional keys (play, stop, record, ...)
- 4 endless rotary encoders
- 10mm fader
- 2 inch LCD display
- 32 LEDs
- built in microphone 
- built in stereo speakers, driven by a small [amplifier](https://www.adafruit.com/product/987)
- headphone output
- volume control
- 3.5mm line-in and line-out
- MIDI-in and MIDI-out
- built-in LiPo battery with [charger](https://www.adafruit.com/product/1944)

The circuit diagram will be published soon.


## Firmware
The firmware is a VSCode project, using the [PlatformIO](https://platformio.org/) extension.

## Status
This project is work in progress and still far from a complete solution.
- Hardware works properly
- Case still in progress
- Basic sampler firmware is ready to use
- Sequencer firmware is in a proof of concept phase and will completely change
- Synthesizer firmware is not implemented yet
- Documentation and instructions on how to build will follow

If you are interested in participating, a PCB or in the project in general, just send me a message.
