# Xorshift example.
add_executable(example_xorshift example_xorshift.c)
target_link_libraries(example_xorshift epsilon)
add_test(NAME example_xorshift COMMAND example_xorshift)

# FNV hash example.
add_executable(example_FNV_hash example_FNV_hash.c)
target_link_libraries(example_FNV_hash epsilon)
add_test(NAME example_FNV_hash COMMAND example_FNV_hash)

# Example for Welfords method.
add_executable(example_Welfords_method example_Welfords_method.c)
target_link_libraries(example_Welfords_method epsilon)
add_test(NAME example_Welfords_method COMMAND example_Welfords_method)

# Fast Walsh-Hadamard example.
add_executable(example_FWHT example_FWHT.c)
target_link_libraries(example_FWHT epsilon m)
add_test(NAME example_FWHT COMMAND example_FWHT)

# BKPA regression example.
add_executable(example_BKPA_regression example_BKPA_regression.c)
target_link_libraries(example_BKPA_regression epsilon m)
add_test(NAME example_BKPA_regression COMMAND example_BKPA_regression)
