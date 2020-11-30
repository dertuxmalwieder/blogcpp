/*
 * blogcpp :: https://www.blogcpp.org
 * RSS class [header].
 */

#ifndef HAS_RSS
#define HAS_RSS // Once is enough.

#include <string>
#include <vector>
#include <sstream>

#include "filesystem.h"
#include "helpers.h"
#include "MarkdownWrapperClass.h"
#include "SingleItemClass.h"
#include "ExcerptExtractorClass.h"
#include "ConfigCollectionClass.h"
#ifdef WITH_DEBUGLOG
#  include "DebugClass.h"
#endif

#include <tinyxml2.h>

namespace fs = NAMESPACE_FILESYSTEM;

class RSSGenerator {
  public:
    RSSGenerator(std::string feeddir, std::string title, std::string outfile, ConfigCollection cfgs);
    void createRSS(std::vector<SingleItem> vec);
  private:
    std::string feeddir;
    std::string title;
    std::string outfile;
    ConfigCollection own_cfgs;
};

#endif
