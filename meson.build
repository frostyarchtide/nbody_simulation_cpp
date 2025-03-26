project(
  'nbody_simulation',
  'cpp',
  default_options: 'default_library=static'
)

epoxy = dependency('epoxy')
glfw3 = dependency('glfw3')
imgui = dependency('imgui')

subdir('src')
executable(
  'nbody_simulation',
  sources,
  dependencies: [epoxy, glfw3, imgui]
)
