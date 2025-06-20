# Define the epsilon library
epsilon_sources = ['rng.c', 'welford.c', 'sorf.c', 'hash.c', 'kpa.c']

# Define the SORF option
sorf_enabled = get_option('sorf_enabled')

# Build the library
epsilon_lib = library(
    'epsilon',
    epsilon_sources,
    include_directories: include_directories('.'),
    install: true,
    version: meson.project_version(),
    c_args: sorf_enabled ? ['-DSORF_ENABLED'] : [],
)

# Declare dependency for downstream use
epsilon_dep = declare_dependency(
    include_directories: include_directories('.'),
    link_with: epsilon_lib,
)

# Install headers
install_headers('epsilon.h', subdir: 'epsilon')
