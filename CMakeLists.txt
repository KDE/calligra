project(calligra)

message(STATUS "Using CMake version: ${CMAKE_VERSION}")

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

if (POLICY CMP0002)
    cmake_policy(SET CMP0002 OLD)
endif()
if (POLICY CMP0017)
    cmake_policy(SET CMP0017 NEW)
endif ()
if (POLICY CMP0022)
    cmake_policy(SET CMP0022 OLD)
endif ()
if (POLICY CMP0026)
    cmake_policy(SET CMP0026 OLD)
endif()
if (POLICY CMP0046)
    cmake_policy(SET CMP0046 OLD)
endif ()
if (POLICY CMP0059)
    cmake_policy(SET CMP0059 OLD)
endif()
if (POLICY CMP0063)
    cmake_policy(SET CMP0063 OLD)
endif()

# ensure out-of-source build
string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" isBuildInSource)
if(isBuildInSource)
    message(FATAL_ERROR "Compiling Calligra inside the source folder is not possible.\nPlease refer to the build instruction: https://community.kde.org/Calligra/Building/3\nYou need to clean up the source folder from all build artifacts just created, otherwise further building attempts will fail again: With a git repo, you can use \"git clean -df\" in the toplevel source folder (attention! will remove also uncommited changes to the source code). With sources from a file bundle (like a zip file), delete the source folder and unbundle the sources again.")
endif()

 ######################
#######################
## Constants defines ##
#######################
######################

# define common versions of Calligra applications, used to generate calligraversion.h
# update these version for every release:
set(CALLIGRA_VERSION_STRING "3.0 Alpha")
set(CALLIGRA_STABLE_VERSION_MAJOR 3) # 3 for 3.x, 4 for 4.x, etc.
set(CALLIGRA_STABLE_VERSION_MINOR 0) # 0 for 3.0, 1 for 3.1, etc.
set(CALLIGRA_VERSION_RELEASE 89)     # 89 for Alpha, increase for next test releases, set 0 for first Stable, etc.
set(CALLIGRA_ALPHA 1) # uncomment only for Alpha
#set(CALLIGRA_BETA 1) # uncomment only for Beta
#set(CALLIGRA_RC 1) # uncomment only for RC
set(CALLIGRA_YEAR 2016) # update every year

if(NOT DEFINED CALLIGRA_ALPHA AND NOT DEFINED CALLIGRA_BETA AND NOT DEFINED CALLIGRA_RC)
    set(CALLIGRA_STABLE 1) # do not edit
endif()

message(STATUS "Calligra version: ${CALLIGRA_VERSION_STRING}")

# Define the generic version of the Calligra libraries here
# This makes it easy to advance it when the next Calligra release comes.
# 14 was the last GENERIC_CALLIGRA_LIB_VERSION_MAJOR of the previous Calligra series
# (2.x) so we're starting with 15 in 3.x series.
if(CALLIGRA_STABLE_VERSION_MAJOR EQUAL 3)
    math(EXPR GENERIC_CALLIGRA_LIB_VERSION_MAJOR "${CALLIGRA_STABLE_VERSION_MINOR} + 15")
else()
    # let's make sure we won't forget to update the "15"
    message(FATAL_ERROR "Reminder: please update offset == 15 used to compute GENERIC_CALLIGRA_LIB_VERSION_MAJOR to something bigger")
endif()
set(GENERIC_CALLIGRA_LIB_VERSION "${GENERIC_CALLIGRA_LIB_VERSION_MAJOR}.0.0")
set(GENERIC_CALLIGRA_LIB_SOVERSION "${GENERIC_CALLIGRA_LIB_VERSION_MAJOR}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      "${CMAKE_SOURCE_DIR}/cmake/modules")

message("Module path:" ${CMAKE_MODULE_PATH})

# fetch git revision for the current build

set(CALLIGRA_GIT_SHA1_STRING "")
set(CALLIGRA_GIT_BRANCH_STRING "")

include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
get_git_branch(GIT_BRANCH)

if(GIT_SHA1 AND GIT_BRANCH)
    string(SUBSTRING ${GIT_SHA1} 0 7 GIT_SHA1)
    set(CALLIGRA_GIT_SHA1_STRING ${GIT_SHA1})
    set(CALLIGRA_GIT_BRANCH_STRING ${GIT_BRANCH})
endif()

if(NOT DEFINED RELEASE_BUILD)
    # estimate mode by CMAKE_BUILD_TYPE content if not set on cmdline
    string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_TOLOWER)
    set(RELEASE_BUILD_TYPES "release" "relwithdebinfo" "minsizerel")
    list(FIND RELEASE_BUILD_TYPES "${CMAKE_BUILD_TYPE_TOLOWER}" INDEX)
    if (INDEX EQUAL -1)
        set(RELEASE_BUILD FALSE)
    else()
        set(RELEASE_BUILD TRUE)
    endif()
endif()
message(STATUS "Release build: ${RELEASE_BUILD}")

 ############
#############
## Options ##
#############
############

option(GHNS "support Get Hot New Stuff" OFF)
option(PACKAGERS_BUILD "Build support of multiple CPU architectures in one binary. Should be used by packagers only." ON)

 #######################
########################
## Productset setting ##
########################
#######################

# For predefined productsets see the definitions in CalligraProducts.cmake and
# in the files in the folder cmake/productsets.

# Finding out the products & features to build is done in 5 steps:
# 1. have the user define the products/features wanted, by giving a productset
# 2. estimate all additional required products/features
# 3. estimate which of the products/features can be build by external deps
# 4. find which products/features have been temporarily disabled due to problems
# 5. estimate which of the products/features can be build by internal deps

