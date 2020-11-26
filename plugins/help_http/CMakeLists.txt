file(GLOB HEADERS "include/help_http/*.hpp")
add_library(
        help_http
        help_http.cpp
        ${HEADERS}
)

target_link_libraries(
        help_http
        appbase
        fc
)

target_include_directories(
        help_http
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
