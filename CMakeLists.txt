cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

# set the project name
set(PROJECT_NAME "Raytracer")
project(${PROJECT_NAME} VERSION 1.0)

# enforce cpp standard version
set(CMAKE_CXX_STANDARD 14)
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
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/lodepng/*.cpp
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
)

# Define the executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDES})
