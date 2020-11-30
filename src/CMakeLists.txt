############################################################################
# Options
############################################################################
OPTION(WITH_DEBUG_LOG "Write a debug log" OFF)
if (WITH_DEBUG_LOG)
	ADD_DEFINITIONS(-DWITH_DEBUGLOG)
endif (WITH_DEBUG_LOG)

OPTION(WITH_PLUGINS "Compile with plugin support" ON)
IF (WITH_PLUGINS)
	ADD_DEFINITIONS(-DWITH_PLUGINS)

        find_package(duktape)
	INCLUDE_DIRECTORIES(${duktape_INCLUDE_DIRS})
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3rdparty/duktape/dukglue/include)

        LINK_DIRECTORIES(${duktape_LIB_DIRS})
        SET(LIBS ${LIBS} ${duktape_LIBRARIES})
ENDIF (WITH_PLUGINS)

OPTION(WITH_OEMBED "Compile with oEmbed support" OFF)
if (WITH_OEMBED)
	ADD_DEFINITIONS(-DWITH_OEMBED)

	find_package(CURL)
	IF(NOT ${CURL_FOUND})
		MESSAGE(FATAL_ERROR "CURL not found: Set in in the ADVANCED SETTINGS")
	ENDIF(NOT ${CURL_FOUND})
	INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIRS})

	SET(TP_SRCS ${TP_SRCS}
		../3rdparty/HTTPDownloader.cpp
	)
	
	SET(LIBS ${LIBS}
		${CURL_LIBRARY}
	)

ENDIF(WITH_OEMBED)

IF (${FREEBSD} OR ${DARWIN})
  # FreeBSD/Darwin could use /usr/local/
  INCLUDE_DIRECTORIES(/usr/local/include)
  LINK_DIRECTORIES(/usr/local/lib)
ENDIF(${FREEBSD} OR ${DARWIN})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3rdparty)

############################################################################
# third-party sources
############################################################################

find_package(inja)
find_package(nlohmann_json)
find_package(ICU)
find_package(tinyxml2)
find_package(cxxopts)

INCLUDE_DIRECTORIES(${pantor_inja_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${nlohmann_json_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${ICU_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${tinyxml2_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${cxxopts_INCLUDE_DIRS})

LINK_DIRECTORIES(${ICU_LIB_DIRS})
LINK_DIRECTORIES(${tinyxml2_LIB_DIRS})

SET(LIBS ${LIBS} ${ICU_LIBRARIES})
SET(LIBS ${LIBS} ${tinyxml2_LIBRARIES})

############################################################################
# blogcpp sources
############################################################################

SET(BLOGCPP_SRCS
    DebugClass.cpp
    ContentsGeneratorClass.cpp
    EmojiParserClass.cpp
    ExcerptExtractorClass.cpp
    MarkdownParserClass.cpp
    MediaEmbedderClass.cpp
    CommentIntegrationClass.cpp
    PluginClass.cpp
    RSSGeneratorClass.cpp
    MarkdownWrapperClass.cpp
    TemplateWrapperClass.cpp
    blogcpp.cpp
)

ADD_EXECUTABLE(blogcpp ${BLOGCPP_SRCS})

############################################################################
# Target specific libraries
############################################################################

IF(UNIX)
    OPTION(USE_CLANG_STDCPP_LIB "Use libc++" ON)
ENDIF(UNIX)

IF (FREEBSD OR DARWIN)
    SET(LIBS    ${LIBS}
        icui18n 
        icuuc
    )
    IF (FREEBSD)
       SET(LIBS ${LIBS}
           thr
       )
    ELSE (FREEBSD)
     SET(LIBS ${LIBS}
           pthread
       )
    ENDIF (FREEBSD)
    IF (USE_CLANG_STDCPP_LIB)
        SET(LIBS    ${LIBS}
            c++
        )
    ELSE(USE_CLANG_STDCPP_LIB)
        SET(LIBS    ${LIBS}
            stdc++
        )
    ENDIF (USE_CLANG_STDCPP_LIB)
ENDIF (FREEBSD OR DARWIN)

IF (${LINUX})
    SET(LIBS    ${LIBS}
        icui18n 
        icuuc 
        stdc++ 
        stdc++fs 
        pthread
    )    
ENDIF (${LINUX})

IF (WIN32)
    SET(LIBS    ${LIBS}
        icuin 
        icuuc
    )    
ENDIF (WIN32)

target_link_libraries(blogcpp ${LIBS})
