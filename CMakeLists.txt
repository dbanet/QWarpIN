SET (PROJECT QWarpIN)

CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
CMAKE_POLICY (VERSION 2.8)
IF (CMAKE_MAJOR_VERSION GREATER 2)
    CMAKE_POLICY (SET CMP0037 OLD)
ENDIF ()

ADD_DEFINITIONS (-DQWP_VERSION="0.2")
PROJECT (${PROJECT})

#######################################
##           FILES SECTION           ##
#######################################

# the base name of the main binary (executable) file
SET (BINARY_NAME wpi)

# where the program will be built to, and
# where the files from ${BUILD_ROOT_SOURCE_DIR}
# will be copied to. Located in ${CMAKE_BINARY_DIR}
SET (BUILD_ROOT
    out)

# this directory's contents will be copied to the program's
# build root. Files that the program will need at runtime
# should be placed there. Located in ${CMAKE_SOURCE_DIR}
SET (BUILD_ROOT_SOURCE_DIR
    bld)

SET (HEADERS
    src/WPI.h
    #src/WCmdLineParser.h
    #src/globals.h
    src/ArchiveInterfaces/WAbstractArchiveInterface.h
    #src/ArchiveInterfaces/index.h
    #src/ArchiveInterfaces/WarpIN/WarpINArchiveInterface.h
    #src/ArchiveInterfaces/WarpIN/WAIFileReader.h
    #src/ArchiveInterfaces/WarpIN/WAIScriptParser/WAIScriptParser.h
    #src/ArchiveInterfaces/WarpIN/WAIScriptParser/Parser.h
    #src/ArchiveInterfaces/WarpIN/WAIScriptParser/Parserbase.h
    #src/ArchiveInterfaces/WarpIN/WAIScriptParser/Scanner.h
    #src/ArchiveInterfaces/WarpIN/WAIScriptParser/Scannerbase.h
    #src/GenericObjectFactory.h
    #src/exceptions.h
    src/WArchive.h
    src/WFileSystem/WFileSystemTree.h
    #src/WFileSystem/WFileSystem.h
    src/WFileSystem/WFileSystemNode.h
    src/WFileSystem/WFile.h
    src/WScriptHost/WScriptHost.h)

SET (SOURCES
    src/globals.cpp
    src/exceptions.cpp
    src/WScriptHost/WScriptHost.cpp
    src/ArchiveInterfaces/index.cpp
    src/ArchiveInterfaces/WAbstractArchiveInterface.cpp
    src/ArchiveInterfaces/WarpIN/WarpINArchiveInterface.cpp
    src/ArchiveInterfaces/WarpIN/WAIFileReader.cpp
    src/ArchiveInterfaces/WarpIN/WAIScriptParser/WAIScriptParser.cpp
    src/WFileSystem/WFileSystemTree.cpp
    src/WFileSystem/WFileSystemNode.cpp
    src/WFileSystem/WFile.cpp
    src/WArchive.cpp
    src/WCmdLineParser.cpp
    src/WPI.cpp
    src/main.cpp)

SET (FORMS src/WPI.ui)

SET (RESOURCES
    src/WPI.qrc
    src/WScriptHost/script.qrc)

#######################################
##         UTILITY FUNCTIONS         ##
#######################################
FUNCTION (APPEND VAR)
    SET (${VAR} ${${VAR}} ${ARGN} PARENT_SCOPE)
ENDFUNCTION (APPEND)

#######################################
##        PREPARE BUILD TREE         ##
#######################################

ADD_CUSTOM_TARGET (
    bld
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/${BUILD_ROOT})

ADD_CUSTOM_COMMAND (
    TARGET bld
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/${BUILD_ROOT_SOURCE_DIR} ${CMAKE_BINARY_DIR}/${BUILD_ROOT})

#######################################
##            MISCELLANEOUS          ##
#######################################

# lots of source and intermediate files are being generated in the
# ${CMAKE_BEINARY_DIR} directory (out-of-source build root), like
# the output of uic, moc, flexc++, bisonc++, etc, so making them
# available for inclusion immediately
INCLUDE_DIRECTORIES (${CMAKE_BINARY_DIR})

# adding the source root to the include directories also to ease
# access to such misc things as globals.h, GenericObjectFactory.h, etc
INCLUDE_DIRECTORIES (${CMAKE_SOURCE_DIR}/src)

# turning on C++14 standart support, activating more warnings
ADD_COMPILE_OPTIONS (-std=c++14 -O0 -Wno-literal-suffix -Wno-unused-variable -Wno-unused-local-typedefs)

