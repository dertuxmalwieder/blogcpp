# blogcpp

A static blog generator, written in C++17.

First things first: How can I get it?
=====================================

1.  You can use the [Fossil](https://www.fossil-scm.org) client:
    `fossil clone https://code.rosaelefanten.org/blogcpp`.
2.  You can use a Git client of your choice to download from the GitHub
    mirror: `git clone https://github.com/dertuxmalwieder/blogcpp`.

Why?
====

Because there are not enough [static site
generators](http://www.staticsitegenerators.net) yet.

No, seriously, why?
===================

-   Just like other static site generators, `blogcpp` is virtually
    unbreakable by intruders as your server is not required to host
    anything but static HTML files - even JavaScript and CSS are,
    depending on your theme, entirely optional.
-   Unlike other static site generators, `blogcpp` is written in modern
    C++, resulting in a fast build speed, reducing the need for
    workarounds like *incremental builds* or advanced caching mechanisms
    while compiling. Still, `blogcpp` tries to use as many CPU cores as
    possible to achieve the maximum speed on your machine.
-   `blogcpp` aims to provide a decent feature set without requiring too
    much configuration; in fact, it would work right out of the box if
    your website was *example.com* (which is, admittedly, rather
    unlikely).

Current version
===============

The latest released version is version 9.

Features
========

-   Markdown support: Write your articles and pages in Markdown, get
    HTML.
-   Theming support: You don\'t like the available themes? Well, nor do
    I. Go on and make a better one!
-   OEmbed support: Just post an OEmbed-enabled link (e.g. Twitter or
    YouTube) and `blogcpp` will embed the media for you.
-   RSS feeds for new articles, tags and categories.
-   Configurable article permalinks.
-   Sticky articles: Just put `Sticky: On` on top of your article file.
-   Emoji support: Write `:-)` and get a *real* smiling face instead.
-   Commenting support: Even a static blog needs a community. ;-)
-   OpenGraph support if you feel like sharing stuff on Facebook or
    whatever.
-   Syntax highlighting via [highlight.js](https://highlightjs.org/).
-   Plug-in support: If anything\'s missing, just write a script.
-   Article series: If you have articles which continue other articles,
    you can just add them to the same series and `blogcpp` will make
    them refer to each other.
-   much more is planned - see [the
    roadmap](https://blogcpp.org/pages/roadmap) for a clue.

Used libraries
==============

`blogcpp` uses the following third-party libraries:

-   [cxxopts](https://github.com/jarro2783/cxxopts)
-   [inja](https://github.com/pantor/inja)
-   [json](https://github.com/nlohmann/json)
-   [TinyXML2](https://github.com/leethomason/tinyxml2)
-   [ConfigParser](https://github.com/underww/ConfigParser)
-   [dukglue](https://github.com/Aloshi/dukglue/) with
    [duktape](http://duktape.org/)
-   [icu](http://icu-project.org/)
-   [libcurl](http://curl.haxx.se) if compiled with `WITH_OEMBED`

Some code was added from other people\'s suggestions:

-   The HTTPDownloader class is taken from [this blog
    article](https://techoverflow.net/blog/2013/03/15/c-simple-http-download-using-libcurl-easy-api/).

FAQ
===

How can I use `blogcpp`?
------------------------

### The easier way:

Set up your `blog.cfg` file first, follow the documentation in the
`blog.cfg-example` file in this repository. Then type
`blogcpp --new-article` to start a new article or `blogcpp --new-page`
to start a new page. When you\'re done, run `blogcpp` and everything
will be there soon.

### The nerd way:

Set up your `blog.cfg` file first, follow the documentation in the
`blog.cfg-example` file in this repository. Then start to add
Markdown-formatted posts (in `/posts`) and/or pages (in `/pages`)
formatted like this:

    Author: Your name here.
    Date: 2016-01-01 12:00:00
    Title: This is the title.
    Tags: tag 1;tag 2;tag 3
    Categories: some;random;categories

    ----------

    (Everything that follows will be a part of the contents.)

When you\'re done, run `blogcpp` and everything will be there soon.

Which meta data types are allowed?
----------------------------------

You mean, except the Title/Tags/Date part? Well:

-   You can additionally set `Markdown: Off` to disable Markdown parsing
    for this specific post or page. Note that pages don\'t have
    categories, every category added to a page will be ignored.
-   If you want your newly created article (for logical reasons, only
    posts are supported) to be \"sticked\" to the top of your index
    page, just add `Sticky: On` into its header part.
-   Articles will always be sorted by their *creation date*; you can
    indicate the *latest change* (which will be displayed or not,
    depending on your theme) with `Changed: yyyy-mm-dd hh:mm:ss`
    somewhere above or below the `Date:` field.
-   If you don\'t want to have emojis on this post or page without
    removing them everywhere, just add `Emojis: Off`.
-   In case you want to have a specific OpenGraph image for this page or
    article, you can set `OpenGraphImage: http://my/host/image.png` or
    whatever you want it to show.
-   You can also disable comments per post (pages don\'t have comments):
    `Comments: Off`.
-   If you use Content plug-ins but you want to disable them for this
    item exclusively, you can set `Plugins: Off`.
-   If you want to add an article to a certain series, it is enough to
    set `Series: Name Of Your Series` here. Note that currently only one
    series is supported per article.
-   Pages are usually sorted alphabetically. If you don\'t want to do
    that, you can set the `Position` parameter with any number larger
    than 0. Note that the \"start page\" is not a real page, so the
    template decides where to put it.

OK, but how can I compile `blogcpp` first?
------------------------------------------

If you\'re on Windows, you can just [get a static
binary](http://blogcpp.org/pages/download). There are also [precompiled
builds for Debian
GNU/Linux](https://blog.mdosch.de/2017/08/26/blogc++-in-debian-benutzen/)
which have, sadly, not been updated in a while. It\'s not hard to
compile `blogcpp` yourself though:

Starting from version 3, `CMake` files are provided, tested on GNU/Linux
and FreeBSD systems. (On other systems you might have to adjust the
paths.) You\'ll need a C++17 compiler, `blogcpp` makes heavy use of the
`<filesystem>` header and other oddities. You will need
[Conan](https://conan.io) installed to fetch some dependencies.

    % mkdir build
    % cd build
    % cmake .. ; cmake --build .

`blogcpp` has been proven to compile on:

-   Windows 10 with Visual Studio 2017.
-   Windows 10 with Clang 6.0.1.
-   Debian GNU/Linux *sid* with `g++` 6.3.0.
-   macOS 11 with the built-in Clang.

Optional preprocessor definitions while linking:

-   `WITH_DEBUGLOG`: Prints verbose debug output to `/debuglog.txt`.
-   `WITH_PLUGINS`: Actually supports loading and evaluating plug-ins.
-   `WITH_OEMBED`: `blogcpp` supports the OEmbed standard. This will
    \- in some cases - affect the page generation speed. (If not used,
    `blogcpp` supports simple replacements for YouTube and Vimeo only.)

Which directories need to be there?
-----------------------------------

Here\'s a site\'s minimal directory structure:

    ./blogcpp
    ./templates/TPLNAME/
    ./INDIR/posts/
    ./INDIR/pages/
    ./OUTDIR/

Of course, the capital letters mean that the values are indeed
variables. By default, `TPLNAME` is `default`, `INDIR` is `site` and
`OUTDIR` is `outdir`. Please use the configuration file when building
your site in order to change these values.

The name is stupid!
-------------------

Well, I am a developer, not an economist. I do software, not shiny
product names. However, `blogcpp` is path-agnostic (as long as it finds
its usual path structure). You think `blogcpp` is a stupid name?
`mv blogcpp klaus` and blog with `klaus`!

Also, please consider to ask a real question next time.

How does syntax highlighting work?
----------------------------------

Four spaces at the beginning of a line mark the beginning of a code
block. `blogcpp` tries to determine the language of the code
automatically. If you want to set the language explicitly, you can use
the [Pelican](http://getpelican.com) syntax for code blocks; example:

    ::::python
    print("This is Python code.")

Which parameters are supported?
-------------------------------

You can call `blogcpp` with the following parameters:

-   `-h` / `--help`: Prints this list of parameters.
-   `-v` / `--version`: Prints the version number of `blogcpp`.
-   `-q` / `--quiet`: Generates your site and shuts up while doing so.
-   `--new-article`: Generates a new article stub and opens it in your
    default editor.
-   `--new-page`: Generates a new page stub and opens it in your default
    editor.
-   `--config [filename]`: Uses a different configuration file than
    blog.cfg. The file must exist.

If compiled with `WITH_PLUGINS`. three more parameters are supported:

-   `--list-plugins`: Shows a list of all available plug-ins with their
    enabled/disabled status.
-   `--enable-plugin [name]`: Enables the given plug-in (if found).
-   `--disable-plugin [name]`: Disables the given plug-in (if found).

Which emojis are supported?
---------------------------

Given that you have *actually enabled* Emoji support in your `blog.cfg`
file and not disabled it for the page or article you want to process,
the following smiley codes will automatically be replaced:

  ----------------------
  Code         Emoji
  ------------ ---------
  `:-)`        ☺

  `;-)`        😉

  `:-D`        😀

  `:-(`        ☹

  `:'(`        😭

  `:-|`        😐

  `>:)`        😈

  `>:-)`       😈

  `>:(`        😠

  `>:-(`       😠

  `:-*`        😘

  `:-O`        😮

  `:-o`        😮

  `:-S`        😕

  `:-s`        😕

  `:-#`        😶

  `0:-)`       😇

  `:o)`        🤡

  `<_<`        😒

  `^^`         😊

  `^_^`        😊

  `<3`         ❤

  `m(`         🤦
  ----------------------

Which comment systems does `blogcpp` support?
---------------------------------------------

While `blogcpp` does not have its own commenting system, you can easily
integrate existing external commenting systems via your `blog.cfg`
(please refer to the `blog.cfg-example` file in this repository).
Currently supported are:

-   [Disqus](http://www.disqus.com) (`comments = disqus`, you need to
    set your `commentdata` to your Disqus shortname)
-   [isso](http://posativ.org/isso) (`comments = isso`, you need to set
    your `commentdata` to your *isso* folder)
-   [Hashover](https://github.com/jacobwb/hashover-next)
    (`comments = hashover`, you need to set your `commentdata` to your
    *Hashover* folder) \-- currently, version 2.0 only
-   Google Plus comments (`comments = google`, no additional settings
    required)
-   [Discourse](http://discourse.org) (`comments = discourse`, you need
    to set your `commentdata` to your *Discourse* domain)
-   [Commento](https://gitlab.com/commento/commento-ce)
    (`comments = commento`, you need to set your `commentdata` to your
    *Commento* server URI)
-   [Hyvor Talk](https://talk.hyvor.com) (`comments = hyvortalk`, you
    need to set your `commentdata` to your website ID)

How can I write a `blogcpp` plug-in? Is it hard?
------------------------------------------------

Good news: `blogcpp` *plug-ins* are actually JavaScript scripts, meaning
that it is rather easy to write one. *Everyone* can write JavaScript
today, right?

The plug-in mechanisms are still in an early phase, they will probably
be more extended in later releases. As of now, `blogcpp` only supports
three kinds of plug-ins:

-   **Content plug-ins:** Will be searched under `<plugindir>/contents`.
    `blogcpp` will draw your contents (articles *and* pages) through all
    available scripts and spit out the results. Note that this happens
    during the initial reading stadium, so your scripts will see the raw
    Markdown text. The `process()` method is expected to take and return
    a string value.
-   **Header plug-ins:** Will be searched under `<plugindir>/header`.
    The `process()` method is expected to return a string value which
    will be attached to the `{{ headscripts }}` template variable (if
    applicable).
-   **Footer plug-ins**: Similar to header plug-ins, but they should be
    in `<plugindir>/footer` and fill the `{{ footscripts }}` variable.

The standard naming scheme is *something.plugin.js*, `blogcpp` will
gladly ignore any files which do *not* have a name like this. You are
also able to have a multi-part plug-in which affects multiple parts of
the blog: Plug-ins with an identical file name are considered to belong
together, `blogcpp` will handle them as one big plug-in which affects
various parts of the software, e.g. `<plugindir>/contents/moo.plugin.js`
for moo\'ing in the contents part, `<plugindir>/header/moo.plugin.js`
for moo\'ing in the site header.

In order for this to work, you\'ll need to have `blogcpp` compiled with
`WITH_PLUGINS` and the configuration variable `plugins` set to `on`. If
you did so, `blogcpp` will read all files in the particular folder under
your plug-in directory. If they are - more or less - valid JavaScript
(ECMAScript 5.1 is mostly supported), `blogcpp` will try to find the
`process()` method in them, fill it with the appropriate text and
replace it by the results of the `process()` method before continuing as
usual.

Plug-ins can also access certain parts of `blogcpp` itself from the
`BlogEngine` object. By the time of writing, those are the following
one:

-   `getVersion()`: Returns the current version of `blogcpp` as a
    number.

Please refer to the `hello-world.plugin.js` example plug-in in this
repository for more or less information.

Can I use raw HTML in my Markdown-enabled article or page?
----------------------------------------------------------

Yes, you can! Everything between `<rawhtml>` and `</rawhtml>` will be
ignored by `blogcpp`\'s Markdown parser.

Which files does a template need to work?
-----------------------------------------

`blogcpp` needs `index.txt`, `post.txt`, `page.txt` and `archives.txt`
in order to be able to process your site correctly. All other template
files are optional. CSS and image files can be put into a subfolder
named `static`, `blogcpp` will automatically copy this folder to your
output directory then.

Starting with version 9, `blogcpp`\'s templates are *mostly* compatible
with the [Jinja2](http://jinja.pocoo.org/) syntax, so porting your
existing Python themes should be rather easy.
