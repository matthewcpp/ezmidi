cmake_minimum_required(VERSION 3.0)

project(ezmidi)

add_library(ezmidi include/ezmidi.h src/coremidi.cpp src/event.h src/event.cpp src/midi.h)
target_include_directories(ezmidi PUBLIC include)
target_link_libraries(ezmidi PUBLIC "-framework CoreMIDI" "-framework CoreFoundation")
set_property(TARGET ezmidi PROPERTY CXX_STANDARD 17)

add_executable(read_notes sample/read_notes.cpp)
target_include_directories(read_notes PRIVATE include)
target_link_libraries(read_notes PRIVATE ezmidi)