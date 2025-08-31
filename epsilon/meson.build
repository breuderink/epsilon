# Define the epsilon library
epsilon_sources = ['rng.c', 'welford.c', 'hash.c', 'fwht.c']

cc = meson.get_compiler('c')
math_dep = cc.find_library('m', required: false)

# Build the library
epsilon_lib = library(
    'epsilon',
    epsilon_sources,
    include_directories: include_directories('.'),
    install: true,
    version: meson.project_version(),
    dependencies: [math_dep],
)

# Declare dependency for downstream use
epsilon_dep = declare_dependency(
    include_directories: include_directories('.'),
    link_with: epsilon_lib,
    dependencies: [math_dep],
)

# Install headers
install_headers('epsilon.h', subdir: 'epsilon')