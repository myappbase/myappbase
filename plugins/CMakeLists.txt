add_subdirectory(plugin_help)
add_subdirectory(help_http)
add_subdirectory(help_http_client)
add_subdirectory(help_kafka)
add_subdirectory(help_mongo)
add_subdirectory(help_mqtt)
add_subdirectory(help_jpush)

add_subdirectory(http_plugin)
add_subdirectory(mqtt2kafka_plugin)
add_subdirectory(mqtt2mongo_plugin)

# Forward variables to top level so packaging picks them up
set(CPACK_DEBIAN_PACKAGE_DEPENDS ${CPACK_DEBIAN_PACKAGE_DEPENDS} PARENT_SCOPE)
