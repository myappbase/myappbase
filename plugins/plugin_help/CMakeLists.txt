file(GLOB HEADERS "include/plugin_help/*.hpp")
add_library(plugin_help
            lock.cpp
            ${HEADERS})

target_include_directories(plugin_help
                           PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
                           PUBLIC "/root/myapp/2.0/include/")

target_link_libraries(
        plugin_help
        appbase
        fc
)

