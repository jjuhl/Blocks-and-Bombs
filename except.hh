/*
 * Simple little class that can be thrown and will print its contained
 * message on construction.
 *
 * Copyright © 2011 by Jesper Juhl
 * Licensed under the terms of the GNU General Public License (GPL) version 2.
 */

#ifndef BNB_EXCEPT_HH
#define BNB_EXCEPT_HH

#include <string>

class Exception {
public:
  Exception(const std::string& s);
  virtual ~Exception() { }
  std::string toString() const;
private:
  const std::string m_msg;
};

#endif
