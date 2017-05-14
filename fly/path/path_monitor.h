#pragma once

#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "fly/fly.h"
#include "fly/task/runner.h"

namespace fly {

DEFINE_CLASS_PTRS(PathMonitor);

/**
 * Virtual interface to monitor a local path. Provides monitoring of either all
 * files or user-specified files under a path for addition, deletion, or change.
 * This interface is platform independent - OS dependent implementations should
 * inherit from this class.
 *
 * @author Timothy Flynn (trflynn89@gmail.com)
 * @version May 14, 2017
 */
class PathMonitor : public Runner
{
public:
    /**
     * Enumerated list of path events.
     */
    enum PathEvent
    {
        NO_CHANGE,
        FILE_CREATED,
        FILE_DELETED,
        FILE_CHANGED
    };

    /**
     * Callback definition for function to be triggered on a path change.
     */
    typedef std::function<void(const std::string &, const std::string &, PathEvent)> PathEventCallback;

    /**
     * Constructor.
     */
    PathMonitor();

    /**
     * Destructor. Stop the path monitor thread if necessary.
     */
    virtual ~PathMonitor();

    /**
     * Check if the monitor implementation is in a good state.
     *
     * @return True if the monitor is healthy.
     */
    virtual bool IsValid() const = 0;

    /**
     * Monitor for changes to all files under a path. Callbacks registered with
     * AddFile take precendence over callbacks registered with AddPath.
     *
     * @param string Path to start monitoring.
     * @param PathEventCallback Callback to trigger when a file under the path changes.
     *
     * @return bool True if the path could be added.
     */
    bool AddPath(const std::string &, PathEventCallback);

    /**
     * Stop monitoring for changes to all files under a path.
     *
     * @param string The path to stop monitoring.
     *
     * @return bool True if the path was removed.
     */
    bool RemovePath(const std::string &);

    /**
     * Stop monitoring all paths.
     */
    void RemoveAllPaths();

    /**
     * Monitor for changes to a single file under a path. Callbacks registered
     * with AddFile take precendence over callbacks registered with AddPath.
     *
     * @param string Path containing the file to start monitoring.
     * @param string Name of the file to start monitoring.
     * @param PathEventCallback Callback to trigger when the file changes.
     *
     * @return bool True if the file could be added.
     */
    bool AddFile(const std::string &, const std::string &, PathEventCallback);

    /**
     * Stop monitoring for changes to a single file under a path.
     *
     * @param string Path containing the file to stop monitoring.
     * @param string Name of the file to stop monitoring.
     *
     * @return bool True if the file was removed.
     */
    bool RemoveFile(const std::string &, const std::string &);

protected:
    DEFINE_STRUCT_PTRS(PathInfo);

    /**
     * Struct to store information about a monitored path. OS dependent
     * implementations of PathMonitor should also have a concrete defintion
     * of this struct.
     */
    struct PathInfo
    {
        /**
         * Check if the monitored path is in a good state.
         *
         * @return bool True if the monitored path is healthy.
         */
        virtual bool IsValid() const = 0;

        PathEventCallback m_pathHandler;
        std::map<std::string, PathEventCallback> m_fileHandlers;
    };

    /**
     * Map of monitored paths to their path information.
     */
    typedef std::map<std::string, PathInfoPtr> PathInfoMap;

    /**
     * @return True if the monitor is in a good state.
     */
    virtual bool StartRunner();

    /**
     * Stop monitoring all paths and close any open handles.
     */
    virtual void StopRunner();

    /**
     * Poll the monitored paths for changes.
     *
     * @return True if the monitor is in a good state.
     */
    virtual bool DoWork();

    /**
     * Create an instance of the OS dependent PathInfo struct.
     *
     * @param string The path to be monitored.
     *
     * @return PathInfoPtr Up-casted shared pointer to the PathInfo struct.
     */
    virtual PathInfoPtr CreatePathInfo(const std::string &) const = 0;

    /**
     * Poll for any changes.
     *
     * @param milliseconds Max time to poll for changes.
     */
    virtual void Poll(const std::chrono::milliseconds &) = 0;

    /**
     * Close any open handles.
     */
    virtual void Close() = 0;

    mutable std::mutex m_mutex;
    PathInfoMap m_pathInfo;

private:
    /**
     * Search for a path to be monitored in the PathInfo map. If the map does
     * not contain the path, create an entry.
     *
     * @param string The path to be monitored.
     *
     * @return PathInfoPtr Shared pointer to the PathInfo struct.
     */
    PathInfoPtr getOrCreatePathInfo(const std::string &);
};

}