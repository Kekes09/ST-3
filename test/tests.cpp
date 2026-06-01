// Copyright 2021 GHA Test Team

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <thread>
#include <memory>
#include "TimedDoor.h"

using ::testing::_;
using ::testing::StrictMock;

class MockTimerClient : public TimerClient {
 public:
    MOCK_METHOD(void, Timeout, (), (override));
};

class MockDoor : public Door {
 public:
    MOCK_METHOD(void, lock, (), (override));
    MOCK_METHOD(void, unlock, (), (override));
    MOCK_METHOD(bool, isDoorOpened, (), (override));
};

class TimedDoorTest : public ::testing::Test {
 protected:
    TimedDoor* door{nullptr};
    static constexpr int kDefaultTimeout = 50;

    void SetUp() override {
        door = new TimedDoor(kDefaultTimeout);
    }

    void TearDown() override {
        delete door;
        door = nullptr;
    }
};

TEST_F(TimedDoorTest, DoorCreatedInClosedState) {
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, ConstructorStoresTimeoutValue) {
    EXPECT_EQ(door->getTimeOut(), kDefaultTimeout);
}

TEST_F(TimedDoorTest, UnlockOpensDoor) {
    door->unlock();
    EXPECT_TRUE(door->isDoorOpened());
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST_F(TimedDoorTest, LockClosesDoor) {
    door->unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    door->lock();
    EXPECT_FALSE(door->isDoorOpened());
}

TEST_F(TimedDoorTest, ThrowStateSafeWhenClosed) {
    door->lock();
    EXPECT_NO_THROW(door->throwState());
}

TEST_F(TimedDoorTest, ThrowStateThrowsWhenFlagSetViaTimeout) {
    TimedDoor quickDoor(20);
    quickDoor.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_THROW(quickDoor.throwState(), std::runtime_error);
}

TEST_F(TimedDoorTest, CloseBeforeTimeoutPreventsException) {
    TimedDoor quickDoor(100);
    quickDoor.unlock();
    quickDoor.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    EXPECT_NO_THROW(quickDoor.throwState());
}

TEST_F(TimedDoorTest, TimerCallsClientTimeout) {
    StrictMock<MockTimerClient> mockClient;
    Timer timer;
    EXPECT_CALL(mockClient, Timeout()).Times(1);
    timer.tregister(10, &mockClient);
}

TEST(TimerValidationTest, RejectsNonPositiveTimeout) {
    Timer timer;
    StrictMock<MockTimerClient> mockClient;
    EXPECT_THROW(timer.tregister(0, &mockClient), std::invalid_argument);
    EXPECT_THROW(timer.tregister(-1, &mockClient), std::invalid_argument);
}

TEST_F(TimedDoorTest, MockDoorInterfaceVerification) {
    StrictMock<MockDoor> mockDoor;
    EXPECT_CALL(mockDoor, unlock()).Times(1);
    EXPECT_CALL(mockDoor, lock()).Times(1);
    EXPECT_CALL(mockDoor, isDoorOpened())
        .Times(2)
        .WillOnce(::testing::Return(true))
        .WillOnce(::testing::Return(false));
    mockDoor.unlock();
    EXPECT_TRUE(mockDoor.isDoorOpened());
    mockDoor.lock();
    EXPECT_FALSE(mockDoor.isDoorOpened());
}

TEST_F(TimedDoorTest, AdapterDelegatesTimeoutToDoor) {
    TimedDoor tempDoor(10);
    DoorTimerAdapter tempAdapter(tempDoor);
    EXPECT_NO_THROW(tempAdapter.Timeout());
}
