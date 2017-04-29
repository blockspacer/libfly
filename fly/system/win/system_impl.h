#pragma once

#include <string>

#include "fly/fly.h"
#include "fly/exit_codes.h"

namespace fly {

/**
 * Windows declaration of the SystemImpl interface.
 *
 * @author Timothy Flynn (trflynn89@gmail.com)
 * @version July 2, 2016
 */
class SystemImpl
{
public:
    static void PrintBacktrace();
    static std::string LocalTime(const std::string &);
    static std::string GetLastError(int *);
    static void SetupSignalHandler();
    static void CleanExit(ExitCode);
    static bool KeepRunning();
    static ExitCode GetExitCode();
};

}