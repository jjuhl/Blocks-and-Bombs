#include <set>
#include <string>
#include "options.hh"

Options::Options(int argc, char** argv)
{
  int i = 0;
  while (i != argc) {
    m_options.insert(argv[i]);
    ++i;
  }
}

bool Options::hasOption(const std::string& opt)
{
  return m_options.find(opt) != m_options.end();
}
