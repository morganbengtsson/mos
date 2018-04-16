[![Documentation status](https://readthedocs.org/projects/mos/badge/?version=latest)](http://mos.readthedocs.io/en/latest/)

MOS is a lightweight cross platform game engine/library, written in modern C++. It includes components for 3D physically based rendering,
text rendering, collision detection, audio playback and asset management.

# Getting the source

A good way to get started is by cloning a CMake based project skeleton:

```bash
git clone --recurse-submodules https://github.com/morganbengtsson/mos-skeleton
```
The skeleton provides a minimal setup, for getting a window with some graphics and sound up and running.

To use it in an existing project, it can also be added as a git submodule:

```bash
git submodule add https://github.com/morganbengtsson/mos.git
git submodule update --init --recursive
```
# Example

# Blender export scripts

The engine uses a custom model/mesh format. Hence a [companion project](https://github.com/morganbengtsson/mos-blender-export),
exists to be able to export models and levels from Blender.