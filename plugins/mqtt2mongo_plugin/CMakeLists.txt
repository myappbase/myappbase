find_package(libmongoc-1.0 1.8)

if (libmongoc-1.0_FOUND)

    find_package(libbson-1.0 REQUIRED)
    message(STATUS "Found bson headers: ${BSON_INCLUDE_DIRS}")

    find_package(libbsoncxx-static REQUIRED)
    find_package(libmongocxx-static REQUIRED)
    find_package(libmongoc-static-1.0 REQUIRED)
    find_package(libbson-static-1.0 REQUIRED)

else()
    message(FATAL_ERROR "Could NOT find mongo-c-driver. Disable mongo support or ensure mongo-c-driver and mongo-cxx-driver is built and installed")
    return()
endif()

# This needs to be after the else/return in the situation that libmongoc isn't found and we need to avoid building mongo :: 'bsoncxx/builder/basic/kvp.hpp' file not found
file(GLOB HEADERS "include/my/mqtt2mongo_plugin/*.hpp")


add_library( mqtt2mongo_plugin
        mqtt2mongo_plugin.cpp
        ${HEADERS} )

target_include_directories(mqtt2mongo_plugin
        PRIVATE ${LIBMONGOCXX_STATIC_INCLUDE_DIRS}
        PRIVATE ${LIBBSONCXX_STATIC_INCLUDE_DIRS}
        PRIVATE ${BSON_INCLUDE_DIRS}
        PRIVATE "/root/myapp/2.0/include/"
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
        )

target_compile_definitions(mqtt2mongo_plugin
        PRIVATE ${LIBMONGOCXX_STATIC_DEFINITIONS}
        PRIVATE ${LIBBSONCXX_STATIC_DEFINITIONS}
        )

foreach(MONGO_S_LIB ${MONGOC_STATIC_LIBRARIES})
    string(REGEX MATCH "libsasl2\\${CMAKE_SHARED_LIBRARY_SUFFIX}$" REGOUT ${MONGO_S_LIB})
    if(REGOUT)
        set(LINK_SASL "sasl2")
    endif()

    string(REGEX MATCH "libicuuc\\${CMAKE_SHARED_LIBRARY_SUFFIX}$" REGOUT ${MONGO_S_LIB})
    if(REGOUT)
        set(LINK_ICU "icuuc")
    endif()

    string(REGEX MATCH "libsnappy\\${CMAKE_SHARED_LIBRARY_SUFFIX}$" REGOUT ${MONGO_S_LIB})
    if(REGOUT)
        set(LINK_SNAPPY "snappy")
    endif()
endforeach()

target_link_libraries(mqtt2mongo_plugin
        appbase
        fc
        help_mqtt
        help_mongo
        ${LIBMONGOCXX_STATIC_LIBRARY_PATH} ${LIBBSONCXX_STATIC_LIBRARY_PATH}
        ${MONGOC_STATIC_LIBRARY} ${BSON_STATIC_LIBRARY}
        resolv ${LINK_SASL} ${LINK_ICU} ${LINK_SNAPPY}
        )
