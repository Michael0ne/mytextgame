#pragma once

#include "Generic.h"

typedef std::chrono::duration<double> TimerDurationType;
typedef void (*TimerEndCallback)(const TimerDurationType& duration);

//  An abstract timer only has start time, end time, duration and a callback function pointer.
//  It must be 'implemented' with what functionality is needed exactly.
struct TimerInterface
{
    friend struct TimerScoped;
    friend struct TimerUnscoped;
protected:
    typedef std::chrono::steady_clock::time_point TimerTimeType;

    TimerTimeType       TimeStart;
    TimerTimeType       TimeEnd;
    TimerDurationType   Duration;

    TimerEndCallback    Callback;
};

//  This implements 'scoped' timer.
//  Upon creation the start time is recorded and callback function is assigned.
//  Upon going out of scope the destructor is called and end time is recorded
//  as well as duration and a callback function is called (if it was set) with duration being provided as it's argument.
struct TimerScoped : public TimerInterface
{
    inline TimerScoped(TimerEndCallback callbackPtr = nullptr)
    {
        TimeStart = std::chrono::high_resolution_clock::now();
        Callback = callbackPtr;
    }

    inline ~TimerScoped()
    {
        TimeEnd = std::chrono::high_resolution_clock::now();
        Duration = TimeEnd - TimeStart;

        if (Callback)
            Callback(Duration);
    }
};

//  This implements the 'unscoped' timer.
//  This timer must be 'started' and 'stopped'.
//  Upon construction the callback function can be provided.
//  When 'start' method is called, the start time is recorded.
//  When 'stop' method is called, the end time is recorded, duration time is calculated
//  and callback function is called (if it was set) with duration being provided as it's argument.
//  If timer goes out of scope and no 'stop' method was called, all values are discarded and no callback function is called.
struct TimerUnscoped : public TimerInterface
{
    inline TimerUnscoped(TimerEndCallback callbackPtr = nullptr)
    {
        Callback = callbackPtr;
    }

    void Start()
    {
        TimeStart = std::chrono::high_resolution_clock::now();
    }

    void Stop()
    {
        TimeEnd = std::chrono::high_resolution_clock::now();
        Duration = TimeEnd - TimeStart;

        if (Callback)
            Callback(Duration);
    }
};

//  This implements timer with a specified interval in milliseconds.
//  Upon creation this creates a new thread that sleeps for specified time
//  Once sleep time is exhausted the thread is terminated and callback (if it was set) is called with actual elapsed time being provided as it's argument.
//  If timer goes out of scope before it was finished, the data is discarded, thread is prematurely finished and no callback is called.
struct TimerIntervalScoped : public TimerInterface
{
private:
    std::thread         SleepingThread;
    std::atomic_bool    TerminateThread;

    void WaitingThread(const TimerDurationType duration)
    {
        while (true)
        {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1ms);

            if (TerminateThread)
                return;
        }

        TimeEnd = std::chrono::high_resolution_clock::now();
        Duration = TimeEnd - TimeStart;

        if (Callback)
            Callback(Duration);
    }

public:
    inline TimerIntervalScoped(const TimerDurationType duration, TimerEndCallback callbackPtr = nullptr)
    {
        TimeStart = std::chrono::high_resolution_clock::now();
        Callback = callbackPtr;
        TerminateThread = false;

        SleepingThread = std::thread(&TimerIntervalScoped::WaitingThread, this, duration);
    }

    inline ~TimerIntervalScoped()
    {
        TerminateThread = true;
        SleepingThread.join();
    }
};

//  This implements timer with a specified interval in milliseconds.
//  Upon creation this creates a new thread that sleeps for specified time
//  Once sleep time is exhausted the thread is terminated and callback (if it was set) is called with actual elapsed time being provided as it's argument.
//  If timer goes out of scope before it was finished, the running waiting thread is not terminated and still gets executed until specified wait time runs out.
struct TimerIntervalUnscoped : public TimerInterface
{
private:
    std::thread         SleepingThread;

    void WaitingThread(const TimerDurationType duration)
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);

        TimeEnd = std::chrono::high_resolution_clock::now();
        Duration = TimeEnd - TimeStart;

        if (Callback)
            Callback(Duration);
    }

public:
    inline TimerIntervalUnscoped(const TimerDurationType duration, TimerEndCallback callbackPtr = nullptr)
    {
        TimeStart = std::chrono::high_resolution_clock::now();
        Callback = callbackPtr;

        SleepingThread = std::thread(&TimerIntervalUnscoped::WaitingThread, this, duration);
    }
};