# get the special macros
include(CalligraProductSetMacros)

# get the definitions of products, features and product sets
include(CalligraProducts.cmake)

set(PRODUCTSET_DEFAULT "ALL")

if(NOT PRODUCTSET)
    set(PRODUCTSET ${PRODUCTSET_DEFAULT} CACHE STRING "Set of products/features to build" FORCE)
endif()

if (RELEASE_BUILD)
    set(CALLIGRA_SHOULD_BUILD_STAGING FALSE)
else ()
    set(CALLIGRA_SHOULD_BUILD_STAGING TRUE)
endif ()

# finally choose products/features to build
calligra_set_productset(${PRODUCTSET})

 ##########################
###########################
## Look for ECM, Qt, KF5 ##
###########################
##########################

find_package(ECM 1.7.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

# CMake macros
include(CMakePackageConfigHelpers)
include(WriteBasicConfigVersionFile)
include(CheckFunctionExists)
include(CheckTypeSize)
include(CheckIncludeFile)
include(GenerateExportHeader)
include(FeatureSummary)
# ECM macros
include(ECMOptionalAddSubdirectory)
include(ECMInstallIcons)
include(ECMAddAppIcon)
include(ECMSetupVersion)
include(ECMAddTests)
include(ECMMarkAsTest)
include(ECMMarkNonGuiExecutable)
include(ECMGenerateHeaders)
# ECM KDE macros
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
# own macros
include(MacroBoolTo01)
include(MacroOptionalFindPackage)
include(MacroEnsureVersion)
include(MacroJPEG)


set(REQUIRED_KF5_VERSION "5.7.0")

find_package(KF5 ${REQUIRED_KF5_VERSION} REQUIRED
    COMPONENTS
        Archive 
        Codecs 
        Completion
        Config 
        ConfigWidgets 
        CoreAddons
        DBusAddons
        DocTools
        GuiAddons 
        I18n 
        IconThemes 
        ItemViews
        KDELibs4Support
        KIO 
        Kross 
        Parts 
        Sonnet
        TextEditor 
        TextWidgets 
        ThreadWeaver
        Wallet 
        WidgetsAddons 
        WindowSystem
        XmlGui 
        NotifyConfig
        Notifications
        KCMUtils
)
find_package(KF5 ${REQUIRED_KF5_VERSION} QUIET
    OPTIONAL_COMPONENTS
        Activities
        KHtml
)
set_package_properties(KF5Activities PROPERTIES
    TYPE OPTIONAL
)
set_package_properties(KF5KHtml PROPERTIES
    PURPOSE "Required for HTML2ODS import filter"
    TYPE OPTIONAL
)

if(KF5Activities_FOUND)
    set(HAVE_KACTIVITIES TRUE)
endif()
if(${KF5_VERSION} VERSION_LESS "5.16.0")
    set(OLD_PLUGIN_MIMETYPE_DATA TRUE)
endif()


set(REQUIRED_QT_VERSION "5.3.0")

find_package(Qt5 ${REQUIRED_QT_VERSION} REQUIRED
    COMPONENTS
        Core
        Gui
        Network
        PrintSupport
        Svg
        Test
        Widgets
        Xml
)
find_package(Qt5 ${REQUIRED_QT_VERSION} QUIET
    COMPONENTS
        DBus
        Declarative
        OpenGL
        Quick
        Sql
        WebKit
        WebKitWidgets
)

set_package_properties(Qt5DBus PROPERTIES
    TYPE RECOMMENDED
)
set_package_properties(Qt5Declarative PROPERTIES
    PURPOSE "Required for QtQuick1 components"
    TYPE RECOMMENDED
)
set_package_properties(Qt5OpenGL PROPERTIES
    PURPOSE "Required for QtQuick1 components"
    TYPE RECOMMENDED
)
set_package_properties(Qt5Quick PROPERTIES
    PURPOSE "Required for QtQuick2 components"
    TYPE RECOMMENDED
)
set_package_properties(Qt5Sql PROPERTIES
    PURPOSE "Optional for Sheets' database connection"
    TYPE OPTIONAL
)
set_package_properties(Qt5WebKit PROPERTIES
    PURPOSE "Required for Braindump's Web shape"
    TYPE OPTIONAL
)
set_package_properties(Qt5WebKitWidget PROPERTIES
    PURPOSE "Required for Stage"
    TYPE RECOMMENDED
)

if(Qt5WebKit_FOUND)
    add_definitions( -DCAN_USE_QTWEBKIT )
endif()

set(HAVE_OPENGL ${Qt5OpenGL_FOUND})


if (GHNS)
    find_package(Attica 3.0)
    find_package(NewStuff)
    set_package_properties(Attica PROPERTIES
        DESCRIPTION "Attica is used for Get Hot New Stuff."
        URL "https://projects.kde.org/projects/kdesupport/attica"
        TYPE OPTIONAL
    )

    if (NOT LIBATTICA_FOUND)
        set(GHNS FALSE)
    else ()
        message(STATUS "WARNING: You are compiling with Get Hot New Stuff enabled. Do not do that when building distribution packages. GHNS is unusable these days until someone starts maintaining it again.")
    endif ()
endif ()

find_package(X11)
if(X11_FOUND)
    find_package(Qt5 ${REQUIRED_QT_VERSION} REQUIRED
        COMPONENTS
            X11Extras
    )
    set(HAVE_X11 TRUE)
    add_definitions(-DHAVE_X11)
else()
    set(HAVE_X11 FALSE)
endif()

# use sane compile flags
add_definitions(
  -DQT_USE_QSTRINGBUILDER
  -DQT_STRICT_ITERATORS
  -DQT_NO_SIGNALS_SLOTS_KEYWORDS
  -DQT_USE_FAST_OPERATOR_PLUS
  -DQT_USE_FAST_CONCATENATION
  -DQT_NO_URL_CAST_FROM_STRING
)

# only with this definition will all the FOO_TEST_EXPORT macro do something
# TODO: check if this can be moved to only those places which make use of it,
# to reduce global compiler definitions that would trigger a recompile of
# everything on a change (like adding/removing tests to/from the build)
if(BUILD_TESTING)
    add_definitions(-DCOMPILING_TESTS)
endif()

# overcome some platform incompatibilities
if(WIN32)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/winquirks)
    add_definitions(-D_USE_MATH_DEFINES)
    add_definitions(-DNOMINMAX)
    set(WIN32_PLATFORM_NET_LIBS ws2_32.lib netapi32.lib)
