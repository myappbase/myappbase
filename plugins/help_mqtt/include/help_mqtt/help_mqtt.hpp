/**
*  @file
*  @copyright defined in eos/LICENSE
*/
#pragma once
#include <string>
#include "mqtt/async_client.h"

namespace my {
namespace help_mqtt {

    class action_listener : public virtual mqtt::iaction_listener
    {
        std::string name_;
        void on_failure(const mqtt::token& tok) override;
        void on_success(const mqtt::token& tok) override ;
    public:
        action_listener(const std::string& name) ;
    };

/////////////////////////////////////////////////////////////////////////////

    class callback : public virtual mqtt::callback,
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
        void reconnect();
        void on_failure(const mqtt::token& tok) override;
        void on_success(const mqtt::token& tok) override;
        void connected(const std::string& cause) override;
        void connection_lost(const std::string& cause) override;
        void message_arrived(mqtt::const_message_ptr msg) override;
        void delivery_complete(mqtt::delivery_token_ptr token) override;

        callback(mqtt::async_client& cli,
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

} // namespace help_mqtt
} // namespace my

