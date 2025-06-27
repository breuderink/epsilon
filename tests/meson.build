# Set up.
unity_dep = dependency('unity')
rng_exhaustive = get_option('rng_exhaustive')

tests = ['hash_test', 'kpa_test', 'rng_test', 'welford_test', 'sorf_test']

# Build and configure tests for each example
foreach ex : tests
    exe = executable(
        ex,
        ex + '.c',
        dependencies: [epsilon_dep, unity_dep],
        install: false,
        c_args: get_option(('rng_exhaustive')) ? ['-DXORSHIFT32_EXHAUSTIVE'] : [],
    )
    test(ex, exe)
endforeach