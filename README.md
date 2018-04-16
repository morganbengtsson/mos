![Documentation build status](https://readthedocs.org/projects/mos/badge/?version=latest "Documentation build status")

MOS is a lightweight utility library for cross platform game development. 

It includes components for rendering, text rendering, colission detection, audio playback and asset management.

OpenGL is used for graphics and OpenAL are used for audio. Together
with smaller libraries for ogg decoding, png loading and math.

To add it to a project. Preferably use it as a git submodule:

```bash
git submodule add https://github.com/morganbengtsson/mos.git externals --init --recursive
```

To see how a simple project setup looks like. The following repository can be cloned:


```bash
git clone --recurse-submodules https://github.com/morganbengtsson/mos-skeleton
```
