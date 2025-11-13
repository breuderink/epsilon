project('epsilon', 'c',
  version: '0.2.1',
  default_options: ['c_std=c99', 'warning_level=3', 'werror=true']
)

# Dependencies
cc = meson.get_compiler('c')
math_dep = cc.find_library('m', required: false)
unity_dep = dependency('unity', required: true)

# Sources
epsilon_sources = files(
  'src/csv.c',
  'src/rng.c',
  'src/stats.c',
  'src/hash.c',
  'src/transform.c'
)

epsilon_headers = files(
  'src/csv.h',
  'src/rng.h',
  'src/stats.h',
  'src/hash.h',
  'src/transform.h',
  'src/pa.h'
)

# Library
epsilon_lib = library('epsilon',
  sources: epsilon_sources,
  dependencies: math_dep,
  install: true,
  version: meson.project_version()
)

# Declare dependency for downstream use (export include dirs / link)
epsilon_dep = declare_dependency(
  link_with: epsilon_lib,
  include_directories: include_directories('src'),
  dependencies: math_dep
)

# Install public headers
install_headers(epsilon_headers, subdir: 'epsilon')

# Examples & tools
examples = {
  'example_hash': 'examples/example_hash.c',
  'example_transform': 'examples/example_transform.c',
  'example_stats': 'examples/example_stats.c',
  'example_rng': 'examples/example_rng.c'
}

# Build and register examples (not installed)
foreach name, source : examples
  exe = executable(name, source,
    dependencies: epsilon_dep,
    install: false
  )
  test(name, exe)
endforeach

# Tools (intended for installation)
executable('razor-cli', files('tools/razor/cli.c'),
  dependencies: epsilon_dep,
  install: true,
  install_dir: get_option('bindir')
)

# Tests
tests = {
  'csv_test': 'tests/csv_test.c',
  'hash_test': 'tests/hash_test.c',
  'rng_test': 'tests/rng_test.c',
  'stats_test': 'tests/stats_test.c',
  'transform_test': 'tests/transform_test.c',
}

foreach name, source : tests
  exe = executable(name, source,
    dependencies: [epsilon_dep, unity_dep],
    install: false
  )
  test(name, exe)
endforeach

# Docs & packaging
install_data(
  files('README.md', 'LICENSE'),
  install_dir: 'share/doc/epsilon'
)

meson.add_dist_script('echo', 'Package contact: boris@cortext.nl')
