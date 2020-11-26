/**
 *  @file
 *  @copyright defined in myappbase/LICENSE
 */
#pragma once

#include <help_kafka/help_kafka.hpp>
#include <string>

namespace my {
namespace help_kafka {

    bool send(const std::string& broker,
              const std::string& topic,
              const std::string& data) ;

    bool send(const std::string& broker,
              const std::string& topic,
              const fc::variant& postdata) ;

} // namespace help_kafka
} // namespace my

