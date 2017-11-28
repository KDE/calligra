
project(calligraplan)

cmake_minimum_required(VERSION 2.8.12)

set(PLAN_SOURCE_DIR ${PROJECT_SOURCE_DIR})

if ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PLAN_SOURCE_DIR}/cmake/modules")
endif()
message(STATUS "CMake module path:" ${CMAKE_MODULE_PATH})

######################
#######################
## Constants defines ##
#######################
######################

# define the version of Plan
# update these version for every release:
set(PLAN_YEAR 2017) # update every year
set(PLAN_VERSION_STRING "3.0.89")
set(PLAN_STABLE_VERSION_MAJOR 3) # 3 for 3.x, 4 for 4.x, etc.
set(PLAN_STABLE_VERSION_MINOR 0) # 0 for 3.0, 1 for 3.1, etc.
set(PLAN_VERSION_RELEASE 89)     # 89 for Alpha, increase for next test releases, set 0 for first Stable, etc.

message(STATUS "Calligra Plan version: ${PLAN_VERSION_STRING}")

# Define the generic version of the Calligra Plan libraries here
# This makes it easy to advance it when the next release comes.
# 14 was the last GENERIC_CALLIGRA_LIB_VERSION_MAJOR of the previous Calligra series
# (2.x) so we're starting with 15 in 3.x series.
if(PLAN_STABLE_VERSION_MAJOR EQUAL 3)
    math(EXPR GENERIC_PLAN_LIB_VERSION_MAJOR "${PLAN_STABLE_VERSION_MINOR} + 15")
else()
    # let's make sure we won't forget to update the "15"
    message(FATAL_ERROR "Reminder: please update offset == 15 used to compute GENERIC_PLAN_LIB_VERSION_MAJOR to something bigger")
endif()
set(GENERIC_PLAN_LIB_VERSION "${GENERIC_PLAN_LIB_VERSION_MAJOR}.0.0")
set(GENERIC_PLAN_LIB_SOVERSION "${GENERIC_PLAN_LIB_VERSION_MAJOR}")


if ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")

    message(STATUS "Using CMake version: ${CMAKE_VERSION}")

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
        cmake_policy(SET CMP0063 NEW)
    endif()

    # ensure out-of-source build
    string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" isBuildInSource)
    if(isBuildInSource)
        message(FATAL_ERROR "Compiling Plan inside the source folder is not possible.\nPlease refer to the build instruction: https://community.kde.org/Calligra/Building/3\nYou need to clean up the source folder from all build artifacts just created, otherwise further building attempts will fail again: With a git repo, you can use \"git clean -df\" in the toplevel source folder (attention! will remove also uncommited changes to the source code). With sources from a file bundle (like a zip file), delete the source folder and unbundle the sources again.")
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

    option(PACKAGERS_BUILD "Build support of multiple CPU architectures in one binary. Should be used by packagers only." ON)

    ##########################
    ###########################
    ## Look for ECM, Qt, KF5 ##
    ###########################
    ##########################

    find_package(ECM 5.19 REQUIRED NO_MODULE)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

    # ECM KDE macros (include first, to have their policies and settings effect all other macros)
    include(KDEInstallDirs)
    include(KDECMakeSettings NO_POLICY_SCOPE)
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
    # own macros
    include(MacroBoolTo01)
    include(MacroOptionalFindPackage)
    include(MacroEnsureVersion)


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
            JobWidgets
            KCMUtils
    #         KDELibs4Support
            KIO
            Notifications
            NotifyConfig
            Parts
