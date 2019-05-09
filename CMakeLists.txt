cmake_minimum_required(VERSION 3.0)

project(ezmidi)

set(ezmidi_source
	include/ezmidi.h
	
	src/event.h 
	src/event.cpp 
	src/midi.h
)

if (APPLE)
	set(ezmidi_source ${ezmidi_source} src/coremidi.cpp)
elseif (WIN32)
	set(ezmidi_source ${ezmidi_source} src/windows.cpp)
endif()

add_library(ezmidi ${ezmidi_source})
set_property(TARGET ezmidi PROPERTY CXX_STANDARD 17)
target_include_directories(ezmidi PUBLIC include)

if (APPLE)
	target_link_libraries(ezmidi PUBLIC "-framework CoreMIDI" "-framework CoreFoundation")
elseif(WIN32)
	target_link_libraries(ezmidi PUBLIC Winmm.lib)
endif()


add_executable(read_notes sample/read_notes.cpp)
target_include_directories(read_notes PRIVATE include)
target_link_libraries(read_notes PRIVATE ezmidi)
set_property(TARGET read_notes PROPERTY CXX_STANDARD 11)