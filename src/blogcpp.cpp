/*
 * blogcpp :: https://www.blogcpp.org
 * Main non-class (still much too long).
 */

#include "blogcpp.h"


void printVersion() {
    // Prints version information.
    cout << endl;
    cout << "This is blogcpp version " << APP_VERSION << "." << endl;
    cout << APP_URL << endl << endl;
    cout << "Licensed under the terms of the WTFPL v2." << endl;
    cout << "See http://wtfpl.net/txt/copying for relevant information." << endl << endl;
}


void collectPostData(string inputfile, bool is_page) {
    // Fills vsi_posts, vsi_pages, vs_cats and vs_tags before processing them entirely.
    stringstream inputfile_path, ss_contents, ss_output;
    string s_cats, s_tags, s_datetime, s_change_datetime, s_author, s_title, s_slug, s_markdown, s_emoji, s_sticky, s_comments, s_ogimage, s_series;
    int i_position = 0;
#ifdef WITH_PLUGINS
    string s_plugins;
#endif
    tm tm_t = {}, change_tm_t = {};
    SingleItem si;
#ifdef WITH_DEBUGLOG
    stringstream ss_debuglog;
    ss_debuglog << "Gathering categories and tags from the " << (is_page ? "page" : "post") << " from '" << inputfile << "'";
    DebugLog::debuglog(ss_debuglog.str());
#endif

    if (is_page) {
        inputfile_path << ss_indir_pages.str() << inputfile;
    }
    else {
        inputfile_path << ss_indir_posts.str() << inputfile;
    }

    ifstream infile(inputfile_path.str());
    string file_contents = [&infile] {
        ostringstream ss{};
        ss << infile.rdbuf();
        return ss.str();
    }();

#ifdef WITH_DEBUGLOG
    ss_debuglog.str("");
    ss_debuglog << "File contents for '" << inputfile << "' are set.";
    DebugLog::debuglog(ss_debuglog.str());
#endif
    // file_contents contains the input text + meta data now.
    // Let's first read the meta data (until HEADER_DIVIDER):
    string line;
    ss_contents << file_contents;
    while (getline(ss_contents, line)) {
        if (line == HEADER_DIVIDER) {
            // The end of meta data.
            break;
        }

        regex re_author("^Author\\s*:[\\s\\t]*(.*?)$", regex_constants::icase);
        regex re_title("^Title\\s*:[\\s\\t]*(.*?)$", regex_constants::icase);
        regex re_slug("^Slug\\s*:[\\s\\t]*(.*?)$", regex_constants::icase);
        regex re_datetime("^Date\\s*:[\\s\\t]*(\\d{4}\\-\\d{2}\\-\\d{2} \\d{2}:\\d{2}:\\d{2}?)$", regex_constants::icase);
        regex re_change_datetime("^Changed\\s*:[\\s\\t]*(\\d{4}\\-\\d{2}\\-\\d{2} \\d{2}:\\d{2}:\\d{2}?)$", regex_constants::icase);
        regex re_cats("^Categories\\s*:[\\s\\t]*(.*?)$", regex_constants::icase);
        regex re_tags("^Tags\\s*:[\\s\\t]*(.*?)$", regex_constants::icase);
        regex re_markdown("^Markdown\\s*:[\\s\\t]*(.*?)$", regex_constants::icase);
        regex re_emoji("^Emoji\\s*:[\\s\\t]*(.*?)$", regex_constants::icase);
        regex re_sticky("^Sticky\\s*:[\\s\\t]*(.*?)$", regex_constants::icase);
        regex re_comments("^Comments\\s*:[\\s\\t]*(.*?)$", regex_constants::icase);
        regex re_ogimage("^OpenGraphImage\\s*:[\\s\\t]*(.*?)$", regex_constants::icase);
#ifdef WITH_PLUGINS
        regex re_plugins("^Plugins\\s*:[\\s\\t]*(.*?)$", regex_constants::icase);
#endif
        regex re_series("^Series\\s*:[\\s\\t]*(.*?)$", regex_constants::icase);
        regex re_position("^Position\\s*:[\\s\\t]*(\\d*?)$", regex_constants::icase);

        regex re_url("https?://", regex_constants::icase); // Avoid stupidities

        smatch match;

        try {
            if (regex_match(line, match, re_author) && match.size() > 1)        { s_author = match.str(1); }
            else if (regex_match(line, match, re_title) && match.size() > 1)    { s_title = match.str(1); }
            else if (regex_match(line, match, re_slug) && match.size() > 1)     { s_slug = match.str(1); }
            else if (regex_match(line, match, re_datetime) && match.size() > 1) {
                // Get the tm from the date/time in the file.
                parseDatestringToTm(match.str(1), inputfile, tm_t);
            }
            else if (regex_match(line, match, re_change_datetime) && match.size() > 1) {
                // Get the tm from the "latest change" date/time in the file.
                parseDatestringToTm(match.str(1), inputfile, change_tm_t);
            }
            else if (!is_page && regex_match(line, match, re_cats) && match.size() > 1) {
                // It makes no sense to categorize pages.
                s_cats = match.str(1);
            }
            else if (regex_match(line, match, re_tags) && match.size() > 1)     { s_tags = match.str(1); }
            else if (regex_match(line, match, re_markdown) && match.size() > 1) { s_markdown = match.str(1); }
            else if (regex_match(line, match, re_emoji) && match.size() > 1)    { s_emoji = match.str(1); }
            else if (regex_match(line, match, re_sticky) && match.size() > 1)   { s_sticky = match.str(1); }
            else if (!is_page && regex_match(line, match, re_comments) && match.size() > 1) {
                s_comments = match.str(1);
            }
            else if (regex_match(line, match, re_ogimage) && regex_match(line, match, re_url) && match.size() > 1) {
                // Add only valid URLs
                s_ogimage = match.str(1);
            }
#ifdef WITH_PLUGINS
            else if (regex_match(line, match, re_plugins) && match.size() > 1)  { s_plugins = match.str(1); }
#endif
            else if (regex_match(line, match, re_series) && match.size() > 1)   { s_series = match.str(1); }
            else if (regex_match(line, match, re_position) && match.size() > 1) { i_position = stoi(match.str(1)); }
        }
        catch (regex_error& e) {
            // Syntax error in the regular expression.
            cout << "An error occurred while trying to match a regular expression: " << e.what() << endl;
            cout << "Please file a blogcpp bug so we can investigate and fix it for you." << endl;
            cout << " --> " << BUGTRACKER << endl << endl;
            exit(1);
        }
        catch (invalid_argument& e) {
            // Invalid position argument in stoi(). Skip.
#ifdef WITH_DEBUGLOG
            ss_debuglog.str("");
            ss_debuglog << "Failed to set the position in article '" << s_title << "' (" << inputfile << "). ";
            ss_debuglog << "You might want to check the parameters...?";
            DebugLog::debuglog(ss_debuglog.str());
#endif
            continue;
        }
    }

    // Add this author to the list of authors if not done yet.
    if (find(vs_authors.begin(), vs_authors.end(), s_author) == vs_authors.end()) {
        lock_guard<mutex> lock(mtx);
        vs_authors.push_back(s_author);
    }

    // Add this series to the list of series if not done yet.
    if (cfgs.cfg_series && !s_series.empty() && find(vs_series.begin(), vs_series.end(), s_series) == vs_series.end()) {
        lock_guard<mutex> lock(mtx);
        vs_series.push_back(s_series);
    }

    // Find the content and put it into the template.
    bool input_has_started = false;
    while (getline(ss_contents, line)) {
        // The stringstream already points below the HEADER_DIVIDER.
        // We can safely continue from here.

        // If the line below the divider is empty, we don't really want to keep it.
        if (!line.empty()) { input_has_started = true; }
        if (!input_has_started) { continue; }

        // Everything that follows now is a part of what we need.
        ss_output << line << "\n";
    }

    // Create the slug (if not specified or contains spaces).
    if (s_slug.empty() || s_slug.find(" ") != string::npos) {
        s_slug = createSlug(s_title.c_str(), tm_t);
    }

    // Positions have to be >= 0.
    if (i_position < 0 && is_page) {
#ifdef WITH_DEBUGLOG
        DebugLog::debuglog("Fixing invalid page position.");
#endif
        i_position = 0;
    }

    vector<string> v_local_cats = vectorSplit(s_cats);
    vector<string> v_local_tags = vectorSplit(s_tags);

    si.s_filename = inputfile;

    si.time = tm_t;
    si.changetime = change_tm_t;
    si.s_title = s_title;
    si.s_slug = s_slug;
    si.s_author = s_author;
#ifdef WITH_PLUGINS
    // Process plug-ins:
    si.s_text = (lowercase(s_plugins) == "off" ? ss_output.str() : plugins.processContentPlugins(ss_output.str()));
#else
    si.s_text = ss_output.str();
#endif
    si.s_markdown = s_markdown;
    si.s_emoji = s_emoji;
    si.s_sticky = s_sticky;
    si.s_comments = s_comments;
    si.s_ogimage = s_ogimage;
    if (cfgs.cfg_series && !is_page) { si.s_series = s_series; }
    else if (is_page) { si.i_position = i_position; }

    bool dont_use_markdown = lowercase(s_markdown) == "off";
    bool dont_use_emoji = !cfgs.cfg_emojis && lowercase(s_emoji) == "off";

    si.mr_text = MarkdownWrapper::markdownify(si.s_text, dont_use_markdown, dont_use_emoji, cfgs.cfg_embeds);

    if (!dont_use_emoji) {
        EmojiParser eparser;
        si.s_title_emojified = eparser.parse(s_title);
    }

    si.cats = v_local_cats;
    si.tags = v_local_tags;

    si.b_isPost = !is_page;
    si.b_isPage = is_page;

    if (!is_page) {
        lock_guard<mutex> lock(mtx);
        vsi_posts.push_back(si);
    }
    else {
        lock_guard<mutex> lock(mtx);
        vsi_pages.push_back(si);
    }

    if (!si.s_series.empty()) {
        // Save this association for later:
        map_series_items.insert(pair<string, SingleItem>(s_series, si));
    }

    if (v_local_cats.size() > 0) {
        // Any new category should be a part of the global categories vector.
#ifdef WITH_DEBUGLOG
        ss_debuglog.str("");
        ss_debuglog << "We have " << v_local_cats.size() << " categories. Let's traverse them.";
        DebugLog::debuglog(ss_debuglog.str());
#endif
        for (auto c : v_local_cats) {
#ifdef WITH_DEBUGLOG
            ss_debuglog.str("");
            ss_debuglog << "Processing category " << c;
            DebugLog::debuglog(ss_debuglog.str());
#endif
            // Add the category to our categories vector for later summarization:
            vs_cats.push_back(c);
        }
    }

    if (v_local_tags.size() > 0) {
        // Any new tag should be a part of the global tags vector.
#ifdef WITH_DEBUGLOG
        ss_debuglog.str("");
        ss_debuglog << "We have " << v_local_tags.size() << " tags. Let's traverse them.";
        DebugLog::debuglog(ss_debuglog.str());
#endif

        for (auto t : v_local_tags) {
#ifdef WITH_DEBUGLOG
            ss_debuglog.str("");
            ss_debuglog << "Processing tag " << t;
            DebugLog::debuglog(ss_debuglog.str());
#endif
            // Add the category to our tags vector for later summarization:
            vs_tags.push_back(t);
        }
    }
}

