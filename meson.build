project(
    'Quetzalcoatl',
    ['cpp'],
    version: '0.0.1',
    default_options: [
        'cpp_std=c++2a',
        'buildtype=debugoptimized',
    ]
)

fmt_dep = subproject('fmt').get_variable('fmt_dep')

# Final executable
sources = [
    'src/frontend/ast.cpp',
    'src/frontend/filetable.cpp',
    'src/frontend/stringtable.cpp',
    'src/frontend/diagnostics.cpp',
    'src/frontend/compile_info.cpp',
    'src/frontend/type.cpp',
    'src/lexer/lexer.cpp',
    'src/lexer/token.cpp',
    'src/parser/parser.cpp',
    'src/main.cpp',
    'src/unicode.cpp',
]

executable(
    'quetzalcoatl',
    [sources],
    dependencies: [fmt_dep],
    install: true,
    build_by_default: true,
    include_directories: [include_directories('include')]
)
