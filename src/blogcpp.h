/*
 * blogcpp :: https://www.blogcpp.org
 * Main non-class (still much too long) [header].
 */

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cfenv>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <regex>
#include <locale>
#include <iomanip>
#include <thread>
#include <mutex>

// blogcpp's own auxiliary classes:
#include "SingleItemClass.h"
#include "MarkdownParserClass.h"
#include "MarkdownWrapperClass.h"
#include "EmojiParserClass.h"
#include "ExcerptExtractorClass.h"
#include "ContentsGeneratorClass.h"
#include "CommentIntegrationClass.h"
#include "RSSGeneratorClass.h"
#include "MarkdownReturnClass.h"
#include "ConfigCollectionClass.h"
#include "TemplateWrapperClass.h"
#include "filesystem.h"
#ifdef WITH_PLUGINS
#  include "PluginClass.h"
#endif
#include "constants.h"
#include "helpers.h"
#ifdef WITH_DEBUGLOG
#  include "DebugClass.h"
#endif

// 3rd party libraries:
#include <cxxopts.hpp>
#include <ConfigParser/ConfigParser.cpp>
#include <nlohmann/json.hpp>
#include <inja/inja.hpp>

// ICU:
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <unicode/translit.h>

using namespace std;
using namespace icu;

using json = nlohmann::json;

namespace fs = NAMESPACE_FILESYSTEM;


// --- ENUMS ---

enum Taxonomies {
    Categories,
    Tags,
    Series,
    TimedArchives, // yearly, monthly, ...
    AuthorArchives
};


// --- PROTOTYPES ---

int main(int argc, char* argv[]);

void   printVersion();
string process(char* configfile);
string createPaginatorHTML(int total_pages, int current_page);

// Functions that collect data:
void collectPostData(string inputfile, bool is_page);
void collectArchiveData();
json renderHeader(ConfigParser config, string extratitle);
json renderSidebarArchives();

// Functions that directly write into templates:
void renderIndex(json extradata, stringstream &ss_ret);
void createPageOrPost(string inputfile, ConfigParser config, bool is_page);
void createTaxonomyHTML(json extradata, Taxonomies taxonomytype, string taxonomy, vector<SingleItem> items);

// Wrappers:
void createTaxonomy(json extradata);
void createTimedArchives(json extradata);


inline void createArticleHTML(string inputfile, ConfigParser config) {
    // Alias: Create an article.
    createPageOrPost(inputfile, config, false);
}

inline void createPageHTML(string inputfile, ConfigParser config) {
    // Alias: Create a page.
    createPageOrPost(inputfile, config, true);
}

string createSlug(char const * title, tm tm_t);
string createBasicSlug(char const * input);

string dateToPrint(tm tm_t, bool shortdate = false);
int    getSuffixID(string init_slug, tm tm_t);

inline int calculateTotalPages(vector<SingleItem> vec, int maxitems) {
    return static_cast<int>(ceil(static_cast<float>(vec.size()) / maxitems));
}


// --- GLOBALS ---

ConfigCollection cfgs;
TemplateWrapper injatpl;

stringstream ss_indir_posts;
stringstream ss_indir_pages;
stringstream ss_outdir;
stringstream ss_tpldir;
stringstream ss_tpl_index;
stringstream ss_tpl_post;
stringstream ss_tpl_page;
stringstream ss_tpl_archive;
stringstream ss_static_in;
stringstream ss_static_out;
stringstream ss_feeddir;

bool silentmode;

mutex mtx;
unsigned int num_threads;

#ifdef WITH_PLUGINS
Plugins plugins;
#endif

multimap<string, SingleItem> map_series_items;

vector<SingleItem> vsi_pages;
vector<SingleItem> vsi_posts;
vector<SingleItem> vsi_posts_with_sticky;

vector<string> vs_slugs;
vector<string> vs_cats;
vector<string> vs_tags;
vector<string> vs_authors;
vector<string> vs_series;
vector<string> vs_archive_months;
vector<string> vs_archive_years;

vector<vector<SingleItem>> vec_years;
vector<vector<SingleItem>> vec_months;