void collectArchiveData() {
    // Fills vs_archive_months, vs_archive_years, vec_months and vec_years.
    stringstream ss_temp_month;
    stringstream ss_temp_year;

    for (const SingleItem& p : vsi_posts) {
        // Traverse through our filled and sorted (!) vsi_posts vector.
        // p.time is a tm field:
        //   p.time.tm_year = years since 1900
        //   p.time.tm_mon = numeric month minus 1
        // We can work with that.

        // Months should have two digits, of course.
        ss_temp_year.str("");
        ss_temp_month.str("");

        ss_temp_year << p.time.tm_year + 1900;
        ss_temp_month << ss_temp_year.str() << "/" << setfill('0') << setw(2) << p.time.tm_mon + 1;

        if (find(vs_archive_years.begin(), vs_archive_years.end(), ss_temp_year.str()) == vs_archive_years.end()) {
            // This year has not been processed yet.
            vector<SingleItem> vsi_curr_year;  // Yearly archives

            for (const SingleItem& p2 : vsi_posts) {
                if (p2.time.tm_year == p.time.tm_year) {
                    // This post is from the same year.
                    vsi_curr_year.push_back(p2);
                }
            }

            vs_archive_years.push_back(ss_temp_year.str());
            vec_years.push_back(vsi_curr_year);
        }

        if (find(vs_archive_months.begin(), vs_archive_months.end(), ss_temp_month.str()) == vs_archive_months.end()) {
            // This month has not been processed yet.
            vector<SingleItem> vsi_curr_month; // Monthly archives

            for (const SingleItem& p2 : vsi_posts) {
                if (p2.time.tm_year == p.time.tm_year && p2.time.tm_mon == p.time.tm_mon) {
                    // This post is from the same month.
                    vsi_curr_month.push_back(p2);
                }
            }

            vs_archive_months.push_back(ss_temp_month.str());
            vec_months.push_back(vsi_curr_month);
        }
    }
}

json renderHeader(ConfigParser config, string extratitle) {
    // Renders a number of common variables into a JSON object, it will be used later.
    stringstream temp, ss_link_url, ss_link_title, ss_link_text, ss_rss;
    vector<int> links;
    int pagecount = 0;

    json ret;

    // Site title
    ret["sitetitle"] = cfgs.cfg_sitetitle;
    ret["subtitle"] = cfgs.cfg_subtitle;

    temp << cfgs.cfg_sitetitle;
    if (!cfgs.cfg_subtitle.empty()) {
        temp << " - " << cfgs.cfg_subtitle;
    }

    if (!extratitle.empty()) {
        // Append something to the page title.
        // Maybe a post title? :-)
        temp << " &raquo; " << extratitle;
    }

    ret["pagetitle"] = temp.str();

    // Highlight.js version
    ret["hljsver"] = HIGHLIGHT_VER;

    // Site URL
    ret["baseurl"] = cfgs.cfg_siteurl;

    // Add the RSS link.
    ss_rss << cfgs.cfg_siteurl << "/" << cfgs.cfg_feeddir << "/RSS.xml";
    ret["rsslink"] = ss_rss.str();

#ifdef WITH_PLUGINS
    // Header and footer plug-ins (if applicable).
    ret["headscripts"] = plugins.processHeaderPlugins();
    ret["footscripts"] = plugins.processFooterPlugins();
#endif

    // Links
    for (unsigned int i = 1; i <= 99; i++) {
        // First, number the links.
        ss_link_url.str("");
        ss_link_url << "link" << i << "_url";

        string hasKey = "";
        config.get(ss_link_url.str(), hasKey);

        if (hasKey != "") {
            // If link<i>_url exists, note that.
            // blogcpp allows skipping numbers here.
            lock_guard<mutex> lock(mtx);
            links.push_back(i);
        }
    }

    for (unsigned int i = 0; i < links.size(); i++) {
        // Now traverse them.
        ss_link_url.str("");
        ss_link_title.str("");
        ss_link_text.str("");

        ss_link_url << "link" << links.at(i) << "_url";
        ss_link_title << "link" << links.at(i) << "_title";
        ss_link_text << "link" << links.at(i) << "_text";

        string strURL = "";
        string strTitle = "";
        string strText = "";

        config.get(ss_link_url.str(), strURL);
        config.get(ss_link_title.str(), strTitle);
        config.get(ss_link_text.str(), strText);

        ret["links"][i]["URL"] = strURL;
        ret["links"][i]["title"] = strTitle;
        // If no text is set, use the URL instead.
        ret["links"][i]["text"] = (strText != "" ? strText : strURL);
    }

    // Lastly, don't forget to show the pages.
    // Sort them first: If they have a "position", the theme should respect this.
    // Otherwise, meh.
    sort(vsi_pages.begin(), vsi_pages.end(), [](const auto & lhs, const auto & rhs) {
        // "Position" defaults to 0 and can't be set to < 1.
        // So we can assume that a "smaller than" check is enough here...
        return lhs.i_position < rhs.i_position;
    });

    // Put them into the template.
    bool hasActivePage = false;

    ret["defaultclass"] = "";
    for (auto it = vsi_pages.begin(); it != vsi_pages.end(); ++it, ++pagecount) {
        stringstream ss_link;
        ss_link << cfgs.cfg_siteurl << "/" << cfgs.cfg_page_subdir << "/" << (*it).s_slug;

        ret["pagenav"][pagecount]["title"] = (*it).s_title;
        ret["pagenav"][pagecount]["link"] = mergeMultipleSlashes(ss_link.str());

        if (!extratitle.empty() && extratitle == (*it).s_title) {
            // This page is the current page. Provide a way to mark it so.
            ret["pagenav"][pagecount]["cssclass"] = " active";
            hasActivePage = true;
        }
        else {
            ret["pagenav"][pagecount]["cssclass"] = "";
        }
    }

    if (!hasActivePage) {
        // None of the known pages is currently clicked.
        // Set the "default page" - if the theme knows such - as active instead.
        ret["defaultclass"] = " active";
    }

    // Bonus: {{ blogcpp }} variable
    stringstream ss_blogcpp;
    ss_blogcpp << "made with love and <a href=\"" << APP_URL << "\" target=\"_blank\">blogcpp</a>";
    ret["blogcpp"] = ss_blogcpp.str();

    return ret;
}