#             Sonnet
            TextWidgets
            Wallet
            WidgetsAddons
            WindowSystem
            XmlGui
    #         Kross
    )

    find_package(KF5Activities)

    set_package_properties(KF5Activities PROPERTIES
        DESCRIPTION "Library for Activities"
        PURPOSE "Required for activities support (optional)"
        TYPE OPTIONAL
    )
    if(KF5Activities_FOUND)
        set(HAVE_KACTIVITIES TRUE)
    endif()

    find_package(KF5KHtml)
    set_package_properties(KF5KHtml PROPERTIES
        PURPOSE "Required for rich text support"
        TYPE REQUIRED
    )

    find_package(KF5Holidays 5.3.3)
    set_package_properties(KF5Holidays PROPERTIES
        DESCRIPTION "Library for generation of public holidays"
        PURPOSE "Required for holidays support (optional)"
        TYPE RECOMMENDED
    )

    if(${KF5_VERSION} VERSION_LESS "5.16.0")
        set(CALLIGRA_OLD_PLUGIN_METADATA TRUE)
    endif()


    set(REQUIRED_QT_VERSION "5.4.0")

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
            OpenGL
    #        Quick
    #        Sql
    #         WebKit
    #         WebKitWidgets
    )
    # Qt5Declarative was removed in Qt 5.6.0 so search for it in a separate call
    # Including it in a collected find_package(Qt5 ...) call can lead to a fatal not-found error:
    # * Qt5 (required version >= 5.3.0)
    # find_package(Qt5Declarative ${REQUIRED_QT_VERSION} QUIET)

    set_package_properties(Qt5DBus PROPERTIES
        DESCRIPTION "Library for dbus access"
        PURPOSE "Required for dbus support (optional)"
        TYPE OPTIONAL
    )

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
    -DQT_NO_CAST_TO_ASCII
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

    ###########################
    ############################
    ## Required dependencies  ##
    ############################
    ###########################

    ##
    ## Test for KGantt
    ##
    macro_optional_find_package(KGantt 2.6.0 QUIET)
    set_package_properties(KGantt PROPERTIES
        DESCRIPTION "Library for creating Gantt diagrams (part of KDiagram)"
        URL "https://www.kde.org/"
        TYPE REQUIRED
    )

    ##
    ## Test for KChart
    ##
    macro_optional_find_package(KChart 2.6.0 QUIET)
    set_package_properties(KChart PROPERTIES
        DESCRIPTION "Library for creating business charts (part of KDiagram)"
        URL "https://www.kde.org/"
        TYPE REQUIRED
    )

    find_package(Perl REQUIRED)
    find_package(ZLIB REQUIRED)

    ###########################
    ############################
    ## Optional dependencies  ##
    ############################
    ###########################

    ##
    ## Test for KF5CalendarCore
    ##
    find_package(KF5CalendarCore CONFIG QUIET)
    set_package_properties(KF5CalendarCore PROPERTIES
        DESCRIPTION "KDE Calendar Library"
        URL "https://www.kde.org/"
        PURPOSE "Required for Ical export"
        TYPE OPTIONAL
    )

    ##
    ## Test for KF5Contacts
    ##
    find_package(KF5Contacts CONFIG QUIET)
    set_package_properties(KF5Contacts PROPERTIES
        DESCRIPTION "KDE Address book Library"
        URL "https://www.kde.org/"
        PURPOSE "Required for address book support"
        TYPE OPTIONAL
    )

    ##
    ## Test for KF5AkonadiContact
    ##
    find_package(KF5AkonadiContact CONFIG QUIET)
    set_package_properties(KF5AkonadiContact PROPERTIES
        DESCRIPTION "Library for Accessing Contacts stored in Akonadi"
        URL "https://www.kde.org/"
        PURPOSE "Required for address book support"
        TYPE OPTIONAL
    )

    ##
    ## Test for KF5AkonadiCore
    ##
    # find_package(KF5Akonadi CONFIG QUIET)
    # set_package_properties(KF5Akonadi PROPERTIES
    #     DESCRIPTION "Library for general Access to Akonadi"
    #     URL "https://www.kde.org/"
    #     PURPOSE "Optionally used by Plan"
    #     TYPE OPTIONAL
    # )


    if(WIN32)
        set(LIB_INSTALL_DIR ${LIB_INSTALL_DIR}
                            RUNTIME DESTINATION ${BIN_INSTALL_DIR}
                            LIBRARY ${INSTALL_TARGETS_DEFAULT_ARGS}
                            ARCHIVE ${INSTALL_TARGETS_DEFAULT_ARGS} )
    endif()

    ##
    ## Test endianess
    ##
    include (TestBigEndian)
    test_big_endian(CMAKE_WORDS_BIGENDIAN)


    ###############################
    ################################
    ## Add Calligra helper macros ##
    ################################
    ###############################

    include(MacroCalligraAddBenchmark)

