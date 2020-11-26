/**
 *  @file
 *  @copyright defined in myappbase/LICENSE
 */
#include <mqtt2kafka_plugin/mqtt2kafka_plugin.hpp>
#include <pwd.h>
#include <string>
#include <vector>
#include <regex>
#include <iostream>

#include <fc/crypto/hex.hpp>
#include <fc/variant.hpp>
#include <fc/io/datastream.hpp>
#include <fc/io/json.hpp>
#include <fc/io/console.hpp>
#include <fc/exception/exception.hpp>
#include <fc/variant_object.hpp>
#include <fc/static_variant.hpp>

#pragma push_macro("N")
#undef N

#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/process/spawn.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/algorithm/string/classification.hpp>

#pragma pop_macro("N")

#include <fc/io/fstream.hpp>

#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <cctype>
#include <thread>
#include <chrono>

#include <signal.h>
#include <string.h>
#include "mqtt/async_client.h"
#include <help_kafka/help_kafka.hpp>
#include <help_mqtt/help_mqtt.hpp>

namespace my
{
    using namespace std;
    using namespace help_mqtt;
    using namespace help_kafka;

    static appbase::abstract_plugin &_mqtt_api_plugin = app().register_plugin<mqtt2kafka_plugin>();

    //////////////// kafka ////////////////
    std::string kafka_broker = "localhost:9092";
    std::string kafka_topic  = "action_trace";

    class mycallback : public virtual mqtt::callback,
                       public virtual mqtt::iaction_listener

    {
        mqtt::async_client& cli_;
        mqtt::connect_options& connOpts_;
        action_listener subListener_ = action_listener("Subscription");

        std::string server;
        std::string topic;
        std::string client_id;

        int	qos = 1;
        int nretry_ = 0;

    public:
        void reconnect() {
            std::this_thread::sleep_for(std::chrono::milliseconds(2500));
            try {
                cli_.connect(connOpts_, nullptr, *this);
            }
            catch (const mqtt::exception& exc) {
                std::cerr << "Error: " << exc.what() << std::endl;
                exit(1);
            }
        }

        // Re-connection failure
        void on_failure(const mqtt::token& tok) override {
            std::cout << "Connection attempt failed" << std::endl;
            if (++nretry_ > 5 )
                exit(1);
            reconnect();
        }

        void on_success(const mqtt::token& tok) override {}

        void connected(const std::string& cause) override {
            std::cout << "\nConnection success, Subscribing to topic '" << topic << "'\n"
                      << "\tfor client " << client_id
                      << " using QoS" << qos << std::endl;
            cli_.subscribe(topic, qos, nullptr, subListener_);
        }

        void connection_lost(const std::string& cause) override {
            std::cout << "\nConnection lost" << std::endl;
            if (!cause.empty())
                std::cout << "\tcause: " << cause << std::endl;

            std::cout << "Reconnecting..." << std::endl;
            nretry_ = 0;
            reconnect();
        }

        void message_arrived(mqtt::const_message_ptr msg) override {
            std::cout << "Message arrived" << std::endl;
            std::cout << "\ttopic: '" << msg->get_topic() << "'" << std::endl;
            std::cout << "\tpayload: '" << msg->to_string() << "'\n" << std::endl;
            help_kafka::send(kafka_broker, kafka_topic, msg->to_string());
        }

        void delivery_complete(mqtt::delivery_token_ptr token) override {}

        mycallback(mqtt::async_client& cli,
                   mqtt::connect_options& connOpts,
                   std::string& server_,
                   std::string& topic_,
                   std::string& client_id_):
                cli_(cli),
                connOpts_(connOpts),
                server(server_),
                topic(topic_),
                client_id(client_id_)
        {}

    };


