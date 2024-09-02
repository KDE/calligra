cmake_minimum_required(VERSION 3.16)

# KDE Applications version, managed by release script.
set(RELEASE_SERVICE_VERSION_MAJOR "4")
set(RELEASE_SERVICE_VERSION_MINOR "0")
set(RELEASE_SERVICE_VERSION_MICRO "1")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

string(TIMESTAMP CALLIGRA_YEAR "%Y")
add_compile_options("-DCALLIGRA_YEAR=${CALLIGRA_YEAR}")

project(calligra VERSION ${RELEASE_SERVICE_VERSION})

if (POLICY CMP0022)
    cmake_policy(SET CMP0022 OLD)
endif ()
if (POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif()
if (POLICY CMP0071)
    cmake_policy(SET CMP0071 NEW)
endif()

set(REQUIRED_KF6_VERSION "6.0.0")
set(REQUIRED_QT_VERSION "6.5.0")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ECM ${REQUIRED_KF6_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

# ECM KDE macros (include first, to have their policies and settings effect all other macros)
include(KDEInstallDirs)
include(KDECMakeSettings NO_POLICY_SCOPE)
set(KDE_COMPILERSETTINGS_LEVEL 5.84.0) # set before including
include(KDECompilerSettings NO_POLICY_SCOPE)
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
include(KDEClangFormat)
include(KDEGitCommitHooks)
# own macros
include(MacroBoolTo01)
include(MacroOptionalFindPackage)
include(MacroEnsureVersion)

math(EXPR GENERIC_CALLIGRA_LIB_VERSION_MAJOR "${RELEASE_SERVICE_VERSION_MAJOR} + 15")

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX CALLIGRA
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/calligra-version.h
    SOVERSION ${GENERIC_CALLIGRA_LIB_VERSION_MAJOR}
)

############
#############
## Options ##
#############
############

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
    set(CALLIGRA_SHOULD_BUILD_STAGING TRUE)
    set(CALLIGRA_SHOULD_BUILD_GEMINI FALSE)
    set(CALLIGRA_SHOULD_BUILD_BRAINDUMP FALSE)
    if(BUILD_UNMAINTAINED)
        set(CALLIGRA_SHOULD_BUILD_UNMAINTAINED TRUE)
    else()
        set(CALLIGRA_SHOULD_BUILD_UNMAINTAINED FALSE)
    endif()
else ()
    set(CALLIGRA_SHOULD_BUILD_STAGING TRUE)
    set(CALLIGRA_SHOULD_BUILD_UNMAINTAINED TRUE)
    set(CALLIGRA_SHOULD_BUILD_GEMINI TRUE)
    set(CALLIGRA_SHOULD_BUILD_BRAINDUMP TRUE)
endif ()

# finally choose products/features to build
calligra_set_productset(${PRODUCTSET})

##########################
###########################
## Look for ECM, Qt, KF6 ##
###########################
##########################

find_package(KF6 ${REQUIRED_KF6_VERSION} REQUIRED
    COMPONENTS
    Archive
    Completion
    Config
    ConfigWidgets
    CoreAddons
    Crash
    DocTools
    GuiAddons
    I18n
    IconThemes
    ItemViews
    JobWidgets
    KCMUtils
    KIO
    Notifications
    NotifyConfig
    Sonnet
    TextWidgets
    WidgetsAddons
    WindowSystem
    XmlGui
)

find_package(Qt6 ${REQUIRED_QT_VERSION} REQUIRED
    COMPONENTS
    Core
    Gui
    Network
    PrintSupport
    Svg
    Test
    Widgets
    Xml
    Quick
    QuickControls2
)
find_package(Qt6 ${REQUIRED_QT_VERSION} QUIET
    COMPONENTS
    OpenGL
    QuickWidgets
    Sql
    WebEngineWidgets
)

qt_policy(SET QTP0001 NEW)


find_package(OpenSSL)
set_package_properties(OpenSSL PROPERTIES
    TYPE REQUIRED
    PURPOSE "Encrypted communications"
)

set_package_properties(Qt6QuickWidgets PROPERTIES
    PURPOSE "Required for Calligra Gemini"
    TYPE RECOMMENDED
    )
set_package_properties(Qt6Sql PROPERTIES
    PURPOSE "Optional for Sheets' database connection"
    TYPE OPTIONAL
    )
set_package_properties(Qt6WebEngine PROPERTIES
    PURPOSE "Required for Braindump's Web shape"
    TYPE OPTIONAL
)

set(HAVE_OPENGL ${Qt6OpenGL_FOUND})

# use sane compile flags
remove_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0)
add_definitions(
    -DQT_USE_QSTRINGBUILDER
    -DQT_STRICT_ITERATORS
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_NO_CAST_TO_ASCII
    -DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x053000
    -DQT_DISABLE_DEPRECATED_BEFORE=0x050600
    )

