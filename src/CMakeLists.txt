set(ezmidi_source
	../include/ezmidi/ezmidi.h
	midi.h
	message_processor.h 
	message_processor.cpp 
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

set_target_properties(ezmidi PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON)
target_include_directories(ezmidi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

if (APPLE)
	target_link_libraries(ezmidi PRIVATE "-framework CoreMIDI" "-framework CoreFoundation")
elseif(WIN32)
	target_link_libraries(ezmidi PRIVATE Winmm.lib)
	target_compile_definitions(ezmidi PRIVATE BUILD_EZMIDI_DLL)
else()
	target_link_libraries(ezmidi PRIVATE "-lasound" PUBLIC Threads::Threads)
endif()