endif()

# TEMPORARY: for initial Qt5/KF5 build porting phase deprecation warnings are only annoying noise
# remove once code porting phase starts, perhaps first locally in product subdirs
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
    add_definitions(-Wno-deprecated -Wno-deprecated-declarations)
endif ()

 ###########################
############################
## Required dependencies  ##
############################
###########################

find_package(Perl REQUIRED)
find_package(ZLIB REQUIRED)

add_definitions(-DBOOST_ALL_NO_LIB)
find_package(Boost REQUIRED COMPONENTS system) # for pigment and stage
if (NOT Boost_FOUND)
    message(FATAL_ERROR "Did not find Boost. Boost is required for the core libraries, stage, sheets.")
endif ()

 ###########################
############################
## Optional dependencies  ##
############################
###########################


##
## Check for OpenEXR
##

macro_optional_find_package(OpenEXR)
macro_bool_to_01(OPENEXR_FOUND HAVE_OPENEXR)

##
## Test for GNU Scientific Library
##
macro_optional_find_package(GSL 1.7)
set_package_properties(GSL_FOUND PROPERTIES
    DESCRIPTION "GNU Scientific Library"
    URL "https://www.gnu.org/software/gsl"
    PURPOSE "Required by Sheets' solver plugin"
    TYPE OPTIONAL
)
macro_bool_to_01(GSL_FOUND HAVE_GSL)
configure_file(config-gsl.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-gsl.h )

##
## Test for Phonon4Qt5
##
find_package(Phonon4Qt5 QUIET)
set_package_properties(Phonon4Qt5 PROPERTIES
    DESCRIPTION "Abstraction lib for multimedia applications"
    URL "https://www.kde.org/"
    PURPOSE "Required by Stage event actions and Videoshape plugin"
    TYPE OPTIONAL
)

##
## Test for KF5CalendarCore
##
find_package(KF5CalendarCore CONFIG QUIET)
set_package_properties(KF5CalendarCore PROPERTIES
    DESCRIPTION "KDE Calendar Library"
    URL "https://www.kde.org/"
    PURPOSE "Required by Plan Ical export and optionally used by semantic item Event"
    TYPE OPTIONAL
)

##
## Test for KF5Contacts
##
find_package(KF5Contacts CONFIG QUIET)
set_package_properties(KF5Contacts PROPERTIES
    DESCRIPTION "KDE Address book Library"
    URL "https://www.kde.org/"
    PURPOSE "Optionally used by semantic item Contact"
    TYPE OPTIONAL
)

##
## Test for KF5AkonadiContact
##
find_package(KF5AkonadiContact CONFIG QUIET)
set_package_properties(KF5AkonadiContact PROPERTIES
    DESCRIPTION "Library for Accessing Contacts stored in Akonadi"
    URL "https://www.kde.org/"
    PURPOSE "Optionally used by Plan"
    TYPE OPTIONAL
)

##
## Test for KF5AkonadiCore
##
find_package(KF5Akonadi CONFIG QUIET)
set_package_properties(KF5Akonadi PROPERTIES
    DESCRIPTION "Library for general Access to Akonadi"
    URL "https://www.kde.org/"
    PURPOSE "Optionally used by semantic items Event and Contact"
    TYPE OPTIONAL
)

##
## Test for KGantt
##
macro_optional_find_package(KGantt 2.6.0 QUIET)
set_package_properties(KGantt PROPERTIES
    DESCRIPTION "Library for creating Gantt diagrams (part of KDiagram)"
    URL "https://www.kde.org/"
    PURPOSE "Required by Plan"
    TYPE RECOMMENDED
)

##
## Test for KChart
##
macro_optional_find_package(KChart 2.6.0 QUIET)
set_package_properties(KChart PROPERTIES
    DESCRIPTION "Library for creating business charts (part of KDiagram)"
    URL "https://www.kde.org/"
    PURPOSE "Required by Chart shape and Plan"
    TYPE RECOMMENDED
)

##
## Test for KReport
##
macro_optional_find_package(KReport 2.96.1 QUIET)
set_package_properties(KReport PROPERTIES
    DESCRIPTION "A framework for the creation and generation of reports in multiple formats"
    URL "https://community.kde.org/KReport"
    PURPOSE "Required by Plan"
    TYPE RECOMMENDED
)

