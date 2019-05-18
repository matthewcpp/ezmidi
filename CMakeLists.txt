cmake_minimum_required(VERSION 3.11)

project(ezmidi)

include(FetchContent)
find_package(Threads REQUIRED)

set(ezmidi_source
	include/ezmidi.h
	
	src/event.h 
	src/event.cpp 
	src/midi.h
	src/common.cpp
)

if (APPLE)
	set(ezmidi_source ${ezmidi_source} src/coremidi.cpp)
elseif (WIN32)
	set(ezmidi_source ${ezmidi_source} src/windows.cpp)
else()
	set(ezmidi_source ${ezmidi_source} src/alsa.cpp)
endif()

if (${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
	set(CMAKE_OSX_DEPLOYMENT_TARGET 9.3)
	add_library(ezmidi STATIC ${ezmidi_source})
else()
	add_library(ezmidi SHARED ${ezmidi_source})
endif()

set_property(TARGET ezmidi PROPERTY CXX_STANDARD 11)
target_include_directories(ezmidi PUBLIC include)

if (APPLE)
	target_link_libraries(ezmidi PUBLIC "-framework CoreMIDI" "-framework CoreFoundation")
elseif(WIN32)
	target_link_libraries(ezmidi PUBLIC Winmm.lib)
    target_compile_definitions(ezmidi PRIVATE BUILD_EZMIDI_DLL)
else()
	target_link_libraries(ezmidi PUBLIC "-lasound" Threads::Threads)
endif()

if (${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
	add_subdirectory(sample/ios)
else()
	add_executable(read_notes sample/read_notes.cpp)
	target_include_directories(read_notes PRIVATE include)
	target_link_libraries(read_notes PRIVATE ezmidi)
	set_property(TARGET read_notes PROPERTY CXX_STANDARD 11)
    
    enable_testing()
    
    add_subdirectory(test)
endif()
