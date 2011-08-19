/*
 * Copyright © 2011 by Jesper Juhl
 * Licensed under the terms of the GNU General Public Licence (GPL) version 2.
 */

#include <string>
#include <iostream>
#include "except.hh"

Exception::Exception(const std::string& s)
  : m_msg(s)
{
  std::cerr << "Exception: " << m_msg << std::endl;
}

std::string Exception::toString() const
{
  return m_msg;
}