void createTaxonomy(json extradata) {
    // Calls createTaxonomyHTML() for all authors, series, tags and categories.
#ifdef WITH_DEBUGLOG
    stringstream ss_debuglog;

    DebugLog::debuglog("Creating the author archives.");
#endif
    for (auto a : vs_authors) {
        // Find all authors and write them into the taxonomy archive.
        vector<SingleItem> vs_this_authors_posts;
        for (auto p : vsi_posts) {
            if (p.s_author == a) {
                // This post belongs to this author.
#ifdef WITH_DEBUGLOG
                ss_debuglog.str("");
                ss_debuglog << "Adding the post '" << p.s_title << "' by the author '" << a << "' to the archives.";
                DebugLog::debuglog(ss_debuglog.str());
#endif
                vs_this_authors_posts.push_back(p);
            }
        }

        createTaxonomyHTML(extradata, AuthorArchives, a, vs_this_authors_posts);
    }

#ifdef WITH_DEBUGLOG
    DebugLog::debuglog("Creating the series archives.");
#endif
    for (auto s : vs_series) {
        // Find all series and write them into the taxonomy archive.
        vector<SingleItem> vs_this_series_posts;
        for (auto p : vsi_posts) {
            if (p.s_series == s) {
                // This post belongs to this series.
#ifdef WITH_DEBUGLOG
                ss_debuglog.str("");
                ss_debuglog << "Adding the post '" << p.s_title << "' from the series '" << s << "' to the archives.";
                DebugLog::debuglog(ss_debuglog.str());
#endif
                vs_this_series_posts.push_back(p);
            }
        }

        createTaxonomyHTML(extradata, Series, s, vs_this_series_posts);
    }

#ifdef WITH_DEBUGLOG
    DebugLog::debuglog("Creating the category archives.");
#endif
    for (auto it = vs_cats.begin(); it != vs_cats.end(); ++it) {
        // Find all posts (pages don't need categories) with this category and write them into the taxonomy archive.
        vector<SingleItem> vs_this_cat_items;
        for (auto p : vsi_posts) {
            if (find(p.cats.begin(), p.cats.end(), *it) == p.cats.end()) {
                // Skip this post if it does not have this category.
                continue;
            }
#ifdef WITH_DEBUGLOG
            ss_debuglog.str("");
            ss_debuglog << "Adding post '" << p.s_title << "' to category '" << *it << "'";
            DebugLog::debuglog(ss_debuglog.str());
#endif
            vs_this_cat_items.push_back(p);
        }

        sort(vs_this_cat_items.begin(), vs_this_cat_items.end(), [](SingleItem & a, SingleItem & b) {
            // Sort by time (descending).
            return mktime(&a.time) > mktime(&b.time);
        });

        createTaxonomyHTML(extradata, Categories, (*it), vs_this_cat_items);
    }

#ifdef WITH_DEBUGLOG
    DebugLog::debuglog("Creating the tag archives.");
#endif
    for (auto it = vs_tags.begin(); it != vs_tags.end(); ++it) {
        // Find all posts and pages with this tag and write them into the taxonomy archive.
        vector<SingleItem> vs_this_tag_items;
        for (const SingleItem& p : vsi_posts) {
            if (find(p.tags.begin(), p.tags.end(), *it) == p.tags.end()) {
                // Skip this post if it does not have this tag.
                continue;
            }
#ifdef WITH_DEBUGLOG
            ss_debuglog.str("");
            ss_debuglog << "Adding post '" << p.s_title << "' to tag '" << *it << "'";
            DebugLog::debuglog(ss_debuglog.str());
#endif
            lock_guard<mutex> lock(mtx);
            vs_this_tag_items.push_back(p);
        }
        for (const SingleItem& p : vsi_pages) {
            if (find(p.tags.begin(), p.tags.end(), *it) == p.tags.end()) {
                // Skip this page if it does not have this tag.
                continue;
            }
#ifdef WITH_DEBUGLOG
            ss_debuglog.str("");
            ss_debuglog << "Adding page '" << p.s_title << "' to tag '" << *it << "'";
            DebugLog::debuglog(ss_debuglog.str());
#endif
            lock_guard<mutex> lock(mtx);
            vs_this_tag_items.push_back(p);
        }

        sort(vs_this_tag_items.begin(), vs_this_tag_items.end(), [](SingleItem & a, SingleItem & b) {
            // Sort by time (descending).
            return mktime(&a.time) > mktime(&b.time);
        });

        createTaxonomyHTML(extradata, Tags, (*it), vs_this_tag_items);
    }
}

void createTimedArchives(json extradata) {
    // Calls createTaxonomyHTML() for all yyyy/mm in vsi_posts.
    stringstream ss_temp_month;
    stringstream ss_temp_year;

    // Create the HTML files (by the end; all of them need to know each other):
    for (auto v : vec_years) {
        stringstream year;
        year << v.front().time.tm_year + 1900;
        createTaxonomyHTML(extradata, TimedArchives, year.str(), v);
    }

    for (auto v : vec_months) {
        stringstream month;
        month << v.front().time.tm_year + 1900 << "/" << setfill('0') << setw(2) << v.front().time.tm_mon + 1;
        createTaxonomyHTML(extradata, TimedArchives, month.str(), v);
    }
}

json renderSidebarArchives() {
    // Adds Taxonomy archives from vs_series, vs_cats and vs_tags to the sidebar.
    // Also adds the latest <cfgs.cfg_maxhistory> posts.
    int catcount = 0;
    int tagcount = 0;
    int seriescount = 0;
    int postcount = 0;
    int yearcount = 0;
    int monthcount = 0;
#ifdef WITH_DEBUGLOG
    stringstream ss_debuglog;
    DebugLog::debuglog("Rendering the categories/tags list for the sidebar.");
#endif

    json ret;

    // Categories:
    sort(vs_cats.begin(), vs_cats.end(), vectorSort);
    vs_cats.erase(unique(vs_cats.begin(), vs_cats.end()), vs_cats.end());

    for (auto it = vs_cats.begin(); it != vs_cats.end(); ++it, ++catcount) {
        ret["allcats"][catcount]["catname"] = (*it);
        ret["allcats"][catcount]["slug"] = createBasicSlug((*it).c_str());
    }

    // Tags:
    sort(vs_tags.begin(), vs_tags.end(), vectorSort);
    vs_tags.erase(unique(vs_tags.begin(), vs_tags.end()), vs_tags.end());

    for (auto it = vs_tags.begin(); it != vs_tags.end(); ++it, ++tagcount) {
        ret["alltags"][tagcount]["tagname"] = (*it);
        ret["alltags"][tagcount]["slug"] = createBasicSlug((*it).c_str());
    }

    // Series:
    ret["series"] = cfgs.cfg_series && !vs_series.empty();
    if (ret["series"]) {
        sort(vs_series.begin(), vs_series.end(), vectorSort);

        for (auto it = vs_series.begin(); it != vs_series.end(); ++it, ++seriescount) {
            ret["allseries"][seriescount]["seriesname"] = (*it);
            ret["allseries"][seriescount]["slug"] = createBasicSlug((*it).c_str());
        }
    }

#ifdef WITH_DEBUGLOG
    ss_debuglog.str("");
    ss_debuglog << "Adding the monthly/yearly archives to the sidebar.";
    DebugLog::debuglog(ss_debuglog.str());
#endif

    // Find all months inside every year and put them into a sub-block:
    sort(vs_archive_years.begin(), vs_archive_years.end(), vectorSort);
    reverse(vs_archive_years.begin(), vs_archive_years.end());

    for (auto it = vs_archive_years.begin(); it != vs_archive_years.end(); ++it, ++yearcount) {
        // (*it) is, for example, "2016" now.
        ret["archives"][yearcount]["year"] = (*it);

        vector<string> vs_year_months;
        for (auto m : vs_archive_months) {
            if (m.find((*it)) == 0) {
                // <month> starts with this year (e.g. "2016/02")
                lock_guard<mutex> lock(mtx);
                vs_year_months.push_back(m);
            }
        }

        // Add every month from vs_year_months to the current template's year/month block.
        json months = nullptr;
        ret["archives"][yearcount]["months"] = months;

        sort(vs_year_months.begin(), vs_year_months.end(), vectorSort);
        for (auto it2 = vs_year_months.rbegin(); it2 != vs_year_months.rend(); ++it2, ++monthcount) {
            ret["archives"][yearcount]["months"][monthcount]["month"] = (*it2);
        }

        monthcount = 0; // Every year has new months.
    }

#ifdef WITH_DEBUGLOG
    ss_debuglog.str("");
    ss_debuglog << "Adding the latest " << cfgs.cfg_maxhistory << " posts to the sidebar.";
    DebugLog::debuglog(ss_debuglog.str());
#endif
    // For archivation, we should (at least) sort the article vector.
    sort(vsi_posts.begin(), vsi_posts.end(), [](SingleItem & a, SingleItem & b) {
        // The older an article is, the further it should be at the end of the article list.
        return mktime(&a.time) > mktime(&b.time);
    });

    // Try to get the newest <maxhistory> items.
    vector<SingleItem> v_newest_x {vsi_posts.begin(), next(vsi_posts.begin(), min(vsi_posts.size(), (size_t)cfgs.cfg_maxhistory))};

    for (auto it = v_newest_x.begin(); it != v_newest_x.end(); ++it, ++postcount) {
        stringstream ss_link;
        ss_link << cfgs.cfg_siteurl << "/";
        if (cfgs.cfg_article_subdir != "/") {
            ss_link << cfgs.cfg_article_subdir << "/";
        }
        ss_link << parseTmToPath((*it).time, cfgs.cfg_permalinks) << "/" << (*it).s_slug;

        bool dont_use_emoji = !cfgs.cfg_emojis && lowercase((*it).s_emoji) == "off";
        ret["latestposts"][postcount]["title"] = dont_use_emoji ? (*it).s_title : (*it).s_title_emojified;
        ret["latestposts"][postcount]["date"] = dateToPrint((*it).time, true);
        ret["latestposts"][postcount]["link"] = ss_link.str();
    }

#ifdef WITH_DEBUGLOG
    DebugLog::debuglog("Done!");
#endif
    return ret;
}

string createPaginatorHTML(int total_pages, int current_page) {
    // Returns the paginator HTML for the input data.
    if (total_pages == 1) {
        // No paginator needed.
        return "";
    }

    stringstream ss_ret;
#ifdef WITH_DEBUGLOG
    stringstream ss_debuglog;
    ss_debuglog << "Add the paginator for page " << current_page << " of " << total_pages;
    DebugLog::debuglog(ss_debuglog.str());
    ss_debuglog.str("");
#endif

    for (int i = 1; i <= total_pages; i++) {
        if (i == current_page) {
            // The current page does not need a link to itself.
            ss_ret << "[" << i << "]";
            if (i < total_pages) {
                ss_ret << " <span class=\"paginator-divider\">|</span> ";
            }

            continue;
        }

        ss_ret << "<a href=\"";

        if (i == 1) {
            // If the target page is "1", note that there is no "page 1".
            ss_ret << "../..";
        }
        else if (current_page > 1) {
            // If current_page > 1, this is a subpage (/page/<k>). Add relative paths for it.
            ss_ret << "../" << i;
        }
        else {
            ss_ret << "page/" << i;
        }
        ss_ret << "\">" << i << "</a>";

        if (i < total_pages) {
            ss_ret << " <span class=\"paginator-divider\">|</span> ";
        }
    }

    return ss_ret.str();
}

