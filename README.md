# SUCOFUNKey - firmware for beatmaker's sketchbook

[Teensy](https://www.pjrc.com/store/teensy41.html) based Open Source Sampler & Sequencer. 

Beatmaker's sketchbook is an open source alternative to different commercial electronic music devices like the [MPC](https://en.wikipedia.org/wiki/Akai_MPC) or [OP-1](https://en.wikipedia.org/wiki/Teenage_Engineering_OP-1).

The goal is to enable everyone interested in the topic to easily build and customize its own device without spending a lot of money.
As sampling is an essential part of the Hip-Hop culture, SUCOFUNKey might be an option for amateur beat makers. It is battery powered and can be used everywhere.

Check out [@sucofunkey on Instagram](https://www.instagram.com/sucofunkey/) for a visual impression of the device and progress.
You can sign up to the newsletter on [www.sucofunk.com](https://sucofunk.com), check some background infos, contact me and stay up to date.

To see the implemented features, read the [user manual](doc/manual.md).

If you are interested in participating, just send me a message. I spend a lot of time into the project without any return yet. If you want to support the project and help to keep the costs (office/workshop rent, hosting, etc.) as low as possible and speed the project up, I would be happy when you donate via Paypal. If you spend more than 25€, your name will be listed on the supporter screen in the device. Leave your name or nickname in the comment field of the donation process. [Donate](https://www.paypal.com/donate/?hosted_button_id=H9K23YHV95UXC)

## Firmware
The firmware is a VSCode project, using the [PlatformIO](https://platformio.org/) extension.

## Status

SUCOFUNKey is work in progress. The firmware functionality is more complex than initially planned and a fully functional DAW for your pocket.
It includes a *sampler*, a *sequencer* (samples and midi) with an approach that is a bit different to what you might know from other tools, an *arranger* to chain patterns from the sequencer to a complete song and a *play* mode for live performances.

Read the [user manual](doc/manual.md) for details.


## Next steps
Currently I am creating almost everything by myself. So the next steps might take a while, hopefully not too long.


### Sampler
- make samplenames editable after they are set once
- set a base note for a sample e.g. C4 to map chromatic pitching to note keys
- ADSR envelope
- bitcrusher

### Sequencer
- add feature to <del>select and</del> move blocks
- copy and paste selections in the sequencer to re-use them

### Play Mode
  - chain snippets
  - option to use beatmaker's sketchbook as a midi keyboard
  - Send MIDI Chords
  - Send MIDI Program Change
  - optimize session recording

### Miscellaneous
- move settings from code to a configuration menu within the device
- create a better manual / feature overview
  
## Roadmap / Vision / Nice-to-have
- Effects
- Timestretching samples
- USB Audio
- Web-application to copy samples and edit/export sketches


## Hardware
Beatmaker's sketchbook houses:
- [Teensy 4.1](https://www.pjrc.com/store/teensy41.html) microcontroller
- 24 Keys to map a two octave keyboard
- 12 functional keys (play, stop, record, ...)
- 4 endless rotary encoders
- 10mm fader
- [2 inch LCD display](https://www.waveshare.com/product/displays/lcd-oled/lcd-oled-3/2inch-lcd-module.htm)
- 32 LEDs
- built-in microphone 
- headphone output
- volume control
- 3.5mm line-in and line-out
- MIDI-in and MIDI-out

Optional:
- built-in LiPo battery with [charger](https://www.adafruit.com/product/1944)
- built in stereo speakers, driven by a small [amplifier](https://www.adafruit.com/product/987)