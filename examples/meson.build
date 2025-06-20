# breuderink-epsilon/examples/meson.build
# List of example executables
examples = [
    'example_xorshift',
    'example_FNV_hash',
    'example_Welfords_method',
    'example_FWHT',
    'example_BKPA_regression',
]

# Build and configure tests for each example
foreach ex : examples
    exe = executable(
        ex,
        ex + '.c',
        dependencies: [epsilon_dep, math_dep],
        install: false,
    )
    test(ex, exe)
endforeach
