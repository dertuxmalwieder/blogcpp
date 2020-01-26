/*
 * blogcpp :: https://www.blogcpp.org
 * Collection of configuration entries to pass around.
 */

#ifndef CONFIG_COLLECTION
#define CONFIG_COLLECTION // Once is enough.

#include <string>

class ConfigCollection {
  public:
    std::string cfg_sitetitle;
    std::string cfg_subtitle;
    std::string cfg_siteurl;
    std::string cfg_author;
    std::string cfg_indir;
    std::string cfg_outdir;
#ifdef WITH_PLUGINS
    bool        cfg_plugins;
    std::string cfg_plugindir;
#endif
    std::string cfg_permalinks;
    std::string cfg_article_subdir;
    std::string cfg_page_subdir;
    std::string cfg_locale;
    bool        cfg_series;
    int         cfg_maxitems;
    int         cfg_maxhistory;
    std::string cfg_template;
    bool        cfg_embeds;
    int         cfg_excerpts;
    std::string cfg_excerpttext;
    bool        cfg_emojis;
    bool        cfg_fullfeed;
    std::string cfg_feeddir;
    bool        cfg_comments;
    std::string cfg_commenttype;
    std::string cfg_commentdata;
    std::string cfg_opengraphimg;
};

#endif
