/*
 * blogcpp :: https://www.blogcpp.org
 * Generator class for new posts and pages.
 */

#include "ContentsGeneratorClass.h"

ContentsGenerator::ContentsGenerator(ContentType type, std::string input_folder, std::string author, std::string title, std::string slug) {
    // A new ContentsGenerator has a ContentType passed.
    this->type = type;
    this->input_folder = input_folder;
    this->author = author;
    this->title = title;
    this->slug = slug;
}


bool ContentsGenerator::generate() {
    // Creates a new article or page file, fills the necessary
    // variables and opens it in the system's default text editor.
    EmojiParser eparser;
    std::stringstream contents, filename;

    switch (type) {
        case Article:
            filename << input_folder << "/posts/" << timeNow("%Y-%m-%d") << " " << trim(genFileName(eparser.clear(title))) << ".txt";
            contents << createDummyArticle();
            break;
        case Page:
            filename << input_folder << "/pages/" << trim(genFileName(eparser.clear(title))) << ".txt";
            contents << createDummyPage();
            break;
    }

    // If this file already exists, we shall not pass.
    if (fs::exists(filename.str())) {
        std::cout << "Duplicate contents found - you might want to choose a new title first." << endl;
        return false;
    }

    // Write into the file. (Will return false on error.)
    std::ofstream os_file(filename.str());
    if (os_file.is_open()) {
#ifdef WITH_DEBUGLOG
        std::stringstream ss_debuglog;
        ss_debuglog << "Writing to the file '" << filename.str() << "'.";
        DebugLog::debuglog(ss_debuglog.str());
#endif
        os_file << contents.str();
        os_file.close();
    }
    else {
        std::cout << "Whoops - I couldn't write into " << filename.str() << "." << endl;
        return false;
    }

#ifdef WITH_DEBUGLOG
    DebugLog::debuglog("File successfully created.");
#endif

    std::cout << "Your stub file '" << filename.str() << "' has been generated." << endl;
    std::cout << "Trying to launch your default editor with it..." << endl;

    return openWithEditor(fs::absolute(filename.str()).string().c_str());
}


std::string ContentsGenerator::createDummyPage() {
    // Returns the dummy text for a new page.
    std::ostringstream ret;

    ret << "Author: " << author << NEWLINE;
    ret << "Title: " << title << NEWLINE;
    ret << "Slug: " << slug << NEWLINE;
    ret << "Date: " << timeNow("%Y-%m-%d %H:%M:%S") << NEWLINE;
    ret << "Tags: " << NEWLINE;
    ret << NEWLINE;
    ret << HEADER_DIVIDER << NEWLINE;
    ret << NEWLINE;

    return ret.str();
}


std::string ContentsGenerator::createDummyArticle() {
    // Returns the dummy text for a new page.
    std::ostringstream ret;

    ret << "Author: " << author << NEWLINE;
    ret << "Title: " << title << NEWLINE;
    ret << "Slug: " << slug << NEWLINE;
    ret << "Date: " << timeNow("%Y-%m-%d %H:%M:%S") << NEWLINE;
    ret << "Tags: " << NEWLINE;
    ret << "Categories: " << NEWLINE;
    ret << NEWLINE;
    ret << HEADER_DIVIDER << NEWLINE;
    ret << NEWLINE;

    return ret.str();
}
