Raytracer C++ framework for "Introduction to Computer Graphics"
===============================================================

This is a C++ framework for a raytracer. It is created for the Computer
Science course "Introduction to Computer Graphics" taught at the 
University of Groningen by Tobias Isenberg.

Author: Pierre Surer and Mathis Brossier based on the original work of Maarten Everts.

This framework is inspired by and uses code of the raytracer framework of 
Bert Freudenberg that can be found at
<http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html>

This framework was modernized from Maarten Everts' version to use a more modern c++
version, refreshed structure and updated dependencies.

The project uses CMake, which is compatible with most systems.
See <https://cmake.org/> for instructions and installation.

Description of the included files
---------------------------------

### The raytracer source files:

main.cpp
	Contains main(), starting point. Responsible for parsing command-line
	arguments.
	
scene.cpp/.h
	Scene class. Responsible for reading the scene description from a yaml file.
	
raytracer.cpp/.h
	Raytracer class. Contains code for the actual raytracing of the scene.
	
image.cpp/.h
	Image class, includes code for reading from and writing to PNG files.
	
light.cpp/.h
	Light class and Ray class.

object.cpp/.h
	Object class. Represents an object in the scene.

sphere.cpp/.h
	Sphere class, which is a subclass of the Object class. Respresents a
	sphere in the scene.
	
### Dependencies:

lodepng:
	Code for reading from and writing to PNG files, used by the Image class.
	LodePNG is created by Lode Vandevenne and can be found at
	<http://members.gamedev.net/lode/projects/LodePNG/>.

yaml:
	Code for parsing YAML documents. Created by Jesse Beder, can be obtained 
	from <http://code.google.com/p/yaml-cpp/>. See also yaml/license.txt.

glm:
	Popular header-only library for vector maths targeting computer graphics.
	code and documetation hosted on <https://github.com/g-truc/glm>. MIT licensed.