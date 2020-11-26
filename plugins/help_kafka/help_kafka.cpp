#include <stdio.h>
#include <string>
#include <fc/crypto/hex.hpp>
#include <fc/variant.hpp>
#include <fc/io/datastream.hpp>
#include <fc/io/json.hpp>
#include <fc/io/console.hpp>
#include <fc/exception/exception.hpp>
#include <fc/variant_object.hpp>
#include <fc/static_variant.hpp>
#include <help_kafka/help_kafka.hpp>
#include <librdkafka/rdkafka.h>

namespace fc {  class variant; }

namespace my {
namespace help_kafka {

    static void dr_msg_cb(rd_kafka_t* rk, const rd_kafka_message_t* rkmessage, void* opaque) {
        if (rkmessage->err)
            fprintf(stderr, "%% Message delivery failed: %s\n", rd_kafka_err2str(rkmessage->err));
        // else
        //    fprintf(stderr,
        //            "%% Message delivered (%zd bytes, "
        //            "partition %" PRId32 ")\n",
        //            rkmessage->len, rkmessage->partition);

        /* The rkmessage is destroyed automatically by librdkafka */
    }


    bool send(const std::string& broker,
              const std::string& topic,
              const std::string& data) {
        rd_kafka_t*      rk;          /* Producer instance handle */
        rd_kafka_conf_t* conf;        /* Temporary configuration object */
        char             errstr[512]; /* librdkafka API error reporting buffer */

        try {

            conf = rd_kafka_conf_new();
            if (rd_kafka_conf_set(conf, "bootstrap.servers", broker.c_str(), errstr, sizeof(errstr)) !=
                RD_KAFKA_CONF_OK) {
                fprintf(stderr, "%s\n", errstr);
                return false;
            }

            rd_kafka_conf_set_dr_msg_cb(conf, dr_msg_cb);

            rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
            if (!rk) {
                fprintf(stderr, "%% Failed to create new producer: %s\n", errstr);
                return false;
            }

            for(int i = 0; i < 10 ; i++ ){

                rd_kafka_resp_err_t err = rd_kafka_producev(
                        rk, RD_KAFKA_V_TOPIC(topic.c_str()), RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
                        RD_KAFKA_V_VALUE((void*)(data.c_str()), data.size()), RD_KAFKA_V_OPAQUE(NULL), RD_KAFKA_V_END);

                if (!err) {
                    // ilog("Enqueued message (${b} bytes) for kafka topic ${t}", ("b",data.size()), ("t", topic.c_str()));
                    break;
                }

                elog("Failed to enqueue kafka topic ${b} : ${t}", ("b", topic.c_str())("t", std::string(rd_kafka_err2str(err))));
                ilog("\n-------------------------------------------"
                     "\n${b}"
                     "\n-------------------------------------------",
                     ("b", data));

                if (err != RD_KAFKA_RESP_ERR__QUEUE_FULL){
                    rd_kafka_destroy(rk);
                    return false;
                }

                wlog("the broker queue is full, wait 1 second, now...");
                rd_kafka_poll(rk, 1000);
            }

            rd_kafka_poll(rk, 0);
            rd_kafka_flush(rk, 10 * 1000);

            if (rd_kafka_outq_len(rk) > 0) {
                elog("${b} message(s) were not delivered", ("b", rd_kafka_outq_len(rk)));
                rd_kafka_destroy(rk);
                return false;
            }

            ilog("\n-------------------------------------------"
                 "\n${b}"
                 "\n-------------------------------------------",
                 ("b", data));

            rd_kafka_destroy(rk);
        } catch (...) {
            elog("Failed to enqueue kafka topic ${b}", ("b", topic.c_str()));
            ilog("\n-------------------------------------------"
                 "\n${b}"
                 "\n-------------------------------------------",
                 ("b", data));

            return false;
        }

        return true;
    }

    bool send(const std::string& broker,
              const std::string& topic,
              const fc::variant& postdata) {
        rd_kafka_t*      rk;          /* Producer instance handle */
        rd_kafka_conf_t* conf;        /* Temporary configuration object */
        char             errstr[512]; /* librdkafka API error reporting buffer */

        std::string postjson;
        if (!postdata.is_null())
            postjson = fc::json::to_pretty_string(postdata);

        try {

            conf = rd_kafka_conf_new();
            if (rd_kafka_conf_set(conf, "bootstrap.servers", broker.c_str(), errstr, sizeof(errstr)) !=
                RD_KAFKA_CONF_OK) {
                fprintf(stderr, "%s\n", errstr);
                return false;
            }

            rd_kafka_conf_set_dr_msg_cb(conf, dr_msg_cb);

            rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
            if (!rk) {
                fprintf(stderr, "%% Failed to create new producer: %s\n", errstr);
                return false;
            }

            for(int i = 0; i < 10 ; i++ ){

                rd_kafka_resp_err_t err = rd_kafka_producev(
                        rk, RD_KAFKA_V_TOPIC(topic.c_str()), RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
                        RD_KAFKA_V_VALUE((void*)(postjson.c_str()), postjson.size()), RD_KAFKA_V_OPAQUE(NULL), RD_KAFKA_V_END);

                if (!err) {
                    // ilog("Enqueued message (${b} bytes) for kafka topic ${t}", ("b",postjson.size()), ("t",topic.c_str()));
                    break;
                }

                elog("Failed to enqueue kafka topic ${b} : ${t}", ("b", topic.c_str())("t", std::string(rd_kafka_err2str(err))));
                ilog("\n-------------------------------------------"
                     "\n${b}"
                     "\n-------------------------------------------",
                     ("b", postjson));

                if (err != RD_KAFKA_RESP_ERR__QUEUE_FULL){
                    rd_kafka_destroy(rk);
                    return false;
                }

                wlog("the broker queue is full, wait 1 second, now...");
                rd_kafka_poll(rk, 1000);
            }

            rd_kafka_poll(rk, 0);
            rd_kafka_flush(rk, 10 * 1000);

            if (rd_kafka_outq_len(rk) > 0) {
                elog("${b} message(s) were not delivered", ("b", rd_kafka_outq_len(rk)));
                rd_kafka_destroy(rk);
                return false;
            }

            ilog("\n-------------------------------------------"
                 "\n${b}"
                 "\n-------------------------------------------",
                 ("b", postjson));

            rd_kafka_destroy(rk);
        } catch (...) {
            elog("Failed to enqueue kafka topic ${b}", ("b", topic.c_str()));
            ilog("\n-------------------------------------------"
                 "\n${b}"
                 "\n-------------------------------------------",
                 ("b", postjson));

            return false;
        }

        return true;
    }

} // namespace help_kafka
} // namespace my
