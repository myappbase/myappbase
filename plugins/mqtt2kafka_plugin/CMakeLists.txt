file(GLOB HEADERS "include/mqtt2kafka_plugin/*.hpp")
add_library( mqtt2kafka_plugin
		mqtt2kafka_plugin.cpp
		${HEADERS} )

target_include_directories( mqtt2kafka_plugin
		PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

target_link_libraries( mqtt2kafka_plugin
		appbase
		fc
		help_mqtt
		help_kafka
)