#include <help_jpush/help_jpush.hpp>

#include <fc/log/logger_config.hpp>

#include "jpush.h"

namespace my {
namespace help_jpush {

    void send(const std::string &account, const std::string &data) {
        cJSON *jpush;
        JPushNotification *note;
        jpushInit(&jpush, &note);
        setPlatform(jpush, J_PUSH_PLATFORM_ANDROID | J_PUSH_PLATFORM_IOS);
        char *target[1] = {(char *) account.c_str()};
        setAudienceTarget(jpush, J_PUSH_AUDIENCE_ALIAS, 1, target);
        setAllApns(note, (char *) "myapp-jpush");
        setNotification(jpush, note);
        setMessage(jpush, (char *) data.c_str(), NULL, NULL, NULL);
        char *ret = jpushEval(jpush);
        jpushClean(jpush);
        ilog("\n-------------------------------------------"
             "\n${a}=${b}"
             "\n-------------------------------------------",
             ("a", account)("b", ret));
    }

} //namespace help_jpush
} // namespace my
