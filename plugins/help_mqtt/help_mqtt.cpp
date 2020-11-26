#include <stdio.h>
#include <string>
#include <iostream>
#include <help_mqtt/help_mqtt.hpp>

namespace my {
namespace help_mqtt {

    void action_listener::on_failure(const mqtt::token& tok) {
        std::cout << name_ << " failure";
        if (tok.get_message_id() != 0)
            std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
        std::cout << std::endl;
    }

    void action_listener:: on_success(const mqtt::token& tok) {
        std::cout << name_ << " success";
        if (tok.get_message_id() != 0)
            std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
        auto top = tok.get_topics();
        if (top && !top->empty())
            std::cout << "\ttoken topic: '" << (*top)[0] << "', ..." << std::endl;
        std::cout << std::endl;
    }

    action_listener::action_listener(const std::string& name) :
            name_(name) {}

/////////////////////////////////////////////////////////////////////////////

    void callback::reconnect() {
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
    void callback::on_failure(const mqtt::token& tok) {
        std::cout << "Connection attempt failed" << std::endl;
        if (++nretry_ > 5 )
            exit(1);
        reconnect();
    }

    void callback::on_success(const mqtt::token& tok) {}

    void callback::connected(const std::string& cause) {
        std::cout << "\nConnection success, Subscribing to topic '" << topic << "'\n"
                  << "\tfor client " << client_id
                  << " using QoS" << qos << std::endl;
        cli_.subscribe(topic, qos, nullptr, subListener_);
    }

    void callback::connection_lost(const std::string& cause) {
        std::cout << "\nConnection lost" << std::endl;
        if (!cause.empty())
            std::cout << "\tcause: " << cause << std::endl;

        std::cout << "Reconnecting..." << std::endl;
        nretry_ = 0;
        reconnect();
    }

    void callback::message_arrived(mqtt::const_message_ptr msg) {
        std::cout << "Message arrived" << std::endl;
        std::cout << "\ttopic: '" << msg->get_topic() << "'" << std::endl;
        std::cout << "\tpayload: '" << msg->to_string() << "'\n" << std::endl;
    }

    void callback::delivery_complete(mqtt::delivery_token_ptr token) {}

} // namespace help_mqtt
} // namespace my
