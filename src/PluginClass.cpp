/*
 * blogcpp :: https://www.blogcpp.org
 * Class for the integration of Duktape plug-ins.
 */

#ifdef WITH_PLUGINS

#include "PluginClass.h"


Plugins::~Plugins() {
    // Destroy the Duktape!
    duk_destroy_heap(ctx);
}


void Plugins::init(bool use_plugins, std::string in_plugindir) {
    // Set up the search path.
    enabled = use_plugins;
    plugindir = in_plugindir;

    // Set up Duktape.
    ctx = duk_create_heap_default();

    // Exported stuff:
    //   getVersion()  returns the BlogC++ version
    dukglue_register_constructor<Exports>(ctx, "BlogEngine");
    dukglue_register_method(ctx, &Exports::getVersion, "getVersion");

#ifdef WITH_DEBUGLOG
    DebugLog::debuglog("Plugin system initialized.");
#endif
}


std::string Plugins::processContentPlugins(std::string input) {
    // Traverses through the {plugindir}/contents directory,
    // executes all process() methods (if any) with the
    // <input> as a parameter, returns the results.
    if (!enabled) {
        // Plug-in processing is temporarily disabled.
        return input;
    }

    std::stringstream ss_plugindir;
    ss_plugindir << plugindir << "/contents";

    if (!fs::exists(ss_plugindir.str())) {
        // The plug-in folder for content plug-ins does
        // not exist.
#ifdef WITH_DEBUGLOG
        stringstream ss_debuglog;
        ss_debuglog << "Could not find the content plug-ins directory '";
        ss_debuglog << ss_plugindir.str() << "', skipping.";
        DebugLog::debuglog(ss_debuglog.str());
        return input;
#endif
    }

    std::vector<std::string> vs_plugins;
    for (fs::recursive_directory_iterator it(ss_plugindir.str()), end; it != end; ++it) {
        if (!fs::is_directory(it->path())) {
            std::string filename = it->path().filename().string();
            if (strEndsWith(filename, ".plugin.js")) {
                std::lock_guard<std::mutex> lock(mtx);
                vs_plugins.push_back(filename);
            }
        }
    }

    for (size_t i = 0; i < vs_plugins.size();) {
#ifdef WITH_DEBUGLOG
        stringstream ss_debuglog;
        ss_debuglog << "Applying content plug-in " << vs_plugins[i] << " ...";
        DebugLog::debuglog(ss_debuglog.str());
#endif

        // Read and process the file.
        stringstream pluginfile;
        pluginfile << ss_plugindir.str() << "/" << vs_plugins[i++];

        std::ifstream infile(pluginfile.str());
        std::string file_contents = [&infile] {
            std::ostringstream ss{};
            ss << infile.rdbuf();
            return ss.str();
        }();

        // Call the method!
        try {
            // Give me a local handle to the script:
            DukValue plugin = dukglue_peval<DukValue>(ctx, file_contents.c_str());

            // Assume that all content plug-ins have a process(string)
            // method to use.
            const char* ret = dukglue_pcall_method<const char*>(ctx, plugin, "process", input.c_str());
            input = ret;
        }
        catch (DukException& e) {
            // Damn.
#ifdef WITH_DEBUGLOG
            stringstream ss_debuglog;
            ss_debuglog << "Failed to call process() from the plug-in! You should report or fix that." << endl;
            ss_debuglog << "Exception: ";
            ss_debuglog << e.what() << endl;
            DebugLog::debuglog(ss_debuglog.str());
#endif
        }
    }

    return input;
}


