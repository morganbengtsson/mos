![Screenshot](https://github.com/morganbengtsson/mos-skeleton/blob/master/screenshot.png)

[![Documentation status](https://readthedocs.org/projects/mos/badge/?version=latest)](http://mos.readthedocs.io/en/latest/)

MOS is a lightweight cross platform game engine/library, written in modern C++. It includes components for 3D rendering
with a physically based material model, text rendering, collision detection, audio playback and asset management.

# The source code

A convenient way to get and use the project, is to add it as a [git submodule](https://git-scm.com/docs/git-submodule):

```bash
git submodule add https://github.com/morganbengtsson/mos.git
git submodule update --init --recursive
```
# Building

The project is preferably used as a sub project with [CMake](https://cmake.org) as so, in your CMakeLists.txt:

```CMake
add_subdirectory(externals/mos)
include_directories(externals/mos/include)
target_link_libraries(${PROJECT_NAME} mos)
```

A CMake based [project skeleton](https://github.com/morganbengtsson/mos-skeleton) is also available, for a minimal setup with working graphics and audio:

```bash
git clone --recurse-submodules https://github.com/morganbengtsson/mos-skeleton
```
The skeleton provides a minimal setup, for getting a window with some graphics and sound up and running.

# Blender export scripts

The engine uses a custom model/mesh format. Hence a [companion project](https://github.com/morganbengtsson/mos-blender-export),
exists to be able to export models and levels from [Blender](http://blender.org).