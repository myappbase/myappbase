/**
 *  @file
 *  @copyright defined in myappbase/LICENSE
 */
#pragma once

#include <iostream>
#include <queue>

#include <fc/io/json.hpp>
#include <fc/variant.hpp>
#include <fc/static_variant.hpp>

namespace my {
namespace help_http_client {

using namespace std;

namespace detail {
class http_context_impl;

struct http_context_deleter {
   void operator()(http_context_impl*) const;
};
} // namespace detail

using http_context = unique_ptr<detail::http_context_impl, detail::http_context_deleter>;

http_context create_http_context();

struct parsed_url {
   string scheme;
   string server;
   string port;
   string path;

   static string normalize_path(const string& path);

   parsed_url operator+(string sub_path) { return {scheme, server, port, path + sub_path}; }
};

parsed_url parse_url(const string& server_url);

struct resolved_url : parsed_url {
   resolved_url(const parsed_url& url, vector<string>&& resolved_addresses, uint16_t resolved_port, bool is_loopback)
       : parsed_url(url)
       , resolved_addresses(move(resolved_addresses))
       , resolved_port(resolved_port)
       , is_loopback(is_loopback) {}

   // used for unix domain, where resolving and ports are nonapplicable
   resolved_url(const parsed_url& url)
       : parsed_url(url) {}

   vector<string> resolved_addresses;
   uint16_t       resolved_port = 0;
   bool           is_loopback   = false;
};

resolved_url resolve_url(const http_context& context, const parsed_url& url);

struct connection_param {
   const http_context&  context;
   resolved_url         url;
   bool                 verify_cert;
   vector<string>& headers;

   connection_param(const http_context& context, const resolved_url& url, bool verify, vector<string>& h)
       : context(context)
       , url(url)
       , headers(h) {
      verify_cert = verify;
   }

