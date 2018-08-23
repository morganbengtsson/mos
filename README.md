![Screenshot](https://raw.githubusercontent.com/morganbengtsson/mos-skeleton/master/screenshot.png)

[![Documentation status](https://readthedocs.org/projects/mos/badge/?version=latest)](http://mos.readthedocs.io/en/latest/)
[![GitHub license](https://img.shields.io/github/license/morganbengtsson/mos.svg)](https://github.com/morganbengtsson/mos/blob/master/LICENCE.rst)

# Design

MOS is a lightweight cross platform game engine/library, written in modern C++. Some of the included functionality is:

- 3D mesh rendering
- Physically based material model
- Global illumination
- Particle rendering
- Text rendering
- Collision detection
- 3D audio playback, with obstruction model
- Asset management

# Companion projects

- [MOS Skeleton](https://github.com/morganbengtsson/mos-skeleton): An example project to quickly get up and running.
- [MOS Blender export](https://github.com/morganbengtsson/mos-blender-export): Export scripts/addon for Blender, to export
models, levels and other entities, usable with MOS.

# Source code

A convenient way to get and use the project, is to add it as a [git submodule](https://git-scm.com/docs/git-submodule):

```bash
git submodule add https://github.com/morganbengtsson/mos.git
git submodule update --init --recursive
```
# Building

The project is preferably used as a sub project with [CMake](https://cmake.org) as so, in your CMakeLists.txt:

```CMake
add_subdirectory(externals/mos)
target_link_libraries(${PROJECT_NAME} mos)
target_include_directories(${PROJECT_NAME} PUBLIC externals/mos/include)

```

A CMake based [project skeleton](https://github.com/morganbengtsson/mos-skeleton) is also available, for a minimal setup
with working graphics and audio:
