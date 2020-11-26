file(GLOB HEADERS "include/help_http_client/*.hpp")
add_library(
        help_http_client
        help_http_client.cpp
        ${HEADERS}
)

target_include_directories(
        help_http_client
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
        help_http_client
        help_http
        plugin_help
        appbase
        fc
)

