cmake_minimum_required(VERSION 3.14)

project(
  codetimer
  VERSION 0.1
  DESCRIPTION "A libcuckoo based simple library for timing"
  LANGUAGES CXX)
include(CMakePackageConfigHelpers)
include(FetchContent)

# Check if this CMakeLists is being used directly or via add_subdirectory
set(INSTALL_CONFIG OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(INSTALL_CONFIG ON)
endif()

find_package(libcuckoo QUIET)
if(NOT libcuckoo)
  message(STATUS "libcuckoo not found, pulling and building...")
  FetchContent_Declare(
    libcuckoo
    GIT_REPOSITORY https://github.com/icoderaven/libcuckoo.git
    GIT_TAG master)
  if(NOT libcuckoo_POPULATED)
    FetchContent_Populate(libcuckoo)
    add_subdirectory(${libcuckoo_SOURCE_DIR} ${libcuckoo_BINARY_DIR}
                     EXCLUDE_FROM_ALL)
  endif()
endif()

add_library(codetimer SHARED src/codetimer.cc)
target_include_directories(
  codetimer PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                   $<INSTALL_INTERFACE:include>)
# Link for this project, and also provide libcuckoo as transitive interface
target_link_libraries(codetimer PUBLIC libcuckoo)
# Add transitive c++11
target_compile_features(codetimer PUBLIC cxx_std_11)

# Quick test app
add_executable(test_codetimer src/timerexample.cc)
target_link_libraries(test_codetimer codetimer)

if(INSTALL_CONFIG)
  # cmake packaging
  set(codetimer_pkgloc "share/cmake/codetimer")

  write_basic_package_version_file(
    "codetimer-config-version.cmake"
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion)
  install(
    TARGETS codetimer
    EXPORT codetimer-targets
    LIBRARY DESTINATION lib)
  # Generate targets file
  install(
    EXPORT codetimer-targets
    FILE "codetimer-targets.cmake"
    DESTINATION ${codetimer_pkgloc})
  # This isn't sufficient, since dependencies also need our dependencies
  # (libcuckoo) This is provided in cmake/codetimer-config.cmmake.
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/codetimer-config.cmake
                ${CMAKE_CURRENT_BINARY_DIR}/codetimer-config-version.cmake
          DESTINATION ${codetimer_pkgloc})
  install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
endif()
