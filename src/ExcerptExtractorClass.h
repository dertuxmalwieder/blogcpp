/*
 * blogcpp :: https://www.blogcpp.org
 * Excerpt extractor class [header].
 */

#ifndef EXCERPT_EXTRACTOR
#define EXCERPT_EXTRACTOR // Once is enough.

#include <iostream>
#include <string>
#include <sstream>
#include <regex>
#include <iterator>
#ifdef WITH_DEBUGLOG
#include "DebugClass.h"
#endif

using namespace std;

class ExcerptExtractor {
  public:
    ExcerptExtractor(string in_string, int in_excerpt_length);

    string extractExcerpt();
    bool shortened;

  private:
    string string_to_extract;
    int max_sentences;
};

#endif
