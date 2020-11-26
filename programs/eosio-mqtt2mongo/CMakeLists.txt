add_executable( ${EOSIO_MQTT2MONGO_EXECUTABLE_NAME} main.cpp )
if( UNIX AND NOT APPLE )
  set(rt_library rt )
endif()

#SET( CMAKE_VERBOSE_MAKEFILE on )

find_package( Gperftools QUIET )
if( GPERFTOOLS_FOUND )
    message( STATUS "Found gperftools; compiling ${EOSIO_MQTT2MONGO_EXECUTABLE_NAME} with TCMalloc")
    list( APPEND PLATFORM_SPECIFIC_LIBS tcmalloc )
endif()

target_link_libraries( ${EOSIO_MQTT2MONGO_EXECUTABLE_NAME}
        PRIVATE appbase
        PRIVATE fc
        PRIVATE mqtt2mongo_plugin
        PRIVATE rt
        )

target_include_directories( ${EOSIO_MQTT2MONGO_EXECUTABLE_NAME}
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

mas_sign(${EOSIO_MQTT2MONGO_EXECUTABLE_NAME})

copy_bin( ${EOSIO_MQTT2MONGO_EXECUTABLE_NAME} )
install( TARGETS
   ${EOSIO_MQTT2MONGO_EXECUTABLE_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR} COMPONENT base
)
