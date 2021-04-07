# Example for xorshift.
add_executable(example_xorshift example_xorshift.c)
target_link_libraries(example_xorshift epsilon)
add_test(NAME example_xorshift COMMAND example_xorshift)

# Example for Welfords method.
add_executable(example_Welfords_method example_Welfords_method.c)
target_link_libraries(example_Welfords_method epsilon)
add_test(NAME example_Welfords_method COMMAND example_Welfords_method)
