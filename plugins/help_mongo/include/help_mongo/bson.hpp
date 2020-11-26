#pragma once

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/types.hpp>

#include <fc/variant.hpp>
#include <fc/exception/exception.hpp>

#include <bson/bson.h>

namespace my {
    namespace help_mongo {
        void to_bson(const fc::variant_object &o, bsoncxx::builder::core &c);
        void to_bson(const fc::variants &v, bsoncxx::builder::core &c);
        void to_bson(const fc::variant &v, bsoncxx::builder::core &c);
        bsoncxx::document::value to_bson(const fc::variant &v);

        void from_bson(const bsoncxx::document::view &view, fc::mutable_variant_object &o);
        void from_bson(const bsoncxx::array::view &bson_array, fc::variants &a);
        template<typename T> void from_bson(const T &ele, fc::variant &v);
        fc::variant from_bson(const bsoncxx::document::view &view);
    }
} // namespace my

