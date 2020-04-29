add_executable(basic_sample basic.c common.h common.c)
target_link_libraries(basic_sample PUBLIC ezmidi)

add_executable(advanced_sample advanced.cpp common.h common.c)
target_link_libraries(advanced_sample PUBLIC ezmidi)
set_target_properties(advanced_sample PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON)
