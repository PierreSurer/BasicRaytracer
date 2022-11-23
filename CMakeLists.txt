cmake_minimum_required (VERSION 3.19)
project(Raytracer)

# enforce cpp standard version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# setup warning flags
if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra) # -Wpedantic
endif()

file(GLOB SOURCE
  ${CMAKE_CURRENT_SOURCE_DIR}/deps/yaml/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

set(INCLUDES
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/deps
)

add_executable(Raytracer ${SOURCE})


target_include_directories(Raytracer PUBLIC ${INCLUDES})