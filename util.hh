/*
 * Copyright © 2011 by Jesper Juhl
 * Licensed under the terms of the GNU General Public License (GPL) version 2.
 */

#ifndef BNB_UTIL_HH
#define BNB_UTIL_HH

#include <sstream>
#include <stdint.h>
#include <stdarg.h>

namespace util {
  // int to string
  std::string int2str(int64_t i, int width = 0, char fill = '0',
                      std::ios_base::fmtflags flags = std::ios::right);
  // unsigned int to string
  std::string uint2str(uint64_t i, int width = 0, char fill = '0',
                       std::ios_base::fmtflags flags = std::ios::right);

  // Format string to std::string - takes a printf() style format
  // string and a variable list of arguments as input and returns a
  // std::string
  std::string fmt2str(const char* fmt, ...);

  // This is the same as fmt2str() except it takes a va_list as input.
  std::string vfmt2str(const char* fmt, va_list vl);

  // Simple little garbage collector template. Takes care of deleting
  // the pointer it holds when the object goes out of scope.
  template <class T>
  class GC {
  public:
    GC(T* ptr) : m_ptr(ptr) { }
    ~GC() { delete m_ptr; }
    void forget() { m_ptr = 0; }
  private:
    T* m_ptr;

  };
}

#endif
