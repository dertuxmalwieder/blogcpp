/*
 * blogcpp :: https://www.blogcpp.org
 * Markdown-to-HTML class.
 */

#include "MarkdownParserClass.h"

MarkdownParser::MarkdownParser(bool basic_markdown, bool add_embeds) {
    // Initialize our booleans as "false" so the parser does not
    // assume that it was inside a tag yet.
    in_blockquote = false;
    in_ol = false;
    in_ul = false;
    in_code = false;
    in_raw_html = false;

    dont_add_paragraphs = false;

    dont_use_full_markdown = basic_markdown;
    use_embeds = add_embeds;
}

void MarkdownParser::prepare(string posttxt) {
    // Perform a complete first inspection of our <inputstream>, e.g.
    // for getting the list of referenced URLs, before doing the actual
    // parser work.
    stringstream istream;
    string line;
    regex legend_url("^\\[(\\d+)\\]:(.+)");
    smatch match;

    istream << posttxt;
    while (getline(istream, line)) {
        // Find and store potential URLs:
        if (!regex_search(line, match, legend_url)) {
            // No need to search further in this line - no URL found.
            continue;
        }

        try {
            legend_urls.insert({stoi(match[1]), match[2]});
        }
        catch (const invalid_argument& ia) {
            // stoi() failed. :-(
            cout << "[Markdown] One of your URLs could not be parsed correctly: " << ia.what() << endl << endl;
#ifdef WITH_DEBUGLOG
            stringstream ss_debuglog;
            ss_debuglog << "[EXCEPTION] Markdown: Failed to parse " << match[1] << " as a number: " << ia.what();
            DebugLog::debuglog(ss_debuglog.str());
#endif
        }
    }
}

