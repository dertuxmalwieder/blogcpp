/*
 * blogcpp :: https://www.blogcpp.org
 * Class to add embedded media to the output stream [header].
 */

#ifndef MEDIA_EMBEDDER_H
#define MEDIA_EMBEDDER_H // Once is enough.

#include <string>
#include <sstream>
#include <iomanip>
#include <regex>
#ifdef WITH_OEMBED
# include <curl/HTTPDownloader.hpp>
# include <nlohmann/json.hpp>
# include <XML/tinyxml2.h>
#endif
#ifdef WITH_DEBUGLOG
# include "DebugClass.h"
#endif

class MediaEmbedder {
  public:
    bool addEmbeds(std::string& inputline);

    MediaEmbedder();

#ifdef WITH_OEMBED
  private:
    bool addOEmbeds(std::string& inputline);
#endif
};

#endif
