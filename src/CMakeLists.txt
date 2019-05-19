set(include_directory ${CMAKE_CURRENT_SOURCE_DIR}/../include/ezmidi)

set(ezmidi_source
	${include_directory}/ezmidi.h
    ${include_directory}/private/midi.h
    ${include_directory}/private/event.h 
	
	event.cpp 
	common.cpp
)

if (APPLE)
	set(ezmidi_source ${ezmidi_source} coremidi.cpp)
elseif (WIN32)
	set(ezmidi_source ${ezmidi_source} windows.cpp)
else()
	set(ezmidi_source ${ezmidi_source} alsa.cpp)
endif()

if (${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
	set(CMAKE_OSX_DEPLOYMENT_TARGET 9.3)
	add_library(ezmidi STATIC ${ezmidi_source})
else()
	add_library(ezmidi SHARED ${ezmidi_source})
endif()

set_property(TARGET ezmidi PROPERTY CXX_STANDARD 11)
target_include_directories(ezmidi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

if (APPLE)
	target_link_libraries(ezmidi PUBLIC "-framework CoreMIDI" "-framework CoreFoundation")
elseif(WIN32)
	target_link_libraries(ezmidi PUBLIC Winmm.lib)
    target_compile_definitions(ezmidi PRIVATE BUILD_EZMIDI_DLL)
else()
	target_link_libraries(ezmidi PUBLIC "-lasound" Threads::Threads)
endif()