if(UNIX AND NOT APPLE AND NOT ANDROID AND NOT CALLIGRA_FLATPAK)
    add_definitions(-DWITH_QTDBUS)
    find_package(Qt6 ${REQUIRED_KF6_VERSION} REQUIRED
        COMPONENTS
        DBus
    )
    find_package(KF6 ${REQUIRED_KF6_VERSION} REQUIRED
        COMPONENTS
        DBusAddons
    )
endif()

# only with this definition will all the FOO_TEST_EXPORT macro do something
# TODO: check if this can be moved to only those places which make use of it,
# to reduce global compiler definitions that would trigger a recompile of
# everything on a change (like adding/removing tests to/from the build)
if(BUILD_TESTING)
    add_definitions(-DCOMPILING_TESTS)
endif()

# overcome some platform incompatibilities
if(WIN32)
    if(NOT MINGW)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/winquirks)
	add_definitions(-D_USE_MATH_DEFINES)
	add_definitions(-DNOMINMAX)
    endif()
    set(WIN32_PLATFORM_NET_LIBS ws2_32.lib netapi32.lib)
endif()

###########################
############################
## Required dependencies  ##
############################
###########################

find_package(Perl REQUIRED)
find_package(ZLIB REQUIRED)

find_package(Qt6Keychain)
set_package_properties(Qt6Keychain PROPERTIES
    TYPE REQUIRED
    PURPOSE "Secure storage of account secrets"
)

add_definitions(-DBOOST_ALL_NO_LIB)
find_package(Boost REQUIRED) # for pigment and stage
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

macro_optional_find_package(Imath 3.0 CONFIG QUIET)
if(TARGET Imath::Imath)
    set(OPENEXR_LIBRARIES Imath::Imath)
    set(OpenEXR_FOUND TRUE)
else()
    macro_optional_find_package(OpenEXR)
endif()
macro_bool_to_01(OpenEXR_FOUND HAVE_OPENEXR)

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
## Test for Phonon4Qt6
##
find_package(Phonon4Qt6 QUIET)
set_package_properties(Phonon4Qt6 PROPERTIES
    DESCRIPTION "Abstraction lib for multimedia applications"
    URL "https://www.kde.org/"
    PURPOSE "Required by Stage event actions and Videoshape plugin"
    TYPE OPTIONAL
    )

