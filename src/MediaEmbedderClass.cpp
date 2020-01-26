/*
 * blogcpp :: https://www.blogcpp.org
 * Class to add embedded media to the output stream.
 */

#include "MediaEmbedderClass.h"

#ifdef WITH_OEMBED
using json = nlohmann::json;
#endif

MediaEmbedder::MediaEmbedder() {
#ifdef WITH_DEBUGLOG
    DebugLog::debuglog("Setting up a media embedder ...");
# ifdef WITH_OEMBED
    DebugLog::debuglog("Using OEmbed.");
# endif
#endif
}

bool MediaEmbedder::addEmbeds(std::string& inputline) {
    // If WITH_OEMBED is not set:
    // If the parser finds a YouTube or Vimeo link on a separate line and the user did not disable it in the
    // configuration, it adds media links automatically.
    //
    // If WITH_OEMBED is set:
    // The parser tries to automatically add media links from a number of oEmbed sources.
    //
    // This functionality is not actually a part of Markdown, but it might be useful.

#ifdef WITH_OEMBED
    return addOEmbeds(inputline);
#else
    bool ret = false;
    std::string parsedline;
    std::stringstream ss_parsedline;
#ifdef WITH_DEBUGLOG
    std::stringstream ss_debuglog;
    DebugLog::debuglog("Using standard embeds.");
#endif

    std::regex youtube("^https?://(www\\.)?youtube\\.com/watch\\?v=([A-Za-z0-9-]+)&?$");
    std::regex vimeo("^https?://(www\\.)?vimeo\\.com/(\\d+)$");

    std::smatch match;

    parsedline = inputline;

    if (std::regex_match(inputline, match, youtube)) {
#ifdef WITH_DEBUGLOG
        ss_debuglog << "Found a YouTube link: " << match.str(0);
#endif
        ss_parsedline << "<iframe frameborder=\"0\" allowfullscreen width=\"640\" height=\"360\" src=\"https://www.youtube-nocookie.com/embed/" << match.str(2) << "?autoplay=0\"></iframe>";
        parsedline = ss_parsedline.str();
        ret = true;
    }
    else if (std::regex_match(inputline, match, vimeo)) {
#ifdef WITH_DEBUGLOG
        ss_debuglog << "Found a Vimeo link: " << match.str(0);
#endif
        ss_parsedline << "<iframe src=\"https://player.vimeo.com/video/" << match.str(2) << "?title=0&byline=0&portrait=0&badge=0\" width=\"640\" height=\"360\" frameborder=\"0\" allowfullscreen></iframe>";
        parsedline = ss_parsedline.str();
        ret = true;
    }

    inputline = parsedline;
    return ret;
#endif
}

