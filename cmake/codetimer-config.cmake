#
# codetimer-config.cmake
#

include(CMakeFindDependencyMacro)

find_dependency(libcuckoo REQUIRED)

# If this file hasn't been called before
if(NOT TARGET codetimer::codetimer)
  include("${CMAKE_CURRENT_LIST_DIR}/codetimer-targets.cmake")
endif()
