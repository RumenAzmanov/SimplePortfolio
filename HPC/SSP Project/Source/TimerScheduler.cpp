#include "..\Headers\TimerScheduler.hpp"

#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "..\Headers\Timer.hpp"
#include "..\Headers\TimersList.hpp"

namespace SSP {  // Seminar on Systems Programming

// PUBLIC --------------------------------------------------

TimerScheduler::TimerScheduler(int callbackThreadCount)
    : threadsCount(callbackThreadCount),
      mainWorker(nullptr),
      isTiming(false),
      freeThreads(0) {
  if (threadsCount > 0) {
    threads.reserve(threadsCount);
    runningTimers.resize(threadsCount);
    for (int i = 0; i < threadsCount; ++i) {
      runningTimers[i] = nullptr;
    }
    for (int i = 0; i < threadsCount; ++i) {
      threads.emplace_back(&TimerScheduler::executeTimerThread, this, i);
    }
    isTiming = true;
    mainWorker = new std::thread(&TimerScheduler::gearTrainExecutor, this);
  } else {
    std::cerr << "Not enough threads!" << std::endl;
  }
}

TimerScheduler::~TimerScheduler() {
  {
    std::unique_lock<std::mutex> lock(mtxQueue);
    isTiming = false;
  }
  mainWorker->join();
  cndvarNotifyTimer.notify_all();
  for (int i = 0; i < threadsCount; ++i) {
    threads[i].join();
  }
}

TimerScheduler::TimerHandle TimerScheduler::ScheduleSingle(ns_t timeout, callback_t&& callback) {
  Timer* timer;
  try {
    timer = new Timer(static_cast<std::function<void()>&&>(callback), timeout, false, 1);
  } catch (...) {
    std::cerr << "Unable to sreate a timer!" << std::endl;
    throw;
  }
  timer->setExecuteTime();
  {
    std::unique_lock<std::mutex> lock(mtxQueue);
    timersQueue.enqueueTimer(timer);
    return TimerHandle(timer);
  }
}

TimerScheduler::TimerHandle TimerScheduler::ScheduleRepeat(ns_t timeout, callback_t&& callback) {
  Timer* timer;
  try {
    timer = new Timer(static_cast<std::function<void()>&&>(callback), timeout, true, 0);
  } catch (...) {
    std::cerr << "Unable to sreate a timer!" << std::endl;
    throw;
  }
  timer->setExecuteTime();
  {
    std::unique_lock<std::mutex> lock(mtxQueue);
    timersQueue.enqueueTimer(timer);
    return TimerHandle(timer);
  }
}

TimerScheduler::TimerHandle TimerScheduler::ScheduleRepeat(ns_t interval, int repeatCount, callback_t&& callback) {
  Timer* timer;
  try {
    timer = new Timer(static_cast<std::function<void()>&&>(callback), interval, false, repeatCount);
  } catch (...) {
    std::cerr << "Unable to sreate a timer!" << std::endl;
    throw;
  }
  timer->setExecuteTime();
  {
    std::unique_lock<std::mutex> lock(mtxQueue);
    timersQueue.enqueueTimer(timer);
    return TimerHandle(timer);
  }
}

bool TimerScheduler::CancelTimer(const TimerHandle& handle) {
  {
    std::unique_lock<std::mutex> lock(mtxQueue);  // Stop all other tasks and threads
    bool isAlreadyRunning = false;
    for (int i = 0; i < threadsCount; ++i) {
      if (runningTimers[i] != nullptr && handle.timer == runningTimers[i]) {
        isAlreadyRunning = true;
        runningTimers[i]->noRepeat();
        break;
      }
    }
    if (isAlreadyRunning) return false;
    bool found = timersQueue.detachTimer(handle.timer);
    if (found) {
      delete handle.timer;
      return true;
    }
    return false;
  }
}

// PRIVATE -------------------------------------------------

void TimerScheduler::executeTimerThread(int threadID) {
  while (true) {
    Timer* timer = nullptr;
    {
      std::unique_lock<std::mutex> lock(mtxQueue);  // Stop all other tasks and threads
      ++freeThreads;
      cndvarNotifyTimer.wait(lock);  // Wait to be notified
      --freeThreads;
      if (!isTiming) return;
      if (timersQueue.empty()) continue;
      timer = timersQueue.detachFirst();  // Get the timer
      runningTimers[threadID] = timer;
    }
    timer->execute();
    if (!timer->getRepeat()) timer->reduceRepeatCount();
    {
      std::unique_lock<std::mutex> lock(mtxQueue);
      if (!isTiming) {
        delete timer;
        return;
      }
      if (timer->getRepeat() || timer->getRepeatCount() > 0) {
        timer->setExecuteTime();
        timersQueue.enqueueTimer(timer);
      } else {
        delete timer;
      }
      runningTimers[threadID] = nullptr;
    }
  }
}

void TimerScheduler::gearTrainExecutor() {
  std::chrono::nanoseconds timeNowMS;
  while (isTiming) {
    timeNowMS = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now().time_since_epoch());
    if (!timersQueue.empty() && timersQueue.front()->getExecuteTime() <= timeNowMS) {
      while (freeThreads == 0) {
        std::this_thread::yield();
        if (!isTiming) return;
      }
      Timer* runningTimer;
      {
        std::unique_lock<std::mutex> lock(mtxQueue);
        if (timersQueue.front()->getExecuteTime() <= timeNowMS) {
          runningTimer = timersQueue.front();
        }
        cndvarNotifyTimer.notify_one();
      }
      while (!timersQueue.empty() && runningTimer == timersQueue.front()) {
        std::this_thread::yield();
        if (!isTiming) return;
      }
    }
  }
}

}  // namespace SSP