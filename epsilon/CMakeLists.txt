add_library(epsilon rng.c sorf.c kpa.c)
set_target_properties(epsilon PROPERTIES 
    VERSION ${PROJECT_VERSION}
    C_STANDARD 99)

option(EPSILON_SORF_ENABLED "enable patented SORF" OFF)
if(EPSILON_SORF_ENABLED)
    target_compile_definitions(epsilon PRIVATE SORF_ENABLED)
endif()

# See https://stackoverflow.com/a/50882216/220978.
if(MSVC)
    target_compile_options(epsilon PRIVATE /W4 /WX)
else()
    target_compile_options(epsilon PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Make headers available.
target_include_directories(epsilon INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
    )

# install rules
install(TARGETS epsilon DESTINATION epsilon)
install(FILES epsilon.h DESTINATION include)