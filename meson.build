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
    'src/frontend/ast.cpp',
    'src/frontend/datatype.cpp',
    'src/frontend/filetable.cpp',
    'src/frontend/stringtable.cpp',
    'src/frontend/diagnostics.cpp',
    'src/frontend/compile_info.cpp',
    'src/lexer/lexer.cpp',
    'src/lexer/token.cpp',
    'src/main.cpp',
    'src/unicode.cpp',
]

executable(
    'quetzalcoatl',
    [sources],
    install: true,
    build_by_default: true,
    include_directories: [include_directories('include')]
)
