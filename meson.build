project('t3d', ['cpp', 'c'], 
  default_options: [
    'cpp_std=c++20',
    'buildtype=debug',
    'warning_level=3',
    ]
  )

# ===== CHANGES HERE =====
# GLFW3 dependency (modern way)
glfw_dep = dependency('glfw3', required: true)

# OpenGL dependency
opengl_dep = dependency('gl', required: true)

# Linker flags for GLAD (if needed)
add_global_link_arguments('-ldl', language: ['c', 'cpp'])  # For Linux
# ========================

#add_global_arguments(
#  language: ['cpp', 'c']
#  )

inc = include_directories('src', 'vendor', 'vendor/glad', 'vendor/simdjson')

cpp_sources = files(
  'src/main.cpp',
  'src/glw.cpp',
  'src/game.cpp',
  'src/gameobjects.cpp',
  'vendor/simdjson/simdjson.cpp'
  )

c_sources = files('vendor/glad/glad.c')

executable('t3d',
  sources: [cpp_sources, c_sources],
  include_directories: inc,
  dependencies: [glfw_dep, opengl_dep],  # Updated here
  install: false
  )
