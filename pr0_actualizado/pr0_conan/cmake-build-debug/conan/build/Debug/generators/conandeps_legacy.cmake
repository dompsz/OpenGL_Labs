message(STATUS "Conan: Using CMakeDeps conandeps_legacy.cmake aggregator via include()")
message(STATUS "Conan: It is recommended to use explicit find_package() per dependency instead")

find_package(opengl-registry)
find_package(FreeGLUT)
find_package(opengl_system)

set(CONANDEPS_LEGACY  opengl-registry::opengl-registry  FreeGLUT::freeglut_static  opengl::opengl )