#pragma once

#include <chrono>
#include <string>

#include "fly/fly.h"
#include "fly/config/config.h"

namespace fly {

FLY_CLASS_PTRS(LoggerConfig);

/**
 * Class to hold configuration values related to the logger.
 *
 * @author Timothy Flynn (trflynn89@gmail.com)
 * @version July 18, 2016
 */
class LoggerConfig : public Config
{
public:
    /**
     * Constructor.
     */
    LoggerConfig();

    /**
     * Destructor.
     */
    virtual ~LoggerConfig();

    /**
     * Get the name to associate with this configuration.
     */
    static std::string GetName();

    /**
     * @return Max log file size (in bytes) before rotating the log file.
     */
    size_t MaxLogFileSize() const;

    /**
     * @return Max message size (in bytes) per log.
     */
    size_t MaxMessageSize() const;

    /**
     * @return Sleep time for logger IO thread.
     */
    std::chrono::seconds QueueWaitTime() const;
};

}
