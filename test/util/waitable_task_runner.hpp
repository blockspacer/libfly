#pragma once

#include "fly/task/task_runner.hpp"
#include "fly/types/concurrency/concurrent_queue.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <typeinfo>

namespace fly {
class TaskManager;
} // namespace fly

namespace fly::test {

/**
 * A pseudo task runner to allow waiting for a specific task to be complete. It is not a valid test
 * runner in itself, in that it doesn't allow actually running tasks. But the implementations below
 * extend this class for common functionality. Only meant to be used by unit tests.
 *
 * @author Timothy Flynn (trflynn89@pm.me)
 * @version August 12, 2018
 */
class WaitableTaskRunner
{
public:
    /**
     * Destructor.
     */
    virtual ~WaitableTaskRunner() = default;

    /**
     * Wait indefinitely for a task from a specific location to complete execution.
     *
     * @param location The location to wait upon.
     */
    void wait_for_task_to_complete(const std::string &location);

protected:
    /**
     * When a task is complete, track it if the task is still valid.
     *
     * @param location The location from which the task was posted.
     */
    virtual void task_complete(fly::TaskLocation &&location) = 0;

private:
    fly::ConcurrentQueue<std::string> m_completed_tasks;
};

/**
 * Subclass of the parallel task runner to provide the same parllel behavior, but also to allow
 * waiting for a specific task to be complete. Only meant to be used by unit tests.
 *
 * @author Timothy Flynn (trflynn89@pm.me)
 * @version August 12, 2018
 */
class WaitableParallelTaskRunner : public fly::ParallelTaskRunner, public WaitableTaskRunner
{
    friend class fly::TaskManager;

protected:
    WaitableParallelTaskRunner(std::weak_ptr<TaskManager> task_manager) noexcept;

    /**
     * When a task is complete, perform the same operations as this runner's parents.
     *
     * @param location The location to wait upon.
     */
    void task_complete(fly::TaskLocation &&location) override;
};

/**
 * Subclass of the sequenced task runner to provide the same sequenced behavior, but also to allow
 * waiting for a specific task to be complete. Only meant to be used by unit tests.
 *
 * @author Timothy Flynn (trflynn89@pm.me)
 * @version August 12, 2018
 */
class WaitableSequencedTaskRunner : public fly::SequencedTaskRunner, public WaitableTaskRunner
{
    friend class fly::TaskManager;

protected:
    WaitableSequencedTaskRunner(std::weak_ptr<TaskManager> task_manager) noexcept;

    /**
     * When a task is complete, perform the same operations as this runner's parents.
     *
     * @param location The location to wait upon.
     */
    void task_complete(fly::TaskLocation &&location) override;
};

} // namespace fly::test