void renderIndex(json extradata, stringstream& ss_ret) {
    // Tries to build the main index.htm file from all collected information.
    // Will produce a debug log on errors.
    stringstream temp;
    json data;

#ifdef WITH_DEBUGLOG
    stringstream ss_debuglog;
    ss_debuglog << "Rendering the index template.";
    DebugLog::debuglog(ss_debuglog.str());
#endif

    // Add the latest <cfgs.cfg_maxitems> posts.
    int total_pages = calculateTotalPages(vsi_posts, cfgs.cfg_maxitems); // <number of total pages> for the paginator
#ifdef WITH_DEBUGLOG
    ss_debuglog.str("");
    ss_debuglog << "We have " << vsi_posts.size() << " posts (" << total_pages << " pages total). Process them.";
    DebugLog::debuglog(ss_debuglog.str());
#endif

    for (int k = 1; k <= total_pages; k++) {
        inja::Environment env = inja::Environment(ss_tpldir.str(), ss_outdir.str());
        inja::Template tpl = injatpl.loadTemplate(env, ss_tpl_index.str());

        // Add OpenGraph data.
        if (cfgs.cfg_opengraphimg != "") {
            stringstream ss_opengraph;
            ss_opengraph << "<meta property=\"og:title\" content=\"" << cfgs.cfg_sitetitle << "\" />" << endl;
            ss_opengraph << "<meta property=\"og:type\" content=\"website\" />" << endl;
            ss_opengraph << "<meta property=\"og:image\" content=\"" << cfgs.cfg_opengraphimg << "\" />" << endl;
            ss_opengraph << "<meta property=\"og:url\" content=\"" << cfgs.cfg_siteurl  << "\" />" << endl;
            ss_opengraph << "<meta property=\"og:site_name\" content=\"" << cfgs.cfg_sitetitle << "\" />" << endl;

            data["opengraphmeta"] = ss_opengraph.str();
        }

        // Create <total_pages>.
        int i = 0;
        int posts_per_page = 0;
        int offset = cfgs.cfg_maxitems * (k - 1); // The second page starts with the (1 * maxitems)th post.

        vector<SingleItem> v_newest_x;

        // Note to myself: If I ever decide to change the following calculations, I'm screwed.
        // There are exactly (total posts / posts per page) posts per page, the last page has "the rest".
        if (total_pages == 1) {
            // No complex calculation when we only have one page.
            v_newest_x = vsi_posts_with_sticky;
        }
        else {
            if (k * cfgs.cfg_maxitems <= static_cast<int>(vsi_posts_with_sticky.size())) {
                // If we are on the first few pages, we display as many articles as we can.
                posts_per_page = cfgs.cfg_maxitems;
            }
            else {
                // Otherwise, we display the rest.
                posts_per_page = vsi_posts_with_sticky.size() % cfgs.cfg_maxitems;
            }

            // The first <x> posts are 0 .. <x-1>, the next <x> posts are <x> .. <2x-1>:
            vector<SingleItem> vec(&vsi_posts_with_sticky[offset], &vsi_posts_with_sticky[offset + posts_per_page - 1]);
            v_newest_x = vec;
        }

        temp.str("./");

        if (k > 1) {
            temp << "page/" << k << "/";
        }

        // As inja writes relative to its environment outdir instead of ss_outdir, blogcpp must prepend
        // ss_outdir when checking whether the path needs to be created in order to write into it.
        useOrCreatePathWithPrefix(ss_outdir.str(), temp.str());

        temp << "index.htm";

        data["pagination"] = createPaginatorHTML(total_pages, k);

        for (auto it = v_newest_x.begin(); it != v_newest_x.end(); ++it, i++) {
            stringstream ss_link, ss_count, ss_text;
            ss_link << cfgs.cfg_siteurl << "/";
            if (cfgs.cfg_article_subdir != "/") {
                ss_link << cfgs.cfg_article_subdir << "/";
            }
            ss_link << parseTmToPath((*it).time, cfgs.cfg_permalinks) << "/" << (*it).s_slug;
            ss_count << v_newest_x.size() - i;

            bool isSticky = lowercase((*it).s_sticky) == "on";

#ifdef WITH_DEBUGLOG
            ss_debuglog.str("");
            ss_debuglog << "Adding post '" << (*it).s_title << "' to the index page.";
            if (isSticky) {
                ss_debuglog << "\nThis post is marked as sticky.";
            }
            DebugLog::debuglog(ss_debuglog.str());
#endif

            // Don't forget to shorten the preview if applicable.
            ExcerptExtractor excerpt((*it).s_text, cfgs.cfg_excerpts);
            ss_text << excerpt.extractExcerpt();

            data["posts"][i]["morelink"] = cfgs.cfg_excerpttext;
            data["posts"][i]["shortened"] = excerpt.shortened;

            bool dont_use_markdown = lowercase((*it).s_markdown) == "off";
            bool dont_use_emoji = !cfgs.cfg_emojis && lowercase((*it).s_emoji) == "off";

            // Mandatory fields:
            data["posts"][i]["author"] = (*it).s_author;
            data["posts"][i]["title"] = dont_use_emoji ? (*it).s_title : (*it).s_title_emojified;
            data["posts"][i]["date"] = dateToPrint((*it).time);
            data["posts"][i]["text"] = !excerpt.shortened ? (*it).mr_text.parsed_text : MarkdownWrapper::markdownify(ss_text.str(), dont_use_markdown, dont_use_emoji, cfgs.cfg_embeds).parsed_text; // Use the already markdownified text if possible
            data["posts"][i]["URL"] = ss_link.str();
            data["posts"][i]["stickyclass"] = isSticky ? " sticky" : "";

            // For optional styling, we determine if i (starting with 0) is odd or even.
            data["posts"][i]["count"] = ss_count.str();
            data["posts"][i]["altclass"] = (i % 2 == 0 ? "even" : "odd");

            // Optional fields:
            // Some people don't want a "change date" in their themes anyway.
            if ((*it).changetime.tm_year > 0) {
                data["posts"][i]["changedate"] = dateToPrint((*it).changetime);
            }
            else {
                // Technically, the latest change was always the creation date.
                data["posts"][i]["changedate"] = dateToPrint((*it).time);
            }

            // Write to the index outfile
            try {
#ifdef WITH_DEBUGLOG
                ss_debuglog.str("");
                ss_debuglog << "Writing to the file " << temp.str();
                DebugLog::debuglog(ss_debuglog.str());
#endif
                // Merge:
                data.update(extradata);

                // Write:
                injatpl.writeTemplate(tpl, env, data, temp.str());
            }
            catch (json::type_error& e) {
                // Ayeeeeeeeeeeeeeeeeee!
#ifdef WITH_DEBUGLOG
                ss_debuglog.str("");
                ss_debuglog << "Caught a JSON exception while trying to renderIndex(): " << e.what();
                DebugLog::debuglog(ss_debuglog.str());
#endif
                ss_ret << endl << "Failed to build the JSON objects: " << e.what() << " - this is probably a bug!";
            }
            catch (exception&) {
                ss_ret << endl << "Failed to write into " << temp.str() << " - do you mind to check your settings?";
            }
        }
    }
}


string dateToPrint(tm tm_t, bool shortdate /*= false*/) {
    // Formats the in-date <tm_t> into a localized time string.
    string s_localeFromConfig = cfgs.cfg_locale;
#if _MSC_VER && !__INTEL_COMPILER
    // Oh, Microsoft. :-(
    // We need to use "en-US" instead of "en_US.utf8" here ...
    // ref.: https://msdn.microsoft.com/en-us/library/hzz3tw78.aspx
    s_localeFromConfig = s_localeFromConfig.substr(0, 5);
    s_localeFromConfig = regex_replace(s_localeFromConfig, regex("_"), "-");
#endif
    locale locdate(s_localeFromConfig);
    ostringstream ss_ret;
    ss_ret.imbue(locdate);

    ss_ret << put_time(&tm_t, shortdate ? "%x" : "%c");

    return ss_ret.str();
}


int getSuffixID(string init_slug, tm tm_t) {
    // Returns the number of duplicates; 0 if the <init_slug> is unique in tm_t.
    int ret = 0;
    stringstream ss_temp;
#ifdef WITH_DEBUGLOG
    stringstream ss_debuglog;
#endif

    const char* const cstr_permalinks = cfgs.cfg_permalinks.c_str();

    for (;;) {
        ss_temp.str("");
        ss_temp << put_time(&tm_t, cstr_permalinks) << "/";

        if (ret > 0) {
            // Add increasing numbers until we're unique.
            ss_temp << init_slug << "-" << ret;
        }
        else {
            ss_temp << init_slug;
        }

        if (find(vs_slugs.begin(), vs_slugs.end(), ss_temp.str()) == vs_slugs.end()) {
            // Finally, no such slug.
            break;
        }

        ret++;
    }

#ifdef WITH_DEBUGLOG
    if (ret > 0) {
        ss_debuglog << "Slug " << init_slug << " was suffixed with " << ret;
        DebugLog::debuglog(ss_debuglog.str());
    }
#endif

    return ret;
}


