========
Tutorial
========

Assets
------

Rendering
---------

The :cpp:class:`mos::Renderer` is the class that takes care of all the rendering, and
what is shown on the screen. The renderer is simply created by constructing
it somwhere as follows. It should probably be stored somewhere, so its lifetime
is as long as the application:

 .. code-block:: c++
	
	auto renderer = mos::Renderer();

That is it. The renderer is initialized, hopefully with a valid OpenGL context.
This ofcourse makes it possible to make OpenGL calls directly, though It is 
incouraged. Everything should be done via the Renderer API.

For each frame it is most often desirable to clear the render state. 
The :cpp:func:`mos::Renderer::clear()` method takes a vec3 of rgb colors:

 .. code-block:: c++
	
	renderer.clear(glm::vec3(0.0f, 0.0f, 0.0f));
	
To do the actual rendering, the API provides a couple of different forms of an update() 
method. Wihich does just that, updates the render state. In its simplest form it looks as follows:

 .. code-block:: c++
	
	renderer.update(model, camera, light);
	
Colission detection
-------------------

Animation
---------

Mos supports keyframe animation.

Audio
-----

3D audio through OpenAL.