endif ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")

if ("${KF5CalendarCore_VERSION}" VERSION_GREATER 5.6.40)
    set(HAVE_QDATETIME_KCALCORE TRUE)
endif()

####################
#####################
## Define includes ##
#####################
####################

include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/interfaces)

set(PLANPLUGIN_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/plugin
)

set(PLANKUNDO2_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/kundo2
    ${CMAKE_CURRENT_BINARY_DIR}/libs/kundo2
)

set(PLANODF_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/odf
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/store
    ${CMAKE_CURRENT_BINARY_DIR}/libs/odf
    ${CMAKE_CURRENT_BINARY_DIR}/libs/store
)

set(PLANWIDGETS_INCLUDES 
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/widgetutils
    ${CMAKE_CURRENT_BINARY_DIR}/libs/widgetutils
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/widgets
    ${CMAKE_CURRENT_BINARY_DIR}/libs/widgets
)

set(PLANMAIN_INCLUDES 
    ${PLANWIDGETS_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/main
    ${CMAKE_CURRENT_BINARY_DIR}/libs/main
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/main/config
)

set(PLANKERNEL_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/kernel
    ${CMAKE_CURRENT_BINARY_DIR}/libs/kernel
)    

set(PLANMODELS_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/models
    ${CMAKE_CURRENT_BINARY_DIR}/libs/models
)    

set(PLANUI_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ui
    ${CMAKE_CURRENT_BINARY_DIR}/libs/ui
)    

set(KPLATO_INCLUDES
    ${CMAKE_CURRENT_BINARY_DIR}

    ${PLANKERNEL_INCLUDES}
    ${PLANMODELS_INCLUDES}
    ${PLANUI_INCLUDES}
    ${PLANMAIN_INCLUDES}
)

# For odf
set(RNG_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/devtools/scripts)

if(KF5Holidays_FOUND)
    add_definitions(-DHAVE_KHOLIDAYS)
endif()

if (KF5AkonadiContact_FOUND)
    # disable for now: there is a bug
    # it only works if you use kde contacts (of course) but many use other stuff, so gets dissapointed
    add_definitions(-DPLAN_KDEPIMLIBS_FOUND)
    message(WARNING "AkonadiContacs available, but funtion is disabled due to Bug 311940")
endif ()

if (PLANCHARTDEBUG)
    add_definitions(-DPLAN_CHART_DEBUG)
endif ()

add_subdirectory(servicetypes)
add_subdirectory(devtools)
add_subdirectory( libs )

add_subdirectory( templates )
add_subdirectory( pics )
add_subdirectory( toolbar )
add_subdirectory( plugins )

if(BUILD_TESTING)
    add_subdirectory( tests )
endif()

add_subdirectory( workpackage )

include_directories(${KPLATO_INCLUDES})

add_definitions(-DTRANSLATION_DOMAIN=\"calligraplan\")

########### KPlato private library ###############

set(planprivate_LIB_SRCS
    kptviewlistdocker.cpp
    kptviewlist.cpp
    kptviewlistdialog.cpp

    kptschedulesdocker.cpp

    kptconfig.cpp
    ConfigWorkVacationPanel.cpp
    ConfigProjectPanel.cpp
    kpttaskdefaultpanel.cpp
    kptworkpackageconfigpanel.cpp
    kptcolorsconfigpanel.cpp

    kptcontext.cpp

    kptfactory.cpp
    kptpart.cpp
    kptmaindocument.cpp
    kptview.cpp
#     KPtViewAdaptor.cpp
    kptprintingcontrolprivate.cpp

    kptschedulerpluginloader.cpp
    kptbuiltinschedulerplugin.cpp
    kptconfigskeleton.cpp

    kptinsertfiledlg.cpp

    about/aboutpage.cpp

    KPlatoXmlLoader.cpp
)

ki18n_wrap_ui(planprivate_LIB_SRCS
    kptviewlistaddview.ui
    kptviewlisteditview.ui
    kptviewlisteditcategory.ui

    ConfigWorkVacationPanel.ui
    ConfigProjectPanel.ui
    kptconfigtaskpanelbase.ui
    kptworkpackageconfigpanel.ui
    kptcolorsconfigpanel.ui

    kptinsertfilepanel.ui
)

kconfig_add_kcfg_files(plansettings_SRCS calligraplansettings.kcfgc)

add_library(planprivate SHARED ${planprivate_LIB_SRCS} ${plansettings_SRCS} )
generate_export_header(planprivate BASE_NAME kplato)

target_link_libraries(planprivate
    PUBLIC
        kplatokernel
        kplatomodels
        kplatoui
        planmain
    PRIVATE
        planplugin
        KF5::IconThemes
        #KF5::KHtml
)
if(KF5AkonadiContact_FOUND)
    target_link_libraries(planprivate  PRIVATE KF5::AkonadiContact)
endif()
set_target_properties(planprivate PROPERTIES VERSION ${GENERIC_PLAN_LIB_VERSION} SOVERSION ${GENERIC_PLAN_LIB_SOVERSION} )

install(TARGETS planprivate ${INSTALL_TARGETS_DEFAULT_ARGS})

########### KPlato part ###############

set(planpart_PART_SRCS kptfactoryinit.cpp )

add_library(calligraplanpart MODULE ${planpart_PART_SRCS})

#calligraplan_part_desktop_to_json(calligraplanpart planpart.desktop)
if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(calligraplanpart planpart.desktop)
else()
    kcoreaddons_desktop_to_json(calligraplanpart planpart.desktop
#         SERVICE_TYPES ${PLAN_SOURCE_DIR}/servicetypes/calligraplan_part.desktop
    )
endif()


target_link_libraries(calligraplanpart PUBLIC KF5::Parts KF5::CoreAddons PRIVATE planprivate)

install(TARGETS calligraplanpart DESTINATION ${PLUGIN_INSTALL_DIR}/calligraplan/parts)


########### KPlato executable ###############

set(calligraplan_KDEINIT_SRCS main.cpp )

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/*-apps-calligraplan.png")
ecm_add_app_icon(calligraplan_KDEINIT_SRCS ICONS ${ICONS_SRCS})

kf5_add_kdeinit_executable( calligraplan ${calligraplan_KDEINIT_SRCS})

if (APPLE)
   set_target_properties(calligraplan PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
   set_target_properties(calligraplan PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.calligra.plan")
   set_target_properties(calligraplan PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Plan")
   install( FILES ${CMAKE_CURRENT_BINARY_DIR}/calligraplan_KDEINIT_SRCS.icns DESTINATION ${BUNDLE_INSTALL_DIR}/calligraplan.app/Contents/Resources)
endif ()

target_link_libraries(kdeinit_calligraplan planmain)

install(TARGETS kdeinit_calligraplan  ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries(calligraplan kdeinit_calligraplan planmain)
install(TARGETS calligraplan  ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( FILES  calligraplan.rc calligraplan_readonly.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/calligraplan)
install( PROGRAMS  org.kde.calligraplan.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES  calligraplanrc DESTINATION ${CONFIG_INSTALL_DIR})
install(FILES calligraplansettings.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install(FILES org.kde.calligraplan.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

# TODO: with the new embedded JSON data for plugins there is no schema ATM to define extended properties
# plan_viewplugin.desktop

install(FILES
    about/top-left-plan.png
    about/main.html
    about/intro.html
    about/tips.html
    about/tutorial.html
    about/plan.css
    DESTINATION ${DATA_INSTALL_DIR}/calligraplan/about
)

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h )

#add_custom_target(apidox doc/api/gendocs.pl WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

if ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")

    ### Summarize findings ####
    feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

endif ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")



