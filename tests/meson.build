# Set up.
unity_dep = dependency('unity')

tests = ['hash_test', 'rng_test', 'welford_test', 'fwht_test']

# Build and configure tests for each example
foreach ex : tests
    exe = executable(
        ex,
        ex + '.c',
        dependencies: [epsilon_dep, unity_dep],
        install: false,
    )
    test(ex, exe)
endforeach