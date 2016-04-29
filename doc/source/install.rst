=======
Install
=======

The only method documented here is how to include MOS in a CMake environment. 
Other means of using the MOS library are possible, though not documented.

Source code
-----------

The MOS source code is hosted in a git repository, to get it, run the following 
command:

 .. code-block:: bash
 
	git clone --recursive https://github.com/morganbengtsson/mos.git

The recursive flag will tell git to also download submodules/dependencies
that are used by MOS. All dependencies are source code, no binary dependencies.

If the project to use MOS already has its own git repository, it is convenient
to use MOS as a submodule instead:

 .. code-block:: bash

	git submodule add https://github.com/morganbengtsson/mos.git

In this case the submodules within MOS are not downloaded, so an extra
command has to be invoked, from within the MOS directory (cd mos):

 .. code-block:: bash
 
	git submodule update --init --recursive

Build
------

To use MOS within a CMake project, put the following in the CMakeLists.txt:
 .. code-block:: cmake
 
	# mos
	add_subdirectory(externals/mos)
	include_directories(externals/mos/include)
	
	target_link_libraries(project_name mos)

Where *project_name* is the name of the parent CMake project and *externals*
is the name of the directory with external libraries in this case.

Blender scripts
---------------

The asset manager in MOS a number of binary file formats *.mesh and *material. To be
able to use theese features, a number of Python scripts for Blender 3D are
provided. To be able to export meshes and materials from a scene.

The scripts are contained in three repositories:

 .. code-block:: bash
 
	git clone https://github.com/morganbengtsson/mos-modules.git

Which contains the general modules for Blender and are to be placed in
the /scripts/modules/ folder of the Blender installation.

The other two repositories contains the actual export addons:

.. code-block:: bash

	git clone https://github.com/morganbengtsson/io-scene-meshes
	git clone https://github.com/morganbengtsson/io-scene-materials
	
Theese are to be placed in the /scripts/addons/ folder of the Blender
installation.

The addons also has to be enabled in the Blender settings. When done
correctly, two new options will be available in the export menu.


