/*
 * blogcpp :: https://www.blogcpp.org
 * Markdown-to-HTML wrapper, allowing a more abstract usage of the MarkdownParser.
 */

#include "MarkdownWrapperClass.h"

MarkdownReturn MarkdownWrapper::markdownify(std::string inputtext, bool basic_markdown, bool no_emojis, bool cfg_embeds) {
    // Returns the HTML text of a Markdown'ed input text.
    // If basic_markdown is true, the parser will only add line breaks.
    // If no_emojis is true, the parser will skip adding emojis.
    MarkdownReturn ret;

    std::ostringstream ostream;
    std::stringstream istream;
    std::string line;

    // Remove <!--more--> from the actual text (we don't need it anymore,
    // it should have been processed by the excerpt extractor by this point.)
    std::regex re_morelink("<!--\\s?more\\s?-->");
    inputtext = regex_replace(inputtext, re_morelink, "");

    istream << inputtext;

    // Send the <inputtext> linewise through our Markdown parser.
    // If the user did not disable them, additionally try to parse embedded media.
    MarkdownParser parser(basic_markdown, cfg_embeds);
    EmojiParser eparser;

    parser.prepare(inputtext); // Inspect the article before doing the actual work

    while (getline(istream, line)) {
        if (!no_emojis) {
            // Put some emojis into the line before markdownifying it.
            line = eparser.parse(line);
        }
        parser.parse(line);
    }

    parser.cleanup(); // Close tags in case any were left open.
    ostream << parser.getParsedText();

    ret.parsed_text = ostream.str();
    ret.used_languages = parser.getCodeLanguages();

    return ret;
}
