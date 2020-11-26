file(GLOB HEADERS "include/help_jpush/*.hpp" "jpush.h" "cJSON/cJSON.h")
add_library(
        help_jpush
        help_jpush.cpp
        jpush.c
        cJSON/cJSON.c
        ${HEADERS})

target_include_directories(
        help_jpush
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "/root/myapp/2.0/include/"
)

target_link_libraries(
        help_jpush
        plugin_help
        appbase
        fc
)
