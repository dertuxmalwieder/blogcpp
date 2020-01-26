@echo off

REM ------------------------------------------
REM This cmd script links blogcpp STATICALLY!
REM The resulting executable file is absurdly
REM large. But it won't require separate .dll
REM files anymore... :-)
REM ------------------------------------------
REM Usage:
REM    .\build_windows.cmd [params]
REM
REM Parameters:
REM    NOCLANG: uses MSVC instead of clang-cl
REM    RELEASE: compiles without DEBUGLOG
REM ------------------------------------------

REM Beautify code
if exist tools\astyle.exe ( tools\astyle --style=java --keep-one-line-blocks --keep-one-line-statements --add-brackets --break-closing-brackets --indent-modifiers --indent-switches --pad-oper --pad-header --unpad-paren --lineend=linux --recursive --quiet --suffix=none --exclude=3rdparty/duktape --exclude=3rdparty/XML --exclude=3rdparty/json *.cpp *.h *.hpp )

REM Set defaults (DEBUGPARAM and COMPILER can be overwritten through parameters)
@set PREPROCESSOR=/DWITH_OEMBED /DCURL_STATICLIB /DU_STATIC_IMPLEMENTATION /DWITH_PLUGINS
@set DEBUGPARAM=/DWITH_DEBUGLOG
@set COMPILER=clang-cl -m32
@set OUTPUTBIN=bin\blogcpp.exe
@set INFILES=3rdparty/XML/tinyxml2.cpp 3rdparty/curl/HTTPDownloader.cpp 3rdparty/duktape/duktape/src/duktape.c src\DebugClass.cpp src\MediaEmbedderClass.cpp src\ContentsGeneratorClass.cpp src\CommentIntegrationClass.cpp src\MarkdownParserClass.cpp src\EmojiParserClass.cpp src\ExcerptExtractorClass.cpp src\PluginClass.cpp src\RSSGeneratorClass.cpp src\MarkdownWrapperClass.cpp src\TemplateWrapperClass.cpp src\blogcpp.cpp
@set LINKLIBS=libcurl_a.lib sicuin.lib sicuio.lib sicuuc.lib sicudt.lib shell32.lib user32.lib


REM ------------------------------------------
REM You are not advised to modify anything
REM below this comment.
REM ------------------------------------------


REM Parse the command-line parameters
:parse
IF "%~1"=="" GOTO endparse
IF "%~1"=="NOCLANG" GOTO noclang
IF "%~1"=="RELEASE" GOTO nodebug
:noclang
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86
@set COMPILER="cl"
SHIFT
GOTO parse
:nodebug
@set DEBUGPARAM=""
SHIFT
GOTO parse
SHIFT
GOTO parse
:endparse

REM Compile
%COMPILER% /W4 %DEBUGPARAM% %PREPROCESSOR% /Ox /MT /GS- /EHsc /Fe%OUTPUTBIN% %INFILES% /I"3rdparty" /I"3rdparty\json\single_include" /I"3rdparty\inja\single_include\inja" /I"3rdparty\icu\include" /I"3rdparty\curl\curl\include" /I "3rdparty\duktape\dukglue\include" /I"3rdparty\duktape\duktape\src" /link /libpath:3rdparty\curl\curl\builds\libcurl-vc14-x86-release-static-ipv6-sspi-winssl-obj-lib /libpath:3rdparty\icu\lib %LINKLIBS% /NODEFAULTLIB:msvcrt.lib /SAFESEH /DYNAMICBASE

REM Cleanup
if exist bin\*.exp del bin\*.exp
if exist bin\*.lib del bin\*.lib