std::string Plugins::processHeaderPlugins() {
    // Traverses through the {plugindir}/header directory,
    // executes all process() methods (if any), returns the
    // results.
    if (!enabled) {
        return "";
    }

    std::stringstream ss_plugindir, ss_html;
    ss_plugindir << plugindir << "/header";

    if (!fs::exists(ss_plugindir.str())) {
        // The plug-in folder for content plug-ins does
        // not exist.
#ifdef WITH_DEBUGLOG
        stringstream ss_debuglog;
        ss_debuglog << "Could not find the header plug-ins directory '";
        ss_debuglog << ss_plugindir.str() << "', skipping.";
        DebugLog::debuglog(ss_debuglog.str());
        return "";
#endif
    }

    std::vector<std::string> vs_plugins;
    for (fs::recursive_directory_iterator it(ss_plugindir.str()), end; it != end; ++it) {
        if (!fs::is_directory(it->path())) {
            std::string filename = it->path().filename().string();
            if (strEndsWith(filename, ".plugin.js")) {
                std::lock_guard<std::mutex> lock(mtx);
                vs_plugins.push_back(filename);
            }
        }
    }

    for (size_t i = 0; i < vs_plugins.size();) {
#ifdef WITH_DEBUGLOG
        stringstream ss_debuglog;
        ss_debuglog << "Applying header plug-in " << vs_plugins[i] << " ...";
        DebugLog::debuglog(ss_debuglog.str());
#endif

        // Read and process the file.
        stringstream pluginfile;
        pluginfile << ss_plugindir.str() << "/" << vs_plugins[i++];

        std::ifstream infile(pluginfile.str());
        std::string file_contents = [&infile] {
            std::ostringstream ss{};
            ss << infile.rdbuf();
            return ss.str();
        }();

        // Call the method!
        try {
            // Give me a local handle to the script:
            DukValue plugin = dukglue_peval<DukValue>(ctx, file_contents.c_str());

            // Assume that all header plug-ins have a process(string)
            // method to use.
            const char* ret = dukglue_pcall_method<const char*>(ctx, plugin, "process");
            ss_html << ret << std::endl;
        }
        catch (DukException& e) {
            // Damn.
#ifdef WITH_DEBUGLOG
            stringstream ss_debuglog;
            ss_debuglog << "Failed to call process() from the plug-in! You should report or fix that." << endl;
            ss_debuglog << "Exception: ";
            ss_debuglog << e.what() << endl;
            DebugLog::debuglog(ss_debuglog.str());
#endif
        }
    }

    return ss_html.str();
}


std::string Plugins::processFooterPlugins() {
    // Traverses through the {plugindir}/footer directory,
    // executes all process() methods (if any), returns the
    // results.
    if (!enabled) {
        return "";
    }

    std::stringstream ss_plugindir, ss_html;
    ss_plugindir << plugindir << "/footer";

    if (!fs::exists(ss_plugindir.str())) {
        // The plug-in folder for content plug-ins does
        // not exist.
#ifdef WITH_DEBUGLOG
        stringstream ss_debuglog;
        ss_debuglog << "Could not find the footer plug-ins directory '";
        ss_debuglog << ss_plugindir.str() << "', skipping.";
        DebugLog::debuglog(ss_debuglog.str());
        return "";
#endif
    }

    std::vector<std::string> vs_plugins;
    for (fs::recursive_directory_iterator it(ss_plugindir.str()), end; it != end; ++it) {
        if (!fs::is_directory(it->path())) {
            std::string filename = it->path().filename().string();
            if (strEndsWith(filename, ".plugin.js")) {
                std::lock_guard<std::mutex> lock(mtx);
                vs_plugins.push_back(filename);
            }
        }
    }

    for (size_t i = 0; i < vs_plugins.size();) {
#ifdef WITH_DEBUGLOG
        stringstream ss_debuglog;
        ss_debuglog << "Applying footer plug-in " << vs_plugins[i] << " ...";
        DebugLog::debuglog(ss_debuglog.str());
#endif

        // Read and process the file.
        stringstream pluginfile;
        pluginfile << ss_plugindir.str() << "/" << vs_plugins[i++];

        std::ifstream infile(pluginfile.str());
        std::string file_contents = [&infile] {
            std::ostringstream ss{};
            ss << infile.rdbuf();
            return ss.str();
        }();

        // Call the method!
        try {
            // Give me a local handle to the script:
            DukValue plugin = dukglue_peval<DukValue>(ctx, file_contents.c_str());

            // Assume that all footer plug-ins have a process(string)
            // method to use.
            const char* ret = dukglue_pcall_method<const char*>(ctx, plugin, "process");
            ss_html << ret << std::endl;
        }
        catch (DukException& e) {
            // Damn.
#ifdef WITH_DEBUGLOG
            stringstream ss_debuglog;
            ss_debuglog << "Failed to call process() from the plug-in! You should report or fix that." << endl;
            ss_debuglog << "Exception: ";
            ss_debuglog << e.what() << endl;
            DebugLog::debuglog(ss_debuglog.str());
#endif
        }
    }

    return ss_html.str();
}


