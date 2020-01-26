/*
 * blogcpp :: https://www.blogcpp.org
 * Markdown-to-HTML class [header].
 */

#ifndef MARKDOWN_PARSER_H
#define MARKDOWN_PARSER_H // Once is enough.

#include <string>
#include <iostream>
#include <sstream>
#include <regex>
#include <vector>
#include <unordered_map>
#include <stdexcept>

#include "MediaEmbedderClass.h"
#include "MarkdownReturnClass.h"
#include "constants.h"
#ifdef WITH_DEBUGLOG
#include "DebugClass.h"
#endif

using namespace std;

class MarkdownParser {
  public:
    MarkdownReturn markdownify(string inputtext, bool basic_markdown, bool no_emojis);
    void prepare(string posttxt);
    void parse(const string inputline);
    string getParsedText() { return full_markdown_text; }
    vector<string> getCodeLanguages() { return used_code_languages; }
    void cleanup();

    MarkdownParser(bool basic_markdown, bool add_embeds);

  private:
    string full_markdown_text;
    vector<string> used_code_languages;
    unordered_map<int, string> legend_urls;

    stringstream ss_codelang;

    bool in_blockquote;
    bool in_ol;
    bool in_ul;
    bool in_code;
    bool in_raw_html;

    bool dont_add_paragraphs;

    bool dont_use_full_markdown;
    bool use_embeds;
};

#endif
