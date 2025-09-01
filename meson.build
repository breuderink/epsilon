project('epsilon', 'c',
  version: '0.2.1',
  default_options: ['c_std=c99', 'warning_level=3', 'werror=true']
)

# Dependencies
cc = meson.get_compiler('c')
math_dep = cc.find_library('m', required: false)
unity_dep = dependency('unity', required: true)

# Library configuration

epsilon_sources = files(
  'src/rng.c',
  'src/stats.c',
  'src/hash.c',
  'src/transform.c'
)


epsilon_headers = files(
  'src/rng.h',
  'src/stats.h',
  'src/hash.h',
  'src/transform.h',
  'src/pa.h'
)

# Build library
epsilon_lib = library('epsilon',
  sources: epsilon_sources,
  dependencies: math_dep,
  install: true,
  version: meson.project_version()
)

# Declare dependency for downstream use

epsilon_dep = declare_dependency(
  link_with: epsilon_lib,
  include_directories: include_directories('src'),
  dependencies: math_dep
)

# Install headers

install_headers(epsilon_headers, subdir: 'epsilon')

# Examples and tests configuration
examples = {
  'example_hash': 'examples/example_hash.c',
  'example_transform': 'examples/example_transform.c',
  'example_stats': 'examples/example_stats.c',
  'example_rng': 'examples/example_rng.c'
}

tests = {
  'hash_test': 'tests/hash_test.c',
  'rng_test': 'tests/rng_test.c',
  'stats_test': 'tests/stats_test.c',
  'transform_test': 'tests/transform_test.c',
  'csv_test': 'tests/csv_test.c'
}

# Build and register examples
foreach name, source : examples
  exe = executable(name, source,
    dependencies: epsilon_dep,
    install: false
  )
  test(name, exe)
endforeach

# Build and register tests
foreach name, source : tests
  exe = executable(name, source,
    dependencies: [epsilon_dep, unity_dep],
    install: false
  )
  test(name, exe)
endforeach

# Documentation installation
install_data(
  files('README.md', 'LICENSE'),
  install_dir: 'share/doc/epsilon'
)

# Distribution metadata
meson.add_dist_script('echo', 'Package contact: boris@cortext.nl')