void createTaxonomyHTML(json extradata, Taxonomies taxonomytype, string taxonomy, vector<SingleItem> items) {
    // Creates a taxonomy archive (from the archive.txt template).
    stringstream ss_outputdir, ss_outputfile, ss_feedfile, ss_feedmeta;
    string s_feedtitle;
    int i = 0;
    bool b_addFeed = false;
#ifdef WITH_DEBUGLOG
    stringstream ss_debuglog;
    ss_debuglog << "Creating the '" << taxonomy << "' taxonomy archives from " << items.size() << " items.";
    DebugLog::debuglog(ss_debuglog.str());
#endif

    json data;
    data["what"] = taxonomy;

    inja::Environment env = inja::Environment(ss_tpldir.str(), ss_outdir.str());
    inja::Template tpl = injatpl.loadTemplate(env, ss_tpl_archive.str());

    ss_feedmeta << "<link rel=\"alternate\" type=\"application/rss+xml\" title=\"";

    // Determine the output directory:
    // If <taxonomytype> is Categories, the subfolder will be "categories/<taxonomy>/".
    // If <taxonomytype> is Tags, the subfolder will be "tags/<taxonomy>/".
    // If <taxonomytype> is Series, the subfolder will be "series/<taxonomy>/".
    // If <taxonomytype> is TimedArchives, the subfolder will be "archives/<taxonomy>/".
    // If <taxonomytype> is AuthorArchives, the subfolder will be "author/<taxonomy>/".
    string s_tax_slug = createBasicSlug(taxonomy.c_str());

    switch (taxonomytype) {
        case Categories:
            s_feedtitle = cfgs.cfg_sitetitle + ": '" + taxonomy + "' Category RSS Feed";

            ss_outputdir << "categories/" << s_tax_slug;
            ss_feedfile << "category-" << s_tax_slug << ".xml";

            b_addFeed = true;

            break;
        case Tags:
            s_feedtitle = cfgs.cfg_sitetitle + ": '" + taxonomy + "' Tag RSS Feed";

            ss_outputdir << "tags/" << s_tax_slug;
            ss_feedfile << "tag-" << s_tax_slug << ".xml";
            b_addFeed = true;

            break;
        case Series:
            s_feedtitle =  cfgs.cfg_sitetitle + ": " + taxonomy + " Series RSS Feed";

            ss_outputdir << "series/" << s_tax_slug;
            ss_feedfile << "series-" << s_tax_slug << ".xml";

            b_addFeed = true;

            break;
        case TimedArchives:
            // There is not much reason to create RSS feeds for monthly archives.
            // Don't do that here.
            ss_outputdir << "archives/" << taxonomy;
            break;
        case AuthorArchives:
            s_feedtitle =  cfgs.cfg_sitetitle + ": " + taxonomy + " RSS Feed";

            ss_outputdir << "author/" << taxonomy;
            ss_feedfile << "author-" << s_tax_slug << ".xml";

            b_addFeed = true;
    }

    ss_feedmeta << s_feedtitle << "\" href=\"" << ss_feeddir.str() << ss_feedfile.str() << "\" />";
    if (b_addFeed) {
        RSSGenerator generator(ss_feeddir.str(), s_feedtitle, ss_feedfile.str(), cfgs);
        generator.createRSS(items);

        data["feedmeta"] = ss_feedmeta.str();
    }

    lock_guard<mutex> lock(mtx);

    // As inja writes relative to its environment outdir instead of ss_outdir, blogcpp must prepend
    // ss_outdir when checking whether the path needs to be created in order to write into it.
    useOrCreatePathWithPrefix(ss_outdir.str(), ss_outputdir.str());

    // Traverse through <items>, collect s_title, s_slug and dateToPrint(time),
    // put them into the JSON's "entries" block.
    for (const SingleItem& si : items) {
        stringstream ss_url;

        if (si.b_isPage) {
            ss_url << cfgs.cfg_siteurl << "/" << cfgs.cfg_page_subdir << "/" << si.s_slug;
        }
        else {
            string outputdate = parseTmToPath(si.time, cfgs.cfg_permalinks);
            ss_url << cfgs.cfg_siteurl << "/";
            if (cfgs.cfg_article_subdir != "/") {
                ss_url << cfgs.cfg_article_subdir << "/";
            }
            ss_url << outputdate << "/" << si.s_slug;
        }

        data["entries"][i]["URL"] = mergeMultipleSlashes(ss_url.str());
        data["entries"][i]["item"] = si.s_title;
        data["entries"][i]["metadata"] = dateToPrint(si.time);

        i++;
    }

    ss_outputfile << ss_outputdir.str() << "/index.htm";

    try {
#ifdef WITH_DEBUGLOG
        ss_debuglog.str("");
        ss_debuglog << "Writing to the file " << ss_outputfile.str();
        DebugLog::debuglog(ss_debuglog.str());
#endif
        // Merge:
        data.update(extradata);

        // Write:
        injatpl.writeTemplate(tpl, env, data, ss_outputfile.str());
    }
    catch (json::type_error& e) {
        // Ayeeeeeeeeeeeeeeeeee!
#ifdef WITH_DEBUGLOG
        ss_debuglog.str("");
        ss_debuglog << endl << "Failed to build the JSON objects: " << e.what() << " - this is probably a bug!";
        DebugLog::debuglog(ss_debuglog.str());
#endif
        if (!silentmode) {
            cout << endl << "Failed to build the JSON objects: " << e.what() << " - this is probably a bug!";
        }
        exit(1);
    }
    catch (exception&) {
#ifdef WITH_DEBUGLOG
        ss_debuglog.str("");
        ss_debuglog << endl << "Failed to write into " << ss_outputfile.str() << " - do you mind to check your settings?";
        DebugLog::debuglog(ss_debuglog.str());
#endif
        if (!silentmode) {
            cout << endl << "Failed to write into " << ss_outputfile.str() << " - do you mind to check your settings?";
        }
        exit(1);
    }

#ifdef WITH_DEBUGLOG
    DebugLog::debuglog("Done!");
#endif
}

