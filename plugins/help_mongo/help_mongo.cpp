#include <string>
#include <iostream>
#include <fc/io/json.hpp>
#include <fc/log/logger_config.hpp>
#include <fc/variant.hpp>
#include <fc/exception/exception.hpp>

#include <help_mongo/help_mongo.hpp>
#include <bsoncxx/types.hpp>

namespace my {
namespace help_mongo {

    using namespace bsoncxx::types;
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;

    bsoncxx::oid make_custom_oid()
    {
        bsoncxx::oid x = bsoncxx::oid();
        const char *p = x.bytes();
        std::swap((short &)p[0], (short &)p[10]);
        return x;
    }

    uint32_t get_last_block(mongocxx::collection &col)
    {
        try
        {
            mongocxx::options::find opts{};
            opts.sort(make_document(kvp("block_num", -1)));
            auto doc = col.find_one({}, opts);
            if (doc)
                return static_cast<uint32_t>(doc->view()["block_num"].get_int64());
        }
        catch (fc::exception &e)
        {
            elog("FC Exception : ${e}", ("e", e.to_string()));
        }
        catch (std::exception &e)
        {
            elog("STD Exception : ${e}", ("e", e.what()));
        }
        catch (...)
        {
            elog("Unknown Exception");
        }
        return 1;
    }

    std::vector<bsoncxx::document::view> get_objects(mongocxx::collection &col, const uint32_t block_num)
    {
        std::vector<bsoncxx::document::view> objects;
        try
        {
            auto find_doc = bsoncxx::builder::basic::document();
            find_doc.append(kvp("block_num", b_int64{static_cast<int64_t>(block_num)}));
            auto cursor = col.find(find_doc.view());
            // Iterate over the cursor.
            for (auto&& doc : cursor) {
                objects.push_back(doc);
            }

        }
        catch (fc::exception &e)
        {
            elog("FC Exception : ${e}", ("e", e.to_string()));
        }
        catch (std::exception &e)
        {
            elog("STD Exception : ${e}", ("e", e.what()));
        }
        catch (...)
        {
            elog("Unknown Exception");
        }

        return objects;
    }

    void handle_mongo_exception(const std::string &desc, int line_num)
    {
        try
        {
            try
            {
                throw;
            }
            catch (mongocxx::logic_error &e)
            {
                // logic_error on invalid key, do not shutdown
                wlog("mongo logic error, ${desc}, line ${line}, code ${code}, ${what}",
                     ("desc", desc)("line", line_num)("code", e.code().value())("what", e.what()));
            }
            catch (mongocxx::operation_exception &e)
            {
                elog("mongo exception, ${desc}, line ${line}, code ${code}, ${details}",
                     ("desc", desc)("line", line_num)("code", e.code().value())("details", e.code().message()));
                if (e.raw_server_error())
                {
                    elog("  raw_server_error: ${e}", ("e", bsoncxx::to_json(e.raw_server_error()->view())));
                }
            }
            catch (mongocxx::exception &e)
            {
                elog("mongo exception, ${desc}, line ${line}, code ${code}, ${what}",
                     ("desc", desc)("line", line_num)("code", e.code().value())("what", e.what()));
            }
            catch (bsoncxx::exception &e)
            {
                elog("bsoncxx exception, ${desc}, line ${line}, code ${code}, ${what}",
                     ("desc", desc)("line", line_num)("code", e.code().value())("what", e.what()));
            }
            catch (fc::exception &er)
            {
                elog("mongo fc exception, ${desc}, line ${line}, ${details}",
                     ("desc", desc)("line", line_num)("details", er.to_detail_string()));
            }
            catch (const std::exception &e)
            {
                elog("mongo std exception, ${desc}, line ${line}, ${what}",
                     ("desc", desc)("line", line_num)("what", e.what()));
            }
            catch (...)
            {
                elog("mongo unknown exception, ${desc}, line ${line_nun}", ("desc", desc)("line_num", line_num));
            }
        }
        catch (...)
        {
            std::cerr << "Exception attempting to handle exception for " << desc << " " << line_num << std::endl;
        }
    }

} // namespace help_mongo
} // namespace my
