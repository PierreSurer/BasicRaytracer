cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
# set the project name
set(PROJECT_NAME "Raytracer")
project(${PROJECT_NAME} VERSION 1.0)

# enforce cpp standard version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# setup warning flags
if (MSVC)
    add_compile_options(/W2)
else()
    add_compile_options(-Wall -Wextra)
endif()

# Add source files
file(GLOB SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/deps/yaml/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/objects/*.cpp
)

# Add header files
file(GLOB HEADER_FILES 
	${CMAKE_SOURCE_DIR}/src/*.h
	${CMAKE_SOURCE_DIR}/src/*.hpp
    ${CMAKE_SOURCE_DIR}/src/objects/*.hpp
)

# Define the include DIRs
set(INCLUDES
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/deps
  ${CMAKE_CURRENT_SOURCE_DIR}/deps/glm
)


# Define the executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDES})

# setup openmp
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} OpenMP::OpenMP_CXX)
endif()

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("${PROJECT_SOURCE_DIR}/deps/glfw")

find_package(OpenGL REQUIRED)

find_package(OpenCL REQUIRED)

target_link_libraries(${PROJECT_NAME} glfw OpenGL::GL OpenCL::OpenCL)

function(add_kernel TARGET KERNEL)
    find_program(CLANG clang)

    set(current-opencl-path ${CMAKE_CURRENT_SOURCE_DIR}/src/opencl/${KERNEL})
    string(REPLACE ".clcpp" "" KERNEL ${KERNEL})
    set(current-output-path ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/opencl/${KERNEL}.spv)


    # Add a custom command to compile GLSL to SPIR-V.
    get_filename_component(current-output-dir ${current-output-path} DIRECTORY)
    file(MAKE_DIRECTORY ${current-output-dir})

    add_custom_command(
        OUTPUT ${current-output-path}
        COMMAND echo ${CLANG} -cl-std=CLC++ -target spirv64 -v -o ${current-output-path} ${current-opencl-path}
        COMMAND ${CLANG} -cl-std=CLC++ -target spirv64 -v -o ${current-output-path} ${current-opencl-path}
        DEPENDS ${current-opencl-path}
        IMPLICIT_DEPENDS CXX ${current-opencl-path}
        VERBATIM)

    # Make sure our build depends on this output.
    set_source_files_properties(${current-output-path} PROPERTIES GENERATED TRUE)
    target_sources(${TARGET} PRIVATE ${current-output-path})
endfunction(add_kernel)

# Find all shaders.
file(GLOB kernel ${CMAKE_SOURCE_DIR}/src/opencl/*.clcpp)

# Add them to the build.
foreach(kernel ${kernel})
	get_filename_component(file ${kernel} NAME)
	message("Compiling " ${file})
	add_kernel(${PROJECT_NAME} ${file})
endforeach(kernel)
