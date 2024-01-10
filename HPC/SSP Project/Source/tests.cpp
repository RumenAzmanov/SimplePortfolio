#define CATCH_CONFIG_MAIN
#include "..\Headers\Timer.hpp"
#include "..\Headers\TimerScheduler.hpp"
#include "..\Headers\TimersList.hpp"
#include "..\catch2\catch.hpp"

TEST_CASE("TimerScheduler - A timer is created and executed") {
  int isExecuted = 0;
  std::function<void()> function1 = [&isExecuted]() { ++isExecuted; };

  SECTION("TimerScheduler created and destroyed") {
    SSP::TimerScheduler TS(4);
  }

  SECTION("Single timer") {
    SSP::TimerScheduler TS(4);
    isExecuted = 0;
    TS.ScheduleSingle((std::chrono::nanoseconds)((std::chrono::seconds)(1)), static_cast<std::function<void()>&&>(function1));
    std::this_thread::sleep_for(std::chrono::seconds(3));
    REQUIRE(isExecuted == 1);
  }

  SECTION("Repeatable timer") {
    SSP::TimerScheduler TS(4);
    isExecuted = 0;
    TS.ScheduleRepeat((std::chrono::nanoseconds)((std::chrono::seconds)(1)), static_cast<std::function<void()>&&>(function1));
    std::this_thread::sleep_for(std::chrono::seconds(3));
    REQUIRE(isExecuted > 0);
  }

  SECTION("Repeatable timer with number of repetitions") {
    SSP::TimerScheduler TS(4);
    isExecuted = 0;
    TS.ScheduleRepeat((std::chrono::nanoseconds)((std::chrono::seconds)(1)), 3, static_cast<std::function<void()>&&>(function1));
    std::this_thread::sleep_for(std::chrono::seconds(10));
    REQUIRE(isExecuted == 3);
  }
}

TEST_CASE("TimerScheduler - Removing timers") {
  int isExecuted = 0;
  std::function<void()> function1 = [&isExecuted]() { ++isExecuted; };
  std::function<void()> function10 = [&isExecuted]() { isExecuted += 10; };

  SECTION("Remove single timer") {
    SSP::TimerScheduler TS(4);
    isExecuted = 0;
    SSP::TimerScheduler::TimerHandle TH =
        TS.ScheduleSingle((std::chrono::nanoseconds)((std::chrono::seconds)(3)), static_cast<std::function<void()>&&>(function1));
    TS.CancelTimer(TH);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    REQUIRE(isExecuted == 0);
  }

  SECTION("Remove repeatable timer") {
    SSP::TimerScheduler TS(4);
    isExecuted = 0;
    SSP::TimerScheduler::TimerHandle TH =
        TS.ScheduleRepeat((std::chrono::nanoseconds)((std::chrono::seconds)(3)), static_cast<std::function<void()>&&>(function1));
    TS.CancelTimer(TH);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    REQUIRE(isExecuted == 0);
  }

  SECTION("Remove repeatable timer with number of repetitions") {
    SSP::TimerScheduler TS(4);
    isExecuted = 0;
    SSP::TimerScheduler::TimerHandle TH =
        TS.ScheduleRepeat((std::chrono::nanoseconds)((std::chrono::seconds)(3)), 3, static_cast<std::function<void()>&&>(function1));
    TS.CancelTimer(TH);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    REQUIRE(isExecuted == 0);
  }
}