##
## Test for KPropertyWidgets
##
macro_optional_find_package(KPropertyWidgets 2.98.0 QUIET)
set_package_properties(KPropertyWidgets PROPERTIES
    DESCRIPTION "A property editing framework with editor widget"
    URL "https://community.kde.org/KProperty"
    PURPOSE "Required by Plan"
    TYPE RECOMMENDED
)

##
## Test for eigen3
##
macro_optional_find_package(Eigen3)
set_package_properties(Eigen3 PROPERTIES
    DESCRIPTION "C++ template library for linear algebra"
    URL "http://eigen.tuxfamily.org"
    PURPOSE "Required by Calligra Sheets"
    TYPE RECOMMENDED
)

##
## Test for QCA2
##
macro_optional_find_package(Qca-qt5 2.1.0 QUIET)
set_package_properties(Qca-qt5 PROPERTIES
    DESCRIPTION "Qt Cryptographic Architecture"
    URL "http:/download.kde.org/stable/qca-qt5"
    PURPOSE "Required for encrypted OpenDocument files and encrypted xls files support (available as a module in kdesupport)"
    TYPE OPTIONAL
)

##
## Test for soprano
##
# QT5TODO: push for released (and maintained) Qt5 version of Soprano, T462, T461
# macro_optional_find_package(Soprano)
set(Soprano_FOUND FALSE)
set_package_properties(Soprano PROPERTIES
    DESCRIPTION "RDF handling library"
    URL "http://soprano.sourceforge.net/"
    PURPOSE "Required to handle RDF metadata in ODF"
    TYPE OPTIONAL
)
if(NOT Soprano_FOUND)
    set(SOPRANO_INCLUDE_DIR "")
endif()

##
## Test for marble
##
set(MARBLE_MIN_VERSION "0.19.2")
macro_optional_find_package(CalligraMarble)
if(NOT MARBLE_FOUND)
    set(CAN_USE_MARBLE FALSE)
    set(MARBLE_INCLUDE_DIR "")
else()
    set(CAN_USE_MARBLE TRUE)
    add_definitions( -DCAN_USE_MARBLE )
    ##
    ## Marble changed addMarbleWidget to setMarbleWidget in MarbleControlBox.h
    ## with commit ea177ca. This is for compatibility with older versions.
    ##
    find_file(MARBLECONTROLBOX_H MarbleControlBox.h ${MARBLE_INCLUDE_DIR} PATH_SUFFIXES marble)
    if( MARBLECONTROLBOX_H )
    file(READ ${MARBLECONTROLBOX_H} MARBLECONTROLBOX_H_CONTENT)
    string(REGEX MATCH "setMarbleWidget" SETMARBLEWIDGET ${MARBLECONTROLBOX_H_CONTENT})
        if( SETMARBLEWIDGET )
            add_definitions(-DHAVE_SETMARBLEWIDGET)
        endif()
    else()
        message( WARNING "MarbleControlBox.h not found, could not properly set the SETMARBLEWIDGET define." )
    endif()
endif()
set_package_properties(Marble PROPERTIES
    DESCRIPTION "World Globe Widget library"
    URL "https://marble.kde.org/"
    PURPOSE "Required by RDF to show locations on a map"
    TYPE OPTIONAL
)

##
## Test for lcms
##
macro_optional_find_package(LCMS2)
set_package_properties(LCMS2 PROPERTIES
    DESCRIPTION "LittleCMS, a color management engine"
    URL "http://www.littlecms.com"
    PURPOSE "Will be used for color management"
    TYPE OPTIONAL
)
if(LCMS2_FOUND)
    if(NOT ${LCMS2_VERSION} VERSION_LESS 2040 )
        set(HAVE_LCMS24 TRUE)
    endif()
    set(HAVE_REQUIRED_LCMS_VERSION TRUE)
    set(HAVE_LCMS2 TRUE)
endif()

##
## Test for Vc
##

set(OLD_CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} )
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )
macro_optional_find_package(Vc 0.6.70)
set_package_properties(Vc PROPERTIES
    DESCRIPTION "Portable, zero-overhead SIMD library for C++"
    URL "http://code.compeng.uni-frankfurt.de/projects/vc"
    PURPOSE "Required by the pigment for vectorization"
    TYPE OPTIONAL
)

macro_bool_to_01(Vc_FOUND HAVE_VC)
macro_bool_to_01(PACKAGERS_BUILD DO_PACKAGERS_BUILD)

configure_file(config-vc.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-vc.h )

