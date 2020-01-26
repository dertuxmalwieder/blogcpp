/*
 * blogcpp :: https://www.blogcpp.org
 * A couple of helper methods for blogcpp.
 *
 * Helper functions are functions which are independent of blogcpp-specific
 * variables like vs_posts et cetera. This file is meant to be as portable
 * as possible.
 */

#ifndef HAS_HELPERS
#define HAS_HELPERS // Once is enough.

#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <regex>
#ifdef _WIN32
// To open files with their default editor, Windows's API is used.
// However, <windows.h> adds weird versions of std::min and std::max
// if it's not told not to.
#  define NOMINMAX // Tell that.
#  include <windows.h>
#  include <ShellApi.h>
#elif defined(__linux__) || defined(__unix__)
#  include <stdlib.h>
#  include <unistd.h>
#else
// I can't really support everything. However, maybe the checks above
// are just incomplete? There is a bug tracker for you ... wink! ;-)
#  error This file is expected to break on your system. Please examine the source code and check if you could contribute a fix.
#endif
#include <unicode/unistr.h> // requires ICU.

#include "filesystem.h"

using namespace std;
using namespace icu;

namespace fs = NAMESPACE_FILESYSTEM;


// -----------------------------
// File Helpers
// -----------------------------

inline bool openWithEditor(const char* filename) {
    // Opens <filename> in your default editor.
#ifdef _WIN32
    // Windows version, using the Windows API.
    return reinterpret_cast<int>(ShellExecuteA(NULL, "open", filename, NULL, NULL, SW_SHOW)) > 32;
#else
    // Other system, other ways...
    string editor = "";

    if (getenv("VISUAL") != NULL) {
        editor = getenv("VISUAL");
    }
    else if (getenv("EDITOR") != NULL) {
        editor = getenv("EDITOR");
    }
    else {
#ifdef __linux__
        editor = "xdg-open";
#else
        // No standard editor found. Sorry!
        return false;
#endif
    }

    return system(string(editor + " \"" + filename + "\"").c_str()) == 0;
#endif
}


inline void useOrCreatePathWithPrefix(const string prefix, const string relpath) {
    // Creates the directory "prefix/relpath" if it does not exist yet.
    // Requires the std::filesystem namespace as set in filesystem.h.
    stringstream pathcheck;
    pathcheck << prefix << relpath;

    if (!fs::exists(pathcheck.str())) {
        // Create the output directory.
        fs::create_directories(pathcheck.str());
    }
}


// -----------------------------
// String Helpers
// -----------------------------


inline string trim(string inputstring) {
    // Removes leading and trailing whitespaces from <inputstring>.
    auto wsfront = find_if_not(inputstring.begin(), inputstring.end(), [](int c) {
        return isspace(c);
    });
    auto wsback = find_if_not(inputstring.rbegin(), inputstring.rend(), [](int c) {
        return isspace(c);
    }).base();
    return (wsback <= wsfront ? string() : string(wsfront, wsback));
}


inline string lowercase(string input) {
    // Transforms a string into lowercase.
    const char* inputcstr = input.c_str();
    string ret;

    UnicodeString uinput(inputcstr, "UTF-8");
    uinput = uinput.toLower();
    uinput.toUTF8String(ret);
    return ret;
}


inline string genFileName(string input) {
    // Generates a valid file name from an input string.
    const string forbiddenChars = "\\/:?\"<>|'";
    transform(input.begin(), input.end(), input.begin(), [&forbiddenChars](char ch) {
        // replace all invalid characters by hyphens
        return forbiddenChars.find(ch) != string::npos ? '-' : ch;
    });

    return input;
}


inline string vectorJoin(vector<string> vec) {
    // Joins the elements of a vector by ",".
    ostringstream ret;
    for (auto i = vec.begin(); i != vec.end(); i++) {
        if (i != vec.begin()) {
            ret << ", ";
        }
        ret << *i;
    }
    return ret.str();
}


inline vector<string> vectorSplit(string inputstring, string divider = ";") {
    // Returns a vector of elements in the <inputstring>.
    vector<string> ret;

    if (inputstring.size() == 0) {
        // Skip empty strings.
        return ret;
    }

    regex re(divider); // Tokenize.
    sregex_token_iterator it(inputstring.begin(), inputstring.end(), re, -1);
    sregex_token_iterator reg_end;

    for (; it != reg_end; ++it) {
        string toadd = trim(it->str());
        if (toadd.empty()) {
            // Empty elements could happen if the user writes ";;" or
            // if we have a page which only has tags, no categories. In
            // this case, we should not try to add an "empty item" as
            // that would not make any sense, would it?
            continue;
        }
        ret.push_back(trim(it->str()));
    }

    return ret;
}


inline bool vectorSort(string a, string b) {
    // Sorting helper for string vectors (alphabetically).
    return a < b;
}


inline bool strEndsWith(const string& s, const string& ending) {
    // true if <s> ends with <ending>.
    return (s.size() >= ending.size()) && equal(ending.rbegin(), ending.rend(), s.rbegin());
}


inline string mergeMultipleSlashes(string inputstring) {
    // Makes "a/b/c" out of "a//b//////c".
    regex slash("([^:])/+"); // Do not replace "https://".
    return regex_replace(inputstring, slash, "$1/");
}


// -----------------------------
// Date/Time Helpers
// -----------------------------


inline string timeNow(const char* format = "%a, %d %b %Y %T %z") {
    // Returns a formatted "now()" string. The output format defaults to ISO time.
    stringstream ss_time;
    time_t t = time(0);
#if (defined(_WIN32) && !__INTEL_COMPILER)
    // Microsoft wants to have localtime_s here.
    tm now;
    localtime_s(&now, &t);
#else
    tm* now = localtime(&t);
#endif
#ifdef _WIN32
    ss_time << put_time(&now, format);
#else
    ss_time << put_time(now, format);
#endif
    return ss_time.str();
}


inline void parseDatestringToTm(string in_datetime, string inputfile, tm &input_tm) {
    // Converts the in_datetime from a post or page into a struct tm.
    istringstream ss_inputdate(in_datetime);

    // Fill the tm:
    ss_inputdate >> get_time(&input_tm, "%Y-%m-%d %H:%M:%S");

    if (ss_inputdate.fail()) {
        cout << "Failed to parse the date from the file " << inputfile << "." << endl;
        cout << "Please check it before you continue." << endl;
    }
}


inline bool isFutureDate(string datestring) {
    // Returns true is <datestring> is in the future; else, false.
    time_t time_now = time(0);
    tm tm_in;

    istringstream timestream(datestring);
    timestream >> get_time(&tm_in, "%Y-%m-%d %H:%M:%S");
    time_t time_in = mktime(&tm_in);

    double difference = difftime(time_now, time_in) / (60 * 60 * 24);

    return (difference < 0);
}


inline string parseTmToPath(tm tm_t, string cfg_permalinks) {
    // Converts the tm_t from a post or page to a correct path structure (see above).
    ostringstream ss_outputdate;

    const char* const cstr_permalinks = cfg_permalinks.c_str();
    ss_outputdate << put_time(&tm_t, cstr_permalinks);

    return ss_outputdate.str();
}

#endif
