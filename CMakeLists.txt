cmake_minimum_required(VERSION 3.19)

project(epsilon 
    VERSION 0.1.0
    LANGUAGES C)

# Library
add_subdirectory(epsilon)

# Installation
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_CONTACT boris@cortext.nl)
include(CPack)
