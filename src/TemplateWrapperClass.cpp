/*
 * blogcpp :: https://www.blogcpp.org
 * Class to make using the inja template engine easier.
 */

#include "TemplateWrapperClass.h"

inja::Template TemplateWrapper::loadTemplate(inja::Environment& env, std::string tplfile) {
    // Tries to load <tplfile> from <env>. Adds debug output where necessary.
    try {
        return env.parse_template(tplfile);
    }
    catch (std::exception& e) {
#ifdef WITH_DEBUGLOG
        std::stringstream ss_debuglog;
        ss_debuglog << "inja error trying to load '" << tplfile << "': " << e.what();
        DebugLog::debuglog(ss_debuglog.str());
#endif
        std::cout << std::endl;
        std::cout << "Could not parse your '" << tplfile << "' template: " << e.what() << std::endl;
        std::cout << "Exiting." << std::endl;
        exit(1);
    }

    return env.parse("");
}

void TemplateWrapper::writeTemplate(inja::Template tpl, inja::Environment& env, json data, std::string outfile) {
    // Tries to write the <outfile>. Adds debug output where necessary.
    try {
        env.write(tpl, data, outfile);
    }
    catch (std::exception& e) {
#ifdef WITH_DEBUGLOG
        std::stringstream ss_debuglog;
        ss_debuglog << "inja error trying to write '" << outfile << "': " << e.what();
        DebugLog::debuglog(ss_debuglog.str());
#endif
        std::cout << std::endl;
        std::cout << "Could not write into '" << outfile << "': " << e.what() << std::endl;
        std::cout << "Exiting." << std::endl;
        exit(1);
    }
}