std::string Plugins::enable(std::string filename) {
    // Enables the plug-in <filename> by removing the "-disabled" part
    // of its file extension (if applicable).
    if (!enabled) {
        return "Plug-ins are disabled in your configuration - sorry! You might want to change that.";
    }

    if (!fs::exists(plugindir) || !fs::is_directory(plugindir)) {
        // Failed to traverse anything.
        return "Sorry, we could not find the plug-in directory.";
    }

    bool found = false;
    for (fs::recursive_directory_iterator it(plugindir), end; it != end; ++it) {
        if (!fs::is_directory(it->path())) {
            std::string foundname = it->path().filename().string();

            std::ostringstream infilename;
            infilename << filename << ".plugin-disabled.js";

            if (foundname == infilename.str()) {
                // Found it. Rename and log:
                std::lock_guard<std::mutex> lock(mtx);
                std::ostringstream outfilename;

                char separator = it->path().preferred_separator;
                outfilename <<  it->path().parent_path() << separator << filename << ".plugin.js";

                fs::rename(it->path().string(), outfilename.str());
                found = true;
                continue;
            }
        }
    }

    if (found) {
        return "Successfully enabled the plug-in.";
    }

    std::ostringstream ret;
    ret << "We could not find the '" << filename << "' plug-in. :-(";
    return ret.str();
}


std::string Plugins::disable(std::string filename) {
    // Disables the plug-in <filename> by adding a "-disabled" part
    // to its file extension (if applicable).
    if (!enabled) {
        return "Plug-ins are disabled in your configuration - sorry! You might want to change that.";
    }

    if (!fs::exists(plugindir) || !fs::is_directory(plugindir)) {
        // Failed to traverse anything.
        return "Sorry, we could not find the plug-in directory.";
    }

    bool found = false;
    for (fs::recursive_directory_iterator it(plugindir), end; it != end; ++it) {
        if (!fs::is_directory(it->path())) {
            std::string foundname = it->path().filename().string();

            std::ostringstream infilename;
            infilename << filename << ".plugin.js";

            if (foundname == infilename.str()) {
                // Found it. Rename and log:
                std::lock_guard<std::mutex> lock(mtx);
                std::ostringstream outfilename;

                char separator = it->path().preferred_separator;
                outfilename <<  it->path().parent_path() << separator << filename << ".plugin-disabled.js";

                fs::rename(it->path().string(), outfilename.str());
                found = true;
                continue;
            }
        }
    }

    if (found) {
        return "Successfully disabled the plug-in.";
    }

    std::ostringstream ret;
    ret << "We could not find the '" << filename << "' plug-in. :-(";
    return ret.str();
}


