#pragma once

#include <appbase/application.hpp>

namespace my
{
using namespace appbase;

class mqtt2mongo_plugin : public plugin<mqtt2mongo_plugin>
{
public:
   APPBASE_PLUGIN_REQUIRES()

   mqtt2mongo_plugin();
   virtual ~mqtt2mongo_plugin();

   virtual void set_program_options(options_description &cli, options_description &cfg) override;

   void plugin_initialize(const variables_map &options);
   void plugin_startup();
   void plugin_shutdown();

private:
   std::shared_ptr<class mongo_instance> mongo;
   std::shared_ptr<class mqtt_instance> mqtt;
};

} // namespace my
