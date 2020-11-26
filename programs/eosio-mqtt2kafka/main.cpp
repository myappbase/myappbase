/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#include <appbase/application.hpp>

#include <mqtt2kafka_plugin/mqtt2kafka_plugin.hpp>

#include <fc/log/logger_config.hpp>
#include <fc/exception/exception.hpp>

#include <boost/exception/diagnostic_information.hpp>

#include <pwd.h>

using namespace appbase;
using namespace my;

int main(int argc, char** argv)
{

   try {
      app().register_plugin<mqtt2kafka_plugin>();
      if(!app().initialize<mqtt2kafka_plugin>(argc, argv))
         return -1;
      app().startup();
      app().exec();
   } catch (const fc::exception& e) {
      elog("${e}", ("e",e.to_detail_string()));
   } catch (const boost::exception& e) {
      elog("${e}", ("e",boost::diagnostic_information(e)));
   } catch (const std::exception& e) {
      elog("${e}", ("e",e.what()));
   } catch (...) {
      elog("unknown exception");
   }
   return 0;
}
