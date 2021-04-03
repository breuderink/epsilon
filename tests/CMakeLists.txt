find_library(GREATEST greatest)

add_executable(xorshift_test xorshift_test.c)
target_include_directories(xorshift_test PRIVATE ${GREATEST_INCLUDE_DIRS})
target_link_libraries(xorshift_test epsilon)
add_test(NAME xorshift_test COMMAND xorshift_test)