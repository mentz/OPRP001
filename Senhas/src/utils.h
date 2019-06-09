#pragma once

#include <chrono>
#include <thread>

/**
 * sleep_for(int ms)
 * Dorme a thread corrente por 'ms' milliseconds
 */
inline void sleep_for(int ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
