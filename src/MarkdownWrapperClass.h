/*
 * blogcpp :: https://www.blogcpp.org
 * Markdown-to-HTML wrapper, allowing a more abstract usage of the MarkdownWrapper [header].
 */

#ifndef MARKDOWN_WRAPPER
#define MARKDOWN_WRAPPER

#include <string>
#include <sstream>
#include <regex>

#include "EmojiParserClass.h"
#include "MarkdownParserClass.h"
#include "MarkdownReturnClass.h"

class MarkdownWrapper {
  public:
    static MarkdownReturn markdownify(std::string inputtext, bool basic_markdown, bool no_emojis, bool cfg_embeds);
};


#endif
