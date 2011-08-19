/*
 * Copyright © 2011 by Jesper Juhl
 * Licensed under the terms of the GNU General Public License (GPL) version 2.
 */

#include <sstream>
#include <iomanip>
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include "except.hh"
#include "util.hh"

namespace util {
  void int2str_set_options(std::stringstream& ss, int width, char fill,
                           std::ios_base::fmtflags flags)
  {
    ss << std::setw(width);
    ss.setf(flags);
    ss.fill(fill);
  }

  std::string int2str(int64_t i, int width, char fill, std::ios_base::fmtflags flags)
  {
    std::stringstream out;
    int2str_set_options(out, width, fill, flags);
    out << i;
    return out.str();
  }

  std::string uint2str(uint64_t i, int width, char fill, std::ios_base::fmtflags flags)
  {
    std::stringstream out;
    int2str_set_options(out, width, fill, flags);
    out << i;
    return out.str();
  }

  std::string fmt2str(const char* fmt, ...)
  {
    va_list vl;
    va_start(vl, fmt);
    std::string ret(vfmt2str(fmt, vl));
    va_end(vl);
    return ret;
  }

  std::string vfmt2str(const char* fmt, va_list vl)
  {
    const size_t size = 1024;
    std::vector<char> buf;
    buf.resize(size);

    int required = vsnprintf(&buf[0], size, fmt, vl);
    std::string result;
    if (required < static_cast<int>(size) && required >= 0) {
      // the string fits in our buffer
      return std::string(&buf[0], required);
    }
    // string doesn't fit, resize buffer to match requirement. We
    // add 1 since the returned size from vsnprintf() is the
    // required size excluding the final \0.
    buf.resize(required + 1);
    required = vsnprintf(&buf[0], size, fmt, vl);
    if (required < 0)
      throw Exception("Unable to convert format string to std::string.");
    return std::string(&buf[0], required);
  }

}
