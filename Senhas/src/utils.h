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

bool nstr_equals(const char *a, const char *b, int n) {
  while (n--) {
    if (*a != *b)
      return false;
    a++;
    b++;
  }
  return true;
}
