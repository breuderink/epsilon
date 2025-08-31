# breuderink-epsilon/examples/meson.build
# List of example executables
examples = [
    'example_xorshift',
    'example_FNV_hash',
    'example_Welfords_method',
    'example_FWHT',
]

# Build and configure tests for each example
foreach ex : examples
    exe = executable(
        ex,
        ex + '.c',
        dependencies: [epsilon_dep],
        install: false,
    )
    test(ex, exe)
endforeach