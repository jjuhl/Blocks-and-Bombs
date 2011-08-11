/*
  Simple class for getting our commandline options/flags
 */

#ifndef BNB_OPTIONS_HH
#define BNB_OPTIONS_HH

#include <string>
#include <set>

class Options {
public:
  Options(int argc, char** argv);
  ~Options() { }

  bool hasOption(const std::string& opt);

  // Todo: add handling of options that have values.
private:
  std::set<std::string> m_options;
};

#endif
