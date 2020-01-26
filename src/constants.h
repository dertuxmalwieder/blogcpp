/*
 * blogcpp :: https://www.blogcpp.org
 * Some everyday constant definitions.
 */

#ifndef HAS_CONSTANTS
#define HAS_CONSTANTS // Once is enough.

#include <string>

#ifdef WITH_DEBUGLOG
const std::string  DEBUGLOGFILE   = "debug.txt";
#endif
const int          APP_VERSION    = 10;
const std::string  APP_URL        = "https://www.blogcpp.org";
const std::string  BUGTRACKER     = "https://bitbucket.org/tux_/blogcpp/issues";
const std::string  HEADER_DIVIDER = "----------";
const std::string  NEWLINE        = "\n"; // Also on Windows. Modern Notepad will eat it.
const std::string  HIGHLIGHT_VER  = "9.15.8";
#endif
