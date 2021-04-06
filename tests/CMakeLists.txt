find_path(GREATEST_INCLUDE_DIRS greatest.h REQUIRED)

add_executable(epsilon_test main.c rng_test.c sorf_test.c)
target_include_directories(epsilon_test PRIVATE ${GREATEST_INCLUDE_DIRS})
target_link_libraries(epsilon_test epsilon)

option(EPSILON_RNG_TEST_EXHAUSTIVE "enable exhaustive test for xorshift32" OFF)
if(EPSILON_RNG_TEST_EXHAUSTIVE)
    target_compile_definitions(epsilon_test PRIVATE XORSHIFT32_EXHAUSTIVE)
endif()
if(EPSILON_SORF_ENABLED)
    target_compile_definitions(epsilon_test PRIVATE SORF_ENABLED)
endif()

add_test(NAME epsilon_test COMMAND epsilon_test)
