FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.8.1
)

FetchContent_GetProperties(googletest)
if(NOT ${googletest_POPULATED})
    FetchContent_Populate(googletest)
endif()

set(gtest_sources 
    ${googletest_SOURCE_DIR}/googletest/src/gtest_main.cc
    ${googletest_SOURCE_DIR}/googletest/src/gtest-all.cc)

set(test_sources
    test_notes.cpp)
    
set(ezmidi_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/event.cpp)

add_executable(ezmidi_test ${gtest_sources} ${test_sources} ${ezmidi_sources})

set_property(TARGET ezmidi_test PROPERTY CXX_STANDARD 11)
target_include_directories(ezmidi_test SYSTEM PUBLIC ${googletest_SOURCE_DIR}/googletest/include ${googletest_SOURCE_DIR}/googletest)
target_include_directories(ezmidi_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(ezmidi_test PUBLIC Threads::Threads)

source_group(googletest FILES ${gtest_sources})
source_group(ezmidi FILES ${ezmidi_sources})
source_group(tests FILES ${test_sources})

add_test(NAME test_notes COMMAND ezmidi_test "--gtest_filter=Notes*")