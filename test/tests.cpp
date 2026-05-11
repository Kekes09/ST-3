// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstdint>
#include "TimedDoor.h"
#include <thread>
#include <chrono>

using ::testing::Return;
using ::testing::Test;


struct TimedDoorTest : public Test {
    TimedDoor* door;
    const int timeout = 1;

    void SetUp() override {
        door = new TimedDoor(timeout);
    }

    void TearDown() override {
        delete door;
    }
};


TEST_F(TimedDoorTest, IsInitiallyClosed) {
    ASSERT_FALSE(door->isDoorOpened());
}


TEST_F(TimedDoorTest, UnlockOpensDoor) {
    door->unlock();
    ASSERT_TRUE(door->isDoorOpened());
    door->lock();
}


TEST_F(TimedDoorTest, LockClosesDoor) {
    door->unlock();
    door->lock();
    ASSERT_FALSE(door->isDoorOpened());
}


TEST_F(TimedDoorTest, GetTimeoutReturnsCorrectValue) {
    ASSERT_EQ(door->getTimeOut(), timeout);
}


TEST_F(TimedDoorTest, ThrowStateThrowsRuntimeError) {
    ASSERT_THROW(door->throwState(), std::runtime_error);
}


TEST_F(TimedDoorTest, ThrowStateThrowsExceptionWithCorrectMessage) {
    try {
        door->throwState();
        FAIL() << "Expected std::runtime_error";
    } catch(const std::runtime_error& err) {
        EXPECT_STREQ("Door is still open!", err.what());
    } catch(...) {
        FAIL() << "Expected std::runtime_error";
    }
}


TEST_F(TimedDoorTest, UnlockTwiceThenLock) {
    door->unlock();
    door->unlock();
    door->lock();
    std::this_thread::sleep_for(std::chrono::seconds(timeout + 1));
    ASSERT_FALSE(door->isDoorOpened());
}


class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

TEST(TimedDoorCreationTest, NewDoorIsClosed) {
    TimedDoor door(5);
    EXPECT_FALSE(door.isDoorOpened());
}
