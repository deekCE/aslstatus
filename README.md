# aslstatus - asynchronous suckless status
If you found any issues report it at
[dm9pzCAq's aslstatus](https://notabug.org/dm9pZCAq/aslstatus).

For any other question, feel free to ask at
[#aslstatus:anontier.nl](https://matrix.to/#/#aslstatus:anontier.nl)

## Demo
![img](https://i.ibb.co/3SRSkTP/lemonbar-with-aslstatus.gif)

<details><summary>dwm</summary>
![dwm demo](https://i.ibb.co/r5PYRj8/Demo.gif)
</details>

<details><summary>dvtm</summary>
[![dvtm asciicast](https://asciinema.org/a/437217.svg)](https://asciinema.org/a/437217)
</details>

## Installation
### Gentoo
Available at [dm9pZCAq's overlay](https://notabug.org/dm9pZCAq/dm9pZCAq-overlay):
```sh
eselect repository enable dm9pZCAq
emerge --sync dm9pZCAq
emerge app-admin/aslstatus::dm9pZCAq
```

If you wanna pack `aslstatus` for your distro, please do so and submit a pull
request at [dm9pzCAq's aslstatus](https://notabug.org/dm9pZCAq/aslstatus) to
add it here.

## Compile
It succesfully compiles with:
* `clang`
* `tcc`
* `gcc`

Needs `pkg-config` for `LDLIBS`

You can find more about it in [config.mk](config.mk)

### make options:
#### X:
Adds support for `X` (needed for `dwm` `WM_NAME`).

You may want to set it to `0` if you use aslstatus in
[dvtm](https://www.brain-dump.org/projects/dvtm/).

* Default: `1`
* Dependencies:
  * `libxcb-dev`

#### XKB:
Adds support for keyboards (needed for `keymap` component).

Works only if `X=1`.

* Default: `1`
* Dependencies:
  * `libxcb-xkb-dev`

#### AUDIO:
Selects the audio library.

* Default: `ALSA`
* Possible values:
  * `ALSA`
  * `PULSE`
  * `''` (empty) to use `sys/soundcard.h` or `sys/audioio.h` on OpenBSD
* Dependencies:
  * `libasound-dev` (if `AUDIO=ALSA`)
  * `libpulse-dev` (if `AUDIO=PULSE`)

## Configuration
More information about components in the [Wiki](wiki/COMPONENTS.md) or in
[components_config.h](components_config.h)
