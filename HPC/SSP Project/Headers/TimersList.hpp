#ifndef TIMERSLIST_HPP
#define TIMERSLIST_HPP

#include <chrono>
#include <functional>

#include "Timer.hpp"

namespace SSP {  // Seminar on Systems Programming

/// @brief Linked list of ordered timers
class TimersList {
 private:
  Timer *head, *tail;
  size_t sizeOfList;

 public:
  TimersList()
      : head(nullptr),
        tail(nullptr),
        sizeOfList(0) {
  }

  ~TimersList() { clear(); }

  bool empty() { return head == nullptr; }

  size_t size() { return sizeOfList; }

  Timer* front() { return head; }

  Timer* back() { return tail; }

  void pop() {
    Timer* delIterator = head;
    head = head->getNext();
    delete delIterator;
  }

  /// @brief Inserts the timer in the list in order of execution time
  /// @param newTimer A pointer to a new timer
  void enqueueTimer(Timer* newTimer) {
    if (head == nullptr && tail == nullptr) {
      newTimer->setNext(nullptr);
      head = newTimer;
      tail = newTimer;
    } else {
      if (tail != nullptr && tail->getExecuteTime() <= newTimer->getExecuteTime()) {
        newTimer->setNext(nullptr);
        tail->setNext(newTimer);
        tail = newTimer;
      } else {
        if (head != nullptr && newTimer->getExecuteTime() < head->getExecuteTime()) {
          newTimer->setNext(head);
          head = newTimer;
        } else {
          Timer* i = head;
          while (i->getNext() != nullptr && i->getNext()->getExecuteTime() < newTimer->getExecuteTime()) {
            i = i->getNext();
          }
          newTimer->setNext(i->getNext());
          i->setNext(newTimer);
        }
      }
    }
    sizeOfList++;
  }

  /// @brief Detaches the first timer in the list and returns it
  /// @return Pointer to the timer on first position
  Timer* detachFirst() {
    Timer* getTimer = head;
    head = head->getNext();
    if (head == nullptr) tail = nullptr;
    getTimer->setNext(nullptr);
    --sizeOfList;
    return getTimer;
  }

  /// @brief Detach timer from list
  /// @param remTimer Pointer to a timer
  /// @return True if successfiully detached, false if not found
  bool detachTimer(Timer* remTimer) {
    if (remTimer == head) {
      if (head == tail) tail = nullptr;
      Timer* ptr = head;
      head = head->getNext();
      return true;
    }
    Timer* i = head;
    while (i->getNext() != nullptr && i->getNext() != remTimer) {
      i = i->getNext();
    }
    if (i->getNext() != nullptr) {
      Timer* ptr = i->getNext();
      i->setNext(i->getNext()->getNext());
      if (tail == ptr) tail = i;
      return true;
    }
    return false;
  }

 private:
  void clear() {
    Timer* iterator = head;
    Timer* delIterator = nullptr;
    while (iterator != nullptr) {
      delIterator = iterator;
      iterator = iterator->getNext();
      delete delIterator;
    }
    head = nullptr;
    tail = nullptr;
  }
};

}  // namespace SSP

#endif