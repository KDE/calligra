project(krita)

add_definitions(${KDE4_ENABLE_EXCEPTIONS})

# To avoid the confusion of an endless stream of Eigen warnings.
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
    add_definitions(-Wno-unused-local-typedefs)
endif ()

set(KRITA_DEFAULT_TEST_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sdk/tests/data/)

macro(macro_add_unittest_definitions)
  add_definitions(-DFILES_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data/")
  add_definitions(-DFILES_OUTPUT_DIR="${CMAKE_CURRENT_BINARY_DIR}")
  add_definitions(-DFILES_DEFAULT_DATA_DIR="${KRITA_DEFAULT_TEST_DATA_DIR}")
endmacro()

#
# The reason for this mode is that the Debug mode disables inlining
#
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS_KRITADEVS "-O3 -g" CACHE STRING "" FORCE)
endif()

if(MSVC OR (WIN32 AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel"))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
    if (USE_BREAKPAD)
        include_directories(${CMAKE_SOURCE_DIR}/3rdparty/google-breakpad/src)
    endif ()
#    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:16777216") # for g'mic...
#    set(CMAKE_CXX_STACK_SIZE "999999999")
endif()

macro_optional_find_package(FFTW3)
macro_log_feature(FFTW3_FOUND "FFTW3" "A fast, free C FFT library" "http://www.fftw.org/" FALSE "" "Required by the Krita for fast convolution operators")
macro_bool_to_01(FFTW3_FOUND HAVE_FFTW3)
configure_file(image/config_convolution.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/image/config_convolution.h)

include(CheckFunctionExists)

macro_optional_find_package(GLEW)
macro_bool_to_01(GLEW_FOUND HAVE_GLEW)
configure_file(config-glew.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-glew.h )
macro_log_feature(HAVE_GLEW "GLEW" "The OpenGL Extension Wrangler Library" "http://glew.sourceforge.net" FALSE "" "Required by the Krita shader filter plugin")

if(HAVE_GLEW)
    message(STATUS "GLEW found!")
else()
    message(STATUS "GLEW NOT found - krita will not be able to use OpenGL.")
endif()

if(HAVE_OPENGL)
    message(STATUS "OpenGL found -- krita will be able  to use OpenGL for hardware accelerated rendering.")
else()
    message(STATUS "OpenGL NOT found - krita will not be able to use OpenGL for hardware accelerated rendering.")
endif()

macro_optional_find_package(OCIO)
macro_log_feature(OCIO_FOUND "OCIO" "The OpenColorIO Library" "http://www.opencolorio.org" FALSE "" "Required by the Krita LUT docker")
macro_bool_to_01(OCIO_FOUND HAVE_OCIO)
configure_file(config-ocio.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-ocio.h )

if(UNIX)
    set(CMAKE_REQUIRED_LIBRARIES "${CMAKE_REQUIRED_LIBRARIES};m")
endif()

check_function_exists(powf HAVE_POWF)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-powf.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-powf.h)

set(LINK_OPENEXR_LIB)
if(OPENEXR_FOUND)
    include_directories(${OPENEXR_INCLUDE_DIR})
    set(LINK_OPENEXR_LIB ${OPENEXR_LIBRARIES})
    add_definitions(${OPENEXR_DEFINITIONS})
endif()

if(WIN32)
    if(MSVC)
        # C4522: 'class' : multiple assignment operators specified
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4522")
    endif()
endif()

set(KRITA_INCLUDES
    # For config-*.h
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/image

    ${CMAKE_SOURCE_DIR}/krita/image
    ${CMAKE_SOURCE_DIR}/krita/image/brushengine
    ${CMAKE_SOURCE_DIR}/krita/image/tiles
    ${CMAKE_SOURCE_DIR}/krita/libbrush
    ${CMAKE_SOURCE_DIR}/krita/libsketch
    ${CMAKE_SOURCE_DIR}/krita/libcolor
    ${CMAKE_SOURCE_DIR}/krita/ui
    ${CMAKE_SOURCE_DIR}/krita/ui/canvas
    ${CMAKE_SOURCE_DIR}/krita/ui/tool
    ${CMAKE_SOURCE_DIR}/krita/ui/flake
    ${CMAKE_SOURCE_DIR}/krita/ui/widgets

    # For generated files from .ui
    ${CMAKE_BINARY_DIR}/krita/ui

    ${PIGMENT_INCLUDES}
    ${BASICFLAKES_INCLUDES}
    ${TEXTLAYOUT_INCLUDES}
    ${CMAKE_SOURCE_DIR}/libs/widgets
    ${CMAKE_SOURCE_DIR}/libs/widgets/resources
    ${CMAKE_SOURCE_DIR}/libs/widgets/colorwidgets
    ${CMAKE_SOURCE_DIR}/libs/widgetutils

    ${EIGEN3_INCLUDE_DIR}
    ${Vc_INCLUDE_DIR}

    # 'Export' this for use by filters
    CACHE INTERNAL ""
)

include_directories(${KRITA_INCLUDES})

if(HAVE_OPENGL)
    include_directories(${OPENGL_INCLUDE_DIR})
    if(HAVE_GLEW)
        include_directories(${GLEW_INCLUDE_DIRS})
    endif()
endif()

add_subdirectory( image )
add_subdirectory( libbrush )
add_subdirectory( libcolor )
add_subdirectory( ui )
add_subdirectory( dtd )
add_subdirectory( plugins )
add_subdirectory( data )
add_subdirectory( pics )
add_subdirectory( integration )

if (WIN32 AND USE_BREAKPAD)
        add_subdirectory( crashreporter )
endif ()

if (NOT WIN32)
    add_subdirectory( benchmarks )
endif ()

if (${QTVERSION} VERSION_GREATER 4.7.0 AND NOT APPLE AND HAVE_OPENGL)
    add_subdirectory(libsketch)
endif ()

set(krita_SRCS main.cc)

if(WIN32 AND USE_BREAKPAD)
    set(krita_SRCS ${krita_SRCS} kis_crash_handler.cpp)
    set(BREAKPAD_LIBS breakpad)
endif()

kde4_add_app_icon(krita_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/app/hi*-app-calligrakrita.png")

kde4_add_executable(krita ${krita_SRCS})
target_link_libraries(krita kritaui ${BREAKPAD_LIBS})

if (Q_WS_MAC)
    set_target_properties(krita PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
    set_target_properties(krita PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.krita")
    set_target_properties(krita PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Krita")
endif ()

install(TARGETS krita ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS krita.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES  krita.rc DESTINATION ${DATA_INSTALL_DIR}/krita)
install(FILES  krita.action DESTINATION ${DATA_INSTALL_DIR}/krita/actions)
install(FILES  krita.appdata.xml DESTINATION ${SHARE_INSTALL_PREFIX}/appdata/ )


# build kritasketch and kritagemini executables
if (${QTVERSION} VERSION_GREATER 4.7.0 AND NOT APPLE AND HAVE_OPENGL)

    # Sketch
    set(kritasketch_SRCS main.cc)
    
    if (WIN32)
        option(ENABLE_SKETCH_STARTUP_MAGIC "Enable Sketch's windows startup magic" ON)

        if (ENABLE_SKETCH_STARTUP_MAGIC)
            add_definitions(-DSKETCH_ENABLE_STARTUP_MAGIC)
        endif ()
    endif ()

    if(WIN32 AND USE_BREAKPAD)
        set(kritasketch_SRCS ${krita_SRCS} kis_crash_handler.cpp)
        set(BREAKPAD_LIBS breakpad)
    endif()
    
    if (WIN32)
        set(kritasketch_SRCS ${kritasketch_SRCS} sketch/KritaSketchWin.rc)
    else ()
        kde4_add_app_icon(kritasketch_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/app/hi*-app-kritasketch.png")
    endif()
    
    kde4_add_executable(kritasketch ${krita_SRCS})
    target_link_libraries(kritasketch kritaui ${BREAKPAD_LIBS} kritasketchlib)
    
    install(TARGETS  kritasketch ${INSTALL_TARGETS_DEFAULT_ARGS})
    install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/sketch/kritasketch.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
    install(FILES    ${CMAKE_CURRENT_SOURCE_DIR}/sketch/kritasketchpanelsrc DESTINATION ${CONFIG_INSTALL_DIR} )
    install(FILES    ${CMAKE_CURRENT_SOURCE_DIR}/sketch/kritasketch.qml     DESTINATION ${DATA_INSTALL_DIR}/krita)

    
    # Gemini
    set(kritagemini_SRCS main.cc)

    if (WIN32)
        option(ENABLE_GEMINI_STARTUP_MAGIC "Enable Gemini's windows startup magic" ON)

        if (ENABLE_GEMINI_STARTUP_MAGIC)
            add_definitions(-DGEMINI_ENABLE_STARTUP_MAGIC)
        endif ()
    endif ()

    if(WIN32 AND USE_BREAKPAD)
        set(kritagemini_SRCS ${krita_SRCS} kis_crash_handler.cpp)
        set(BREAKPAD_LIBS breakpad)
    endif()

    if (WIN32)
        set(kritagemini_SRCS ${kritagemini_SRCS} gemini/KritaGeminiWin.rc)
    else ()
        kde4_add_app_icon(kritagemini_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/app/hi*-app-kritagemini.png")
    endif()

    kde4_add_executable(kritagemini ${krita_SRCS})
    target_link_libraries(kritagemini kritaui ${BREAKPAD_LIBS} kritasketchlib)

    install(TARGETS kritagemini ${INSTALL_TARGETS_DEFAULT_ARGS})
    install(PROGRAMS  ${CMAKE_CURRENT_SOURCE_DIR}/gemini/kritagemini.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
    install(FILES     ${CMAKE_CURRENT_SOURCE_DIR}/gemini/kritageminipanelsrc DESTINATION ${CONFIG_INSTALL_DIR} )
    install(FILES     ${CMAKE_CURRENT_SOURCE_DIR}/gemini/kritagemini.qml     DESTINATION ${DATA_INSTALL_DIR}/krita)



endif()
