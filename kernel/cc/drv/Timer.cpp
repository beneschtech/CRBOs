#include "Timer.h"

volatile u64 *countPtr;
volatile u64 *apicCtPtr;

Timer *Timer::myInstance = NULL;
volatile u64 Timer::_timerCount = 0;
volatile u64 Timer::_apicTimerCount = 0;
void calibrateTimer();
void _irqEntry0();

void timerTest()
{
    KernelInstance->console.printf(".");
}

Timer::Timer(Driver *parent)
{
    myInstance = this;
    setParent(parent);
    countPtr = &_timerCount;
    apicCtPtr = &_apicTimerCount;
    // Set PIT to 20 HZ (50 ms)
    outb(0x40,0xd9);
    outb(0x40,0xe8);
    calibrateTimer();
    KernelInstance->interrupts.setISR(0x20,(u64)_irqEntry0);
    startupMessage(true);
}

void Timer::setTimeoutMS(u32 ms)
{
    u32 val = _apicTimerCount * ms;
    LAPIC_WRITE(APIC_TMRINITCNT,val);
}

void Timer::startPeriodicTimer()
{
    LAPIC_WRITE(APIC_LVT_TMR,TMR_PERIODIC | 0x20);
    LAPIC_WRITE(APIC_TMRDIV,3);
}

void Timer::stopPeriodicTimer()
{
    LAPIC_WRITE(APIC_LVT_TMR,APIC_DISABLE);
}

void calibrateTimer1(u64 count)
{
    *countPtr = count/50;
    i32 apicCnt = 0-(i32)(LAPIC_READ(APIC_TMRCURRCNT));
    LAPIC_WRITE(APIC_LVT_TMR,APIC_DISABLE);
    *apicCtPtr = apicCnt / 50;
    asm volatile (
                "pop %rbp\n"
                "xorq %rcx,%rcx\n"
                "inc %rcx\n"
                "iretq");
}

void calibrateTimer()
{
    LAPIC_WRITE(APIC_LVT_TMR,0x22);
    LAPIC_WRITE(APIC_TMRDIV,3);
    KernelInstance->apic.setIRQVector(0,0x20,false);
    asm volatile("hlt");
    LAPIC_WRITE(APIC_TMRINITCNT,0xffffffff);
    KernelInstance->interrupts.setISR(0x20,(u64)calibrateTimer1);
    KernelInstance->apic.EOI();
    asm volatile ("xorq %rcx,%rcx\n"
                  "xorq %rdi,%rdi\n"
                  "dec %rcx\n"
                  "1:\n"
                  "inc %rdi\n"
                  "loop 1b");
    KernelInstance->apic.setIRQVector(0,0x20,true);
    KernelInstance->apic.EOI();
}

void msleep(u32 ms)
{
    u64 cv = *countPtr;
    cv *= ms;
    asm volatile ("movq %0,%%rcx\n"
                  "xorq %%r10,%%r10\n"
                  "1:\n"
                  "inc %%r10\n"
                  "loop 1b"::"r"(cv));
}