if(HAVE_VC)
    message(STATUS "Vc found!")
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                        "${CMAKE_SOURCE_DIR}/cmake/vc")

    include (VcMacros)

    if(Vc_COMPILER_IS_CLANG)
        set(ADDITIONAL_VC_FLAGS "-Wabi -ffp-contract=fast")
    else()
        set(ADDITIONAL_VC_FLAGS "-Wabi -fabi-version=0 -ffp-contract=fast")
    endif()

    #Handle Vc master
    if(Vc_VERSION_MAJOR GREATER 0 OR Vc_VERSION_MINOR GREATER 7)
        message(STATUS "Vc version is greater than 0.7, enabling AVX2 support")

        if(Vc_COMPILER_IS_GCC OR Vc_COMPILER_IS_CLANG)
            AddCompilerFlag("-std=c++11" _ok)
            if(NOT _ok)
                AddCompilerFlag("-std=c++0x" _ok)
            endif()
        endif()

        macro(ko_compile_for_all_implementations_no_scalar _objs _src)
        if(PACKAGERS_BUILD)
            vc_compile_for_all_implementations(${_objs} ${_src} FLAGS ${ADDITIONAL_VC_FLAGS} -fPIC ONLY SSE2 SSSE3 SSE4_1 AVX AVX2)
        else()
            set(${_objs} ${_src})
        endif()
        endmacro()

        macro(ko_compile_for_all_implementations _objs _src)
        if(PACKAGERS_BUILD)
            vc_compile_for_all_implementations(${_objs} ${_src} FLAGS ${ADDITIONAL_VC_FLAGS} -fPIC ONLY Scalar SSE2 SSSE3 SSE4_1 AVX AVX2)
        else()
            set(${_objs} ${_src})
        endif()
        endmacro()
    else()
        macro(ko_compile_for_all_implementations_no_scalar _objs _src)
        if(PACKAGERS_BUILD)
            vc_compile_for_all_implementations(${_objs} ${_src} FLAGS ${ADDITIONAL_VC_FLAGS} -fPIC ONLY SSE2 SSSE3 SSE4_1 AVX)
        else()
            set(${_objs} ${_src})
        endif()
        endmacro()

        macro(ko_compile_for_all_implementations _objs _src)
        if(PACKAGERS_BUILD)
            vc_compile_for_all_implementations(${_objs} ${_src} FLAGS ${ADDITIONAL_VC_FLAGS} -fPIC ONLY Scalar SSE2 SSSE3 SSE4_1 AVX)
        else()
            set(${_objs} ${_src})
        endif()
        endmacro()
    endif()

    if (NOT PACKAGERS_BUILD)
      # Optimize the whole Calligra for current architecture
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Vc_DEFINITIONS}")
    endif ()
endif()
set(CMAKE_MODULE_PATH ${OLD_CMAKE_MODULE_PATH} )

if(WIN32)
    set(LIB_INSTALL_DIR ${LIB_INSTALL_DIR}
                        RUNTIME DESTINATION ${BIN_INSTALL_DIR}
                        LIBRARY ${INSTALL_TARGETS_DEFAULT_ARGS}
                        ARCHIVE ${INSTALL_TARGETS_DEFAULT_ARGS} )
endif()

##
## Test for Fontconfig
##
## Only test if on non-Windows system
if(NOT WIN32 AND NOT APPLE)
    macro_optional_find_package(Fontconfig)
    set_package_properties(Fontconfig PROPERTIES
        DESCRIPTION "Library for configuring and customizing font access"
        URL "http://fontconfig.org"
        PURPOSE "Required to handle exact font size"
        TYPE RECOMMENDED
    )
endif()

##
## Test for Freetype
##
## Only test if on non-Windows system
if(NOT WIN32 AND NOT APPLE)
    macro_optional_find_package(Freetype)
    set_package_properties(Freetype PROPERTIES
        DESCRIPTION "A Free, High-Quality, and Portable Font Engine"
        URL "http://www.freetype.org/"
        PURPOSE "Required to handle exact font size"
        TYPE RECOMMENDED
    )
endif()

if(NOT FONTCONFIG_FOUND OR NOT FREETYPE_FOUND)
    set(FONTCONFIG_INCLUDE_DIR "")
    set(FREETYPE_INCLUDE_DIRS "")
else()
    add_definitions( -DSHOULD_BUILD_FONT_CONVERSION )
endif()


##
## Test endianess
##
include (TestBigEndian)
test_big_endian(CMAKE_WORDS_BIGENDIAN)

##
## Test SharedMimeInfo
##
macro_optional_find_package(SharedMimeInfo)
set_package_properties(SharedMimeInfo PROPERTIES
    DESCRIPTION "Shared Mime Info"
    URL "http://freedesktop.org/wiki/Software/shared-mime-info"
    PURPOSE "Required to determine file types SVM or all of MSOOXML."
    TYPE RECOMMENDED
)

##
## Test for Okular
##
macro_optional_find_package(Okular5 1.0.0 QUIET)
set_package_properties(Okular5 PROPERTIES
    DESCRIPTION "A unified document viewer"
    URL "https://okular.kde.org/"
    PURPOSE "Required to build the plugins for Okular"
    TYPE OPTIONAL
)

##
## Test for librevenge
##
macro_optional_find_package(LibRevenge)
set_package_properties(LibRevenge PROPERTIES
    DESCRIPTION "A base library for writing document import filters"
    URL "http://sf.net/p/libwpd/librevenge/"
    PURPOSE "Required by various import filters"
    TYPE OPTIONAL
)

##
## Test for libodfgen
##
macro_optional_find_package(LibOdfGen)
set_package_properties(LibOdfGen PROPERTIES
    DESCRIPTION "Open Document Format Generation Library"
    URL "http://sf.net/p/libwpd/libodfgen/"
    PURPOSE "Required by various import filters"
    TYPE OPTIONAL
)

##
## Test for WordPerfect Document Library
##
macro_optional_find_package(LibWpd)
set_package_properties(LibWpd PROPERTIES
    DESCRIPTION "WordPerfect Document Library"
    URL "http://libwpd.sourceforge.net/"
    PURPOSE "Required by the Words WPD import filter"
    TYPE OPTIONAL
)

##
## Test for WordPerfect Graphics Library
##
macro_optional_find_package(LibWpg)
set_package_properties(LibWpg PROPERTIES
    DESCRIPTION "WordPerfect Graphics Library"
    URL "http://libwpg.sourceforge.net/"
    PURPOSE "Required by the Karbon WPG import filter"
    TYPE OPTIONAL
)