std::string Plugins::listAll() {
    // Lists all available plug-ins unter <plugindir>. Returns a formatted
    // list (where C = Contents etc.):
    //
    //  Enabled   Name                    Affects
    //  X         hello-world             C
    //            sample-plugin           CHF
    if (!enabled) {
        return "Plug-ins are disabled in your configuration - sorry! You might want to change that.";
    }

    // Recursively list all plug-in files into a vector first so we can
    // find out how wide the name column needs to be
    std::vector<PluginFile> pfiles;

    // The three plug-in folders as of now are: /contents, /header, /footer.
    // Traverse through them separately, filling the vector on the way:
    std::stringstream ss_plugindir;
    ss_plugindir << plugindir << "/contents";

    if (!traversePlugins(&pfiles, ss_plugindir.str(), 'C')) {
        // Failed to traverse the contents plug-ins.
#ifdef WITH_DEBUGLOG
        DebugLog::debuglog("An error occurred while trying to traverse the content plug-ins directory. It probably doesn't exist.");
#endif
    }

    ss_plugindir.str("");
    ss_plugindir << plugindir << "/header";

    if (!traversePlugins(&pfiles, ss_plugindir.str(), 'H')) {
        // Failed to traverse the header plug-ins.
#ifdef WITH_DEBUGLOG
        DebugLog::debuglog("An error occurred while trying to traverse the header plug-ins directory. It probably doesn't exist.");
#endif
    }

    ss_plugindir.str("");
    ss_plugindir << plugindir << "/footer";

    if (!traversePlugins(&pfiles, ss_plugindir.str(), 'F')) {
        // Failed to traverse the footer plug-ins.
#ifdef WITH_DEBUGLOG
        DebugLog::debuglog("An error occurred while trying to traverse the footer plug-ins directory. It probably doesn't exist.");
#endif
    }

    // Done.
#ifdef WITH_DEBUGLOG
    std::stringstream ss_debuglog;
    ss_debuglog << "Found " << pfiles.size() << " plug-in" << (pfiles.size() > 1 ? "s" : "") << ".";
    DebugLog::debuglog(ss_debuglog.str());
#endif

    if (pfiles.size() == 0) {
        return "You don't have any plug-ins.";
    }

    // Find the longest file name:
    unsigned int maxLen = 0;
    std::vector<PluginFile>::iterator it;

    for (it = pfiles.begin(); it < pfiles.end(); it++) {
        if (it->shortname.length() > maxLen) {
            maxLen = it->shortname.length();
        }
    }

    // Output:
    std::ostringstream ss_out;
    ss_out << std::endl;
    ss_out << " Enabled  " << std::left << std::setw(maxLen) << std::setfill(' ') << "Name" << std::setw(0) << "  Affects" << std::endl;
    for (it = pfiles.begin(); it < pfiles.end(); it++) {
        ss_out << (it->enabled ? " X" : "  ") << "        ";
        ss_out << std::left << std::setw(maxLen) << std::setfill(' ') << it->shortname << std::setw(0);

        std::string strAffects;
        for (auto const& affects : it->vc_affects) { strAffects += affects; }

        ss_out << "  " << strAffects;
        ss_out << std::endl;
    }
    ss_out << std::endl;

    return ss_out.str();
}


bool Plugins::traversePlugins(std::vector<PluginFile>* pfiles, std::string directory, char affects) {
    // Fills the <pfiles> vector with the filenames from <directory>.
    // affects: C = Contents, H = Header, F = Footer.
    // Returns false on errors.
    if (!fs::exists(directory)) {
        return false;
    }

    for (fs::recursive_directory_iterator it(directory), end; it != end; ++it) {
        if (!fs::is_directory(it->path())) {
            std::lock_guard<std::mutex> lock(mtx);
            if (!strEndsWith(it->path().filename().string(), ".plugin.js") && !strEndsWith(it->path().filename().string(), ".plugin-disabled.js")) {
                // This is not a valid plug-in file name. Skip.
                continue;
            }

            // Does <pfiles> already have this file? If so, add this <affects>,
            // else, add the whole file.
            std::vector<PluginFile>::iterator vit;
            bool updating = false;

            for (vit = pfiles->begin(); vit < pfiles->end(); vit++) {
                if (vit->filename == it->path().filename().string()) {
                    updating = true;

                    // As we never visit a plug-in file twice, an existing
                    // file of this name (probably) means that we have a
                    // new <affects>. If this is not true, the plug-in
                    // author has fucked up the directory structures. Well,
                    // we don't care here.
                    vit->vc_affects.push_back(affects);
                    break;
                }
            }

            if (!updating) {
                std::string filename = it->path().filename().string();

                PluginFile pluginfile;
                pluginfile.enabled = strEndsWith(filename, ".plugin.js");
                pluginfile.filename = filename;
                pluginfile.vc_affects.push_back(affects);

                // strip the extension for displaying:
                size_t dot1 = filename.rfind(".plugin.js");
                size_t dot2 = filename.rfind(".plugin-disabled.js");

                if (dot1 != std::string::npos) { filename.resize(dot1); }
                if (dot2 != std::string::npos) { filename.resize(dot2); }

                pluginfile.shortname = filename;

                pfiles->push_back(pluginfile);
            }
        }
    }

    return true;
}



// ------------------------
// Exported class:
// ------------------------

Exports::Exports() {
    // noop
}


int Exports::getVersion() {
    return APP_VERSION;
}


#endif
