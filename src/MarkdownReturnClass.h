/*
 * blogcpp :: https://www.blogcpp.org
 * Return type for Markdown actions
 */

#ifndef MARKDOWN_RETURN
#define MARKDOWN_RETURN // Once is enough.

#include <string>
#include <vector>

class MarkdownReturn {
  public:
    std::vector<std::string> used_languages;
    std::string parsed_text;
};


#endif
