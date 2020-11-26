add_executable( ${MYAPP_MQTT2KAFKA_EXECUTABLE_NAME} main.cpp )
if( UNIX AND NOT APPLE )
  set(rt_library rt )
endif()

#SET( CMAKE_VERBOSE_MAKEFILE on )

find_package( Gperftools QUIET )
if( GPERFTOOLS_FOUND )
    message( STATUS "Found gperftools; compiling ${MYAPP_MQTT2KAFKA_EXECUTABLE_NAME} with TCMalloc")
    list( APPEND PLATFORM_SPECIFIC_LIBS tcmalloc )
endif()

target_link_libraries( ${MYAPP_MQTT2KAFKA_EXECUTABLE_NAME}
        PRIVATE appbase
        PRIVATE mqtt2kafka_plugin
        PRIVATE ${CMAKE_DL_LIBS} ${PLATFORM_SPECIFIC_LIBS}
        )
target_include_directories(${MYAPP_MQTT2KAFKA_EXECUTABLE_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

mas_sign(${MYAPP_MQTT2KAFKA_EXECUTABLE_NAME})

copy_bin( ${MYAPP_MQTT2KAFKA_EXECUTABLE_NAME} )
install( TARGETS
   ${MYAPP_MQTT2KAFKA_EXECUTABLE_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR} COMPONENT base
)
