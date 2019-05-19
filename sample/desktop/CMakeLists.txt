add_executable(read_notes read_notes.cpp)

target_link_libraries(read_notes PUBLIC ezmidi)
set_property(TARGET read_notes PROPERTY CXX_STANDARD 11)