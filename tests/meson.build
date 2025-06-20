# Define test sources
test_sources = [
    'main.c',
    'rng_test.c',
    'sorf_test.c',
    'welford_test.c',
    'hash_test.c',
    'kpa_test.c',
]

# Define the RNG exhaustive test option
rng_exhaustive = get_option('rng_exhaustive')

# Build the test executable
epsilon_test = executable(
    'epsilon_test',
    test_sources,
    dependencies: [epsilon_dep],
    c_args: rng_exhaustive ? ['-DXORSHIFT32_EXHAUSTIVE'] : [],
    install: false,
)

# Register the test
test('epsilon_test', epsilon_test)