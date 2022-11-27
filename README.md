# aslstatus - asynchronous suckless status
If you found any issues report it
[here] (https://notabug.org/dm9pZCAq/aslstatus)

For all other questions, feel free to ask in Matrix
[#aslstatus:anontier.nl](https://matrix.to/#/#aslstatus:anontier.nl)

### Demo

<details><summary>bspwm & lemonbar</summary>
![bspwm demo](https://i.ibb.co/3SRSkTP/lemonbar-with-aslstatus.gif)
</details>

<details><summary>dwm</summary>
![dwm demo](https://i.ibb.co/r5PYRj8/Demo.gif)
</details>

<details><summary>dvtm</summary>
[![dvtm asciicast](https://asciinema.org/a/437217.svg)](https://asciinema.org/a/437217)
</details>

# Installation
### Gentoo
Avaliable in [dm9pZCAq's overlay](https://notabug.org/dm9pZCAq/dm9pZCAq-overlay):
```sh
eselect repository enable dm9pZCAq
emerge --sync dm9pZCAq
emerge app-admin/aslstatus::dm9pZCAq
```

If you wanna pack `aslstatus` for your distro, please do so and submit PR to
add it here

# Compile

It succesfully compiles with:
* `clang`
* `tcc`
* `gcc`

Needs `pkg-config` for `LDLIBS`

You can find out more in [config.mk](config.mk)

## make options
#### X
add support for `X` (needed for `dwm` `WM_NAME`)

you may want to set it to `0` if you use aslstatus in [dvtm](https://www.brain-dump.org/projects/dvtm/)

* default: `1`
* dependencies:
  + `libxcb-dev`

#### XKB
add support for keyboard (needed for `keymap` component)

works only if `X=1`

* default: `1`
* dependencies:
  + `libxcb-xkb-dev`

#### AUDIO
select audio library

* default: `ALSA`
* possible values:
  - `ALSA`
  - `PULSE`
  - `''` (empty) to use `sys/soundcard.h` or `sys/audioio.h` on OpenBSD
* dependencies:
  + `libasound-dev` (if `AUDIO=ALSA`)
  + `libpulse-dev` (if `AUDIO=PULSE`)


---
# config

for more info about components see [wiki](wiki/COMPONENTS.md) and [components_config.h](components_config.h)

---
see also [main wiki page](wiki/), you can find some useful stuff here