void MarkdownParser::parse(const string inputline) {
    // Process <inputline> from Markdown, return HTML text.
    // Reference: http://daringfireball.net/projects/markdown/syntax
    //
    // blogcpp currently markdownifies one line at a time, so we need to
    // store whether we're inside an <ol>, <ul>, <code> or <blockquote> area.
    // We can use the bools in_ul, in_ol, in_blockquote and in_code for that.

    string ret;
    stringstream ss_ret, ss_temp;
#ifdef WITH_DEBUGLOG
    stringstream ss_debuglog;

    ss_debuglog << "Markdown: Parsing '" << inputline << "' ...";
    if (dont_use_full_markdown) {
        ss_debuglog << " (reduced mode)";
    }

    DebugLog::debuglog(ss_debuglog.str());
    ss_debuglog.str("");
#endif

    ss_ret << full_markdown_text; // We're adding every line up (for multi-line blockquotes etc.)
    ret = inputline;

    if (!dont_use_full_markdown) {
        // The user can disable full Markdown parsing per file.
        // Basic "Markdown" parsing only adds paragraphs and calls the MediaEmbedder.
        try {
            regex bold_and_italic("\\*{3}(.*?)\\*{3}");
            regex bold("\\*{2}(.*?)\\*{2}");
            regex italic("\\*(.*?)\\*");

            regex pre("`(.*?)`");
            regex code("^\\s{4}(.*)");
            regex codelang("^\\s{4}::::(\\S+)");

            regex del("~~(.*?)~~");

            regex h6("^#{6}[\\s\\t]*(.*)(#{6})?");
            regex h5("^#{5}[\\s\\t]*(.*)(#{5})?");
            regex h4("^#{4}[\\s\\t]*(.*)(#{4})?");
            regex h3("^#{3}[\\s\\t]*(.*)(#{3})?");
            regex h2("^#{2}[\\s\\t]*(.*)(#{2})?");
            regex h1("^#[\\s\\t]*(.*)#?");

            regex blockquote("^>[\\s\\t]*(.*)");

            regex hr("^([\\*\\-_]\\s?){3,}\\s?$");

            regex ul("^[\\*\\+\\-][\\s\\t]+(.*)");
            regex ol("^\\d+\\.[\\s\\t]+(.*)");

            regex url("\\[(.+?)\\]\\((.+?)\\)");      // [link text](URL)
            regex url_alt("\\[(.+?)\\]\\[(\\d+)\\]"); // [link text][refno]
            regex legend_url("^\\[(\\d+)\\]:(.+)");   // [refno]:[URL]

            regex img("!\\[(.*?)\\]\\((.*?)\\)");

            regex rawhtml_start("<rawhtml>");
            regex rawhtml_stop("</rawhtml>");

            // Don't parse raw HTML parts:
            if (regex_search(ret, rawhtml_start)) {
#ifdef WITH_DEBUGLOG
                DebugLog::debuglog("Raw HTML part found. Skipping.");
#endif
                ret = regex_replace(ret, rawhtml_start, "");
                in_raw_html = true;
            }

            if (in_raw_html) {
                if (regex_search(ret, rawhtml_stop)) {
                    ret = regex_replace(ret, rawhtml_stop, "");
                    in_raw_html = false;
                }

                ss_ret << ret << "\n";
                full_markdown_text = ss_ret.str();
                return;
            }

            // Skip URLs from the legend, already processed in prepare():
            if (regex_match(ret, legend_url)) { return; }

            // We don't add <p> tags around just everything, do we?
            dont_add_paragraphs =
                regex_match(ret, h1) || // implies h2-h6
                regex_match(ret, code) ||
                regex_match(ret, blockquote) ||
                regex_match(ret, hr) ||
                regex_match(ret, ul) ||
                regex_match(ret, ol);

            // All regular expressions are set up. Time to process them.

            // Find code:
            if (regex_match(ret, code)) {
                cmatch cm;
                if (regex_match(ret.c_str(), cm, codelang)) {
                    // The language of the code block is set, e.g. ::::dos.
                    // Add the language to the number of required Highlight.js
                    // components:
                    ss_codelang.str("");
                    ss_codelang << cm[1];
                    used_code_languages.push_back(ss_codelang.str());
                    ret = ""; // Don't leave the language command in the output file.
                }
                else {
                    if (!in_code) {
                        in_code = true;
                        ret = regex_replace(ret, code, ss_codelang.str().empty() ? "<pre><code>$1" : "<pre><code lang=\"" + ss_codelang.str() + "\">$1");
                    }
                    else {
                        // An already open code block is continued here.
                        // Don't add another <code> tag.
                        ret = regex_replace(ret, code, "$1");
                    }
                }
            }
            else if (in_code) {
                // No code block found, but we had one open.
                // Close it.
                ret = "</code></pre>\n" + ret;
                in_code = false;
                dont_add_paragraphs = true;
            }

            ret = regex_replace(ret, del, "<del>$1</del>");

            // Find blockquote:
            if (regex_match(ret, blockquote)) {
                if (!in_blockquote) {
                    in_blockquote = true;
                    ret = regex_replace(ret, blockquote, "<blockquote><p>$1</p>");
                }
                else {
                    // An already open blockquote is continued here.
                    // Don't add another <blockquote> tag.
                    ret = regex_replace(ret, blockquote, "<p>$1</p>");
                }
            }
            else if (in_blockquote) {
                // No blockquote found, but we had one open.
                // Close it.
                ret = "</blockquote>\n" + ret;
                in_blockquote = false;
                dont_add_paragraphs = true;
            }

            ret = regex_replace(ret, h6, "<h6>$1</h6>");
            ret = regex_replace(ret, h5, "<h5>$1</h5>");
            ret = regex_replace(ret, h4, "<h4>$1</h4>");
            ret = regex_replace(ret, h3, "<h3>$1</h3>");
            ret = regex_replace(ret, h2, "<h2>$1</h2>");
            ret = regex_replace(ret, h1, "<h1>$1</h1>");

            ret = regex_replace(ret, hr, "<hr />");

            // Find lists:
            if (regex_match(ret, ul)) {
                if (!in_ul) {
                    in_ul = true;
                    ret = regex_replace(ret, ul, "<ul><li>$1</li>");
                }
                else {
                    // An already open unordered list is continued here.
                    // Don't add another <ul> tag.
                    ret = regex_replace(ret, ul, "<li>$1</li>");
                }
            }
            else if (regex_match(ret, ol)) {
                if (!in_ol) {
                    in_ol = true;
                    ret = regex_replace(ret, ol, "<ol><li>$1</li>");
                }
                else {
                    // An already open ordered list is continued here.
                    // Don't add another <ol> tag.
                    ret = regex_replace(ret, ol, "<li>$1</li>");
                }
            }
            else if (in_ul || in_ol) {
                // No new list item found, but we had a list open.
                // Close it.
                ss_temp.str("");
                ss_temp << (in_ul ? "</ul>" : "</ol>") << "\n" << ret;
                ret = ss_temp.str();
                in_ul = in_ol = false;
                dont_add_paragraphs = true;
            }

            // Do the lists first, then the other asterisks. * *italic text* could be disturbing.
            ret = regex_replace(ret, bold_and_italic, "<strong><em>$1</em></strong>");
            ret = regex_replace(ret, bold, "<strong>$1</strong>");
            ret = regex_replace(ret, italic, "<em>$1</em>");

            ret = regex_replace(ret, pre, "<tt>$1</tt>");

            ret = regex_replace(ret, url, "<a href=\"$2\">$1</a>");
            ret = regex_replace(ret, img, "<img title=\"$1\" alt=\"$1\" src=\"$2\" />");

            // Find referenced URLs:
            smatch urlfinder;
            if (regex_search(ret, urlfinder, url_alt)) {
                // Do we know this URL from our prepare() call?
                if (legend_urls.count(stoi(urlfinder[2])) > 0) {
                    // Yep.
                    stringstream new_url;
                    new_url << "<a href=\"" << legend_urls.at(stoi(urlfinder[2])) << "\">" << urlfinder[1] << "</a>";
                    ret = regex_replace(ret, url_alt, new_url.str());
                }
            }
        }
        catch (regex_error& e) {
            cout << "[Markdown] An error occurred while trying to match a regular expression: " << e.what() << endl;
            cout << "Please file a blogcpp bug so we can investigate and fix it for you." << endl;
            cout << " --> " << BUGTRACKER << endl << endl;
#ifdef WITH_DEBUGLOG
            ss_debuglog << "[EXCEPTION] Markdown: " << e.what();
            DebugLog::debuglog(ss_debuglog.str());
#endif
            exit(1);
        }
    }

    // The following condition is the only :-) non-portable part here.
    // It calls the MediaEmbedder if an URL is found.
    regex re_url("^https?://([\\dA-Za-z\\.-]+)\\.([A-Za-z\\.]{2,6})([\\?&%;#=/\\w \\.-]*)*/?$");
    if (use_embeds && regex_match(ret, re_url)) {
        // Try to find and parse embedded media in the current line.
        MediaEmbedder embedder;
        if (embedder.addEmbeds(ret)) {
            dont_add_paragraphs = true;
        }
    }

    regex mdash(" -- ");
    ret = regex_replace(ret, mdash, " &mdash; ");

    if (dont_add_paragraphs) {
        ss_ret << ret;
    }
    else {
        ss_ret << "<p>" << ret << "</p>";
    }

    ss_ret << "\n"; // A line should usually end in a real line break so the source code looks less nasty later.

    full_markdown_text = ss_ret.str();
}

void MarkdownParser::cleanup() {
    // Closes tags left open.
    ostringstream ss_out;
#ifdef WITH_DEBUGLOG
    DebugLog::debuglog("Cleaning tags left open.");
#endif
    ss_out << full_markdown_text;

    if (in_blockquote) {
        ss_out << "</blockquote>";
        in_blockquote = false;
    }

    if (in_ol) {
        ss_out << "</ol>";
        in_ol = false;
    }

    if (in_ul) {
        ss_out << "</ul>";
        in_ul = false;
    }

    if (in_code) {
        ss_out << "</code>";
        in_code = false;
    }

    ss_out << "\n";
    full_markdown_text = ss_out.str();
}