## PIM libraries detection disabled until semanticitems plugin is ported to Qt6
##
## Test for KF6CalendarCore
##
# find_package(KF6CalendarCore CONFIG QUIET)
# set_package_properties(KF6CalendarCore PROPERTIES
#     DESCRIPTION "KDE Calendar Library"
#     URL "https://www.kde.org/"
#     PURPOSE "Optionally used by semantic item Event"
#     TYPE OPTIONAL
#     )
# 
##
## Test for KF6Contacts
##
# find_package(KF6Contacts CONFIG QUIET)
# set_package_properties(KF6Contacts PROPERTIES
#     DESCRIPTION "KDE Address book Library"
#     URL "https://www.kde.org/"
#     PURPOSE "Optionally used by semantic item Contact"
#     TYPE OPTIONAL
#     )
# 
##
## Test for KPim6AkonadiCore
##
# find_package(KPim6Akonadi CONFIG QUIET)
# set_package_properties(KFPimAkonadi PROPERTIES
#     DESCRIPTION "Library for general Access to Akonadi"
#     URL "https://www.kde.org/"
#     PURPOSE "Optionally used by semantic items Event and Contact"
#     TYPE OPTIONAL
#     )
# 
##
## Test for KChart
##
macro_optional_find_package(KChart6 3.0.0 QUIET)
set_package_properties(KChart6 PROPERTIES
    DESCRIPTION "Library for creating business charts (part of KDiagram)"
    URL "https://www.kde.org/"
    PURPOSE "Required by Chart shape"
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
macro_optional_find_package(Qca-qt6 2.1.0 QUIET)
set_package_properties(Qca-qt6 PROPERTIES
    DESCRIPTION "Qt Cryptographic Architecture"
    URL "http:/download.kde.org/stable/qca-qt6"
    PURPOSE "Required for encrypted OpenDocument files and encrypted xls files support (available as a module in kdesupport)"
    TYPE OPTIONAL
    )

##
## Test for soprano
##
# QT5TODO: push for released (and maintained) Qt6 version of Soprano, T462, T461
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
# Temporary fix to avoid looking for Marble unnecessarily
# Its only used in RDF so until soprano is ported there is no use for Marble
if (Soprano_FOUND)
    macro_optional_find_package(Marble CONFIG)
    set(Marble_FOUND FALSE)
    set_package_properties(Marble PROPERTIES
        DESCRIPTION "World Globe Widget library"
        URL "https://marble.kde.org/"
        PURPOSE "Required by RDF to show locations on a map"
        TYPE OPTIONAL
    )
else()
    message(STATUS "Soprano not found. Skipped looking for Marble.")
endif()

##
## Test for lcms
##
macro_optional_find_package(LCMS2 2.4)
set_package_properties(LCMS2 PROPERTIES
    DESCRIPTION "LittleCMS, a color management engine"
    URL "http://www.littlecms.com"
    PURPOSE "Will be used for color management"
    TYPE OPTIONAL
    )
if(LCMS2_FOUND)
    set(HAVE_LCMS2 TRUE)
endif()

##
## Test for Vc
##

set(HAVE_VC FALSE)
if (BUILD_VC)
    # NOTE: This tampers with cmake variables (at least cmake_minimum_required), so may give build problems
    set(OLD_CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} )
    set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )
    if( NOT MSVC)
	macro_optional_find_package(Vc 1.1.0)
	set_package_properties(Vc PROPERTIES
	    DESCRIPTION "Portable, zero-overhead SIMD library for C++"
	    URL "https://github.com/VcDevel/Vc"
	    PURPOSE "Required by the pigment for vectorization"
	    TYPE OPTIONAL
	    )
	macro_bool_to_01(Vc_FOUND HAVE_VC)
	macro_bool_to_01(PACKAGERS_BUILD DO_PACKAGERS_BUILD)
    endif()
    configure_file(config-vc.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-vc.h )

    if(HAVE_VC)
	message(STATUS "Vc found!")
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
	    "${CMAKE_SOURCE_DIR}/cmake/vc")

	include (VcMacros)

	if(Vc_COMPILER_IS_CLANG)
	    set(ADDITIONAL_VC_FLAGS "-Wabi -ffp-contract=fast -fPIC")
	elseif (NOT MSVC)
	    set(ADDITIONAL_VC_FLAGS "-Wabi -fabi-version=0 -ffp-contract=fast -fPIC")
	endif()

	macro(ko_compile_for_all_implementations_no_scalar _objs _src)
	    if(PACKAGERS_BUILD)
		vc_compile_for_all_implementations(${_objs} ${_src} FLAGS ${ADDITIONAL_VC_FLAGS} ONLY SSE2 SSSE3 SSE4_1 AVX AVX2+FMA+BMI2)
	    else()
		set(${_objs} ${_src})
	    endif()
	endmacro()

	macro(ko_compile_for_all_implementations _objs _src)
	    if(PACKAGERS_BUILD)
		vc_compile_for_all_implementations(${_objs} ${_src} FLAGS ${ADDITIONAL_VC_FLAGS} ONLY Scalar SSE2 SSSE3 SSE4_1 AVX AVX2+FMA+BMI2)
	    else()
		set(${_objs} ${_src})
	    endif()
	endmacro()

	if (NOT PACKAGERS_BUILD)
	    # Optimize the whole Calligra for current architecture
	    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Vc_DEFINITIONS}")
	endif ()
    endif()
    set(CMAKE_MODULE_PATH ${OLD_CMAKE_MODULE_PATH} )

