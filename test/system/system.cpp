#include <csignal>
#include <string>

#include <gtest/gtest.h>

#include "fly/system/system.h"

namespace
{
    static int s_lastSignal = 0;

    void handleSignal(int signal)
    {
        s_lastSignal = signal;
    }
}

//==============================================================================
TEST(SystemTest, PrintBacktraceTest)
{
    fly::System::PrintBacktrace();
}

//==============================================================================
TEST(SystemTest, LocalTimeTest)
{
    std::string time = fly::System::LocalTime();
    EXPECT_FALSE(time.empty());
}

//==============================================================================
TEST(SystemTest, ErrorCodeTest)
{
    int code = fly::System::GetErrorCode();

    std::string error1 = fly::System::GetErrorString();
    std::string error2 = fly::System::GetErrorString(code);

    EXPECT_FALSE(error1.empty());
    EXPECT_FALSE(error2.empty());
    EXPECT_EQ(error1, error2);
}

//==============================================================================
TEST(SystemTest, SignalTest)
{
    fly::System::SignalHandler handler(&handleSignal);
    fly::System::SetSignalHandler(handler);

    std::raise(SIGINT);
    EXPECT_EQ(s_lastSignal, SIGINT);

    std::raise(SIGSEGV);
    EXPECT_EQ(s_lastSignal, SIGSEGV);

    fly::System::SetSignalHandler(nullptr);
}