find_path(GREATEST_INCLUDE_DIRS greatest.h REQUIRED)

add_executable(xorshift_test 
    main.c
    xorshift_test.c
    sorf_test.c
    )
target_include_directories(xorshift_test PRIVATE ${GREATEST_INCLUDE_DIRS})
target_link_libraries(xorshift_test epsilon)
add_test(NAME xorshift_test COMMAND xorshift_test -x exhaustive)
