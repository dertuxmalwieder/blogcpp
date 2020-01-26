/*
 * blogcpp :: https://www.blogcpp.org
 * Debug logging functionality class [header].
 */

#ifdef WITH_DEBUGLOG

#include <fstream>
#include "DebugClass.h"
#include "constants.h"


void DebugLog::resetDebuglog() {
    // Resets the debug log file.
    std::ofstream ofs;
    ofs.open(DEBUGLOGFILE, std::ofstream::out | std::ofstream::trunc);
    ofs.close();
}

void DebugLog::debuglog(const std::string& debugtext) {
    // Writes a debug log file if required.
    std::ofstream log_file(DEBUGLOGFILE, std::ios_base::out | std::ios_base::app);
    log_file << debugtext << std::endl;
}

#endif
