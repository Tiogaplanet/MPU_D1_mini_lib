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

namespace mip_detail {

/**
 * @brief A lightweight circular queue (ring buffer) template for internal
 * library use.
 *
 * @details Overwrites oldest data when the queue is full. Primarily used to
 * buffer asynchronous events from MiP.
 *
 * @tparam ElementType Type of elements stored in the queue.
 * @tparam Size        Maximum number of elements the queue can hold.
 */
template<class ElementType, uint8_t Size> class CircularQueue {
  static_assert(Size > 0, "CircularQueue Size must be greater than 0");

public:
  /**
   * @brief Constructs a new CircularQueue object and initializes internal
   * state.
   */
  CircularQueue() {
    clear();
  }

  /**
   * @brief Clears all elements from the queue.
   */
  void clear() {
    m_count = 0;
    m_readIndex = 0;
    m_writeIndex = 0;
  }

  /**
   * @brief Checks whether the queue contains no elements.
   *
   * @return true if the queue is empty (count is 0), false otherwise.
   */
  bool isEmpty() const {
    return m_count == 0;
  }

  /**
   * @brief Checks whether the queue has reached maximum capacity.
   *
   * @return true if the queue is full (count equals Size), false otherwise.
   */
  bool isFull() const {
    return m_count == Size;
  }

  /**
   * @brief Returns the maximum number of elements the queue can store.
   *
   * @return constexpr uint8_t Queue capacity (Size).
   */
  constexpr uint8_t capacity() const {
    return Size;
  }

  /**
   * @brief Returns the number of unread elements currently stored in the queue.
   *
   * @return uint8_t Number of available elements (0 to Size).
   */
  uint8_t available() const {
    return m_count;
  }

  /**
   * @brief Pushes a new element into the back of the queue.
   *
   * @details If the queue is full, the oldest element at the front is
   * overwritten and the read index is advanced to discard it.
   *
   * @param element Reference to the element value to store in the queue.
   */
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

  /**
   * @brief Pops and retrieves the oldest element from the front of the queue.
   *
   * @param[out] element Reference to a variable where the popped element value
   * will be stored.
   * @return true if an element was successfully popped, false if the queue was
   * empty.
   */
  bool pop(ElementType& element) {
    if (isEmpty()) { return false; }

    // Pop the oldest element from the circular queue.
    element = m_elements[m_readIndex];
    advanceReadIndex();
    m_count--;
    return true;
  }

private:
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

}  // namespace mip_detail

#endif  // MPU_QUEUE_H
