#ifndef TIMERSCHEDULER_HPP
#define TIMERSCHEDULER_HPP

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "Timer.hpp"
#include "TimersList.hpp"

namespace SSP {  // Seminar on Systems Programming

/// @brief System for timer management
struct TimerScheduler {
 public:
  using ns_t = std::chrono::nanoseconds;
  using callback_t = std::function<void()>;

  /// @brief Structure used to identify a timer
  struct TimerHandle {
    Timer* timer;
    TimerHandle(Timer* timer) : timer(timer) {}
  };

 private:
  int threadsCount;                   ///< Number of threads
  bool isTiming;                      ///< Flag if the TimerScheduler is working
  std::thread* mainWorker;            ///< Thread that distributes the timers
  TimersList timersQueue;             ///< Priority queue of waiting timers
  std::vector<std::thread> threads;   ///< Vector of working threads
  std::vector<Timer*> runningTimers;  ///< List of running timers (the size is the same as number of threads)
  std::atomic<int> freeThreads;       ///< Number of free threads

  std::mutex mtxQueue;                        ///< Synchronization mutex
  std::condition_variable cndvarNotifyTimer;  ///< Notifies the threads to get a timer

 public:
  TimerScheduler(int callbackThreadCount);

  ~TimerScheduler();

  /// @brief Shedule a timer that will be executed once
  TimerHandle ScheduleSingle(ns_t timeout, callback_t&& callback);

  /// @brief Shedule a single timer that will be executed an infinite number of times
  TimerHandle ScheduleRepeat(ns_t timeout, callback_t&& callback);

  /// @brief Shedule a single timer that will be executed a finite number of times
  TimerHandle ScheduleRepeat(ns_t interval, int repeatCount, callback_t&& callback);

  /// @brief Remove a timer
  bool CancelTimer(const TimerHandle& handle);

 private:
  /// @brief Method for all threads to execute a timer from the front of list
  /// @param threadID The index of the current thread (used to save the timer in runningTimers)
  void executeTimerThread(int threadID);

  /// @brief Infinitely repeats a cycle that performs the next task in the timersQueue
  void gearTrainExecutor();
};

}  // namespace SSP

#endif