IF (OS2)
    # for OS/2, turning on highmem support
    ADD_COMPILE_OPTIONS (-Zhigh-mem)
    # and linking against stdcpp6.dll
    APPEND (LIBRARIES stdc++)
    # and fixing search
    SET (CMAKE_PREFIX_PATH "/@unixroot/usr")
    SET (CMAKE_FIND_LIBRARY_PREFIXES "lib" "")
ENDIF ()

# build time in UTC ISO 8601
FILE (WRITE ${CMAKE_BINARY_DIR}/timestamp.cmake "STRING(TIMESTAMP TIMEZ UTC)\n")
FILE (APPEND ${CMAKE_BINARY_DIR}/timestamp.cmake "FILE(WRITE timestamp.h \"#ifndef TIMESTAMP_H\\n\")\n")
FILE (APPEND ${CMAKE_BINARY_DIR}/timestamp.cmake "FILE(APPEND timestamp.h \"#define TIMESTAMP_H\\n\\n\")\n")
FILE (APPEND ${CMAKE_BINARY_DIR}/timestamp.cmake "FILE(APPEND timestamp.h \"#define _TIMEZ_ \\\"\${TIMEZ}\\\"\\n\\n\")\n")
FILE (APPEND ${CMAKE_BINARY_DIR}/timestamp.cmake "FILE(APPEND timestamp.h \"#endif // TIMESTAMP_H\\n\")\n")
ADD_CUSTOM_TARGET (
    timestamp
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/timestamp.cmake
    ADD_DEPENDENCIES ${CMAKE_BINARY_DIR}/timestamp.cmake)

#######################################
##             EXTERNALS             ##
#######################################

FIND_PACKAGE (BZip2 REQUIRED)
INCLUDE_DIRECTORIES (${BZIP_INCLUDE_DIRS})
APPEND (LIBRARIES ${BZIP2_LIBRARIES})

FIND_PACKAGE (Boost REQUIRED COMPONENTS program_options)
INCLUDE_DIRECTORIES (${Boost_INCLUDE_DIRS})
APPEND (LIBRARIES ${Boost_LIBRARIES})

FIND_PROGRAM (Flexc++ "flexc++")
IF (NOT Flexc++)
    MESSAGE (FATAL_ERROR "Flexc++ has not been found; please refer to the documentation.")
ELSE ()
    MESSAGE (STATUS "Found Flexc++: ${Flexc++}")
ENDIF ()

FIND_PROGRAM (Bisonc++ "bisonc++")
IF (NOT Bisonc++)
    MESSAGE (FATAL_ERROR "Bisonc++ has not been found; please refer to the documentation.")
ELSE ()
    MESSAGE (STATUS "Found Bisonc++: ${Bisonc++}")
ENDIF ()

#######################################
##                Qt                 ##
#######################################

# including Qt itself
FIND_PACKAGE (Qt4 REQUIRED)

# including Qt modules we need
SET (QT_USE_QTSCRIPT TRUE)
SET (QT_USE_QTXML TRUE)

# in order to build a Qt application, the Qt include directories
# needs to be added as well as a range of defines need to be set
INCLUDE (${QT_USE_FILE})
ADD_DEFINITIONS (${QT_DEFINITIONS})

# Qt code generators processing
QT4_WRAP_UI (MOC_FORMS ${FORMS})
QT4_WRAP_CPP (MOC_SOURCES ${HEADERS})
QT4_ADD_RESOURCES (QRC_SOURCES ${RESOURCES})

#######################################
##       QT LANGUAGE PROCSESING      ##
#######################################

SET (TS_PATH ${CMAKE_SOURCE_DIR}/lng)
SET (QM_PATH ${CMAKE_BINARY_DIR}/${BUILD_ROOT}/lng)
SET (LANGUAGES
    )

ADD_CUSTOM_TARGET (
    lng
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${TS_PATH} ${QM_PATH})

FOREACH (LANGUAGE ${LANGUAGES})
    APPEND (TSS ${QM_PATH}/${LANGUAGE}.ts)
ENDFOREACH ()

