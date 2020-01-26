/*
 * blogcpp :: https://www.blogcpp.org
 * Definition of single blog entries.
 */


#ifndef HAS_SINGLE_ITEM
#define HAS_SINGLE_ITEM

#include <string>
#include <vector>

#include "MarkdownReturnClass.h"

class SingleItem {
    // The SingleItem class holds the meta data of a post or page
    // so it can be used to generate archive pages and/or lists.
  public:
    // -------------------------
    // Meta data (from input)
    // -------------------------
    tm time;                 // The creation date/time of the item.
    tm changetime;           // The date/time of the latest change of the item.
    std::string s_title;     // The contents of the title of the item.
    std::string s_slug;      // The (title/URL) slug of the item.
    std::string s_author;    // The author of the item.
    std::string s_text;      // The actual contents of the item.
    std::string s_markdown;  // Could be "Off" if markdownify() is not wanted.
    std::string s_emoji;     // Could be "Off" if this item must not have emojis.
    std::string s_sticky;    // Could be "On" if the article should be sticky.
    std::string s_comments;  // Could be "Off" if this item must not have comments.
    std::string s_ogimage;   // An optional OpenGraph image URL.
    std::string s_series;    // An optional unique name for the series (if !b_isPage).
    int i_position;          // An optional position (1..infinity) if b_isPage. Defaults to 0.

    std::vector<std::string> cats;   // Contains the categories of the item.
    std::vector<std::string> tags;   // Contains the tags of the item.

    // -------------------------
    // Meta data (internal)
    // -------------------------
    bool b_isPost;                 // Is this a post?
    bool b_isPage;                 // Or is this even a page?

    MarkdownReturn mr_text;        // Can contain the markdownified text (if needed).
    std::string s_title_emojified; // We'll save this for later so we save some time.
    std::string s_filename;        // Store the input file name.

    SingleItem() {
        time = {};
        changetime = {};
        s_title = "";
        s_slug = "";
        s_author = "";
        s_text = "";
        s_markdown = "";
        s_emoji = "";
        s_sticky = "";
        s_comments = "";
        s_ogimage = "";
        s_series = "";
        i_position = 0;

        cats = tags = {};

        b_isPost = false;
        b_isPage = false;

        s_title_emojified = "";
        s_filename = "";
    }
};

#endif
