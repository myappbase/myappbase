#pragma once

#include <appbase/application.hpp>
#include <fc/exception/exception.hpp>

#include <fc/reflect/reflect.hpp>
#include <help_http/help_http.hpp>

namespace my {
    using namespace appbase;
    using namespace my::help_http;

    class http_plugin : public appbase::plugin<http_plugin> {
    public:
        http_plugin();

        virtual ~http_plugin();

        //must be called before initialize
        static void set_defaults(const http_plugin_defaults config);

        APPBASE_PLUGIN_REQUIRES()

        virtual void set_program_options(options_description &, options_description &cfg) override;

        void plugin_initialize(const variables_map &options);

        void plugin_startup();

        void plugin_shutdown();

        void handle_sighup() override;

        void add_handler(const string &url, const url_handler &);

        void add_api(const api_description &api) {
            for (const auto &call : api)
                add_handler(call.first, call.second);
        }

        // standard exception handling for api handlers
        static void handle_exception(
                const char *api_name,
                const char *call_name,
                const string &body,
                url_response_callback cb);

        bool is_on_loopback() const;

        bool is_secure() const;

        bool verbose_errors() const;

        struct get_supported_apis_result {
            vector<string> apis;
        };

        get_supported_apis_result get_supported_apis() const;

    private:
        std::shared_ptr<class http_plugin_impl> my;
    };

} //namespace my

FC_REFLECT(my::http_plugin::get_supported_apis_result, (apis))
