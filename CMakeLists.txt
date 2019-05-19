cmake_minimum_required(VERSION 3.11)

project(ezmidi)

include(FetchContent)
find_package(Threads REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)

if (${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
	add_subdirectory(sample/ios)
else()
    add_subdirectory(sample/desktop)
    
    enable_testing()
    add_subdirectory(test)
endif()
