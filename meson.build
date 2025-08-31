project('epsilon', 'c',
  version: '0.2.1',
  default_options: ['c_std=c99', 'warning_level=3', 'werror=true']
)

cc = meson.get_compiler('c')
math_dep = cc.find_library('m', required: false)
unity_dep = dependency('unity', required: true)

# Library sources
epsilon_sources = [
  'epsilon/rng.c',
  'epsilon/stats.c',
  'epsilon/hash.c',
  'epsilon/transform.c'
]

# Build the library
epsilon_lib = library('epsilon',
  sources: epsilon_sources,
  dependencies: math_dep,
  install: true,
  version: meson.project_version()
)

# Dependency object for downstream use
epsilon_dep = declare_dependency(
  link_with: epsilon_lib,
  dependencies: math_dep
)

# Install headers
epsilon_headers = [
  'epsilon/rng.h',
  'epsilon/stats.h',
  'epsilon/hash.h',
  'epsilon/transform.h',
  'epsilon/pa.h'
]
install_headers(epsilon_headers, subdir: 'epsilon')

# Examples
example_exes = [
  ['example_hash', 'examples/example_hash.c'],
  ['example_transform', 'examples/example_transform.c'],
  ['example_stats', 'examples/example_stats.c'],
  ['example_rng', 'examples/example_rng.c']
]

foreach pair : example_exes
  exe = executable(pair[0], pair[1], dependencies: epsilon_dep, install: false)
  test(pair[0], exe)
endforeach

# Tests
test_exes = [
  ['hash_test', 'tests/hash_test.c'],
  ['rng_test', 'tests/rng_test.c'],
  ['stats_test', 'tests/stats_test.c'],
  ['transform_test', 'tests/transform_test.c']
]

foreach pair : test_exes
  exe = executable(pair[0], pair[1], dependencies: [epsilon_dep, unity_dep], install: false)
  test(pair[0], exe)
endforeach

# Documentation
install_data('README.md', install_dir: 'share/doc/epsilon')
install_data('LICENSE', install_dir: 'share/licenses/epsilon')

# Optional metadata
meson.add_dist_script('echo', 'Package contact: boris@cortext.nl')