else(BUILD_VC)
    configure_file(config-vc.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-vc.h )
endif(BUILD_VC)

if(WIN32)
    set(LIB_INSTALL_DIR ${KDE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${KDE_INSTALL_BINDIR}
	LIBRARY ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
	ARCHIVE ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )
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

if(NOT Fontconfig_FOUND OR NOT FREETYPE_FOUND)
    set(Fontconfig_INCLUDE_DIRS "")
    set(FREETYPE_INCLUDE_DIRS "")
else()
    add_definitions( -DSHOULD_BUILD_FONT_CONVERSION )
endif()


##
## Test endianness
##
include (TestBigEndian)
test_big_endian(CMAKE_WORDS_BIGENDIAN)

##
## Test SharedMimeInfo
##
macro_optional_find_package(SharedMimeInfo 1.3)
set_package_properties(SharedMimeInfo PROPERTIES
    PURPOSE "Required to determine file types SVM or all of MSOOXML."
    TYPE RECOMMENDED
    )

##
## Test for Okular
##
macro_optional_find_package(Okular6 24.04.0 QUIET)
set_package_properties(Okular6 PROPERTIES
    DESCRIPTION "A unified document viewer"
    URL "https://okular.kde.org/"
    PURPOSE "Required to build the plugins for Okular"
    TYPE RECOMMENDED
)

##
## Test for librevenge
##
macro_optional_find_package(LibRevenge)
set_package_properties(LibRevenge PROPERTIES
    PURPOSE "Required by various import filters"
    TYPE RECOMMENDED
)

macro_optional_find_package(LibRevengeStream)
set_package_properties(LibRevengeStream PROPERTIES
    PURPOSE "Required by various import filters"
    TYPE RECOMMENDED
)

##
## Test for libodfgen
##
macro_optional_find_package(LibOdfGen)
set_package_properties(LibOdfGen PROPERTIES
    DESCRIPTION "Open Document Format Generation Library"
    TYPE RECOMMENDED
)

##
## Test for WordPerfect Document Library
##
macro_optional_find_package(LibWpd)
set_package_properties(LibWpd PROPERTIES
    PURPOSE "Required by the Words WPD import filter"
    TYPE RECOMMENDED
)

##
## Test for WordPerfect Graphics Library
##
macro_optional_find_package(LibWpg)
set_package_properties(LibWpg PROPERTIES
    PURPOSE "Required by the Karbon WPG import filter"
    TYPE RECOMMENDED
)

##
## Test for Microsoft Works Document Library
##
macro_optional_find_package(LibWps)
set_package_properties(LibWps PROPERTIES
    PURPOSE "Required by the Words WPS import filter"
    TYPE RECOMMENDED
)

##
## Test for Microsoft Visio Document Library
##
macro_optional_find_package(LibVisio)
set_package_properties(LibVisio PROPERTIES
    PURPOSE "Required by the visio import filter"
    TYPE RECOMMENDED
)

##
## Test for Apple Keynote Document Library
##
macro_optional_find_package(LibEtonyek)
set_package_properties(LibEtonyek PROPERTIES
    PURPOSE "Required by the Stage keynote import filter"
    TYPE RECOMMENDED
)

##
## Test for qt-poppler
##
macro_optional_find_package(Poppler "22.02.0" COMPONENTS Qt6)
set_package_properties(Poppler PROPERTIES
    PURPOSE "Required by the Karbon PDF import filter and CSTester PDF feature"
    TYPE RECOMMENDED
)

##
## Test for qt-poppler not-officially-supported XPDF Headers
## Installing these is off by default in poppler sources, so lets make
## sure they're really there before trying to build the pdf import
##
macro_optional_find_package(PopplerXPDFHeaders)
set_package_properties(PopplerXPDFHeaders PROPERTIES
    DESCRIPTION "XPDF headers in the Poppler Qt6 interface library"
    URL "http://poppler.freedesktop.org"
    PURPOSE "Required by the Karbon PDF import filter"
    TYPE OPTIONAL
)

##
## Test for libgit2
##
macro_optional_find_package(LibGit2)

##
## Generate a file for prefix information
##

###############################
################################
## Add Calligra helper macros ##
################################
###############################

include(MacroCalligraAddBenchmark)

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
if (KReport_FOUND AND KREPORT_SCRIPTING)
    set(KReport_WithScripting_FOUND TRUE)
endif()

calligra_drop_product_on_bad_condition( FEATURE_RDF
    Soprano_FOUND "Soprano not found"
    )

calligra_drop_product_on_bad_condition( PART_SHEETS
    EIGEN3_FOUND "Eigen devel not found"
    )

calligra_drop_product_on_bad_condition( OKULAR_GENERATOR_ODP
    Okular6_FOUND "Okular devel not found"
)

calligra_drop_product_on_bad_condition( OKULAR_GENERATOR_ODT
    Okular6_FOUND "Okular devel not found"
)

calligra_drop_product_on_bad_condition( PLUGIN_CHARTSHAPE
    KChart6_FOUND "KChart devel not found"
    )

calligra_drop_product_on_bad_condition( PLUGIN_VIDEOSHAPE
    Phonon4Qt6_FOUND "Phonon4Qt6 devel not found"
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
    PopplerXPDFHeaders_FOUND "poppler xpdf headers not found"
    Poppler_FOUND "poppler qt6 headers not found"
    )

calligra_drop_product_on_bad_condition( FILTER_HTML_TO_ODS
    NOT_WIN "not supported on Windows"
    NOT_COCOA "not supported with Qt Cocoa"
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
    Qt6WebEngineWidgets_FOUND "QWebEnginePage needed for webpage plugin"
)

calligra_drop_product_on_bad_condition( PLUGIN_CALLIGRAGEMINI_GIT
    LIBGIT2_FOUND  "libgit2 devel not found"
    )

calligra_drop_product_on_bad_condition( PART_QTQUICK
    Qt6Quick_FOUND "QtQuick not found"
)

calligra_drop_product_on_bad_condition( PART_COMPONENTS
    Qt6Quick_FOUND "QtQuick not found"
)

calligra_drop_product_on_bad_condition( APP_SLIDECOMPARE
    Qt6OpenGL_FOUND "Qt OpenGL not found"
)


#############################################
####  Backward compatibility BUILD_x=off ####
#############################################

# workaround: disable directly all products which might be activated by internal
# dependencies, but belong to scope of old flag
calligra_drop_products_on_old_flag(braindump APP_BRAINDUMP)
calligra_drop_products_on_old_flag(karbon APP_KARBON)
calligra_drop_products_on_old_flag(sheets PART_SHEETS APP_SHEETS)
calligra_drop_products_on_old_flag(stage  PART_STAGE APP_STAGE)
calligra_drop_products_on_old_flag(words  PART_WORDS APP_WORDS)

#############################################
####      Temporarily broken products    ####
#############################################

# If a product does not build due to some temporary brokenness disable it here,
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

add_subdirectory(stage)

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

#if(SHOULD_BUILD_GEMINI)
#    add_subdirectory(gemini)
#endif()

# non-app directories are moved here because they can depend on SHOULD_BUILD_{appname} variables set above
add_subdirectory(libs)
add_subdirectory(interfaces)
add_subdirectory(pics)
add_subdirectory(plugins)
add_subdirectory(servicetypes)
add_subdirectory(devtools)
add_subdirectory(extras)
add_subdirectory(filters)
add_subdirectory(data)
add_subdirectory(launcher)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

calligra_product_deps_report("product_deps")
calligra_log_should_build()

add_custom_target(apidox doc/api/gendocs.pl WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

configure_file(KoConfig.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/KoConfig.h )
if (SHOULD_BUILD_DEVEL_HEADERS)
    install( FILES  ${CMAKE_CURRENT_BINARY_DIR}/KoConfig.h DESTINATION ${KDE_INSTALL_INCLUDEDIR}/calligra COMPONENT Devel)
endif()

if (BUILD_TESTING)
    add_subdirectory(tests)
endif(BUILD_TESTING)

ki18n_install(po)
if(KF6DocTools_FOUND)
    kdoctools_install(po)
endif()

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