##
## Test for Microsoft Works Document Library
##
macro_optional_find_package(LibWps)
set_package_properties(LibWps PROPERTIES
    DESCRIPTION "Microsoft Works Document Library"
    URL "http://libwps.sourceforge.net/"
    PURPOSE "Required by the Words WPS import filter"
    TYPE OPTIONAL
)

##
## Test for Microsoft Visio Document Library
##
macro_optional_find_package(LibVisio)
set_package_properties(LibVisio PROPERTIES
    DESCRIPTION "Visio Import Filter Library"
    URL "https://wiki.documentfoundation.org/DLP/Libraries/libvisio"
    PURPOSE "Required by the Flow visio import filter"
    TYPE OPTIONAL
)

##
## Test for Apple Keynote Document Library
##
macro_optional_find_package(LibEtonyek)
set_package_properties(LibEtonyek PROPERTIES
    DESCRIPTION "Apple Keynote Document Library"
    URL "https://wiki.documentfoundation.org/DLP/Libraries/libetonyek"
    PURPOSE "Required by the Stage keynote import filter"
    TYPE OPTIONAL
)

##
## Test for qt-poppler
##
macro_optional_find_package(Poppler)
set_package_properties(Poppler PROPERTIES
    DESCRIPTION "A PDF rendering library"
    URL "http://poppler.freedesktop.org"
    PURPOSE "Required by the Karbon PDF import filter and CSTester PDF feature"
    TYPE OPTIONAL
)

##
## Test for qt-poppler not-officially-supported XPDF Headers
## Installing these is off by default in poppler sources, so lets make
## sure they're really there before trying to build the pdf import
##
macro_optional_find_package(PopplerXPDFHeaders)
set_package_properties(PopplerXPDFHeaders PROPERTIES
    DESCRIPTION "XPDF headers in the Poppler Qt5 interface library"
    URL "http://poppler.freedesktop.org"
    PURPOSE "Required by the Karbon PDF import filter"
    TYPE OPTIONAL
)


##
## Test for libgit2 and Libqgit2
##
macro_optional_find_package(Libgit2)
macro_optional_find_package(Libqgit2)

##
## Generate a file for prefix information
##

 ###############################
################################
## Add Calligra helper macros ##
################################
###############################

include(MacroCalligraAddBenchmark)
include(MacroCalligraAddTest)

 ####################
#####################
## Define includes ##
#####################
####################

# WARNING: make sure that QT_INCLUDES is the first directory to be added to include_directory before
#          any other include directory

# for config.h and <toplevel/foo.h> includes (if any?)
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/interfaces )


set(KOVERSION_INCLUDES ${CMAKE_SOURCE_DIR}/libs/version 
                       ${CMAKE_BINARY_DIR}/libs/version 
)

include_directories(${KOVERSION_INCLUDES})

# koplugin is at the bottom of the stack
set(KOPLUGIN_INCLUDES ${CMAKE_SOURCE_DIR}/libs/plugin)

set(KUNDO2_INCLUDES ${CMAKE_SOURCE_DIR}/libs/kundo2
                    ${CMAKE_BINARY_DIR}/libs/kundo2)

# koodf is at the bottom of the stack
set(KOODF_INCLUDES ${CMAKE_SOURCE_DIR}/libs/odf
                   ${CMAKE_SOURCE_DIR}/libs/store
                   ${CMAKE_BINARY_DIR}/libs/odf
                   ${CMAKE_BINARY_DIR}/libs/store
                   ${KOVERSION_INCLUDES} 
                   )

# pigment depends on koplugin and lcms
set(PIGMENT_INCLUDES ${KOPLUGIN_INCLUDES}
                   ${KOVERSION_INCLUDES} 
                   ${CMAKE_SOURCE_DIR}/libs/pigment
                   ${CMAKE_BINARY_DIR}/libs/pigment
                   ${CMAKE_SOURCE_DIR}/libs/pigment/compositeops
                   ${CMAKE_SOURCE_DIR}/libs/pigment/resources
                   ${Boost_INCLUDE_DIRS}
                   
                   )

# flake depends on koodf and pigment
set(FLAKE_INCLUDES ${CMAKE_SOURCE_DIR}/libs/flake
                   ${KOODF_INCLUDES}
                   ${PIGMENT_INCLUDES}
                   ${KUNDO2_INCLUDES}
                   ${CMAKE_SOURCE_DIR}/libs/widgetutils
                   ${CMAKE_SOURCE_DIR}/libs/flake/commands
                   ${CMAKE_SOURCE_DIR}/libs/flake/tools
                   ${CMAKE_SOURCE_DIR}/libs/flake/svg
                   ${CMAKE_BINARY_DIR}/libs/flake)

# vectorimage
set(VECTORIMAGE_INCLUDES
                   ${CMAKE_SOURCE_DIR}/libs/vectorimage
                   ${CMAKE_SOURCE_DIR}/libs/vectorimage/libemf
                   ${CMAKE_SOURCE_DIR}/libs/vectorimage/libsvm
                   ${CMAKE_SOURCE_DIR}/libs/vectorimage/libwmf)

# KoText depends on koplugin, odf
set(KOTEXT_INCLUDES
                   ${CMAKE_SOURCE_DIR}/libs/text
                   ${CMAKE_BINARY_DIR}/libs/text
                   ${CMAKE_SOURCE_DIR}/libs/text/changetracker
                   ${CMAKE_SOURCE_DIR}/libs/text/styles
                   ${CMAKE_SOURCE_DIR}/libs/text/opendocument
                   ${SOPRANO_INCLUDE_DIR}
                   ${FLAKE_INCLUDES}
                   ${KOODF_INCLUDES})

