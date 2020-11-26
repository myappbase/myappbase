#pragma once
#include <appbase/application.hpp>
#include <fc/exception/exception.hpp>
#include <fc/variant.hpp>
#include <fc/reflect/reflect.hpp>

namespace my {
namespace help_http {
   using namespace appbase;

   class AAA {
   public:
       int a ;
       string b ;
   };


   /**
    * @brief A callback function provided to a URL handler to
    * allow it to specify the HTTP response code and body
    *
    * Arguments: response_code, response_body
    */
   using url_response_callback = std::function<void(int,fc::variant)>;

   /**
    * @brief Callback type for a URL handler
    *
    * URL handlers have this type
    *
    * The handler must gaurantee that url_response_callback() is called;
    * otherwise, the connection will hang and result in a memory leak.
    *
    * Arguments: url, request_body, response_callback
    **/
   using url_handler = std::function<void(string,string,url_response_callback)>;

   /**
    * @brief An API, containing URLs and handlers
    *
    * An API is composed of several calls, where each call has a URL and
    * a handler. The URL is the path on the web server that triggers the
    * call, and the handler is the function which implements the API call
    */
   using api_description = std::map<string, url_handler>;

   struct http_plugin_defaults {
      //If empty, unix socket support will be completely disabled. If not empty,
      // unix socket support is enabled with the given default path (treated relative
      // to the datadir)
      string default_unix_socket_path;
      //If non 0, HTTP will be enabled by default on the given port number. If
      // 0, HTTP will not be enabled by default
      uint16_t default_http_port{0};
   };

   /**
    * @brief Structure used to create JSON error responses
    */
   struct error_results {
      uint16_t code;
      string message;

      struct error_info {
         int64_t code;
         string name;
         string what;

         struct error_detail {
            string message;
            string file;
            uint64_t line_number;
            string method;
         };

         vector<error_detail> details;

         static const uint8_t details_limit = 10;

         error_info() {};

         error_info(const fc::exception& exc, bool include_full_log) {
            code = exc.code();
            name = exc.name();
            what = exc.what();
            uint8_t limit = include_full_log ? details_limit : 1;
            for( auto itr = exc.get_log().begin(); itr != exc.get_log().end(); ++itr ) {
               // Prevent sending trace that are too big
               if( details.size() >= limit ) break;
               // Append error
               error_detail detail = {
                     include_full_log ? itr->get_message() : itr->get_limited_message(),
                     itr->get_context().get_file(),
                     itr->get_context().get_line_number(),
                     itr->get_context().get_method()
               };
               details.emplace_back( detail );
            }
         }
      };

      error_info error;
   };
} //help_http
} //my

FC_REFLECT(my::help_http::error_results::error_info::error_detail, (message)(file)(line_number)(method))
FC_REFLECT(my::help_http::error_results::error_info, (code)(name)(what)(details))
FC_REFLECT(my::help_http::error_results, (code)(message)(error))
FC_REFLECT(my::help_http::AAA, (a)(b))
