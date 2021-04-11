# Xorshift example.
add_executable(example_xorshift example_xorshift.c)
target_link_libraries(example_xorshift epsilon)
add_test(NAME example_xorshift COMMAND example_xorshift)

# Fast Walsh-Hadamard example.
add_executable(example_FWHT example_FWHT.c)
target_link_libraries(example_FWHT epsilon m)
add_test(NAME example_FWHT COMMAND example_FWHT)