# TextLayout depends on kotext
set(TEXTLAYOUT_INCLUDES ${KOTEXT_INCLUDES}
                   ${CMAKE_SOURCE_DIR}/libs/textlayout
                   ${CMAKE_BINARY_DIR}/libs/textlayout)

# Widgets depends on kotext and flake
set(KOWIDGETS_INCLUDES ${KOTEXT_INCLUDES}
                   ${CMAKE_SOURCE_DIR}/libs/widgetutils
                   ${CMAKE_BINARY_DIR}/libs/widgetutils
                   ${CMAKE_SOURCE_DIR}/libs/widgets
                   ${CMAKE_BINARY_DIR}/libs/widgets)

# BasicFlakes depends on flake, widgets
set(BASICFLAKES_INCLUDES ${KOWIDGETS_INCLUDES}
                   ${CMAKE_SOURCE_DIR}/libs/basicflakes
                   ${CMAKE_SOURCE_DIR}/libs/basicflakes/tools)


# komain depends on kotext & flake
set(KOMAIN_INCLUDES 
                    ${KOWIDGETS_INCLUDES}
                    ${TEXTLAYOUT_INCLUDES}
                    ${CMAKE_SOURCE_DIR}/libs/main
                    ${CMAKE_BINARY_DIR}/libs/main
                    ${CMAKE_SOURCE_DIR}/libs/main/config)

set(KORDF_INCLUDES ${KOMAIN_INCLUDES}
                   ${CMAKE_SOURCE_DIR}/libs/rdf
)
set(KORDF_LIBS kordf)

if(SHOULD_BUILD_FEATURE_SCRIPTING)
  set(KOKROSS_INCLUDES
                    ${CMAKE_SOURCE_DIR}/libs/kross
                    ${CMAKE_BINARY_DIR}/libs/kross)
endif()

# kopageapp
set(KOPAGEAPP_INCLUDES ${TEXTLAYOUT_INCLUDES}
                    ${PIGMENT_INCLUDES}
                    ${KOMAIN_INCLUDES}
                    ${CMAKE_SOURCE_DIR}/libs/widgets
                    ${CMAKE_SOURCE_DIR}/libs/pageapp
                    ${CMAKE_SOURCE_DIR}/libs/pageapp/commands
                    ${CMAKE_BINARY_DIR}/libs/pageapp )

#############################################
####           filter libraries          ####
#############################################

# libodf2
set(KOODF2_INCLUDES
    ${CMAKE_SOURCE_DIR}/filters/libodf2
    ${CMAKE_SOURCE_DIR}/filters/libodf2/chart
)

# libodfreader
set(KOODFREADER_INCLUDES
    ${CMAKE_SOURCE_DIR}/filters/libodfreader
)

 ###################################################
####################################################
## Detect which products/features can be compiled ##
####################################################
###################################################

if (NOT WIN32)
    set(NOT_WIN TRUE)
endif()
if (NOT QT_MAC_USE_COCOA)
    set(NOT_COCOA TRUE)
endif()

calligra_drop_product_on_bad_condition( LIB_KOMSOOXML
    SHARED_MIME_INFO_FOUND "SharedMimeInfo not found (needed to install mimetypes)"
)

calligra_drop_product_on_bad_condition( FEATURE_RDF
    Soprano_FOUND "Soprano not found"
)

calligra_drop_product_on_bad_condition( PART_STAGE
    Qt5WebKitWidgets_FOUND "Qt5WebKitWidgets devel not found"
)

calligra_drop_product_on_bad_condition( PART_SHEETS
    EIGEN3_FOUND "Eigen devel not found"
)

calligra_drop_product_on_bad_condition( OKULAR_GENERATOR_ODP
    Okular5_FOUND "Okular devel not found"
)

calligra_drop_product_on_bad_condition( OKULAR_GENERATOR_ODT
    Okular5_FOUND "Okular devel not found"
)

calligra_drop_product_on_bad_condition( PLUGIN_CHARTSHAPE
    KChart_FOUND "KChart devel not found"
)

calligra_drop_product_on_bad_condition( PLUGIN_VIDEOSHAPE
    Phonon4Qt5_FOUND "Phonon4Qt5 devel not found"
)

calligra_drop_product_on_bad_condition( FILTER_KEY_TO_ODP
    LIBODFGEN_FOUND "libodfgen devel not found"
    LIBETONYEK_FOUND "libetonyek devel not found"
    LIBREVENGE_FOUND "librevenge devel not found"
)

calligra_drop_product_on_bad_condition( FILTER_VISIO_TO_ODG
    LIBODFGEN_FOUND "libodfgen devel not found"
    LIBVISIO_FOUND "libvisio devel not found"
    LIBREVENGE_FOUND "librevenge devel not found"
)

calligra_drop_product_on_bad_condition( FILTER_WORDPERFECT_TO_ODT
    LIBODFGEN_FOUND "libodfgen devel not found"
    LIBWPD_FOUND "libwpd devel not found"
    LIBWPG_FOUND "libwpg devel not found"
    LIBREVENGE_FOUND "librevenge devel not found"
)

calligra_drop_product_on_bad_condition( FILTER_WORKS_TO_ODT
    LIBODFGEN_FOUND "libodfgen devel not found"
    LIBWPS_FOUND "libwps devel not found"
    LIBREVENGE_FOUND "librevenge devel not found"
)

