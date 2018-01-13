# Ogrian Carpet

 Fly around an island casting spells, summon monsters, collect mana, and build castles.

 An outdoor first person shooter game with real-time strategy elements inspired by the game 'Magic Carpet.'

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

What things you need to install the software and how to install them

* RakNet 2.013 - http://www.rakkarsoft.com/
* fMod 3.74linux - http://www.fmod.org/
* OGRE 0.15 - http://www.ogre3d.org/
* CEGUI 0.2.1 - http://cegui.org.uk/

### Installing

Basic Steps to run the game (until packaging is finished)

  0. Install the Prerequisites
  1. Move Plugins.cfg -> plugins.cfg to provide the OpenGL default render method because Linux filesystems are case-sensitive.
  2. Copy fmod libs to proper location since the fmod `make install` leaves it in the build location.
  3. Run from a text-session (e.g. xterm, gnome-terminal, kterm) due to the Ogre-0.15 start menu.

## Running the tests

This will explain how to run the automated tests for this system once tests are added

## Deployment

Dependencies are as bellow.

### RakNet 2.013
*  networking: RakNet
* http://www.rakkarsoft.com/

Needs the install to be manually completed on Linux.

```bash
  /usr/lib/pkgconfig/raknet.pc
  ln -s /usr/lib/librakvoice.so.2.0.13 /usr/lib/librakvoice.so
  ln -s /usr/lib/libraknet.so.2.0.13 /usr/lib/libraknet.so
```

### fMod 3.74linux
 * audio: FMOD
 * http://www.fmod.org/

This library needs to be in `$LD_LIBRARY_PATH`.

```bash
 $PREFIX/lib/libfmod-3.74.so
```

### OGRE 0.15
* rendering: Ogre3d
* http://www.ogre3d.org/

```bash
/usr/local/pkgconfig/OGRE.pc
mkdir -p /usr/local/lib/OGRE
while read plugin; do cp "${plugin}" /usr/local/lib/OGRE/;done <EOPLUGS
  Plugin_BSPSceneManager.so
  Plugin_CgProgramManager.so
  Plugin_FileSystem.so
  Plugin_GuiElements.so
  Plugin_NatureSceneManager.so
  Plugin_OctreeSceneManager.so
  Plugin_ParticleFX.so
  RenderSystem_GL.so
EOPLUGS
```

### CEGUI 0.2.1

This is installed a a local library in the project (for now).

## Built With

* [Ogre3d](http://www.ogre3d.org/) - Ogre 3D Rendering Engine
* [fMOD](http://www.fmod.org/) - fMOD Audio
* [RakNet](http://www.rakkarsoft.com/) - RakNet Networking
* [Crazy Eddies GUI](http://cegui.org.uk/) - 2D Layout Engine

### Full Build information

Example is as follows:
```
 ldd ogrian-9.1_anonymous@buildhost_SuSE9.3-Linux-i686
	linux-gate.so.1 =>  (0xffffe000)
	libOgreMain.so.4 => /usr/local/lib/libOgreMain.so.4 (0x40019000)
	/usr/lib/libraknet.so (0x40293000)
	/opt/fmodapi374linux/api/libfmod-3.74.so (0x402d7000)
	libgcc_s.so.1 => /lib/libgcc_s.so.1 (0x40396000)
	libc.so.6 => /lib/tls/libc.so.6 (0x4039e000)
	libstdc++.so.5 => /usr/X11R6/lib/tls/i686/libstdc++.so.5 (0x404b8000)
	libm.so.6 => /lib/tls/libm.so.6 (0x40576000)
	libfreetype.so.6 => /usr/X11R6/lib/tls/i686/libfreetype.so.6 (0x40599000)
	libCg.so => /usr/X11R6/lib/tls/i686/libCg.so (0x40608000)
	libILU.so.1 => /usr/local/lib/libILU.so.1 (0x40824000)
	libIL.so.1 => /usr/local/lib/libIL.so.1 (0x40839000)
	libz.so.1 => /usr/X11R6/lib/tls/i686/libz.so.1 (0x4093a000)
	libdl.so.2 => /lib/libdl.so.2 (0x4094b000)
	libSDL-1.2.so.0 => /usr/X11R6/lib/tls/i686/libSDL-1.2.so.0 (0x4094f000)
	libpthread.so.0 => /lib/tls/libpthread.so.0 (0x409e1000)
	/lib/ld-linux.so.2 (0x40000000)
	libpng.so.3 => /usr/X11R6/lib/tls/i686/libpng.so.3 (0x409f3000)
	libjpeg.so.62 => /usr/X11R6/lib/tls/i686/libjpeg.so.62 (0x40a22000)
	libtiff.so.3 => /usr/X11R6/lib/tls/i686/libtiff.so.3 (0x40a41000)
	libmng.so.1 => /usr/X11R6/lib/tls/i686/libmng.so.1 (0x40a91000)
	libX11.so.6 => /usr/X11R6/lib/libX11.so.6 (0x40af6000)
	libXext.so.6 => /usr/X11R6/lib/libXext.so.6 (0x40bf2000)
	libaa.so.1 => /usr/X11R6/lib/tls/i686/libaa.so.1 (0x40c01000)
	liblcms.so.1 => /usr/X11R6/lib/tls/i686/liblcms.so.1 (0x40c1b000)
	libslang-utf8.so.1 => /usr/X11R6/lib/tls/i686/libslang-utf8.so.1 (0x40c49000)
	libgpm.so.1 => /usr/X11R6/lib/tls/i686/libgpm.so.1 (0x40cbb000)
	libncurses.so.5 => /lib/libncurses.so.5 (0x40cc1000)
```

## Contributing

Please read [CONTRIBUTING.md](https://github.com/waveclaw/ogrian-carpet/CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

We use [SemVer](http://semver.org/) for version numbering. For the versions available, see the [tags on this repository](https://github.com/waveclaw/ogrian-carpet/tags).

## Authors

* See [AUTHORS](https://github.com/waveclaw/ogrian-carpet/AUTHORS)

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* [README.md](https://gist.github.com/PurpleBooth/109311bb0361f32d87a2) GitHub Template by [Billie Thompson](https://github.com/PurpleBooth)
* Based on [Arabian Flights](https://sourceforge.net/projects/arabian/) that was based on the game Magic Carpet
