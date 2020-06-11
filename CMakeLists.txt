cmake_minimum_required(VERSION 3.11)

project(
  codetimer
  VERSION 0.1
  DESCRIPTION "A libcuckoo based simple library for timing"
  LANGUAGES CXX)
include(CMakePackageConfigHelpers)
include(FetchContent)

# Check if this CMakeLists is being used directly or via add_subdirectory
set(CODETIMER_MASTER_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(CODETIMER_MASTER_PROJECT ON)
endif()
# Allow for parent CMakeLists to populate the outgoing targets file
if(NOT CODETIMER_EXPORT_NAME)
  set(CODETIMER_EXPORT_NAME "${PROJECT_NAME}-targets")
endif()
message(STATUS "[codetimer]:: Export name is ${CODETIMER_EXPORT_NAME}")

message(STATUS "[codetimer]:: Pulling and configuring libcuckoo")
FetchContent_Declare(
  libcuckoo
  GIT_REPOSITORY https://github.com/icoderaven/libcuckoo.git
  GIT_TAG master)
if(NOT libcuckoo_POPULATED)
  FetchContent_Populate(libcuckoo)
  set(LIBCUCKOO_EXPORT_NAME "${CODETIMER_EXPORT_NAME}")
  option(LIBCUCKOO_BUILD_EXAMPLES OFF)
  add_subdirectory(${libcuckoo_SOURCE_DIR} ${libcuckoo_BINARY_DIR})
endif()

add_library(codetimer SHARED src/codetimer.cc)
# add alias to match exported target when calling using add_subdirectory()
add_library(codetimer::codetimer ALIAS codetimer)
target_include_directories(
  codetimer PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                   $<INSTALL_INTERFACE:include>)
# Link for this project, and also provide libcuckoo as transitive interface
target_link_libraries(codetimer PUBLIC libcuckoo::libcuckoo)
# Add transitive c++11
target_compile_features(codetimer PUBLIC cxx_std_11)

# Quick test app
add_executable(test_codetimer src/timerexample.cc)
target_link_libraries(test_codetimer codetimer)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

# cmake packaging
set(CODETIMER_CMAKECONFIG_INSTALL_DIR
    "share/cmake/${PROJECT_NAME}"
    CACHE STRING "install path for codetimer-config.cmake")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/codetimer-config.cmake.in
               codetimer-config.cmake @ONLY)

write_basic_package_version_file(
  "codetimer-config-version.cmake"
  VERSION ${PACKAGE_VERSION}
  COMPATIBILITY AnyNewerVersion)

# Generate targets file
install(TARGETS codetimer EXPORT "${CODETIMER_EXPORT_NAME}")

# Install the targets only if we're the Master project
if(CODETIMER_MASTER_PROJECT)
  install(
    EXPORT "${CODETIMER_EXPORT_NAME}"
    NAMESPACE "${PROJECT_NAME}::"
    DESTINATION ${CODETIMER_CMAKECONFIG_INSTALL_DIR})

  # This isn't sufficient, since dependencies also need our dependencies
  # (libcuckoo) This is provided in cmake/codetimer-config.cmmake.
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/codetimer-config.cmake
                ${CMAKE_CURRENT_BINARY_DIR}/codetimer-config-version.cmake
          DESTINATION ${CODETIMER_CMAKECONFIG_INSTALL_DIR})
endif()
