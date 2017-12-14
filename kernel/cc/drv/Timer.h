#ifndef TIMER_H
#define TIMER_H

#include <CRBOs.h>

class Timer : public Driver
{
public:
    Timer(Driver *);
    ~Timer() { }
    const char *type() { return "Timer"; }
    static Timer *instance() { return myInstance; }
    void startPeriodicTimer();
    void setTimeoutMS(u32 ms);
    void stopPeriodicTimer();

private:
    volatile static u64 _timerCount;
    volatile static u64 _apicTimerCount;
    static Timer *myInstance;
};

#endif // TIMER_H