void createPageOrPost(string inputfile, ConfigParser config, bool is_page) {
    // Creates /outdir/{posts|pages}/yyyy/mm/slug/index.htm from the vsi_posts or vsi_pages entry
    // of <inputfile>.
    SingleItem ret, si_prev, si_next, si_prev_in_series, si_next_in_series;
    CommentIntegration ci(cfgs.cfg_commenttype, cfgs.cfg_commentdata);
    stringstream inputfile_path, ss_output, ss_subdir, ss_link_creator, permalink_path, outputfile_path, outputfile;

    // Meta tags (from the header):
    string s_datetime, s_changed, s_author, s_cats, s_tags, s_title, s_slug, s_contents, s_markdown, s_emoji, s_comments, s_ogimage, s_series;

    // Meta tags (generated):
    string s_title_emojified;
    MarkdownReturn mr_text;
    int catcount = 0, tagcount = 0;

    // Series:
    string s_prev_url, s_prev_title, s_next_url, s_next_title, s_prev_in_series_url, s_prev_in_series_title, s_next_in_series_url, s_next_in_series_title;
    bool has_prev = false, has_next = false, has_prev_in_series = false, has_next_in_series = false;

#ifdef WITH_DEBUGLOG
    stringstream ss_debuglog;
    ss_debuglog << "Creating a new " << (is_page ? "page" : "post") << " with the " << (is_page ? ss_tpl_page.str() : ss_tpl_post.str()) << " template from '" << inputfile << "'";

    DebugLog::debuglog(ss_debuglog.str());
#endif

    lock_guard<mutex> lock(mtx);
    inja::Environment env = inja::Environment(ss_tpldir.str(), ss_outdir.str());
    inja::Template tpl = injatpl.loadTemplate(env, is_page ? ss_tpl_page.str() : ss_tpl_post.str());

    json data;

    // Find the <inputfile> SingleItem in vsi_posts or vsi_pages:
    vector<SingleItem> vec = {};
    vec = is_page ? vsi_pages : vsi_posts;

    for (auto it = vec.begin(); it != vec.end(); ++it) {
        SingleItem currItem = *it;
        if (currItem.s_filename == inputfile) {
            // This is the item we are looking for.
            ret = currItem;

            if (!is_page) {
                // This is a single article. Do we have surrounding posts yet?
                // Note that <vec> needs to be sorted (reverse) chronically at this point!
                if (it != vec.begin()) {
                    si_next = *prev(it);

                    ss_link_creator << cfgs.cfg_siteurl << "/";
                    if (cfgs.cfg_article_subdir != "/") {
                        ss_link_creator << cfgs.cfg_article_subdir << "/";
                    }

                    ss_link_creator << parseTmToPath(si_next.time, cfgs.cfg_permalinks) << "/" << si_next.s_slug << "/";
                    s_next_title = si_next.s_title;
                    s_next_url = ss_link_creator.str();

                    has_next = true;

                    ss_link_creator.str("");
                }

                if (it != prev(vec.end())) {
                    si_prev = *next(it);

                    ss_link_creator << cfgs.cfg_siteurl << "/";
                    if (cfgs.cfg_article_subdir != "/") {
                        ss_link_creator << cfgs.cfg_article_subdir << "/";
                    }

                    ss_link_creator << parseTmToPath(si_prev.time, cfgs.cfg_permalinks) << "/" << si_prev.s_slug << "/";
                    s_prev_title = si_prev.s_title;
                    s_prev_url = ss_link_creator.str();

                    has_prev = true;

                    ss_link_creator.str("");
                }
            }

            break;
        }
    }

    // Series detection:
    if (cfgs.cfg_series && !is_page && !ret.s_series.empty()) {
        // Which (other) posts are part of this series?
        pair<multimap<string, SingleItem>::iterator, multimap<string, SingleItem>::iterator> limits;
        limits = map_series_items.equal_range(ret.s_series);

        SingleItem prev_in_series, curr_in_series;
        bool bNextIsLast = false;
        for (auto it = limits.first; it != limits.second; ++it) {
            prev_in_series = curr_in_series;
            curr_in_series = it->second;
            if (curr_in_series.s_title == ret.s_title) {
                // This is the current item.
                if (!prev_in_series.s_title.empty()) {
                    // Build the HTML.
                    ss_link_creator << cfgs.cfg_siteurl << "/";
                    if (cfgs.cfg_article_subdir != "/") {
                        ss_link_creator << cfgs.cfg_article_subdir << "/";
                    }

                    ss_link_creator << parseTmToPath(prev_in_series.time, cfgs.cfg_permalinks) << "/" << prev_in_series.s_slug << "/";
                    s_prev_in_series_title = prev_in_series.s_title;
                    s_prev_in_series_url = ss_link_creator.str();

                    has_prev_in_series = true;

                    ss_link_creator.str("");
                }
                bNextIsLast = true;
            }
            else if (bNextIsLast) {
                // The previous article was the current one, so this is its successor.
                ss_link_creator << cfgs.cfg_siteurl << "/";
                if (cfgs.cfg_article_subdir != "/") {
                    ss_link_creator << cfgs.cfg_article_subdir << "/";
                }

                ss_link_creator << parseTmToPath(curr_in_series.time, cfgs.cfg_permalinks) << "/" << curr_in_series.s_slug << "/";
                s_next_in_series_title = curr_in_series.s_title;
                s_next_in_series_url = ss_link_creator.str();

                has_next_in_series = true;

                ss_link_creator.str("");
                break;
            }
        }
    }

    // Everything's already in our vector. Read it and put it into our template.
    s_datetime = dateToPrint(ret.time);
    s_changed = ret.changetime.tm_year > 0 ? dateToPrint(ret.changetime) : "";
    s_title = ret.s_title;
    s_slug = ret.s_slug;
    s_author = ret.s_author;
    s_contents = ret.s_text;
    s_markdown = ret.s_markdown;
    s_emoji = ret.s_emoji;
    s_comments = ret.s_comments;
    s_ogimage = ret.s_ogimage;
    mr_text = ret.mr_text;

    if (cfgs.cfg_series) {
        s_series = ret.s_series;
    }

    if (cfgs.cfg_emojis) {
        s_title_emojified = ret.s_title_emojified;
    }

    vector<string> v_local_cats = ret.cats;
    vector<string> v_local_tags = ret.tags;

    // Build the output path.
    // The HTML file resides in:
    //   $outdir/posts/yyyy/mm/$slug/index.htm
    //   $outdir/pages/$slug/index.htm
    if (is_page) {
        ss_subdir << cfgs.cfg_page_subdir;
    }
    else {
#ifdef WITH_DEBUGLOG
        DebugLog::debuglog("Parsing the time.");
#endif
        // Parse the time stamp.
        string output_path_date = parseTmToPath(ret.time, cfgs.cfg_permalinks);

#ifdef WITH_DEBUGLOG
        DebugLog::debuglog("Checking the schedule.");
#endif

        // Is the date in the future?
        // If so, skip this file.
        if (isFutureDate(output_path_date)) {
            return;
        }

        ss_subdir << cfgs.cfg_article_subdir << "/" << output_path_date;
    }
    outputfile_path << mergeMultipleSlashes(ss_subdir.str()) << "/" << s_slug << "/";

    // Create the permalink
    permalink_path << cfgs.cfg_siteurl << "/";
    if (ss_subdir.str() != "/") {
        permalink_path << ss_subdir.str() << "/";
    }
    permalink_path << s_slug << "/";

    // As inja writes relative to its environment outdir instead of ss_outdir, blogcpp must prepend
    // ss_outdir when checking whether the path needs to be created in order to write into it.
    useOrCreatePathWithPrefix(ss_outdir.str(), outputfile_path.str());

    // Set the basics:
#ifdef WITH_DEBUGLOG
    DebugLog::debuglog("Parsing the header.");
#endif

    json header = renderHeader(config, s_title);
    json sidebar = renderSidebarArchives();

    // Merge:
    data.update(header);
    data.update(sidebar);

    // Add comments if required:
    if (lowercase(s_comments) != "off" && cfgs.cfg_comments && !is_page) {
        try {
            stringstream ss_comment_header;
            ss_comment_header << data["headscripts"] << endl;
            ss_comment_header << ci.addHeader();
            data["headscripts"] = ss_comment_header.str();
        }
        catch (...) {
            // Oops...?
#ifdef WITH_DEBUGLOG
            DebugLog::debuglog("Failed to add the comments part to your <HEAD>. :-(");
#endif
        }
    }

    // Set meta data:
#ifdef WITH_DEBUGLOG
    DebugLog::debuglog("Setting the meta data.");
#endif
    data["author"] = s_author;
    data["authorslug"] = createBasicSlug(s_author.c_str());
    data["date"] = s_datetime;

    // Some people don't want a "change date" in their themes anyway.
    if (!s_changed.empty()) {
        data["changedate"] = s_changed;
    }
    else {
        // Technically, the latest change was always the creation date.
        data["changedate"] = s_datetime;
    }

    if (!is_page) {
        // Add surroundings.
        if (has_prev) {
            data["prevlink"] = "<a href=\"" + s_prev_url + "\">&laquo; " + s_prev_title + "</a>";
            data["prevmeta"] = "<link rel=\"prev\" href=\"" + s_prev_url + "\" />";
        }

        if (has_next) {
            data["nextlink"] = "<a href=\"" + s_next_url + "\">" + s_next_title + " &raquo;</a>";
            data["nextmeta"] = "<link rel=\"next\" href=\"" + s_next_url + "\" />";
        }

        // Article series (if any):
        data["series"] = cfgs.cfg_series && !s_series.empty();
        if (data["series"]) {
            data["seriesnav"]["series"] = s_series;
            data["seriesnav"]["title"] = s_title;

            if (has_prev_in_series) {
                data["seriesnav"]["seriesprevlink"] = "<a href=\"" + s_prev_in_series_url + "\">&laquo; " + s_prev_in_series_title + "</a>";
            }

            if (has_next_in_series) {
                data["seriesnav"]["seriesnextlink"] = "<a href=\"" + s_next_in_series_url + "\">" + s_next_in_series_title + " &raquo;</a>";
            }
        }
    }

    if (v_local_cats.size() > 0) {
#ifdef WITH_DEBUGLOG
        ss_debuglog.str("");
        ss_debuglog << "We have " << v_local_cats.size() << " categories. Let's traverse them.";
        DebugLog::debuglog(ss_debuglog.str());
#endif
        data["cats_title"] = "Categories: ";

        for (auto it = v_local_cats.begin(); it != v_local_cats.end(); ++it, ++catcount) {
            data["categories"][catcount]["name"] = *it;
            data["categories"][catcount]["slug"] = createBasicSlug((*it).c_str());
#ifdef WITH_DEBUGLOG
            ss_debuglog.str("");
            ss_debuglog << "Processing category " << *it;
            DebugLog::debuglog(ss_debuglog.str());
#endif
        }
    }

    if (v_local_tags.size() > 0) {
#ifdef WITH_DEBUGLOG
        ss_debuglog.str("");
        ss_debuglog << "We have " << v_local_tags.size() << " tags. Let's traverse them.";
        DebugLog::debuglog(ss_debuglog.str());
#endif
        stringstream ss_keywords;
        ss_keywords << "<meta name=\"keywords\" content=\"" << vectorJoin(v_local_tags) << "\" />";
        data["keywordsmeta"] = ss_keywords.str();
        data["tags_title"] = "Tags: ";

        for (auto it = v_local_tags.begin(); it != v_local_tags.end(); ++it, ++tagcount) {
            data["tags"][tagcount]["name"] = *it;
            data["tags"][tagcount]["slug"] = createBasicSlug((*it).c_str());
#ifdef WITH_DEBUGLOG
            ss_debuglog.str("");
            ss_debuglog << "Processing tag " << *it;
            DebugLog::debuglog(ss_debuglog.str());
#endif
        }
    }

    bool dont_use_emoji = !cfgs.cfg_emojis && lowercase(s_emoji) == "off";
    data["title"] = dont_use_emoji ? s_title : s_title_emojified;
    data["text"] = mr_text.parsed_text;

    // Embed all used Highlight.js languages:
    stringstream ss_hl_js;
    for (auto it = mr_text.used_languages.begin(); it != mr_text.used_languages.end(); ++it) {
        ss_hl_js << "    <script type=\"text/javascript\" src=\"//cdnjs.cloudflare.com/ajax/libs/highlight.js/" << HIGHLIGHT_VER << "/languages/" << *it << ".min.js\"></script>" << endl;
    }
    data["highlightdata"] = ss_hl_js.str();

    // Add comments if required:
    if (lowercase(s_comments) != "off" && cfgs.cfg_comments && !is_page) {
        data["comments"] = ci.addComments();
    }

    // Add OpenGraph data:
    string ogimage = (s_ogimage != "" ? s_ogimage : (cfgs.cfg_opengraphimg != "" ? cfgs.cfg_opengraphimg : ""));
    if (ogimage != "") {
        stringstream ss_opengraph;
        ss_opengraph << "<meta property=\"og:title\" content=\"" << (dont_use_emoji ? s_title : s_title_emojified) << "\" />" << endl;
        ss_opengraph << "<meta property=\"og:type\" content=\"website\" />" << endl;
        ss_opengraph << "<meta property=\"og:image\" content=\"" << ogimage << "\" />" << endl;
        ss_opengraph << "<meta property=\"og:url\" content=\"" << permalink_path.str() << "\" />" << endl;
        ss_opengraph << "<meta property=\"og:site_name\" content=\"" << cfgs.cfg_sitetitle << "\" />" << endl;

        data["opengraphmeta"] = ss_opengraph.str();
    }

    data["permalink"] = permalink_path.str();

    // Write to the article or page outfile
    outputfile << "./" << outputfile_path.str() << "index.htm";
    try {
#ifdef WITH_DEBUGLOG
        ss_debuglog.str("");
        ss_debuglog << "Writing to the file " << outputfile.str();
        DebugLog::debuglog(ss_debuglog.str());
#endif

        // Write:
        injatpl.writeTemplate(tpl, env, data, outputfile.str());
    }
    catch (json::type_error& e) {
        // Ayeeeeeeeeeeeeeeeeee!
#ifdef WITH_DEBUGLOG
        ss_debuglog.str("");
        ss_debuglog << endl << "Failed to build the JSON objects: " << e.what() << " - this is probably a bug!";
        DebugLog::debuglog(ss_debuglog.str());
#endif
        if (!silentmode) {
            cout << endl << "Failed to build the JSON objects: " << e.what() << " - this is probably a bug!";
        }
        exit(1);
    }
    catch (exception&) {
#ifdef WITH_DEBUGLOG
        ss_debuglog.str("");
        ss_debuglog << endl << "Failed to write into " << outputfile.str() << " - do you mind to check your settings?";
        DebugLog::debuglog(ss_debuglog.str());
#endif
        if (!silentmode) {
            cout << endl << "Failed to write into " << outputfile.str() << " - do you mind to check your settings?";
        }
        exit(1);
    }

#ifdef WITH_DEBUGLOG
    DebugLog::debuglog("Done!");
#endif
}


