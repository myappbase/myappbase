cmake_minimum_required( VERSION 3.8 )

project( EOSIO )
include(CTest) # suppresses DartConfiguration.tcl error
enable_testing()

if ("${CMAKE_INSTALL_PREFIX}" STREQUAL "/usr/local")
   message(WARNING "CMAKE_INSTALL_PREFIX is set to /usr/local. This is not recommended.")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

include( GNUInstallDirs )
include( InstallDirectoryPermissions )
include( MASSigning )

set( BLOCKCHAIN_NAME "EOSIO" )
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_EXTENSIONS ON )
set( CXX_STANDARD_REQUIRED ON)

set(VERSION_MAJOR 2)
set(VERSION_MINOR 0)
set(VERSION_PATCH 2)
set(VERSION_FULL "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

set( EOSIO_MQTT2KAFKA_EXECUTABLE_NAME myapp-mqtt2kafka )
set( EOSIO_MQTT2MONGO_EXECUTABLE_NAME myapp-mqtt2mongo )

# http://stackoverflow.com/a/18369825
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0)
        message(FATAL_ERROR "GCC version must be at least 7.0!")
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0)
        message(FATAL_ERROR "Clang version must be at least 5.0!")
    endif()
endif()

if ("${CMAKE_GENERATOR}" STREQUAL "Ninja")
   add_compile_options(-fdiagnostics-color=always)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
set(BUILD_DOXYGEN FALSE CACHE BOOL "Build doxygen documentation on every make")

# add defaults for openssl
if(APPLE AND UNIX AND "${OPENSSL_ROOT_DIR}" STREQUAL "")
   set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl@1.1")
endif()

set( Boost_USE_STATIC_LIBS ON CACHE STRING "ON or OFF" )
# Most boost deps get implictly picked up via fc, as just about everything links to fc. In addition we pick up
# the pthread dependency through fc.
find_package(Boost 1.67 REQUIRED COMPONENTS program_options unit_test_framework)

# Linux Specific Options Here
message( STATUS "Configuring MyAppbase on Linux" )
set( CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS} -Wall" )
if ( FULL_STATIC_BUILD )
  set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
endif ( FULL_STATIC_BUILD )

if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
    if( CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.0.0 OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.0.0 )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-partial-specialization" )
    endif()
endif()

# based on http://www.delorie.com/gnu/docs/gdb/gdb_70.html
# uncomment this line to tell GDB about macros (slows compile times)
# set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-2 -g3" )

set(ENABLE_COVERAGE_TESTING FALSE CACHE BOOL "Build EOSIO for code coverage analysis")

include(utils)

add_subdirectory( libraries )
add_subdirectory( plugins )
add_subdirectory( programs )
add_subdirectory( scripts )
add_subdirectory( tools )

install_directory_permissions(DIRECTORY ${CMAKE_INSTALL_FULL_SYSCONFDIR}/myapp)

install_directory_permissions(DIRECTORY ${CMAKE_INSTALL_FULL_SYSCONFDIR}/myapp)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/my.version.in ${CMAKE_CURRENT_BINARY_DIR}/myappbase.version.hpp)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/my.version.hpp DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR})

set(EOS_ROOT_DIR ${CMAKE_BINARY_DIR})
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/my-config.cmake.in ${CMAKE_BINARY_DIR}/lib/cmake/myappbase/my-config.cmake @ONLY)

set(EOS_ROOT_DIR ${CMAKE_INSTALL_PREFIX})
configure_file(${CMAKE_SOURCE_DIR}/CMakeModules/my-config.cmake.in ${CMAKE_BINARY_DIR}/modules/my-config.cmake @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/modules/my-config.cmake DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/myappbase)

configure_file(${CMAKE_SOURCE_DIR}/libraries/softfloat/COPYING.txt
               ${CMAKE_BINARY_DIR}/licenses/myappbase/LICENSE.softfloat COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/libraries/fc/secp256k1/upstream/COPYING
               ${CMAKE_BINARY_DIR}/licenses/myappbase/LICENSE.secp256k1 COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/libraries/fc/src/network/LICENSE.go
               ${CMAKE_BINARY_DIR}/licenses/myappbase/LICENSE.go COPYONLY)

install(FILES LICENSE DESTINATION ${CMAKE_INSTALL_FULL_DATAROOTDIR}/licenses/myappbase/ COMPONENT base)
install(FILES libraries/softfloat/COPYING.txt DESTINATION ${CMAKE_INSTALL_FULL_DATAROOTDIR}/licenses/myappbase/ RENAME LICENSE.softfloat COMPONENT base)
install(FILES libraries/fc/secp256k1/upstream/COPYING DESTINATION ${CMAKE_INSTALL_FULL_DATAROOTDIR}/licenses/myappbase/ RENAME LICENSE.secp256k1 COMPONENT base)
install(FILES libraries/fc/src/network/LICENSE.go DESTINATION ${CMAKE_INSTALL_FULL_DATAROOTDIR}/licenses/myappbase/ COMPONENT base)

add_custom_target(base-install
  COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}"
  COMMAND "${CMAKE_COMMAND}" -DCMAKE_INSTALL_COMPONENT=base -P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
  USES_TERMINAL
)

get_property(_CTEST_CUSTOM_TESTS_IGNORE GLOBAL PROPERTY CTEST_CUSTOM_TESTS_IGNORE)

include(package)
include(doxygen)
