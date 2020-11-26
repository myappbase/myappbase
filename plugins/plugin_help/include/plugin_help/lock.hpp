#pragma once

#include <pthread.h>

namespace my {
namespace plugin_help {

    class plugin_rwlock {
    public:
        enum READ_WRITE_TYPE {
            READ, WRITE
        };

    public:
        plugin_rwlock(pthread_rwlock_t &, READ_WRITE_TYPE);

        ~plugin_rwlock();

    private:
        pthread_rwlock_t &m_lock;
        bool success;
    };

    class plugin_mutex {
    public:
        plugin_mutex(pthread_mutex_t &);

        ~plugin_mutex();

    private:
        pthread_mutex_t &m_lock;
    };

} // namespace plugin_help
} // namespace my
