/*
 * blogcpp :: https://www.blogcpp.org
 * Class for the integration of Duktape plug-ins [header].
 */

#ifndef PLUGINCLASS_H
#define PLUGINCLASS_H

#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <mutex>
#include <iomanip>
#ifdef _MSC_VER
#include <filesystem>
#else
#include <experimental/filesystem>
#endif
#include <dukglue/dukglue.h>

#include "constants.h"
#include "helpers.h"
#ifdef WITH_DEBUGLOG
#include "DebugClass.h"
#endif

namespace fs = std::experimental::filesystem;


class Exports {
  public:
    Exports();
    int getVersion();
};


class PluginFile {
  public:
    bool enabled;
    std::string filename;
    std::string shortname; // usually, <filename> without the extensions.
    std::vector<char> vc_affects;
};


class Plugins {
  private:
    std::string plugindir;
    duk_context* ctx;
    std::mutex mtx;
    bool enabled;
    bool traversePlugins(std::vector<PluginFile>* pfiles, std::string directory, char affects);

  public:
    virtual ~Plugins();

    void         init(bool use_plugins, std::string in_plugindir);
    std::string  enable(std::string filename);
    std::string  disable(std::string filename);
    std::string  listAll();
    std::string  processContentPlugins(std::string input);
    std::string  processHeaderPlugins();
    std::string  processFooterPlugins();
};


#endif
