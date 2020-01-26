/*
 * blogcpp :: https://www.blogcpp.org
 * Emoji parser class.
 */

#include "EmojiParserClass.h"

string EmojiParser::clear(const string text) {
    // Removes emojis from <text>.
    if (text.length() == 0) {
        return "";
    }

    istringstream iss(text);
    stringstream ret;
    string word;

#ifdef WITH_DEBUGLOG
    stringstream ss_debuglog;

    ss_debuglog << "Emojis: Clearing '" << text << "' ...";

    DebugLog::debuglog(ss_debuglog.str());
    ss_debuglog.str("");
#endif

    while (iss >> word) {
        // Remove <word> if it is an emoji.
        unordered_map<string, string>::const_iterator emoji = emojis.find(word);
        if (emoji != emojis.end()) {
            ret << "";
        }
        else {
            ret << word;
        }
        ret << " ";
    }

    return ret.str();
}


string EmojiParser::parse(const string text) {
    // Puts emojis into <text>.
    if (text.length() == 0) {
        return "";
    }

    istringstream iss(text);
    stringstream temp;
    ostringstream ret;
    string word;

#ifdef WITH_DEBUGLOG
    ostringstream ss_debuglog;

    ss_debuglog << "Emojis: Parsing '" << text << "' ...";

    DebugLog::debuglog(ss_debuglog.str());
#endif

    // Try to find leading and trailing white spaces. Add them back
    // after all parsing has been done.
    smatch match;
    regex whitespaces("^([\\s\\t]*).*?([\\s\\t]*)$");
    bool bHasSurroundingWS = regex_search(text, match, whitespaces);

    if (bHasSurroundingWS && (match[1].length() > 0)) {
        // Add the leading spaces:
        temp << match[1];
    }

    while (iss >> word) {
        // Add every word to the output string; convert it first
        // if it's an emoji.
        unordered_map<string, string>::const_iterator emoji = emojis.find(word);
        if (emoji != emojis.end()) {
            temp << emoji->second;
        }
        else {
            temp << word;
        }
        temp << " "; // This is double
    }

    // Remove the last space again. (Alternatively, count the tokens beforehand,
    // but that would probably double the calculations.)
    if (temp.str().length() > 0) {
        string truncated = temp.str();
        truncated.erase(truncated.end() - 1, truncated.end());
        temp.str(truncated);
    }

    // We need a copy of temp here, since the previous if(){} might have
    // destroyed our poor little stringstream. :-(
    ret << temp.str();
    if (bHasSurroundingWS && match.size() == 3 && (match[2].length() > 0)) {
        // Add the trailing spaces:
        ret << match[2];
    }

    return ret.str();
}
