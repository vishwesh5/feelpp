# Copyright Louis Dionne 2013-2016
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")


##############################################################################
# Setup project
#
# We parse the canonical version number located in <boost/hana/version.hpp>.
# This is done to allow the library to be used without requiring a proper
# installation during which the version would be written to this header.
##############################################################################
foreach(level MAJOR MINOR PATCH)
    file(STRINGS include/boost/hana/version.hpp
                 _define_${level}
                 REGEX "#define BOOST_HANA_${level}_VERSION")
    string(REGEX MATCH "([0-9]+)" _version_${level} "${_define_${level}}")
endforeach()

set(Boost.Hana_VERSION_STRING "${_version_MAJOR}.${_version_MINOR}.${_version_PATCH}")
project(Boost.Hana VERSION ${Boost.Hana_VERSION_STRING} LANGUAGES CXX)

# Perform checks to make sure we support the current compiler
include(CheckCxxCompilerSupport)


##############################################################################
# Setup CMake options
##############################################################################
option(BOOST_HANA_ENABLE_WERROR   "Fail and stop if a warning is triggered." OFF)
option(BOOST_HANA_ENABLE_MEMCHECK "Run the unit tests and examples under Valgrind if it is found." OFF)
option(BOOST_HANA_ENABLE_CONCEPT_CHECKS "Enable concept checking in the interface methods." ON)
option(BOOST_HANA_ENABLE_DEBUG_MODE "Enable Hana's debug mode." OFF)

option(BOOST_HANA_ENABLE_STRING_UDL
"Enable the GNU extension allowing the special string literal operator\
 template, which enables the _s suffix for creating compile-time strings." ON)

option(BOOST_HANA_ENABLE_EXCEPTIONS
"Build with exceptions enabled. Note that Hana does not make use of exceptions,\
 but this switch can be disabled when building the tests to assess that it is\
 really the case." ON)


##############################################################################
# Setup compiler flags (more can be set on a per-target basis or in subdirectories)
##############################################################################
include(CheckCXXCompilerFlag)
macro(boost_hana_append_flag testname flag)
    check_cxx_compiler_flag(${flag} ${testname})
    if (${testname})
        add_compile_options(${flag})
    endif()
endmacro()

# Compiler flags controlled by CMake options above
if (BOOST_HANA_ENABLE_WERROR)
    boost_hana_append_flag(BOOST_HANA_HAS_WERROR -Werror)
    boost_hana_append_flag(BOOST_HANA_HAS_WX -WX)
endif()

if (NOT BOOST_HANA_ENABLE_CONCEPT_CHECKS)
    add_definitions(-DBOOST_HANA_CONFIG_DISABLE_CONCEPT_CHECKS)
endif()

if (BOOST_HANA_ENABLE_DEBUG_MODE)
    add_definitions(-DBOOST_HANA_CONFIG_ENABLE_DEBUG_MODE)
endif()

if (BOOST_HANA_ENABLE_STRING_UDL)
    add_definitions(-DBOOST_HANA_CONFIG_ENABLE_STRING_UDL)
    # GCC pretends to have the flag, but produces a "unrecognized command line option"
    # warning when we use it.
    if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
        boost_hana_append_flag(BOOST_HANA_HAS_WNO_GNU_STRING_UDL
                               -Wno-gnu-string-literal-operator-template)
    endif()
endif()

if (NOT BOOST_HANA_ENABLE_EXCEPTIONS)
    boost_hana_append_flag(BOOST_HANA_HAS_FNO_EXCEPTIONS -fno-exceptions)
endif()

# Other compiler flags
boost_hana_append_flag(BOOST_HANA_HAS_FDIAGNOSTICS_COLOR         -fdiagnostics-color)
boost_hana_append_flag(BOOST_HANA_HAS_FTEMPLATE_BACKTRACE_LIMIT  -ftemplate-backtrace-limit=0)
boost_hana_append_flag(BOOST_HANA_HAS_PEDANTIC                   -pedantic)
boost_hana_append_flag(BOOST_HANA_HAS_STDCXX1Y                   -std=c++1y)
boost_hana_append_flag(BOOST_HANA_HAS_QUNUSED_ARGUMENTS          -Qunused-arguments)
boost_hana_append_flag(BOOST_HANA_HAS_W                          -W)
boost_hana_append_flag(BOOST_HANA_HAS_WALL                       -Wall)
boost_hana_append_flag(BOOST_HANA_HAS_WEXTRA                     -Wextra)
boost_hana_append_flag(BOOST_HANA_HAS_WNO_UNUSED_LOCAL_TYPEDEFS  -Wno-unused-local-typedefs)
boost_hana_append_flag(BOOST_HANA_HAS_WWRITE_STRINGS             -Wwrite-strings)


##############################################################################
# Setup include paths. More include paths can be added in subdirectories.
##############################################################################
#include_directories(${Boost.Hana_SOURCE_DIR}/include)

#find_package(Boost 1.59)
#if (Boost_FOUND)
#    include_directories(${Boost_INCLUDE_DIRS})
#else()
#    message(WARNING
#        "The Boost library headers were not found; targets depending "
#        "on Boost won't be available.")
#endif()


##############################################################################
# Setup custom functions to ease the creation of targets
##############################################################################
#   boost_hana_target_name_for(<output variable> <source file> [ext])
#
# Return the target name associated to a source file. If the path of the
# source file relative from the root of Hana is `path/to/source/file.ext`,
# the target name associated to it will be `path.to.source.file`.
#
# The extension of the file should be specified as a last argument. If no
# extension is specified, the `.cpp` extension is assumed.
function(boost_hana_target_name_for out file)
    if (NOT ARGV2)
        set(_extension ".cpp")
    else()
        set(_extension "${ARGV2}")
    endif()

    file(RELATIVE_PATH _relative ${Boost.Hana_SOURCE_DIR} ${file})
    string(REPLACE "${_extension}" "" _name ${_relative})
    string(REGEX REPLACE "/" "." _name ${_name})
    set(${out} "${_name}" PARENT_SCOPE)
endfunction()

#   boost_hana_add_test(<name> <command> [<arg>...])
#
# Creates a test called `name`, which runs the given `command` with the given
# `arg`uments. However, if `BOOST_HANA_ENABLE_MEMCHECK` is set to `ON`, the
# test will run the provided command under the memory checker.
if (BOOST_HANA_ENABLE_MEMCHECK)
    find_package(Valgrind REQUIRED)
    function(boost_hana_add_test name)
        add_test(${name} ${Valgrind_EXECUTABLE} --leak-check=full --error-exitcode=1 ${ARGN})
    endfunction()
else()
    function(boost_hana_add_test name)
        add_test(${name} ${ARGN})
    endfunction()
endif()


##############################################################################
# Setup the `check` target to build and then run all the tests and examples.
##############################################################################
add_custom_target(check-hana
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Build and then run all the tests and examples.")


##############################################################################
# Setup subdirectories
##############################################################################
enable_testing()
add_subdirectory(benchmark)
add_subdirectory(doc)
add_subdirectory(example)
add_subdirectory(experimental)
add_subdirectory(test)


##############################################################################
# Setup the 'install' target.
# This copies the whole content of include/ to ${CMAKE_INSTALL_PREFIX}.
##############################################################################
install(DIRECTORY include/boost DESTINATION include
        PATTERN ".DS_Store" EXCLUDE)

# We also install an optional pkg-config file.
configure_file(cmake/hana.pc.in hana.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hana.pc DESTINATION lib/pkgconfig)
