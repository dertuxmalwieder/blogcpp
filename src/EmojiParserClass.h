/*
 * blogcpp :: https://www.blogcpp.org
 * Emoji parser class [header].
 */

#ifndef HAS_EMOJIS
#define HAS_EMOJIS // Once is enough.

#include <string>
#include <iostream>
#include <sstream>
#include <regex>
#include <unordered_map>

#ifdef WITH_DEBUGLOG
#include "DebugClass.h"
#endif

using namespace std;

class EmojiParser {
  public:
    string clear(const string text);
    string parse(const string text);

  private:
    // The emojis are kept in an assignment table for your and
    // my own convenience.
    // Code reference: http://emojipedia.org/people/
    const unordered_map<string, string> emojis = {
        { ":-)",  u8"\U0001F642"s },
        { ";-)",  u8"\U0001F609"s },
        { ":-D",  u8"\U0001F600"s },
        { ":-(",  u8"\U00002639"s },
        { ":'(",  u8"\U0001F62D"s },
        { ":-|",  u8"\U0001F610"s },
        { ">:)",  u8"\U0001F608"s },
        { ">:-)", u8"\U0001F608"s },
        { ">:(",  u8"\U0001F620"s },
        { ">:-(", u8"\U0001F620"s },
        { ":-*",  u8"\U0001F618"s },
        { ":-O",  u8"\U0001F62E"s },
        { ":-o",  u8"\U0001F62E"s },
        { ":-S",  u8"\U0001F615"s },
        { ":-s",  u8"\U0001F615"s },
        { ":-#",  u8"\U0001F636"s },
        { "0:-)", u8"\U0001F607"s },
        { ":o)",  u8"\U0001F921"s },
        { "<_<",  u8"\U0001F612"s },
        { "^^",   u8"\U0001F60A"s },
        { "^_^",  u8"\U0001F60A"s },
        { "<3",   u8"\U00002764"s },
        { "m(",   u8"\U0001F926"s }
    };
};

#endif