string createSlug(char const * title, tm tm_t) {
    // Creates an "extended" slug from a title.
    // Do NOT call this function twice on the same item - this will
    // probably break things as the slug will be found as existing then!
    string ret;
    ostringstream ss_temp;
    int suffix = 0;
#ifdef WITH_DEBUGLOG
    ostringstream ss_debuglog;
    ss_debuglog << "Creating a slug out of " << title << "... ";
#endif

    // Get the basic slug first:
    ret = createBasicSlug(title);

    // Find possible duplicates:
    suffix = getSuffixID(ret, tm_t);
    ss_temp << ret;
    if (suffix > 0) {
        ss_temp << "-" << suffix;
    }

    // Add the newly generated slug to the list of known slugs:
    stringstream ss_slug;
    const char* const cstr_permalinks = cfgs.cfg_permalinks.c_str();
    ss_slug << put_time(&tm_t, cstr_permalinks) << "/" << ss_temp.str();

    lock_guard<mutex> lock(mtx);
    vs_slugs.push_back(ss_slug.str());

#ifdef WITH_DEBUGLOG
    ss_debuglog << "the slug is '" << ss_temp.str() << "'.";
    DebugLog::debuglog(ss_debuglog.str());
#endif

    return ss_temp.str();
}

string createBasicSlug(char const * input) {
    // Creates a basic slug.
    // Mainly used for category/tag/archive slugs.
    string ret;
    UnicodeString uinput(input, "UTF-8");

    // Lowercase:
    uinput = uinput.toLower();

    // Umlauts/diacritics -> ASCII:
    UErrorCode status = U_ZERO_ERROR;
    Transliterator *accentsConverter = Transliterator::createInstance("NFD; [:M:] Remove; NFC", UTRANS_FORWARD, status);
    accentsConverter->transliterate(uinput);
    uinput.toUTF8String(ret);

    // Emojis:
    EmojiParser eparser;
    ret = eparser.clear(ret);

    // Hyphenization:
    ret = trim(genFileName(ret));
    transform(ret.begin(), ret.end(), ret.begin(), [](char ch) { return ch == ' ' ? '-' : ch; });

    // Avoid double hyphenization though:
    ret = regex_replace(ret, regex("--+"), "-");

    // It's rather wise not to end a slug with ".", "\" or "/".
    // Also we could remove trailing hyphens and exclamation marks while we're here.
    string::iterator it = ret.end() - 1;
    if (*it == '.' || *it == '\\' || *it == '/' || *it == '!' || *it == '-') {
        ret.erase(it);
    }

    return ret;
}

string process(string configfile) {
    // Do ALL the work!
    stringstream ret;

    if (silentmode) {
        // We need to return something, at least.
        // Why not an empty string?
        ret.str("");
    }

    // ------------------------------------------------
    // Initialize the config.
    // ------------------------------------------------

    ConfigParser config(configfile);

    cfgs.cfg_sitetitle = "Just Another Blog";
    config.get("sitetitle", cfgs.cfg_sitetitle);

    cfgs.cfg_subtitle = "";
    config.get("subtitle", cfgs.cfg_subtitle);

    cfgs.cfg_siteurl = APP_URL;
    config.get("siteurl", cfgs.cfg_siteurl);

    cfgs.cfg_outdir = "outdir";
    config.get("outdir", cfgs.cfg_outdir);

    cfgs.cfg_permalinks = "%Y/%m";
    config.get("permalinks", cfgs.cfg_permalinks);

    cfgs.cfg_article_subdir = "posts";
    config.get("articlepath", cfgs.cfg_article_subdir);

    cfgs.cfg_page_subdir = "pages";
    config.get("pagepath", cfgs.cfg_page_subdir);

    cfgs.cfg_locale = "en_US.utf8";
    config.get("locale", cfgs.cfg_locale);

    string s_use_series = "On";
    config.get("series", s_use_series);
    cfgs.cfg_series = lowercase(s_use_series) == "on";

    cfgs.cfg_maxitems = 25;
    config.get("maxitems", cfgs.cfg_maxitems);

    cfgs.cfg_maxhistory = 5;
    config.get("maxhistory", cfgs.cfg_maxhistory);

    cfgs.cfg_template = "default";
    config.get("template", cfgs.cfg_template);

    cfgs.cfg_excerpts = 2;
    config.get("excerptlength", cfgs.cfg_excerpts);

    cfgs.cfg_excerpttext = "&hellip;";
    config.get("excerpttext", cfgs.cfg_excerpttext);

    cfgs.cfg_feeddir = "RSS";
    config.get("rssdir", cfgs.cfg_feeddir);

    cfgs.cfg_commentdata = "";
    config.get("commentdata", cfgs.cfg_commentdata);

    cfgs.cfg_opengraphimg = "";
    config.get("opengraphimage", cfgs.cfg_opengraphimg);

    string s_use_embeds = "On";
    config.get("embeds", s_use_embeds);
    cfgs.cfg_embeds = lowercase(s_use_embeds) == "on";

    string s_generate_full_feed = "On";
    config.get("fullfeed", s_generate_full_feed);
    cfgs.cfg_fullfeed = lowercase(s_generate_full_feed) == "on";

    string s_emojis = "On";
    config.get("emojis", s_emojis);
    cfgs.cfg_emojis = lowercase(s_emojis) == "on";

    string s_comments = "Off";
    config.get("comments", s_comments);
    cfgs.cfg_comments = lowercase(s_comments) != "off";
    cfgs.cfg_commenttype = lowercase(s_comments);

    // Validity check:
    regex re_url("^https?://");
    smatch match;
    if (cfgs.cfg_opengraphimg != "" && !regex_search(cfgs.cfg_opengraphimg, match, re_url)) {
        // Invalid OpenGraph URL. Reset.
        cfgs.cfg_opengraphimg = "";
    }

    // Set up multi-core processing:
    num_threads = thread::hardware_concurrency() * 2;
    if (!num_threads) { num_threads = 1; }

    if (!silentmode) {
        ret << "Site generation started." << endl << endl;
        ret << "Using configuration file: " << configfile << "." << endl;
#ifdef WITH_DEBUGLOG
        ret << "Using debug log file:     " << DEBUGLOGFILE << "." << endl;
#endif
        if (num_threads > 1) {
            ret << "Max. concurrent threads:  " << num_threads << "." << endl;
        }
        ret << endl;
        ret << "Site title:         " << cfgs.cfg_sitetitle << endl;
        ret << "Site subtitle:      " << cfgs.cfg_subtitle << endl;
        ret << "Site URL:           " << cfgs.cfg_siteurl << endl;
        ret << "Incoming directory: " << cfgs.cfg_indir << endl;
        ret << "Outgoing directory: " << cfgs.cfg_outdir << endl;
#ifdef WITH_PLUGINS
        if (cfgs.cfg_plugins) {
            ret << "Plug-in directory:  " << cfgs.cfg_plugindir << endl;
        }
#endif
        ret << "Path for articles:  " << cfgs.cfg_article_subdir << endl;
        ret << "Path for pages:     " << cfgs.cfg_page_subdir << endl;
        ret << "Localization:       " << cfgs.cfg_locale << endl;
        ret << "HTML template:      " << cfgs.cfg_template << endl << endl;
    }


#ifdef WITH_DEBUGLOG
    DebugLog::resetDebuglog();
    stringstream ss_startlog;
    ss_startlog << "Processing started with " << num_threads << " threads." << endl;
    ss_startlog << "Output directory: " << cfgs.cfg_outdir << endl;
    DebugLog::debuglog(ss_startlog.str());
#endif

    // ------------------------------------------------
    // Walk through the Indir and fill the Outdir.
    // ------------------------------------------------

    ss_tpldir << "templates/" << cfgs.cfg_template << "/";
    ss_tpl_index << "./index.txt";
    ss_tpl_archive << "./archives.txt";
    ss_tpl_post << "./post.txt";
    ss_tpl_page << "./page.txt";

    ss_indir_posts << cfgs.cfg_indir << "/posts/";
    ss_indir_pages << cfgs.cfg_indir << "/pages/";
    ss_outdir << cfgs.cfg_outdir << "/";
    ss_feeddir << ss_outdir.str() << cfgs.cfg_feeddir << "/";

    if (!fs::exists(ss_outdir.str())) {
        // Just in case...
#ifdef WITH_DEBUGLOG
        ss_startlog.str("");
        ss_startlog << "Creating directory " << ss_outdir.str();
        DebugLog::debuglog(ss_startlog.str());
#endif
        fs::create_directories(ss_outdir.str());
    }
    else {
        // Cleanup.
#ifdef WITH_DEBUGLOG
        DebugLog::debuglog("Cleaning up your old files ...");
#endif
        for (fs::directory_iterator end_dir_it, it(ss_outdir.str()); it != end_dir_it; ++it) {
            try {
                fs::remove_all(it->path());
            }
            catch (fs::filesystem_error& e) {
                // Something went wrong here.
#ifdef WITH_DEBUGLOG
                DebugLog::debuglog("Error doing so. Aborting site generation.");
#endif
                ret << "Failed to clean up the existing output files: " << e.what() << endl;
                ret << "Aborting.";
                return ret.str();
            }
        }
    }

    // Traverse posts and pages.
    // We need to go through all files twice; first time to collect all cats/tags, second
    // time to be able to write them into the templates instantly.
    vector<string> vs_postfiles;
    vector<string> vs_pagefiles;

    for (fs::recursive_directory_iterator it(ss_indir_posts.str()), end; it != end; ++it) {
        if (!fs::is_directory(it->path())) { lock_guard<mutex> lock(mtx); vs_postfiles.push_back(it->path().filename().string()); }
    }

    for (fs::recursive_directory_iterator it(ss_indir_pages.str()), end; it != end; ++it) {
        if (!fs::is_directory(it->path())) { lock_guard<mutex> lock(mtx); vs_pagefiles.push_back(it->path().filename().string()); }
    }

    // Traverse through vs_postfiles to gather all post data.
    if (!silentmode) { ret << "Collecting all posts ..." << endl; }
    for (size_t i = 0; i < vs_postfiles.size();) {
        vector<std::thread> threadvec;
        for (size_t j = 0; j < num_threads && i < vs_postfiles.size(); j++) {
            threadvec.emplace_back(collectPostData, vs_postfiles[i++], false);
        }
        for (auto& tv : threadvec) {
            // Wait for all <num_threads> threads to finish before continuing:
            tv.join();
        }
    }

    // Traverse through vs_pagefiles to gather all page data:
    if (!silentmode) { ret << "Collecting all pages ..." << endl; }
    for (size_t i = 0; i < vs_pagefiles.size();) {
        vector<std::thread> threadvec;
        for (size_t j = 0; j < num_threads && i < vs_pagefiles.size(); j++) {
            threadvec.emplace_back(collectPostData, vs_pagefiles[i++], true);
        }
        for (auto& tv : threadvec) {
            // Wait for all <num_threads> threads to finish before continuing:
            tv.join();
        }
    }

    // Find all years and months:
    collectArchiveData();

    // The sidebar does not differ between contents. The other parts might.
    json header = renderHeader(config, "");
    json sidebar = renderSidebarArchives();

    // Merge:
    json extradata;
    extradata.update(header);
    extradata.update(sidebar);

    // For archivation, we should (at least) sort the vsi_posts vector so that the entry with
    // the newest creation date/time is the topmost one.
    sort(vsi_posts.begin(), vsi_posts.end(), [](SingleItem & a, SingleItem & b) {
        // Sort by time (descending).
        return mktime(&a.time) > mktime(&b.time);
    });

    // Now that our posts vector is sorted, we need to place the sticky posts in front of it.
    // This should not affect the order outside the Index page, so we'll better copy the posts
    // vector and only use the "sticky" field in a separate vector.
    vsi_posts_with_sticky = vsi_posts;
    stable_partition(vsi_posts_with_sticky.begin(), vsi_posts_with_sticky.end(), [](SingleItem & si) {
        // s_sticky == "on" or "On"? This post is sticky! :-)
        return lowercase(si.s_sticky) == "on";
    });

    // Create the archives.
    createTaxonomy(extradata);
    createTimedArchives(extradata);

    // Create the articles from vs_postfiles and the collected data as well as the pages:
    if (!silentmode) {
        ret << "Writing " << vs_postfiles.size() << " posts and " << vs_pagefiles.size() << " pages ..." << endl;
    }

    // Thread-wise processing:
    // Dig through the vectors vs_postfiles and vs_pagefiles. Split each one
    // of them into blocks of max. <num_threads> items and wait for the next
    // block until the vector is completely processed.
    for (size_t i = 0; i < vs_postfiles.size();) {
        // Articles:
        vector<std::thread> threadvec;
        for (size_t j = 0; j < num_threads && i < vs_postfiles.size(); j++) {
            threadvec.emplace_back(createArticleHTML, vs_postfiles[i++], config);
        }
        for (auto& tv : threadvec) {
            // Wait for all <num_threads> threads to finish before continuing:
            tv.join();
        }
    }

    for (size_t i = 0; i < vs_pagefiles.size();) {
        // Pages:
        vector<std::thread> threadvec;
        for (size_t j = 0; j < num_threads && i < vs_pagefiles.size(); j++) {
            threadvec.emplace_back(createPageHTML, vs_pagefiles[i++], config);
        }
        for (auto& tv : threadvec) {
            // Wait for all <num_threads> threads to finish before continuing:
            tv.join();
        }
    }

    // Create the RSS feed:
    RSSGenerator generator(ss_feeddir.str(), cfgs.cfg_sitetitle, "RSS.xml", cfgs);
    generator.createRSS(vsi_posts);

    // Finally, render the index page.
    renderIndex(extradata, ret);

    // ------------------------------------------------
    // Copy static/ files to the outdir/
    // ------------------------------------------------

    ss_static_in << ss_tpldir.str() << "static/";
    ss_static_out << ss_outdir.str() << "static/";

    try {
        fs::copy(ss_static_in.str(), ss_static_out.str(), fs::copy_options::recursive | fs::copy_options::update_existing);
        if (!silentmode) {
            ret << "Copied the template's static/ directory into " << ss_static_out.str() << "." << endl;
        }
    }
    catch (exception&) {
        // No static directory found or we can't just access it.
        if (!silentmode) {
            ret << "Could not copy the template's static/ directory into " << ss_static_out.str() << "." << endl;
            ret << "You can safely ignore this warning if your template doesn't have one." << endl;
        }
    }

    if (!silentmode) {
        ret << "Your site was created successfully." << endl << endl;
    }

#ifdef WITH_DEBUGLOG
    DebugLog::debuglog("Site processed successfully.");
#endif

    return ret.str();
}