ADD_CUSTOM_COMMAND (
    TARGET lng
    COMMAND ${QT_LUPDATE_EXECUTABLE} ${HEADERS} ${SOURCES} -ts ${TSS}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

ADD_CUSTOM_COMMAND (
    TARGET lng
    COMMAND ${QT_LRELEASE_EXECUTABLE} ${TSS})

ADD_CUSTOM_COMMAND (
    TARGET lng
    COMMAND ${CMAKE_COMMAND} -E remove ${QM_PATH}/.gitkeep ${TSS})

# must have build directories created to release translations to
ADD_DEPENDENCIES (lng bld)

#######################################
##    GENERATE LEXERS AND PARSERS    ##
#######################################

ADD_CUSTOM_COMMAND (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Scanner.h
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/src/ArchiveInterfaces/WarpIN/WAIScriptParser/Scanner.h" "${CMAKE_CURRENT_BINARY_DIR}/Scanner.h"
    DEPENDS ${CMAKE_SOURCE_DIR}/src/ArchiveInterfaces/WarpIN/WAIScriptParser/Scanner.h)

ADD_CUSTOM_COMMAND (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Scanner.ih
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/src/ArchiveInterfaces/WarpIN/WAIScriptParser/Scanner.ih" "${CMAKE_CURRENT_BINARY_DIR}/Scanner.ih"
    DEPENDS ${CMAKE_SOURCE_DIR}/src/ArchiveInterfaces/WarpIN/WAIScriptParser/Scanner.ih)

ADD_CUSTOM_COMMAND (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Parser.h
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/src/ArchiveInterfaces/WarpIN/WAIScriptParser/Parser.h" "${CMAKE_CURRENT_BINARY_DIR}/Parser.h"
    DEPENDS ${CMAKE_SOURCE_DIR}/src/ArchiveInterfaces/WarpIN/WAIScriptParser/Parser.h)

ADD_CUSTOM_COMMAND (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Parser.ih
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/src/ArchiveInterfaces/WarpIN/WAIScriptParser/Parser.ih" "${CMAKE_CURRENT_BINARY_DIR}/Parser.ih"
    DEPENDS ${CMAKE_SOURCE_DIR}/src/ArchiveInterfaces/WarpIN/WAIScriptParser/Parser.ih)

ADD_CUSTOM_COMMAND (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lex.cc
           ${CMAKE_CURRENT_BINARY_DIR}/Scannerbase.h
    COMMAND ${Flexc++} "${CMAKE_SOURCE_DIR}/src/ArchiveInterfaces/WarpIN/WAIScriptParser/lexer.ll" --target-directory="${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS ${CMAKE_SOURCE_DIR}/src/ArchiveInterfaces/WarpIN/WAIScriptParser/lexer.ll)

ADD_CUSTOM_COMMAND (
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/parse.cc
           ${CMAKE_CURRENT_BINARY_DIR}/Parserbase.h
    COMMAND ${Bisonc++} "${CMAKE_SOURCE_DIR}/src/ArchiveInterfaces/WarpIN/WAIScriptParser/parser.yy" --target-directory="${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS ${CMAKE_SOURCE_DIR}/src/ArchiveInterfaces/WarpIN/WAIScriptParser/parser.yy)

ADD_CUSTOM_TARGET (
    WAIScriptParser
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Scanner.h
            ${CMAKE_CURRENT_BINARY_DIR}/Scanner.ih
            ${CMAKE_CURRENT_BINARY_DIR}/Parser.h
            ${CMAKE_CURRENT_BINARY_DIR}/Parser.ih
            ${CMAKE_CURRENT_BINARY_DIR}/lex.cc
            ${CMAKE_CURRENT_BINARY_DIR}/parse.cc
    SOURCES ${CMAKE_SOURCE_DIR}/src/ArchiveInterfaces/WarpIN/WAIScriptParser/lexer.ll
            ${CMAKE_SOURCE_DIR}/src/ArchiveInterfaces/WarpIN/WAIScriptParser/parser.yy
            ${CMAKE_SOURCE_DIR}/src/ArchiveInterfaces/WarpIN/WAIScriptParser/Scanner.h
            ${CMAKE_SOURCE_DIR}/src/ArchiveInterfaces/WarpIN/WAIScriptParser/Scanner.ih
            ${CMAKE_SOURCE_DIR}/src/ArchiveInterfaces/WarpIN/WAIScriptParser/Parser.h
            ${CMAKE_SOURCE_DIR}/src/ArchiveInterfaces/WarpIN/WAIScriptParser/Parser.ih)

SET (WAIScriptParser_CXX ${CMAKE_CURRENT_BINARY_DIR}/lex.cc ${CMAKE_CURRENT_BINARY_DIR}/parse.cc)

#######################################
##             BUILDING              ##
#######################################

ADD_EXECUTABLE (${BUILD_ROOT}/${BINARY_NAME} ${HEADERS} ${SOURCES} ${MOC_FORMS} ${MOC_SOURCES} ${QRC_SOURCES} ${WAIScriptParser_CXX})
# no lng for now ADD_DEPENDENCIES (${BUILD_ROOT}/${BINARY_NAME} bld lng WAIScriptParser)
ADD_DEPENDENCIES (${BUILD_ROOT}/${BINARY_NAME} bld timestamp WAIScriptParser)
TARGET_LINK_LIBRARIES (${BUILD_ROOT}/${BINARY_NAME} ${LIBRARIES} ${QT_LIBRARIES})
