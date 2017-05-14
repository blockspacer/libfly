#include "fly/path/nix/path_monitor_impl.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>

#include <poll.h>
#include <unistd.h>

#include "fly/logger/logger.h"
#include "fly/system/system.h"

namespace fly {

namespace
{
    static const int s_initFlags = (
        IN_NONBLOCK
    );

    static const int s_changeFlags = (
        IN_CREATE |
        IN_DELETE |
        IN_MOVED_TO |
        IN_MOVED_FROM |
        IN_MODIFY
    );
}

//==============================================================================
PathMonitorImpl::PathMonitorImpl() :
    PathMonitor(),
    m_monitorDescriptor(::inotify_init1(s_initFlags))
{
    if (m_monitorDescriptor == -1)
    {
        LOGW(-1, "Could not initialize monitor: %s", System::GetLastError());
    }
}

//==============================================================================
PathMonitorImpl::~PathMonitorImpl()
{
    Close();
}

//==============================================================================
bool PathMonitorImpl::IsValid() const
{
    return (m_monitorDescriptor != -1);
}

//==============================================================================
PathMonitor::PathInfoPtr PathMonitorImpl::CreatePathInfo(const std::string &path) const
{
    PathMonitor::PathInfoPtr spInfo;

    if (IsValid())
    {
        spInfo = std::make_shared<PathInfoImpl>(m_monitorDescriptor, path);
    }

    return spInfo;
}

//==============================================================================
void PathMonitorImpl::Poll(const std::chrono::milliseconds &timeout)
{
    struct pollfd pollFd;

    pollFd.fd = m_monitorDescriptor;
    pollFd.events = POLLIN;

    int numEvents = ::poll(&pollFd, 1, timeout.count());

    if (numEvents == -1)
    {
        LOGW(-1, "Could not create poller: %s", System::GetLastError());
    }
    else if ((numEvents > 0) && (pollFd.revents & POLLIN))
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        while (readEvents())
        {
        }
    }
}

//==============================================================================
void PathMonitorImpl::Close()
{
    if (m_monitorDescriptor != -1)
    {
        ::close(m_monitorDescriptor);
        m_monitorDescriptor = -1;
    }
}

//==============================================================================
bool PathMonitorImpl::readEvents() const
{
    static const size_t eventSize = sizeof(struct inotify_event);

    // Some systems cannot read integer variables if they are not properly
    // aligned. On other systems, incorrect alignment may decrease performance.
    // Hence, the buffer used for reading from the inotify file descriptor
    // should have the same alignment as struct inotify_event.
    char buff[8 << 10] __attribute__ ((aligned(__alignof__(struct inotify_event))));

    ssize_t len = ::read(m_monitorDescriptor, buff, sizeof(buff));

    if (len <= 0)
    {
        if (len == -1)
        {
            int error = 0;
            std::string errorStr = System::GetLastError(&error);

            if (error != EAGAIN)
            {
                LOGW(-1, "Could not read polled event: %s", errorStr);
            }
        }
    }
    else
    {
        const struct inotify_event *pEvent;

        for (char *ptr = buff; ptr < buff + len; ptr += eventSize + pEvent->len)
        {
            pEvent = (struct inotify_event *)ptr;

            if (pEvent->len > 0)
            {
                handleEvent(pEvent);
            }
        }
    }

    return (len > 0);
}

//==============================================================================
void PathMonitorImpl::handleEvent(const struct inotify_event *pEvent) const
{
    auto it = std::find_if(m_pathInfo.begin(), m_pathInfo.end(),
        [&pEvent](const PathInfoMap::value_type &value) -> bool
        {
            PathInfoImplPtr spInfo(DownCast<PathInfoImpl>(value.second));
            return (spInfo->m_watchDescriptor == pEvent->wd);
        }
    );

    if (it != m_pathInfo.end())
    {
        PathInfoImplPtr spInfo(DownCast<PathInfoImpl>(it->second));
        PathMonitor::PathEvent event = convertToEvent(pEvent->mask);

        if (event != PathMonitor::NO_CHANGE)
        {
            PathMonitor::PathEventCallback callback = spInfo->m_fileHandlers[pEvent->name];

            if (callback == nullptr)
            {
                callback = spInfo->m_pathHandler;
            }

            if (callback != nullptr)
            {
                LOGI(-1, "Handling event %d for \"%s\" in \"%s\"",
                    event, pEvent->name, it->first);

                callback(it->first, pEvent->name, event);
            }
        }
    }
}

//==============================================================================
PathMonitor::PathEvent PathMonitorImpl::convertToEvent(int mask) const
{
    PathMonitor::PathEvent event = PathMonitor::NO_CHANGE;

    if ((mask & IN_CREATE) || (mask & IN_MOVED_TO))
    {
        event = PathMonitor::FILE_CREATED;
    }
    else if ((mask & IN_DELETE) || (mask & IN_MOVED_FROM))
    {
        event = PathMonitor::FILE_DELETED;
    }
    else if (mask & IN_MODIFY)
    {
        event = PathMonitor::FILE_CHANGED;
    }

    return event;
}

//==============================================================================
PathMonitorImpl::PathInfoImpl::PathInfoImpl(
    int monitorDescriptor,
    const std::string &path
) :
    PathMonitorImpl::PathInfo(),
    m_monitorDescriptor(monitorDescriptor),
    m_watchDescriptor(-1)
{
    m_watchDescriptor = ::inotify_add_watch(
        m_monitorDescriptor, path.c_str(), s_changeFlags
    );

    if (m_watchDescriptor == -1)
    {
        LOGW(-1, "Could not add watcher for \"%s\": %s", path, System::GetLastError());
    }
}

//==============================================================================
PathMonitorImpl::PathInfoImpl::~PathInfoImpl()
{
    if (m_watchDescriptor != -1)
    {
        ::inotify_rm_watch(m_monitorDescriptor, m_watchDescriptor);
        m_watchDescriptor = -1;
    }
}

//==============================================================================
bool PathMonitorImpl::PathInfoImpl::IsValid() const
{
    return (m_watchDescriptor != -1);
}

}