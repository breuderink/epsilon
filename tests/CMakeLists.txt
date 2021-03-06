# Greatest unit-test dependency.
include(FetchContent)
FetchContent_Declare(
  greatest
  GIT_REPOSITORY https://github.com/silentbicycle/greatest
  GIT_TAG 11a6af1919049df502405913da64fb385c219361 # v1.5.0
)
FetchContent_MakeAvailable(greatest)

# Define test executable.
add_executable(epsilon_test 
  main.c rng_test.c sorf_test.c welford_test.c hash_test.c kpa_test.c)
target_include_directories(epsilon_test PRIVATE ${greatest_SOURCE_DIR})
target_link_libraries(epsilon_test epsilon m)

# See https://stackoverflow.com/a/50882216/220978.
if(MSVC)
    target_compile_options(epsilon_test PRIVATE /W4 /WX)
else()
    target_compile_options(epsilon_test PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Handle options.
option(EPSILON_RNG_TEST_EXHAUSTIVE "enable exhaustive test for xorshift32" OFF)
if(EPSILON_RNG_TEST_EXHAUSTIVE)
    target_compile_definitions(epsilon_test PRIVATE XORSHIFT32_EXHAUSTIVE)
endif()
if(EPSILON_SORF_ENABLED)
    target_compile_definitions(epsilon_test PRIVATE SORF_ENABLED)
endif()

# Add test.
add_test(NAME epsilon_test COMMAND epsilon_test)
