[![Documentation status](https://readthedocs.org/projects/mos/badge/?version=latest)](http://mos.readthedocs.io/en/latest/)

MOS is a lightweight cross platform game engine/library, written in C++. It includes components for physically based rendering,
text rendering, collision detection, audio playback and asset management.

A good way to get started is by cloning a CMake based project skeleton:

```bash
git clone --recurse-submodules https://github.com/morganbengtsson/mos-skeleton
```
The skeleton provides a minimal setup, for getting a window with some graphics and sound up and running.

To use it in an existing project, the preferred way is to add it as a git submodule:

```bash
git submodule add https://github.com/morganbengtsson/mos.git externals --init --recursive
```

The preferable way to export models and levels is to use Blender, with the [following export scripts](https://github.com/morganbengtsson/mos-blender-export).