#ifdef WITH_OEMBED
bool MediaEmbedder::addOEmbeds(std::string& inputline) {
    // Tries to find known OEmbeds in <inputline> and add links from their JSON or XML data.
    bool ret = false;
    std::string raw_html;
    std::string s_weblink;
    std::string s_weblink_contents;
    std::string s_oembed_tag;
    std::string s_oembed_link;
    std::string parsedline;
    HTTPDownloader downloader;
    std::stringstream ss_link_creator;
#ifdef WITH_DEBUGLOG
    std::stringstream ss_debuglog;
#endif

    parsedline = inputline;

    // Try to find an OEmbed in the header of any linked website.
    const std::string url_regex = "https?://[^\\s\"<>]+";

    std::regex re_url("^" + url_regex + "/?$");
    std::regex re_url_inline(url_regex);
    std::regex re_oembed_json("<link.*?type=\"application/json\\+oembed\".*?>");
    std::regex re_oembed_xml("<link.*?type=\"text/xml\\+oembed\".*?>");

    std::smatch match_url;
    std::smatch match_oembed;
    bool bIsXML = false;

    if (!std::regex_match(inputline, match_url, re_url)) {
        // This is not a website.
        return ret;
    }

    s_weblink = match_url.str(0);
#ifdef WITH_DEBUGLOG
    ss_debuglog << "Found a website link: " << s_weblink << " - looking for OEmbeds...";
    DebugLog::debuglog(ss_debuglog.str());
    ss_debuglog.str("");
#endif

    s_weblink_contents = downloader.download(s_weblink);
    if (s_weblink_contents.empty()) {
#ifdef WITH_DEBUGLOG
        DebugLog::debuglog("The website could not be reached.");
#endif
        return ret;
    }

    if (!std::regex_search(s_weblink_contents, match_oembed, re_oembed_json)) {
        // No JSON OEmbed found.
        if (!std::regex_search(s_weblink_contents, match_oembed, re_oembed_xml)) {
            // No XML OEmbed found.
#ifdef WITH_DEBUGLOG
            DebugLog::debuglog("This website doesn't have any OEmbeds.");
#endif
            return ret;
        }
        else {
#ifdef WITH_DEBUGLOG
            DebugLog::debuglog("This website has XML OEmbeds.");
#endif
            bIsXML = true;
        }
    }
#ifdef WITH_DEBUGLOG
    else {
        DebugLog::debuglog("This website has JSON OEmbeds.");
    }
#endif

    s_oembed_tag = match_oembed.str(0);

    // Find the OEmbed URL inside the <link> tag.
    if (!std::regex_search(s_oembed_tag, match_url, re_url_inline)) {
        // No OEmbed link found.
#ifdef WITH_DEBUGLOG
        DebugLog::debuglog("This website doesn't have any valid OEmbeds.");
#endif
        return ret;
    }

    // match_url.str(0) is the given OEmbed link now.
    // However, it might contain &amp; (e.g. on YouTube) instead of &.
    // Undo it.
    std::regex amp("&amp;");
    s_oembed_link = std::regex_replace(match_url.str(0), amp, "&");

#ifdef WITH_DEBUGLOG
    ss_debuglog << "Downloading " << s_oembed_link;
    DebugLog::debuglog(ss_debuglog.str());
    ss_debuglog.str("");
#endif
    raw_html = downloader.download(s_oembed_link);
    if (raw_html.empty()) {
#ifdef WITH_DEBUGLOG
        DebugLog::debuglog("The OEmbed could not be reached.");
#endif
        return ret;
    }

    try {
        if (bIsXML) {
            // XML OEmbed:
            tinyxml2::XMLDocument oembed;
            oembed.Parse(raw_html.c_str());
            if (oembed.Error()) {
                throw oembed.ErrorStr();
            }
            tinyxml2::XMLElement* parentElem = oembed.FirstChildElement("oembed");
            if (!oembed.Error()) {
                tinyxml2::XMLElement* htmlElement = parentElem->FirstChildElement("html");
                if (oembed.Error()) {
                    throw std::exception("No <html> element found.");
                }
                else {
                    parsedline = htmlElement->GetText();
                }
            }
            else {
                throw std::exception("No <oembed> element found.");
            }

#ifdef WITH_DEBUGLOG
            DebugLog::debuglog("XML->HTML replacement succeeded.");
#endif
            ret = true;
        }
        else {
            // JSON OEmbed:
            auto jsons = json::parse(raw_html);

            // No exception thrown yet? Great!
            // Write the suggested replacement back to our input string.
            parsedline = jsons["html"].get<std::string>();

#ifdef WITH_DEBUGLOG
            DebugLog::debuglog("JSON->HTML replacement succeeded.");
#endif
            ret = true;
        }
    }
    catch (const char* s) {
        // We threw a string, so we probably have a better explanation here.
#ifdef WITH_DEBUGLOG
        DebugLog::debuglog("Failed to parse the OEmbed string correctly.");
        DebugLog::debuglog("Erroneous code:");
        DebugLog::debuglog(s);
#endif
        return ret;
    }
    catch (std::exception& e) {
#ifdef WITH_DEBUGLOG
        DebugLog::debuglog("Failed to parse the OEmbed string correctly.");
        DebugLog::debuglog(e.what());
#endif
        return ret;
    }

    inputline = parsedline;
    return ret;
}
#endif
