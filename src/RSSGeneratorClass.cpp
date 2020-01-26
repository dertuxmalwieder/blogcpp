/*
 * blogcpp :: https://www.blogcpp.org
 * RSS class.
 */

#include "RSSGeneratorClass.h"

RSSGenerator::RSSGenerator(std::string feeddir, std::string title, std::string outfile, ConfigCollection cfgs) {
    this->feeddir = feeddir;
    this->title = title;
    this->outfile = outfile;
    own_cfgs.cfg_maxitems = cfgs.cfg_maxitems;
    own_cfgs.cfg_fullfeed = cfgs.cfg_fullfeed;
    own_cfgs.cfg_embeds = cfgs.cfg_embeds;
    own_cfgs.cfg_excerpts = cfgs.cfg_excerpts;
    own_cfgs.cfg_permalinks = cfgs.cfg_permalinks;
    own_cfgs.cfg_subtitle = cfgs.cfg_subtitle;
    own_cfgs.cfg_siteurl = cfgs.cfg_siteurl;
    own_cfgs.cfg_article_subdir = cfgs.cfg_article_subdir;
}

void RSSGenerator::createRSS(std::vector<SingleItem> vec) {
    // Creates the site's RSS.xml file.
    // Requires vec to be reverse-sorted (which is probably the case here).
#ifdef WITH_DEBUGLOG
    std::ostringstream ss_debuglog;
    DebugLog::debuglog("Creating the RSS feed.");
#endif

    if (!fs::exists(feeddir)) {
        // Create the RSS directory.
#ifdef WITH_DEBUGLOG
        ss_debuglog << "Directory " << feeddir << " is missing. Creating it.";
        DebugLog::debuglog(ss_debuglog.str());
#endif
        fs::create_directories(feeddir);
    }

    tinyxml2::XMLDocument rss;
    std::stringstream ss_item_time, ss_text, ss_url, ss_outfile;
    int counter = 0;

    tinyxml2::XMLDeclaration* pDeclaration = rss.NewDeclaration();
    pDeclaration->SetValue("xml version=\"1.0\" encoding=\"utf-8\"");
    rss.InsertFirstChild(pDeclaration);

    tinyxml2::XMLElement* pRoot = rss.NewElement("rss");
    pRoot->SetAttribute("version", "2.0");
    rss.InsertEndChild(pRoot);

    tinyxml2::XMLNode* pChannel = rss.NewElement("channel");
    pRoot->InsertFirstChild(pChannel);

    tinyxml2::XMLElement* pTitle = rss.NewElement("title");
    pTitle->SetText(title.c_str());
    pChannel->InsertFirstChild(pTitle);

    tinyxml2::XMLElement* pLink = rss.NewElement("link");
    pLink->SetText(own_cfgs.cfg_siteurl.c_str());
    pChannel->InsertEndChild(pLink);

    if (!own_cfgs.cfg_subtitle.empty()) {
        tinyxml2::XMLElement* pDescription = rss.NewElement("description");
        pDescription->SetText(own_cfgs.cfg_subtitle.c_str());
        pChannel->InsertEndChild(pDescription);
    }

    tinyxml2::XMLElement* pUpdated = rss.NewElement("lastBuildDate");
    pUpdated->SetText(timeNow().c_str());
    pChannel->InsertEndChild(pUpdated);

    // Traverse through the latest <maxitems> posts and add them to the feed..
    std::vector<SingleItem> v_newest_x {vec.begin(), next(vec.begin(), min(vec.size(), static_cast<size_t>(own_cfgs.cfg_maxitems)))};
    for (auto p : v_newest_x) {
        if (p.b_isPage) {
            // It makes no sense to have pages in your RSS feed.
            continue;
        }

        tinyxml2::XMLNode* pItem = rss.NewElement("item");
        pChannel->InsertEndChild(pItem);

        tinyxml2::XMLElement* pItemTitle = rss.NewElement("title");
        pItemTitle->SetText(p.s_title.c_str());
        pItem->InsertFirstChild(pItemTitle);

        tinyxml2::XMLElement* pItemDescription = rss.NewElement("description");
        bool dont_use_markdown = lowercase(p.s_markdown) == "off";
        bool dont_use_emoji = lowercase(p.s_emoji) == "off";

        // We probably have processed the Markdown text by this point unless the
        // user has chosen "full feed" but "shortened articles" (or vice versa).
        // In the best case, we don't want to repeat ourselves.
        ss_text.str("");
        if (own_cfgs.cfg_fullfeed == (own_cfgs.cfg_excerpts == 0)) {
            // We're good.
            ss_text << p.mr_text.parsed_text;
        }
        else {
            // Nah. Extract the first <cfg_excerpts> sentences again.
            ExcerptExtractor excerpt(p.s_text, own_cfgs.cfg_excerpts);
            ss_text << MarkdownWrapper::markdownify(excerpt.extractExcerpt(), dont_use_markdown, dont_use_emoji, own_cfgs.cfg_embeds).parsed_text;
        }

        tinyxml2::XMLText* pItemDescText = rss.NewText(ss_text.str().c_str());
        pItemDescText->SetCData(true);

        pItemDescription->InsertEndChild(pItemDescText);
        pItem->InsertEndChild(pItemDescription);

        tinyxml2::XMLElement* pItemLink = rss.NewElement("link");
        string outputdate = parseTmToPath(p.time, own_cfgs.cfg_permalinks);
        ss_url.str("");
        ss_url << own_cfgs.cfg_siteurl << "/";
        if (own_cfgs.cfg_article_subdir != "/") {
            ss_url << own_cfgs.cfg_article_subdir << "/";
        }
        ss_url << outputdate << "/" << p.s_slug;
        pItemLink->SetText(ss_url.str().c_str());
        pItem->InsertEndChild(pItemLink);

        tinyxml2::XMLElement* pItemPubDate = rss.NewElement("pubDate");
        ss_item_time.str("");
        ss_item_time << put_time(&p.time, "%a, %d %b %Y %T %z");

        pItemPubDate->SetText(ss_item_time.str().c_str());
        pItem->InsertEndChild(pItemPubDate);

        counter++;
    }

    if (counter > 0) {
        // Don't write empty feeds.
        ss_outfile << feeddir << outfile;
#ifdef WITH_DEBUGLOG
        ss_debuglog.str("");
        ss_debuglog << "Writing the RSS file into " << ss_outfile.str();
        DebugLog::debuglog(ss_debuglog.str());
#endif
        rss.SaveFile(ss_outfile.str().c_str());
    }
    else {
#ifdef WITH_DEBUGLOG
        DebugLog::debuglog("There are no items for this feed. Skipping.");
#endif
    }
}
