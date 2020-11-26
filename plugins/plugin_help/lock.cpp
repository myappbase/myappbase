#include <plugin_help/lock.hpp>
#include <fc/log/logger.hpp>
#include <pthread.h>

namespace my {
namespace plugin_help {

plugin_rwlock::plugin_rwlock(pthread_rwlock_t &lock, READ_WRITE_TYPE type)
        : m_lock(lock) {
    switch (type) {
        case READ: {
            if (pthread_rwlock_rdlock(&lock) != 0)
                success = false;
            else
                success = true;
            break;
        }

        case WRITE: {
            if (pthread_rwlock_wrlock(&lock) != 0)
                success = false;
            else
                success = true;
            break;
        }
    }
}

plugin_rwlock::~plugin_rwlock() {
    if (success) {
        if (pthread_rwlock_unlock(&m_lock) != 0)
            elog("plugin_rwlock.pthread_rwlock_unlock() error");
    }
}


plugin_mutex::plugin_mutex(pthread_mutex_t &lock)
        : m_lock(lock) {
    pthread_mutex_lock(&lock);
}

plugin_mutex::~plugin_mutex() { pthread_mutex_unlock(&m_lock); }

} // namespace plugin_help
} // namespace my
