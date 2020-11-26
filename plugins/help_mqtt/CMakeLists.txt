file(GLOB HEADERS "include/help_mqtt/*.hpp")
add_library( help_mqtt
        help_mqtt.cpp
        ${HEADERS} )

target_include_directories( help_mqtt
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
        PUBLIC "/root/myapp/2.0/include/")

target_link_libraries(
        help_mqtt
        paho-mqtt3as
        /usr/local/lib/libpaho-mqttpp3.a
)

