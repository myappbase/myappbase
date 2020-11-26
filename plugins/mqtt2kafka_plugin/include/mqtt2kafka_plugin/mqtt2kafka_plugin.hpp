/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#pragma once

#include <appbase/application.hpp>
#include <fc/variant.hpp>
#include <mqtt2kafka_plugin/mqtt2kafka_plugin.hpp>

namespace my {

    using namespace appbase;

    class mqtt2kafka_plugin : public plugin<mqtt2kafka_plugin> {
    public:
        APPBASE_PLUGIN_REQUIRES()

        mqtt2kafka_plugin();
        mqtt2kafka_plugin(const mqtt2kafka_plugin &) = delete;
        mqtt2kafka_plugin(mqtt2kafka_plugin &&) = delete;
        mqtt2kafka_plugin &operator=(const mqtt2kafka_plugin &) = delete;
        mqtt2kafka_plugin &operator=(mqtt2kafka_plugin &&) = delete;
        virtual ~mqtt2kafka_plugin() override = default;

        virtual void set_program_options(options_description &cli, options_description &cfg) override;
        void plugin_initialize(const variables_map &options);
        void plugin_startup();
        void plugin_shutdown();

    private:
        std::shared_ptr<class mqtt_instance> mqtt;
    };

} // namespace my