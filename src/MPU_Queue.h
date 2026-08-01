/**
 * @file MPU_Queue.h
 * @brief Fixed-size circular queue (ring buffer) used internally by the MPU
 *        library.
 *
 * @details This template class overwrites the oldest element when full. It is
 * not thread-safe. Used for buffering clap, gesture, IR, and detection
 * events.
 *
 * @author Adam Green (Original Author)
 * @author Samuel Trassare (Maintainer)
 * @copyright Copyright (C) 2018-2026 Samuel Trassare
 * (https://github.com/Tiogaplanet) Licensed under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 */
#ifndef MPU_QUEUE_H
#define MPU_QUEUE_H

#include <stdint.h>

/**
 * @brief A lightweight circular queue (ring buffer) template.
 *
 * Overwrites oldest data when the queue is full. Primarily used to buffer
 * asynchronous events from the MiP robot.
 *
 * @tparam ElementType Type of elements stored in the queue.
 * @tparam Size        Maximum number of elements the queue can hold.
 */
template <class ElementType, uint8_t Size>
class CircularQueue {
 public:
  CircularQueue() {
    clear();
  }

  void clear() {
    m_count = 0;
    m_readIndex = 0;
    m_writeIndex = 0;
  }

  bool isEmpty() {
    return m_count == 0;
  }

  uint8_t available() {
    return m_count;
  }

  void push(const ElementType& element) {
    m_elements[m_writeIndex] = element;
    advanceWriteIndex();
    if (m_count < Size) {
      m_count++;
    } else {
      // Queue was full so oldest response was overwritten. Increment read index
      // to discard oldest.
      advanceReadIndex();
    }
  }

  bool pop(ElementType& element) {
    if (isEmpty()) {
      return false;
    }

    // Pop the oldest element from the circular queue.
    element = m_elements[m_readIndex];
    advanceReadIndex();
    m_count--;
    return true;
  }

 protected:
  void advanceWriteIndex() {
    if (m_writeIndex == Size - 1) {
      // Wrap around to beginning of circular queue.
      m_writeIndex = 0;
    } else {
      m_writeIndex++;
    }
  }

  void advanceReadIndex() {
    if (m_readIndex == Size - 1) {
      // Wrap around to beginning of circular queue.
      m_readIndex = 0;
    } else {
      m_readIndex++;
    }
  }

  ElementType m_elements[Size];
  uint8_t m_count;
  uint8_t m_readIndex;
  uint8_t m_writeIndex;
};

#endif  // MPU_QUEUE_H
