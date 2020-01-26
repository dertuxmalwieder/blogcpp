/*
 * blogcpp :: https://www.blogcpp.org
 * Class to make using the inja template engine easier [header].
 */

#ifndef HAS_TPL
#define HAS_TPL // Once is enough.

#include <string>
#include <sstream>

#include <nlohmann/json.hpp>
#include <inja.hpp>
#ifdef WITH_DEBUGLOG
#  include "DebugClass.h"
#endif

using json = nlohmann::json;

class TemplateWrapper {
  public:
    inja::Template loadTemplate(inja::Environment& env, std::string tplfile);
    void writeTemplate(inja::Template tpl, inja::Environment& env, json data, std::string outfile);
};

#endif