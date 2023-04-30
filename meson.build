# declare project
project('playground', 'cpp', 
    version: '1.0.0',
    default_options : ['cpp_std=c++20'])

# include files in the "include" directory
inc_dir = include_directories('include/')

# set our sources
playground_src = ['src/main.cpp']

### Setup Dependencies ###

# hacky workaround - manually copy libbgfx-shared-libDebug into /usr/local/lib
bgfx_dep = declare_dependency(
    include_directories: ['lib/bgfx/include'],
    link_args: ['-L/Users/bailycase/Developer/side-projects/cpp-playground/lib/bgfx/.build/osx-arm64/bin', '-lbgfx-shared-libDebug']
)

add_global_arguments('-DBX_CONFIG_DEBUG=1', language: 'cpp')

bx_dep = declare_dependency(
    include_directories: ['lib/bx/include'],
    link_args: ['-L/Users/bailycase/Developer/side-projects/cpp-playground/lib/bx/.build/osx-arm64/bin', '-lbxDebug']
)

glfw = dependency('glfw3')

imgui_sources = [
    'lib/imgui/imgui.cpp',
    'lib/imgui/imgui_tables.cpp',
    'lib/imgui/imgui_widgets.cpp',
    'lib/imgui/imgui_draw.cpp',
    'lib/imgui/imgui_impl_glfw.cpp',
    'lib/imgui/imgui_impl_bgfx.cpp'
]

imgui_dep = declare_dependency(
    include_directories : include_directories('lib/imgui'),
    sources : imgui_sources
)

entt = declare_dependency(include_directories: include_directories('lib/entt'))


executable('playground', playground_src, dependencies : [bgfx_dep, glfw, imgui_dep, bx_dep, entt], include_directories : [inc_dir])