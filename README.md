Afterburner
===========

*(Working title)*

Open-source reboot of [James Bond 007: Nightfire](https://en.wikipedia.org/wiki/James_Bond_007:_Nightfire). The primary purpose of this mod is to recreate and improve the experience of the PC version of Nightfire, and to provide in the process an open-source codebase that will allow for extension and refinement. The Xash3D (GoldSrc) engine was chosen because its codebase is fully available, it's smaller and simpler than Source, it's closer to the genuine Nightfire engine, and it doesn't come with the expectation that games will be photorealistic (I don't have the time or the energy as a single hobbyist to be working on some sort of HD photorealistic remake).

## I want to play, where are the downloads?

See [here](https://github.com/x6herbius/afterburner-game/releases). Please note, however, that at time of writing only pre-release versions of the game are available for testing purposes. These are liable to be buggy and will not be representative of the final game.

## I want to compile the game from source, how do I do that?

### Prerequisites

*64-bit builds of the game will be supported in future, but are not at the moment.* Until 64-bit support is properly implemented, 32-bit compilers and libraries are required.

Ensure that you have Python 3 installed on your computer, as well as the appropriate C/C++ development tools for your platform (Windows and Linux are supported). You will also need to ensure that the **32-bit development** version of the [SDL2 library](https://www.libsdl.org/download-2.0.php) is installed. If this is not available for download for your platform, you should compile it from source.

For detailed information on the prerequisites for your platform, see the Xash3D FWGS readme [here](https://github.com/FWGS/xash3d-fwgs), or at the bottom of this file. Note that some of the 32-bit dependencies on Linux may also need to have their 64-bit counterparts installed in order for the configuration step to correctly detect them; for example, on Ubuntu the `libfreetype-dev` and `libfontconfig1-dev` dependencies need to be installed alongside their `libfreetype-dev:i386` and `libfontconfig1-dev:i386` counterparts.

Finally, clone this repository using `git clone --recursive`. If you don't use the `--recursive` switch, the game **will not** compile without manually initialising the subrepositories. If you find yourself needing to do this, run `git submodule update --init --recursive` from within the root directory of the repository.

### Building

To build, open a terminal in the root of the repository and call:

```
python3 ./waf configure --disable-vgui --win-style-install --build-type=debug --prefix=build/debug
```

This will set up the build for a debug version of the game. Note that if you downloaded SDL2 from the website and it is not available in the system path, you will need to pass the path to the directory you extracted it to via the `--sdl2` option, eg. `--sdl2=/path/to/SDL2`.

To build the game and copy all output to the `build/debug` folder in the root of the repo, call:

```
python3 ./waf build install
```

For more information, or a list of all available options, run `python3 ./waf --help`. Note that most of these will be for advanced users only.

## Why is Mac not supported any more?

[Apple have deprecated OpenGL support on Mac](https://www.anandtech.com/show/12894/apple-deprecates-opengl-across-all-oses). I'm not gonna lie - this is incredibly irritating, because OpenGL was the easiest way to write graphics-based applications that worked across the three big operating systems. In response, the developers of the Xash3D engine used by Afterburner have decided [not to support the engine on Mac](https://github.com/FWGS/xash3d-fwgs/issues/61), because to do so would now require too much extra work and testing.

If you wish to try compiling the game for Mac then feel free - in theory the engine should be compatible as long as you have a version of MacOS that supports OpenGL. However, I won't be able to help you out, so you'll be on your own.

## I want to contribute!

Since this project is currently only in active development by myself, I don't have any set roadmap or procedure for contributing. If you would like to get involved, please drop me a line (jonathan.poncelet@talk21.com, or [@x6herbius](https://twitter.com/x6herbius) on Twitter) and we can discuss!

## Credits

Many thanks to:

* Gearbox for the original game.
* The FWGS team for the [Xash3D engine](https://github.com/FWGS/xash3d-fwgs) and [Xash3D version of Half Life SDK](https://github.com/FWGS/hlsdk-xash3d).
* j-yaniv and others for [Rho-Bot Half Life bots](https://sourceforge.net/projects/rhobot/).
* Developers of [STB libraries](https://github.com/nothings/stb).
* Developers of [rapidjson library](http://rapidjson.org/).
* Contributors to [XeNTaX game reverse-engineering forums](https://forum.xentax.com/).
* ZapSplat.com for various free sound effects.
* Various Nightfire modders over the years whose tools and reverse-engineering have proven invaluable when developing this mod (specifically Tres, 009, Ford, UltimateSniper).

For posterity, the Xash3D engine readme is below.

-----

## Xash3D FWGS Engine Readme
[![Build Status](https://api.travis-ci.org/FWGS/xash3d-fwgs.svg?branch=master)](https://travis-ci.org/FWGS/xash3d-fwgs) [![Discord Server](https://img.shields.io/discord/355697768582610945.svg)](https://discord.gg/TbnHcVb)
=======
# Xash3D FWGS Engine
[![Build Status](https://api.travis-ci.org/FWGS/xash3d-fwgs.svg?branch=master)](https://travis-ci.org/FWGS/xash3d-fwgs) [![FreeBSD Build Status](https://img.shields.io/cirrus/github/FWGS/xash3d-fwgs?label=freebsd%20build)](https://cirrus-ci.com/github/FWGS/xash3d-fwgs) [![Discord Server](https://img.shields.io/discord/355697768582610945.svg)](https://discord.gg/TbnHcVb) \
[![Download Stable](https://img.shields.io/badge/download-stable-yellow)](https://github.com/FWGS/xash3d-fwgs/releases/latest) [![Download Testing](https://img.shields.io/badge/downloads-testing-orange)](https://github.com/FWGS/xash3d-deploy/tree/anewengine-master)

Xash3D FWGS is a fork of Xash3D Engine by Unkle Mike with extended features and crossplatform.

```
Xash3D is a game engine, aimed to provide compatibility with Half-Life Engine,
as well as to give game developers well known workflow and extend it.
Read more about Xash3D on ModDB: https://www.moddb.com/engines/xash3d-engine
```

## Fork features
* HLSDK 2.4 support.
* Crossplatform: supported x86 and ARM on Windows/Linux/BSD/Android. ([see docs for more info](Documentation/ports.md))
* Modern compilers support: say no more to MSVC6.
* Better multiplayer support: multiple master servers, headless dedicated server.
* Mobility API: allows better game integration on mobile devices(vibration, touch controls)
* Different input methods: touch, gamepad and classic mouse & keyboard.
* TrueType font rendering, as a part of mainui_cpp.
* Multiple renderers support: OpenGL, GLESv1, GLESv2, Software
* A set of small improvements, without broken compatibility.

## Planned fork features
* Virtual Reality support and game API
* Voice support
* Vulkan renderer

## Contributing
* Before sending an issue, check if someone already reported your issue. Make sure you're following "How To Ask Questions The Smart Way" guide by Eric Steven Raymond. Read more: http://www.catb.org/~esr/faqs/smart-questions.html
* Before sending a PR, check if you followed our contribution guide in CONTRIBUTING.md file.

## Build instructions
We are using Waf build system. If you have some Waf-related questions, I recommend you to read https://waf.io/book/

If you're stuck somewhere and you need a clear example, read `.travis.yml` and `scripts/build*.sh`.

NOTE: NEVER USE GitHub's ZIP ARCHIVES. They are broken and don't contain external dependencies sources we're using.

### Prerequisites
#### Windows(Visual Studio)
* Install Visual Studio.
* Install latest [Python](https://python.org) **OR** run `cinst python.install` if you have Chocolatey.
* Install latest [Git](https://git-scm.com/download/win) **OR** run `cinst git.install` if you have Chocolatey.
* Download [SDL2](https://libsdl.org/download-2.0.php) development package for Visual Studio.
* Clone this repository: `git clone --recursive https://github.com/FWGS/xash3d-fwgs`.
* Make sure you have at least 12GB of free space to store all build-time dependencies: ~10GB for Visual Studio, 300 MB for Git, 100 MB for Python and other.

#### GNU/Linux
NOTE FOR USERS WITH X86 COMPATIBLE CPUs:
We have forced build system to throw an error, if you're trying to build 64-bit engine. This done for keeping compatibility with Steam releases of Half-Life and based on it's engine games.
Even if Xash3D FWGS does support targetting 64-bit, you can't load games without recompiling them from source code!

##### Debian/Ubuntu
* Enable i386 on your system, if you're compiling 32-bit engine on amd64. If not, skip this

`$ sudo dpkg --add-architecture i386`
* Install development tools
  * For 32-bit engine on amd64: \
    `$ sudo apt install build-essential gcc-multilib g++-multilib python libsdl2-dev:i386 libfontconfig-dev:i386 libfreetype6-dev:i386`
  * For everything else: \
    `$ sudo apt install build-essential python libsdl2-dev libfontconfig-dev libfreetype6-dev`
* Clone this repostory:
`$ git clone --recursive https://github.com/FWGS/xash3d-fwgs`

#### ArchLinux
* ArchLinux users are lucky enough and there is a `xash3d-git` AUR package for them.

### Building
#### Windows(Visual Studio)
0) Open command line
1) Navigate to `xash3d-fwgs` directory.
2) Carefully examine which build options are available: `waf --help`
3) Configure build: `waf configure -T release --sdl2=c:/path/to/SDL2 --prefix=c:/path/to/any/output/directory`
4) Compile: `waf build`
5) Install: `waf install`

#### Linux
0) Examine which build options are available: `./waf --help`
1) Configure build: `./waf configure -T release --prefix=/path/to/any/output/directory`
(To compile 64-bit engine on 64-bit x86 processor, you need to pass `-8` also)
2) Compile: `./waf build`
3) Install(optional): `./waf install`

## Running
0) Copy libraries and main executable somewhere, if you're skipped installation stage.
1) Copy game files to same directory
2) Run `xash3d.exe`/`xash3d.sh`/`xash3d` depending on which platform you're using.

For additional info, run Xash3D with `-help` command line key.
