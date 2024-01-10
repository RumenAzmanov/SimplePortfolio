#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>
#include <functional>
#include <iostream>

namespace SSP {  // Seminar on Systems Programming

/// A timer structure for a single timer task
/// Created to be used as an element in linked list data structure
struct Timer {
 private:
  std::function<void()> callback;        ///< The callback function
  std::chrono::nanoseconds timeout;      ///< Timeout or interval
  std::chrono::nanoseconds executeTime;  ///< Execution time
  Timer* next;                           ///< Pointer to next timer in list
  int repeatCount;                       ///< Repeat count if finitely repeatabe or 1 for single execution
  bool repeat;                           ///< Flag for infinetely repeatable timers

 public:
  Timer(std::function<void()> callback,
        std::chrono::nanoseconds timeout,
        bool repeat,
        int repeatCount)
      : callback(callback),
        timeout(timeout),
        executeTime(0),
        next(nullptr),
        repeatCount(repeatCount),
        repeat(repeat) {
  }

  /// @brief Execute the callback function
  void execute() {
    try {
      this->callback();
    } catch (...) {
      std::cerr << "Unable to execute timer function!" << std::endl;
      throw;
    }
  }

  /// @brief Set execute time from this moment + the timeout
  void setExecuteTime() {
    executeTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
                      std::chrono::system_clock::now().time_since_epoch()) +
                  timeout;
  }

  std::chrono::nanoseconds getExecuteTime() { return executeTime; }

  bool getRepeat() { return repeat; }

  int getRepeatCount() { return repeatCount; }

  /// @brief The timer will not be repeated again (makes it ready to be destroyed)
  void noRepeat() {
    repeat = false;
    repeatCount = 0;
  }

  void reduceRepeatCount() { --repeatCount; }

  void setNext(Timer* nextTimer) { next = nextTimer; }

  Timer* getNext() { return next; }
};

}  // namespace SSP

#endif