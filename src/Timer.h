#include "Generic.h"

struct TimerScoped
{
    typedef const std::chrono::duration<double> TimerDurationType;
    typedef void (*TimerEndCallback)(const std::chrono::duration<double>& duration);

    std::chrono::steady_clock::time_point   TimeStart;
    std::chrono::steady_clock::time_point   TimeEnd;
    std::chrono::duration<double>           Duration;

    TimerEndCallback                        Callback;

    inline TimerScoped(TimerEndCallback callbackPtr)
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