calligra_drop_product_on_bad_condition( FILTER_WPG_TO_SVG
    LIBWPG_FOUND "libwpg devel not found"
    LIBREVENGE_FOUND "librevenge devel not found"
)

calligra_drop_product_on_bad_condition( FILTER_WPG_TO_ODG
    LIBODFGEN_FOUND "libodfgen devel not found"
    LIBWPG_FOUND "libwpg devel not found"
    LIBREVENGE_FOUND "librevenge devel not found"
)

calligra_drop_product_on_bad_condition( FILTER_PDF_TO_SVG
    NOT_WIN "not supported on Windows"
    POPPLER_XPDF_HEADERS_FOUND "poppler xpdf headers not found"
)

calligra_drop_product_on_bad_condition( FILTER_HTML_TO_ODS
    NOT_WIN "not supported on Windows"
    NOT_COCOA "not supported with Qt Cocoa"
    KF5KHtml_FOUND "KF5KHtml devel not found"
)

calligra_drop_product_on_bad_condition( FILTER_SHEETS_TO_HTML
    NOT_WIN "not supported on Windows"
    NOT_COCOA "not supported with Qt Cocoa"
)

calligra_drop_product_on_bad_condition( FILTER_KSPREAD_TO_LATEX
    NOT_WIN "not supported on Windows"
    NOT_COCOA "not supported with Qt Cocoa"
)

calligra_drop_product_on_bad_condition( APP_BRAINDUMP
    NOT_WIN "unmaintained on Windows"
)

calligra_drop_product_on_bad_condition( APP_PLAN
    KGantt_FOUND "KGantt devel not found"
    KChart_FOUND "KChart devel not found"
    KReport_FOUND "KReport not found"
    KPropertyWidgets_FOUND "KPropertyWidgets not found"
)

calligra_drop_product_on_bad_condition( PLUGIN_CALLIGRAGEMINI_GIT
    LIBGIT2_FOUND  "libgit2 devel not found"
    LIBQGIT2_FOUND "libqgit2 devel not found"
)

calligra_drop_product_on_bad_condition( PART_QTQUICK
    Qt5OpenGL_FOUND "Qt OpenGL not found"
    Qt5Declarative_FOUND "QtDeclarative not found"
)

calligra_drop_product_on_bad_condition( PART_COMPONENTS
    Qt5Quick_FOUND "QtQuick not found"
)

calligra_drop_product_on_bad_condition( APP_SLIDECOMPARE
    Qt5OpenGL_FOUND "Qt OpenGL not found"
)

#############################################
####  Backward compatibility BUILD_x=off ####
#############################################

# workaround: disable directly all products which might be activated by internal
# dependencies, but belong to scope of old flag
calligra_drop_products_on_old_flag(braindump APP_BRAINDUMP)
calligra_drop_products_on_old_flag(flow APP_FLOW)
calligra_drop_products_on_old_flag(karbon APP_KARBON)
calligra_drop_products_on_old_flag(plan  APP_PLAN)
calligra_drop_products_on_old_flag(sheets PART_SHEETS APP_SHEETS)
calligra_drop_products_on_old_flag(stage  PART_STAGE APP_STAGE)
calligra_drop_products_on_old_flag(words  PART_WORDS APP_WORDS)

#############################################
####      Temporarily broken products    ####
#############################################

# If a product does not build due to some temporary brokeness disable it here,
# by calling calligra_disable_product with the product id and the reason,
# e.g.:
# calligra_disable_product(APP_FOO "isn't buildable at the moment")

#############################################
####     Calculate buildable products    ####
#############################################

calligra_drop_unbuildable_products()


#############################################
####     Setup product-depending vars    ####
#############################################

if(SHOULD_BUILD_FEATURE_RDF)
   add_definitions( -DSHOULD_BUILD_RDF )
endif()

 ###################
####################
## Subdirectories ##
####################
###################

add_subdirectory(words)

if (SHOULD_BUILD_APP_FLOW)
    add_subdirectory(flow)
endif ()

add_subdirectory(stage)

if(SHOULD_BUILD_APP_PLAN)
    add_subdirectory(plan)
endif()

add_subdirectory(sheets)

if(SHOULD_BUILD_APP_KARBON)
  add_subdirectory(karbon)
endif()

if(SHOULD_BUILD_APP_BRAINDUMP)
  add_subdirectory(braindump)
endif()

if(SHOULD_BUILD_DOC)
    add_subdirectory(doc)
endif()

if(SHOULD_BUILD_PART_QTQUICK)
    add_subdirectory(qtquick)
endif()

if(SHOULD_BUILD_PART_COMPONENTS)
    add_subdirectory(components)
endif()

if(SHOULD_BUILD_GEMINI)
    add_subdirectory(gemini)
endif()

# non-app directories are moved here because they can depend on SHOULD_BUILD_{appname} variables set above
add_subdirectory(libs)
add_subdirectory(interfaces)
add_subdirectory(cmake)
add_subdirectory(pics)
add_subdirectory(plugins)
add_subdirectory(servicetypes)
add_subdirectory(devtools)
add_subdirectory(extras)
add_subdirectory(filters)
add_subdirectory(data)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

calligra_product_deps_report("product_deps")
calligra_log_should_build()

add_custom_target(apidox doc/api/gendocs.pl WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

configure_file(KoConfig.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/KoConfig.h )
install( FILES  ${CMAKE_CURRENT_BINARY_DIR}/KoConfig.h DESTINATION ${INCLUDE_INSTALL_DIR}/calligra COMPONENT Devel)
