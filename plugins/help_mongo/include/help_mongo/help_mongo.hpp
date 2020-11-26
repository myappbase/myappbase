/**
*  @file
*  @copyright defined in eos/LICENSE
*/
#pragma once
#include <string>

#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/exception/logic_error.hpp>

#include <help_mongo/bson.hpp>


namespace my {
    namespace help_mongo {
        bsoncxx::oid make_custom_oid();
        uint32_t get_last_block(mongocxx::collection &);
        std::vector<bsoncxx::document::view> get_objects(
                mongocxx::collection &col,
                const uint32_t);
        void handle_mongo_exception(const std::string &desc, int line_num);
    } // namespace help_mongo
} // namespace my

