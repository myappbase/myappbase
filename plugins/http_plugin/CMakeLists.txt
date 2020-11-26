file(GLOB HEADERS "include/http_plugin/*.hpp")
add_library(
        http_plugin
        http_plugin.cpp
        ${HEADERS}
)

target_include_directories(
        http_plugin
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
        http_plugin
        help_http
        plugin_help
        appbase
        fc
)