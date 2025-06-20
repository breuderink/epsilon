# Define project
project(
    'epsilon',
    'c',
    version: '0.2.0',
    default_options: ['c_std=c99', 'warning_level=3', 'werror=true'],
)

# Define subdirectories
subdir('epsilon')
subdir('examples')
subdir('tests')

# Installation rules
install_data('README.md', install_dir: 'share/doc/epsilon')
install_data('LICENSE', install_dir: 'share/licenses/epsilon')

# CPack equivalent metadata
meson.add_dist_script('echo', 'Package contact: boris@cortext.nl')