int main(int argc, char* argv[]) {
    // Parse command-line options.
    cxxopts::Options options(argv[0], " - compile your blog.");
    silentmode = false;
    string configfile = "blog.cfg";

    try {
        options.add_options()
        ("h,help", "Prints this help.")
        ("v,version", "Prints version information.")
        ("q,quiet", "Quietly generates your site.")
        ("new-article", "Creates a new article with your $EDITOR.")
        ("new-page", "Creates a new page with your $EDITOR.")
#ifdef WITH_PLUGINS
        ("list-plugins", "Lists all available plug-ins in your plug-in directory.")
        ("enable-plugin", "Enables the given plug-in if disabled.", cxxopts::value<string>(), "name")
        ("disable-plugin", "Disables the given plug-in if enabled.", cxxopts::value<string>(), "name")
#endif
        ("config", "Uses a different config file than blog.cfg.", cxxopts::value<string>(), "configfile");

        auto roptions = options.parse(argc, argv);

        if (roptions.count("quiet")) {
            // The "silent mode" has to be checked first, of course.
            // Note that it's not "silent" anymore when a critical error occurs
            silentmode = true;
        }

        if (roptions.count("help")) {
            cout << options.help({""}) << endl;
            exit(0);
        }
        else if (roptions.count("version")) {
            printVersion();
            exit(0);
        }
        else if (roptions.count("config")) {
            configfile = roptions["config"].as<string>();
        }

        ConfigParser config(configfile);

        cfgs.cfg_author = "";
        config.get("author", cfgs.cfg_author);

        cfgs.cfg_indir = "site";
        config.get("indir", cfgs.cfg_indir);

#ifdef WITH_PLUGINS
        string s_use_plugins  = "On";
        config.get("plugins", s_use_plugins);
        cfgs.cfg_plugins = lowercase(s_use_plugins) == "on";

        cfgs.cfg_plugindir = "plugins";
        config.get("plugindir", cfgs.cfg_plugindir);

        // Initialize the plug-ins.
        plugins.init(cfgs.cfg_plugins, cfgs.cfg_plugindir);
#endif

        if (roptions.count("new-article")) {
            string title;
            cout << "Looks like you're planning to start a new blog article." << endl;
            while (title.empty()) {
                cout << "Please type the prospected title to start, press Ctrl+C to abort: ";
                getline(cin, title);
            }
            cout << "The magic begins." << endl;

            title = trim(title);
            ContentsGenerator generator(Article, cfgs.cfg_indir, cfgs.cfg_author, title, createBasicSlug(title.c_str()));
            if (!generator.generate()) {
                // The ContentsGenerator has probably already written a useful information message.
                cout << "We could not continue. Sorry for that!" << endl;
                exit(0);
            }

            cout << "Your blog article has been created in '" << cfgs.cfg_indir << "/posts/'." << endl;
            cout << "Run " << fs::path(argv[0]).filename() << " again to generate your updated website." << endl;
            exit(0);
        }
        else if (roptions.count("new-page")) {
            string title;
            cout << "Looks like you're planning to start a new page." << endl;
            while (title.empty()) {
                cout << "Please type the prospected title to start, press Ctrl+C to abort: ";
                getline(cin, title);
            }
            cout << "The magic begins." << endl;

            title = trim(title);
            ContentsGenerator generator(Page, cfgs.cfg_indir, cfgs.cfg_author, title, createBasicSlug(title.c_str()));
            if (!generator.generate()) {
                // The ContentsGenerator has probably already written a useful information message.
                cout << "We could not continue. Sorry for that!" << endl;
                exit(0);
            }

            cout << "Your page has been created in '" << cfgs.cfg_indir << "/pages/'." << endl;
            cout << "Run " << fs::path(argv[0]).filename() << " again to generate your updated website." << endl;
            exit(0);
        }
#ifdef WITH_PLUGINS
        else if (roptions.count("list-plugins")) {
            cout << plugins.listAll();
            exit(0);
        }
        else if (roptions.count("enable-plugin")) {
            cout << plugins.enable(roptions["enable-plugin"].as<string>());
            exit(0);
        }
        else if (roptions.count("disable-plugin")) {
            cout << plugins.disable(roptions["disable-plugin"].as<string>());
            exit(0);
        }
#endif

        // It is possible that the user ignored the README, not having a working
        // blog.cfg file. So we should try{} it.
        cout << process(configfile);
    }
    catch (const cxxopts::OptionException&) {
        // The user gave us invalid options. :-(
        cout << "Sorry, this combination of options is not supported as of now." << endl << endl;
        cout << options.help({""}) << endl;
        exit(1);
    }
    catch (const std::exception& ex) {
        cout << "Sorry: " << ex.what() << endl;
        exit(1);
    }
    catch (...) {
        cout << "Sorry: Unhandled exception" << endl;
        exit(1);
    }

    exit(0);
}