    ////////////////////////////////////////////////////////////////////////////////////////
    class mqtt_instance : std::enable_shared_from_this<mqtt_instance> {
    public:
        std::string mqtt_server = "tcp://post-cn-n6w1qkoir06.mqtt.aliyuncs.com:1883";
        std::string mqtt_topic = "eosio_topic" ;
        std::string mqtt_client = "GID_eosio@@@kafka";
        std::string mqtt_user = "Signature|LTAI4GJKsfb8VexSkcQodggS|post-cn-n6w1qkoir06";
        std::string mqtt_password = "5rQ0uE7a8vIYd+MtyeZutAmtq38=" ;
        fc::optional<mqtt::async_client> client;

        bool subscribe() {
            mqtt::connect_options connOpts = mqtt::connect_options(mqtt_user, mqtt_password);
            connOpts.set_keep_alive_interval(20);
            connOpts.set_clean_session(true);

            mycallback cb(*client, connOpts, mqtt_server, mqtt_topic, mqtt_client );
            client->set_callback(cb);
            try {
                std::cout << "Connecting to the MQTT server..." << std::flush;
                client->connect(connOpts, nullptr, cb);
            }
            catch (const mqtt::exception&) {
                std::cerr << "\nERROR: Unable to connect to MQTT server: '"
                          << mqtt_server << "'" << std::endl;
                return false;
            }

            return true;
        }

        void close() {
            try {
                std::cout << "\nDisconnecting from the MQTT server..." << std::flush;
                client->disconnect()->wait();
                std::cout << "OK" << std::endl;
            }
            catch (const mqtt::exception& exc) {
                std::cerr << exc.what() << std::endl;
            }
        }

    }; // class mqtt_instance


    mqtt2kafka_plugin::mqtt2kafka_plugin()
            : mqtt(std::make_shared<mqtt_instance>()) {}

    void mqtt2kafka_plugin::set_program_options(options_description& cli, options_description& cfg) {
        auto options = cfg.add_options();
        options("kafka-broker", bpo::value<string>()->default_value("localhost:9092"), "the kafka broker server address.");
        options("kafka-topic", bpo::value<string>()->default_value("action_trace"), "the kafka kafka topic.");
        options("mqtt-server", bpo::value<std::string>(), " Example: tcp://post-cn-n6w1qkoir06.mqtt.aliyuncs.com:1883");
        options("mqtt-topic", bpo::value<std::string>(), " Example: eosio_topic");
        options("mqtt-client", bpo::value<std::string>(), " Example: GID_eosio@@@kafka");
        options("mqtt-user", bpo::value<std::string>(), " Example: Signature|LTAI4GJKsfb8VexSkcQodggS|post-cn-n6w1qkoir06");
        options("mqtt-password", bpo::value<std::string>(), " Example: 5rQ0uE7a8vIYd+MtyeZutAmtq38=");
    }

    void mqtt2kafka_plugin::plugin_initialize(const variables_map &options) {
        ilog("initializing...");
        try
        {
            if (options.count("kafka-broker"))
                kafka_broker = options.at("kafka-broker").as<string>();

            if (options.count("kafka-topic"))
                kafka_topic  = options.at("kafka-topic").as<string>();

            if (options.count("mqtt-server"))
                mqtt->mqtt_server = options.at("mqtt-server").as<std::string>();

            if( options.count("mqtt-topic"))
                mqtt->mqtt_topic = options.at("mqtt-topic").as<std::string>();

            if( options.count("mqtt-client"))
                mqtt->mqtt_client = options.at("mqtt-client").as<std::string>();

            if( options.count("mqtt-user"))
                mqtt->mqtt_user = options.at("mqtt-user").as<std::string>();

            if( options.count("mqtt-password"))
                mqtt->mqtt_password = options.at("mqtt-password").as<std::string>();

            mqtt->client.emplace(mqtt->mqtt_server,mqtt->mqtt_client);
        }
        FC_LOG_AND_RETHROW()
    }

    void mqtt2kafka_plugin::plugin_startup()
    {
        ilog("starting...");
        mqtt->subscribe();
    }

    void mqtt2kafka_plugin::plugin_shutdown() {
        ilog("shutdown...");
        mqtt->close();
    }

} // namespace my
