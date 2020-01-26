/*
 * blogcpp :: https://www.blogcpp.org
 * Class for the integration of comments [header].
 */

#ifndef HAS_COMMENTING
#define HAS_COMMENTING // Once is enough.

#include <string>
#include <iostream>
#include <sstream>
#ifdef WITH_DEBUGLOG
#include "DebugClass.h"
#endif


class CommentIntegration {
  public:
    CommentIntegration(std::string commenttype, std::string commentdata);
    std::string addHeader();
    std::string addComments();

  private:
    std::string commenttype;
    std::string commentdata;
};

#endif
