#include <mqtt2mongo_plugin/mqtt2mongo_plugin.hpp>

#include <fc/io/json.hpp>
#include <fc/log/logger_config.hpp>
#include <fc/variant.hpp>

#include <help_mqtt/help_mqtt.hpp>
#include <help_mongo/help_mongo.hpp>

namespace my
{
    using namespace std;
    using namespace help_mongo;
    using namespace help_mqtt;

    using namespace bsoncxx::types;
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;

    static appbase::abstract_plugin &_mqtt2mongo_plugin = app().register_plugin<mqtt2mongo_plugin>();

    class mongo_instance
    {
    public:
        std::string mongod_uri = "mongodb://127.0.0.1:27017/datastore";
        std::string db_name = "datastore";
        mongocxx::instance mongo_inst;
        fc::optional<mongocxx::pool> mongo_pool;

        mongocxx::collection _datastore_col;
        std::atomic_bool startup{true};
        const std::string datastore_collection_name = "datastore";

        void store_datastore(const std::string& data)
        {
            auto doc = bsoncxx::builder::basic::document{};

            ilog("data=${d}",("d", data));
            try
            {
                doc.append(kvp("_id", help_mongo::make_custom_oid()));
                fc::variant v = fc::json::from_string(data);
                doc.append(bsoncxx::builder::concatenate_doc{to_bson(v)});
            }
            catch (...)
            {
                help_mongo::handle_mongo_exception("create bson doc", __LINE__);
                elog( "${d}", ("d", data));
                return ;
            }

            try
            {
                auto result = _datastore_col.insert_one(doc.view());
                if( !result)
                    elog( "Failed to insert datastore ${d}", ("d", data));
            }
            catch (...)
            {
                help_mongo::handle_mongo_exception("datastore insert", __LINE__);
                elog( "${d}", ("d", data));
            }
        }

        void start()
        {
            ilog("init mongo");
            try
            {
                auto mongo_client = mongo_pool->acquire();
                auto &mongo_conn = *mongo_client;

                try
                {
                    _datastore_col = mongo_conn[db_name][datastore_collection_name];
                    _datastore_col.create_index(bsoncxx::from_json(R"xxx({ "id" : 1, "_id" : 1 })xxx"));
                }
                catch (...)
                {
                    help_mongo::handle_mongo_exception("create indexes", __LINE__);
                    ilog("quit now ...");
                    raise(SIGINT);
                    return;
                }
            }
            catch (...)
            {
                help_mongo::handle_mongo_exception("mongo init", __LINE__);
                ilog("quit now ...");
                raise(SIGINT);
                return;
            }

            startup = false;
        }

        mongo_instance()
        {
        }

        ~mongo_instance()
        {
            if (!startup)
            {
                try
                {
                    mongo_pool.reset();
                }
                catch (std::exception &e)
                {
                    elog("Exception on mqtt2mongo_plugin shutdown of consume thread: ${e}", ("e", e.what()));
                }
            }
        }

    }; // class mongo_instance

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

        std::shared_ptr<class mongo_instance> mongo;

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
            mongo->store_datastore(msg->to_string());
        }

        void delivery_complete(mqtt::delivery_token_ptr token) override {}

        mycallback(mqtt::async_client& cli,
                   mqtt::connect_options& connOpts,
                   std::string& server_,
                   std::string& topic_,
                   std::string& client_id_,
                   std::shared_ptr<class mongo_instance> mongo_ ):
                cli_(cli),
                connOpts_(connOpts),
                server(server_),
                topic(topic_),
                client_id(client_id_),
                mongo(mongo_)
        {}

    };

    ////////////////////////////////////////////////////////////////////////////////////////
    class mqtt_instance : std::enable_shared_from_this<mqtt_instance> {
    public:
        std::string mqtt_server = "tcp://post-cn-n6w1qkoir06.mqtt.aliyuncs.com:1883";
        std::string mqtt_topic = "eosio_topic" ;
        std::string mqtt_client = "GID_eosio@@@mongo";
        std::string mqtt_user = "Signature|LTAI4GJKsfb8VexSkcQodggS|post-cn-n6w1qkoir06";
        std::string mqtt_password = "9cut+eYpoc4CCd9IpOuXToqdocA=" ;
        fc::optional<mqtt::async_client> client;

        bool subscribe(std::shared_ptr<class mongo_instance>& mongo) {
            mqtt::connect_options connOpts = mqtt::connect_options(mqtt_user, mqtt_password);
            connOpts.set_keep_alive_interval(20);
            connOpts.set_clean_session(true);

            mycallback cb(*client, connOpts, mqtt_server, mqtt_topic, mqtt_client, mongo );
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


    mqtt2mongo_plugin::mqtt2mongo_plugin() :
            mongo(std::make_shared<mongo_instance>()),
            mqtt(std::make_shared<mqtt_instance>())
    {
    }

    mqtt2mongo_plugin::~mqtt2mongo_plugin()
    {
        ilog("quit...");
    }

    void mqtt2mongo_plugin::set_program_options(options_description &cli, options_description &cfg)
    {
        auto options = cfg.add_options();
        options("mongodb-uri", bpo::value<std::string>(), " Example: mongodb://127.0.0.1:27017/datastore");
        options("mqtt-server", bpo::value<std::string>(), " Example: tcp://post-cn-n6w1qkoir06.mqtt.aliyuncs.com:1883");
        options("mqtt-topic", bpo::value<std::string>(), " Example: eosio_topic");
        options("mqtt-client", bpo::value<std::string>(), " Example: GID_eosio@@@mongo");
        options("mqtt-user", bpo::value<std::string>(), " Example: Signature|LTAI4GJKsfb8VexSkcQodggS|post-cn-n6w1qkoir06");
        options("mqtt-password", bpo::value<std::string>(), " Example: 9cut+eYpoc4CCd9IpOuXToqdocA=");
    }

    void mqtt2mongo_plugin::plugin_initialize(const variables_map &options)
    {
        ilog("initializing...");
        try
        {
            if (options.count("mongodb-uri"))
            {
                mongo->mongod_uri = options.at("mongodb-uri").as<std::string>();
                mongocxx::uri uri = mongocxx::uri{mongo->mongod_uri};
                mongo->db_name = uri.database();
                mongo->mongo_pool.emplace(uri);
            }
            ilog("connecting to ${u}", ("u", mongo->mongod_uri));

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

    void mqtt2mongo_plugin::plugin_startup()
    {
        ilog("starting...");
        mongo->start();
        mqtt->subscribe(mongo);
    }

    void mqtt2mongo_plugin::plugin_shutdown()
    {
        ilog("shutdown...");

        mongo.reset();
        mqtt->close();
    }

} // namespace my

