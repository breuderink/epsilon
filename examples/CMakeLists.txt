# Xorshift example.
add_executable(example_xorshift example_xorshift.c)
target_link_libraries(example_xorshift epsilon)
add_test(NAME example_xorshift COMMAND example_xorshift)

# Example for Welfords method.
add_executable(example_Welfords_method example_Welfords_method.c)
target_link_libraries(example_Welfords_method epsilon)
add_test(NAME example_Welfords_method COMMAND example_Welfords_method)

# Fast Walsh-Hadamard example.
add_executable(example_FWHT example_FWHT.c)
target_link_libraries(example_FWHT epsilon m)
add_test(NAME example_FWHT COMMAND example_FWHT)

# KPA regression example.
add_executable(example_KPA_regression example_KPA_regression.c)
target_link_libraries(example_KPA_regression epsilon m)
add_test(NAME example_KPA_regression COMMAND example_KPA_regression)
