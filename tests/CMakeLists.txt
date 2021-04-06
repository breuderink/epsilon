find_path(GREATEST_INCLUDE_DIRS greatest.h REQUIRED)

add_executable(epsilon_test main.c rng_test.c sorf_test.c)
target_include_directories(epsilon_test PRIVATE ${GREATEST_INCLUDE_DIRS})
target_link_libraries(epsilon_test epsilon)

option(XORSHIFT32_EXHAUSTIVE "enabale exhaustive test for xorshift32" OFF)
if(XORSHIFT32_EXHAUSTIVE)
    target_compile_definitions(epsilon_test PRIVATE XORSHIFT32_EXHAUSTIVE)
endif()

add_test(NAME epsilon_test COMMAND epsilon_test)
