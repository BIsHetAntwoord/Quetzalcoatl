project(
    'Quetzalcoatl',
    ['cpp'],
    version: '0.0.1',
    default_options: [
        'cpp_std=c++2a',
        'buildtype=debugoptimized',
    ]
)

# Final executable
sources = [
    'src/lexer/filetable.cpp',
    'src/lexer/lexer.cpp',
    'src/main.cpp'
]

executable(
    'quetzalcoatl',
    [sources],
    install: true,
    build_by_default: true,
    include_directories: [include_directories('include')]
)