# Xorshift example.
add_executable(example_xorshift example_xorshift.c)
target_link_libraries(example_xorshift epsilon)
add_test(NAME example_xorshift COMMAND example_xorshift)

# KPA regression example.
add_executable(example_KPA_regression example_KPA_regression.c)
target_link_libraries(example_KPA_regression epsilon)
add_test(NAME example_KPA_regression COMMAND example_KPA_regression)
