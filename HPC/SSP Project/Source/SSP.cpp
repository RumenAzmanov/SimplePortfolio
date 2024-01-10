#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

#include "..\Headers\Timer.hpp"
#include "..\Headers\TimerScheduler.hpp"
#include "..\Headers\TimersList.hpp"

void function1() {
  std::cout << "Hello world!" << std::endl;
}

void function2() {
  std::cout << "***###***" << std::endl;
}

void threadFunction(SSP::TimerScheduler& TS) {
  for (int i = 0; i < 8; ++i) {
    TS.ScheduleSingle(
        (std::chrono::nanoseconds)((std::chrono::seconds)(2)),
        function1);
  }
}

int main() {
  SSP::TimerScheduler TS(2);

  std::thread thread1(threadFunction, std::ref(TS));
  std::thread thread2(threadFunction, std::ref(TS));
  thread1.join();
  thread2.join();

  std::this_thread::sleep_for(std::chrono::seconds(5));
  std::cout << "Goodbye world!" << std::endl;
  return 0;
}

// struct TimerHandle {};
// TimerScheduler(int callbackThreadCount);
// TimerHandle ScheduleSingle(ns_t timeout, callback_t &&callback);
// TimerHandle ScheduleRepeat(ns_t interval, callback_t &&callback);
// TimerHandle ScheduleRepeat(ns_t interval, int repeatCount, callback_t &&callback);
// bool CancelTimer(const TimerHandle &handle);