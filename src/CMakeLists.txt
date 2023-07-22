cmake_minimum_required(VERSION 3.21)

project(coil_dump VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZAITON ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(coil_core REQUIRED CONFIG)

add_executable(example_render_fonts
  example_render_fonts.cpp
)
target_link_libraries(example_render_fonts
  coil_core_appidentity
  coil_core_asset
  coil_core_entrypoint_console
  coil_core_fs
  coil_core_fthb
  coil_core_render_fonts
  coil_core_sdl
  coil_core_sdl_vulkan
  coil_core_vulkan
)
list(APPEND coil_dump_executables example_render_fonts)

add_executable(test_unicode
  test_unicode.cpp
)
target_link_libraries(test_unicode
  coil_core_asset
  coil_core_entrypoint_console
  coil_core_fs
  coil_core_unicode
)
list(APPEND coil_dump_executables test_unicode)

add_executable(test_webm
  test_webm.cpp
)
target_link_libraries(test_webm
  coil_core_asset
  coil_core_av1
  coil_core_entrypoint_console
  coil_core_fs
  coil_core_image_png
  coil_core_webm
)
list(APPEND coil_dump_executables test_webm)

# installation
install(TARGETS ${coil_dump_executables})
