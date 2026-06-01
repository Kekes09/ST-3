// Copyright 2021 GHA Test Team

#ifndef INCLUDE_TIMEDDOOR_H_
#define INCLUDE_TIMEDDOOR_H_

#include <thread>

class DoorTimerAdapter;
class Timer;
class Door;
class TimedDoor;

class TimerClient {
 public:
    virtual ~TimerClient() = default;
    virtual void Timeout() = 0;
};

class Door {
 public:
    virtual ~Door() = default;
    virtual void lock() = 0;
    virtual void unlock() = 0;
    virtual bool isDoorOpened() = 0;
};

class DoorTimerAdapter : public TimerClient {
 private:
    TimedDoor& door;

 public:
    explicit DoorTimerAdapter(TimedDoor&);
    void Timeout() override;
};

class TimedDoor : public Door {
    friend class DoorTimerAdapter;

 private:
    DoorTimerAdapter* adapter;
    int iTimeout;
    bool isOpened;
    bool isThrow;
    std::thread* th;

 public:
    explicit TimedDoor(int);
    ~TimedDoor();

    bool isDoorOpened() override;
    void unlock() override;
    void lock() override;
    int getTimeOut() const;
    void throwState();
};

class Timer {
 private:
    TimerClient* client;
    void sleep(int);

 public:
    Timer() : client(nullptr) {}
    void tregister(int, TimerClient*);
};

#endif  // INCLUDE_TIMEDDOOR_H_
