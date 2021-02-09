/*
 * blogcpp :: https://www.blogcpp.org
 * Class for the integration of comments.
 */

#include "CommentIntegrationClass.h"

CommentIntegration::CommentIntegration(std::string commenttype, std::string commentdata) {
    // Set up our values.
    this->commenttype = commenttype;
    this->commentdata = commentdata;
}

std::string CommentIntegration::addHeader() {
    // Returns a string that should be appended to the <HEAD> area where applicable.
    std::stringstream ret;

#ifdef WITH_DEBUGLOG
    DebugLog::debuglog("Adding comments (header) ...");
#endif

    if (commenttype == "google") {
        // Surprisingly, Google does not require additional settings.
        ret << "<script src=\"https://apis.google.com/js/plusone.js\"></script>" << std::endl;
    }

    return ret.str();
}

std::string CommentIntegration::addComments() {
    // The magic happens here. :-)
    std::stringstream ret;

#ifdef WITH_DEBUGLOG
    DebugLog::debuglog("Adding comments (body) ...");
#endif

    // We support the following comment types:
    //  * Disqus comments: commentdata is the shortname.
    //  * isso comments: commentdata is the URL to your isso installation.
    //  * Hashover 2.x comments: commentdata is the URL to your Hashover installation.
    //  * Google comments: No setup needed.
    //  * Discourse: commentdata is the URL to your Discourse installation.
    //  * Commento comments: commentdata is the URL to your Commento installation.
    //  * Hyvor Talk comments: commentdata is the website ID.

    if (commenttype == "disqus") {
        // No commentdata? No comments.
        if (commentdata == "") {
#ifdef WITH_DEBUGLOG
            DebugLog::debuglog("Invalid Disqus settings: Please check your Disqus ID in commentdata!");
#endif
            return ret.str();
        }

        ret << "<div id=\"disqus_thread\" class=\"comments\"></div>" << std::endl;
        ret << "<script type=\"text/javascript\">" << std::endl;
        ret << "  (function() {" << std::endl;
        ret << "    var dsq = document.createElement('script');" << std::endl;
        ret << "    dsq.type = 'text/javascript';" << std::endl;
        ret << "    dsq.async = true;" << std::endl;
        ret << "    dsq.src = '//" << commentdata << ".disqus.com/embed.js';" << std::endl;
        ret << "    (document.getElementsByTagName('head')[0] || document.getElementsByTagName('body')[0]).appendChild(dsq);" << std::endl;
        ret << "  })();" << std::endl;
        ret << "</script>" << std::endl;
        ret << "<noscript>Please enable JavaScript to view the <a href=\"//disqus.com/?ref_noscript\">comments powered by Disqus.</a></noscript>" << std::endl;
        ret << "<a href=\"//disqus.com\" class=\"dsq-brlink\">comments powered by <span class=\"logo-disqus\">Disqus</span></a>";
    }
    else if (commenttype == "isso") {
        // No commentdata? No comments.
        if (commentdata == "") {
#ifdef WITH_DEBUGLOG
            DebugLog::debuglog("Invalid isso settings: Please check your isso URL in commentdata!");
#endif
            return ret.str();
        }

        ret << "<div class=\"comments\">" << std::endl;
        ret << "<section id=\"isso-thread\"></section>" << std::endl;
        ret << "<script data-isso=\"" << commentdata << "/\"" << std::endl;
        ret << "        src=\"" << commentdata << "/js/embed.min.js\"></script>" << std::endl;
        ret << "</div>";
    }
    else if (commenttype == "hashover") {
        // No commentdata? No comments.
        if (commentdata == "") {
#ifdef WITH_DEBUGLOG
            DebugLog::debuglog("Invalid Hashover settings: Please check your Hashover URL in commentdata!");
#endif
            return ret.str();
        }

        ret << "<div id=\"hashover\" class=\"comments\"></div>" << std::endl;
        ret << "<script type=\"text/javascript\">" << std::endl;
        ret << "  (function() {" << std::endl;
        ret << "    var s = document.createElement('script')," << std::endl;
        ret << "        t = document.getElementsByTagName('link')[0];" << std::endl;
        ret << "    s.type = 'text/javascript';" << std::endl;
        ret << "    s.async = true;" << std::endl;
        ret << "    s.src = \"" << commentdata << "/hashover.js\";" << std::endl;
        ret << "    t.parentNode.insertBefore(s, t);" << std::endl;
        ret << "  })();" << std::endl;
        ret << "</script>" << std::endl;
        ret << "<noscript>Please enable JavaScript to view the comments.</noscript>";
    }
    else if (commenttype == "google") {
        // Surprisingly, Google does not require additional settings.
        ret << "<div id=\"comments\" class=\"comments\"></div>" << std::endl;
        ret << "<script>" << std::endl;
        ret << "gapi.comments.render('comments', {" << std::endl;
        ret << "    href: window.location," << std::endl;
        ret << "    width: '624'," << std::endl;
        ret << "    first_party_property: 'BLOGGER'," << std::endl;
        ret << "    view_type: 'FILTERED_POSTMOD'" << std::endl;
        ret << "});" << std::endl;
        ret << "</script>";
    }
    else if (commenttype == "discourse") {
        // No commentdata? No comments.
        if (commentdata == "") {
#ifdef WITH_DEBUGLOG
            DebugLog::debuglog("Invalid Discourse settings: Please check your Discourse URL in commentdata!");
#endif
            return ret.str();
        }

        ret << "<div id='discourse-comments' class='comments'></div>" << std::endl;
        ret << "<script type=\"text/javascript\">" << std::endl;
        ret << "  DiscourseEmbed = { discourseUrl: '" << commentdata << "/'," << std::endl;
        ret << "                     discourseEmbedUrl: window.location };" << std::endl;
        ret << "  (function() {" << std::endl;
        ret << "    var d = document.createElement('script'); d.type = 'text/javascript'; d.async = true;" << std::endl;
        ret << "    d.src = DiscourseEmbed.discourseUrl + 'javascripts/embed.js';" << std::endl;
        ret << "    (document.getElementsByTagName('head')[0] || document.getElementsByTagName('body')[0]).appendChild(d);" << std::endl;
        ret << "  })();" << std::endl;
        ret << "</script>" << std::endl;
        ret << "<noscript>Please enable JavaScript to view the comments.</noscript>";
    }
    else if (commenttype == "commento") {
        // No commentdata? No comments.
        if (commentdata == "") {
#ifdef WITH_DEBUGLOG
            DebugLog::debuglog("Invalid Commento settings: Please check your Commento URL in commentdata!");
#endif
            return ret.str();
        }

        ret << "<div class=\"commento\"></div>" << std::endl;
        ret << "<script defer src=\"" << commentdata << "/commento.min.js\" data-div=\"#commento\"></script>" << std::endl;
    }
    else if (commenttype == "hyvortalk") {
        // No commentdata? No comments.
        if (commentdata == "") {
#ifdef WITH_DEBUGLOG
            DebugLog::debuglog("Invalid Hyvor Talk settings: Please check your Hyvor Talk website ID in commentdata!");
#endif
            return ret.str();
        }

        ret << "<div id=\"hyvor-talk-view\"></div>" << std::endl;
        ret << "<script type=\"text/javascript\">" << std::endl;
        ret << "var HYVOR_TALK_WEBSITE = \"" << commentdata << "\";" << std::endl;
        ret << "var HYVOR_TALK_CONFIG = {" << std::endl;
        ret << "    url: false," << std::endl;
        ret << "    id: false" << std::endl;
        ret << "};" << std::endl;
        ret << "</script>" << std::endl;
        ret << "<script async type=\"text/javascript\" src=\"//talk.hyvor.com/web-api/embed\"></script>" << std::endl;
    }

#ifdef WITH_DEBUGLOG
    DebugLog::debuglog("Done.");
#endif

    return ret.str();
}
