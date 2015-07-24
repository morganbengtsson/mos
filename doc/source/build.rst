=====
Build
=====

The only method documented here is how to include MOS in a CMAKE environment. 
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

CMake
-----

To use MOS within a CMake project, put the following in the CMakeLists.txt:
 .. code-block:: cmake
 
	# mos
	add_subdirectory(externals/mos)
	include_directories(externals/mos/include)
	
	target_link_libraries(project_name mos)

Where project_name is the name of the parent CMake project.
