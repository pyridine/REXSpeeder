cmake_minimum_required(VERSION 3.1)
project("REXSpeeder")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)

add_library(rexspeeder ../src/REXSpeeder.cpp)
add_executable(example ../example/example.cpp)
target_link_libraries(example rexspeeder)

# Include the "include" folder for headers
target_include_directories(rexspeeder PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
)

# We depend upon zlib
find_package(ZLIB REQUIRED)
if (ZLIB_FOUND)
	include_directories(${ZLIB_INCLUDE_DIRS})
	target_link_libraries(rexspeeder ${ZLIB_LIBRARIES})
endif()

# Create a symlink to the files needed for example
add_custom_command(
    TARGET rexspeeder POST_BUILD
    COMMAND ln -sf ../example/files .
)