   connection_param(const http_context& context, const parsed_url& url, bool verify, vector<string>& h)
       : context(context)
       , url(resolve_url(context, url))
       , headers(h) {
      verify_cert = verify;
   }
};

fc::variant do_http_call(const connection_param& cp, const string& body_data = "");

fc::variant do_http_call(const connection_param& cp, const fc::variant& body_data = fc::variant());

fc::variant do_http_call(const connection_param& cp, const std::map<std::string, fc::variant>& body_data);


//这是异步处理事件
struct async_result_visitor : public fc::visitor<fc::variant> {
   template <typename T>
   fc::variant operator()(const T& v) const {
      return fc::variant(v);
   }
};

const string chain_func_base             = "/v1/chain";
const string get_info_func               = chain_func_base + "/get_info";
const string push_txn_func               = chain_func_base + "/push_transaction";
const string push_txns_func              = chain_func_base + "/push_transactions";
const string json_to_bin_func            = chain_func_base + "/abi_json_to_bin";
const string get_block_func              = chain_func_base + "/get_block";
const string get_block_header_state_func = chain_func_base + "/get_block_header_state";
const string get_account_func            = chain_func_base + "/get_account";
const string get_table_func              = chain_func_base + "/get_table_rows";
const string get_table_by_scope_func     = chain_func_base + "/get_table_by_scope";
const string get_code_func               = chain_func_base + "/get_code";
const string get_code_hash_func          = chain_func_base + "/get_code_hash";
const string get_abi_func                = chain_func_base + "/get_abi";
const string get_raw_abi_func            = chain_func_base + "/get_raw_abi";
const string get_raw_code_and_abi_func   = chain_func_base + "/get_raw_code_and_abi";
const string get_currency_balance_func   = chain_func_base + "/get_currency_balance";
const string get_currency_stats_func     = chain_func_base + "/get_currency_stats";
const string get_producers_func          = chain_func_base + "/get_producers";
const string get_schedule_func           = chain_func_base + "/get_producer_schedule";
const string get_required_keys           = chain_func_base + "/get_required_keys";

const string history_log_func_base                 = "/v1/history_log";
const string get_last_block_in_block_log_fun       = history_log_func_base + "/get_range_in_block_log";
const string get_last_block_in_trans_trace_log_fun = history_log_func_base + "/get_range_in_trans_trace_log";

FC_DECLARE_EXCEPTION(connection_exception, 1100000, "Connection Exception");

struct call_result {
   fc::variant result;
   bool        status;
};

template <typename T>
call_result call(const string& url, const string& path, const T& v) {
   call_result r{fc::variant(), true};
   try {
      vector<string> headers;
      http_context   context = create_http_context();
      auto sp = std::make_unique<connection_param>(context, parse_url(url) + path, false, headers);
      r.result = do_http_call(*sp, fc::variant(v));
      return r;
   } catch (fc::exception& e) {
      elog("FC Exception : ${e}", ("e", e.to_string()));
      r.status = false;
      throw;
   } catch (std::exception& e) {
      elog("STD Exception : ${e}", ("e", e.what()));
      r.status = false;
      throw;
   } catch (...) {
      elog("Failed to connect to ${u}", ("u", url + path));
      r.status = false;
      throw;
   }
   return r;
}

template <typename T>
call_result call(const std::string& url, const T& v) {
   call_result r{fc::variant(), true};
   try {
      vector<string> headers;
      http_context   context = create_http_context();
      auto           sp      = std::make_unique<connection_param>(context, parse_url(url), true, headers);
      r.result = do_http_call(*sp, fc::variant(v));
      return r;
   } catch (fc::exception& e) {
      elog("FC Exception : ${e}", ("e", e.to_string()));
      r.status = false;
      throw;
   } catch (std::exception& e) {
      elog("STD Exception : ${e}", ("e", e.what()));
      r.status = false;
      throw;
   } catch (...) {
      elog("Failed to connect to ${u}", ("u", url));
      r.status = false;
      throw;
   }
   return r;
}

#define CALL(api_name, api_handle, call_name, INVOKE, http_response_code)                                              \
   {                                                                                                                   \
      std::string("/v1/" #api_name "/" #call_name),                                                                    \
          [api_handle](string, string body, url_response_callback cb) mutable {                                        \
             try {                                                                                                     \
                if (body.empty())                                                                                      \
                   body = "{}";                                                                                        \
                INVOKE                                                                                                 \
                cb(http_response_code, fc::variant(result));                                                           \
             } catch (...) {                                                                                           \
                http_plugin::handle_exception(#api_name, #call_name, body, cb);                                        \
             }                                                                                                         \
          }                                                                                                            \
   }

#define INVOKE_R_R(api_handle, call_name, in_param) \
   auto result = api_handle->call_name(fc::json::from_string(body).as<in_param>());

#define INVOKE_R_R_R(api_handle, call_name, in_param0, in_param1)         \
   const auto &vs = fc::json::json::from_string(body).as<fc::variants>(); \
   auto result = api_handle->call_name(vs.at(0).as<in_param0>(), vs.at(1).as<in_param1>());

#define INVOKE_R_R_R_R(api_handle, call_name, in_param0, in_param1, in_param2) \
   const auto &vs = fc::json::json::from_string(body).as<fc::variants>();      \
   auto result = api_handle->call_name(vs.at(0).as<in_param0>(), vs.at(1).as<in_param1>(), vs.at(2).as<in_param2>());

#define INVOKE_R_V(api_handle, call_name) \
   auto result = api_handle->call_name();

#define CALL_ASYNC_P1(api_name, api_handle, call_name, call_param, call_result, http_response_code)                       \
   {                                                                                                                   \
      std::string("/v1/" #api_name "/" #call_name),                                                                    \
          [api_handle](string, string body, url_response_callback cb) mutable {                                        \
             if (body.empty())                                                                                         \
                body = "{}";                                                                                           \
             api_handle->call_name(fc::json::from_string(body).as<call_param>(),                                       \
                                   [cb, body](const fc::static_variant<fc::exception_ptr, call_result>& result) {      \
                                      if (result.contains<fc::exception_ptr>()) {                                      \
                                         try {                                                                         \
                                            result.get<fc::exception_ptr>()->dynamic_rethrow_exception();              \
                                         } catch (...) {                                                               \
                                            http_plugin::handle_exception(#api_name, #call_name, body, cb);            \
                                         }                                                                             \
                                      } else {                                                                         \
                                         cb(http_response_code, result.visit(async_result_visitor()));                 \
                                      }                                                                                \
                                   });                                                                                 \
          }                                                                                                            \
   }

#define CALL_ASYNC(api_name, api_handle, call_name, call_result, http_response_code)                                   \
   {                                                                                                                   \
      std::string("/v1/" #api_name "/" #call_name),                                                                    \
          [api_handle](string, string body, url_response_callback cb) mutable {                                        \
             if (body.empty())                                                                                         \
                body = "{}";                                                                                           \
             api_handle->call_name([cb, body](const fc::static_variant<fc::exception_ptr, call_result>& result) {      \
                if (result.contains<fc::exception_ptr>()) {                                                            \
                   try {                                                                                               \
                      result.get<fc::exception_ptr>()->dynamic_rethrow_exception();                                    \
                   } catch (...) {                                                                                     \
                      http_plugin::handle_exception(#api_name, #call_name, body, cb);                                  \
                   }                                                                                                   \
                } else {                                                                                               \
                   cb(http_response_code, result.visit(async_result_visitor()));                                       \
                }                                                                                                      \
             });                                                                                                       \
          }                                                                                                            \
   }

} // namespace help_http_client
} // namespace my


FC_REFLECT(my::help_http_client::call_result, (result)(status))