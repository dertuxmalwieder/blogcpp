/*
 * blogcpp :: https://www.blogcpp.org
 * Debug logging functionality class [header].
 */

#ifndef HAS_DEBUGLOG // Make sure nothing is defined twice
#define HAS_DEBUGLOG

#include <string>
#include <sstream>


class DebugLog {
  public:
    static void resetDebuglog();
    static void debuglog(const std::string& debugtext);
};

#endif
