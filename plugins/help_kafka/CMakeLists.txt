file(GLOB HEADERS "include/help_kafka/*.hpp")
add_library( help_kafka
        help_kafka.cpp
        ${HEADERS} )

target_include_directories( help_kafka
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
        PUBLIC "/root/myapp/2.0/include/")

target_link_libraries(
        help_kafka
        fc
        /root/myapp/2.0/lib/librdkafka.a
)

