#
# codetimer-config.cmake
#

include (CMakeFindDependencyMacro)

find_dependency(libcuckoo REQUIRED)

include ("${CMAKE_CURRENT_LIST_DIR}/codetimer-targets.cmake")
