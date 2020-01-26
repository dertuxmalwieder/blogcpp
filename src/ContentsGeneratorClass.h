/*
 * blogcpp :: https://www.blogcpp.org
 * Generator class for new posts and pages [header].
 */

#include <string>
#include <sstream>
#include <fstream>

#include "filesystem.h"
#include "constants.h"
#include "helpers.h"
#include "EmojiParserClass.h"
#ifdef WITH_DEBUGLOG
#include "DebugClass.h"
#endif

namespace fs = NAMESPACE_FILESYSTEM;

enum ContentType {
    Article,
    Page
};

class ContentsGenerator {
  public:
    ContentsGenerator(ContentType type, std::string input_folder, std::string author, std::string title, std::string slug);

    bool generate();

  private:
    ContentType type;

    std::string createDummyPage();
    std::string createDummyArticle();

    std::string input_folder;
    std::string author;
    std::string title;
    std::string slug;
};
