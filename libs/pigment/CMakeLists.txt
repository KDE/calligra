project(pigmentcms)

# we have to repeat platform specifics from top-level
if (WIN32)
    include_directories(${CMAKE_SOURCE_DIR}/winquirks)
    add_definitions(-D_USE_MATH_DEFINES)
    add_definitions(-DNOMINMAX)
    set(WIN32_PLATFORM_NET_LIBS ws2_32.lib netapi32.lib)
endif ()

include_directories( ${PIGMENT_INCLUDES}  ${Boost_INCLUDE_DIR})

set(FILE_OPENEXR_SOURCES)
set(LINK_OPENEXR_LIB)
if(OpenEXR_FOUND)
    include_directories(${OPENEXR_INCLUDE_DIR})
    set(LINK_OPENEXR_LIB ${OPENEXR_LIBRARIES})
    add_definitions(${OPENEXR_DEFINITIONS})
endif()

set(LINK_VC_LIB)

if(HAVE_VC)
    include_directories(${Vc_INCLUDE_DIR})
    set(LINK_VC_LIB ${Vc_LIBRARIES})
    kde_enable_exceptions()
    ko_compile_for_all_implementations_no_scalar(__per_arch_factory_objs compositeops/KoOptimizedCompositeOpFactoryPerArch.cpp)

    message("Following objects are generated from the per-arch lib")
    message(${__per_arch_factory_objs})
    # silence warnings for using older Vc API for now, where "the tuple" should be built "with Vc::tie"
    if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
        add_definitions(-Wno-deprecated-declarations)
    endif ()
endif()

if(BUILD_TESTING)
    add_subdirectory(tests)
    add_subdirectory(benchmarks)
endif()

set(pigmentcms_SRCS
    DebugPigment.cpp
    KoBasicHistogramProducers.cpp
    KoColor.cpp
    KoColorDisplayRendererInterface.cpp
    KoColorConversionAlphaTransformation.cpp
    KoColorConversionCache.cpp
    KoColorConversions.cpp
    KoColorConversionSystem.cpp
    KoColorConversionTransformation.cpp
    KoColorConversionTransformationFactory.cpp
    KoColorModelStandardIds.cpp
    KoColorProfile.cpp
    KoColorSpace.cpp
    KoColorSpaceEngine.cpp
    KoColorSpaceFactory.cpp
    KoColorSpaceMaths.cpp
    KoColorSpaceRegistry.cpp
    KoColorTransformation.cpp
    KoColorTransformationFactory.cpp
    KoColorTransformationFactoryRegistry.cpp
    KoCompositeColorTransformation.cpp
    KoCompositeOp.cpp
    KoCompositeOpRegistry.cpp
    KoCopyColorConversionTransformation.cpp
    KoFallBackColorTransformation.cpp
    KoHistogramProducer.cpp
    KoMultipleColorConversionTransformation.cpp
    KoUniqueNumberForIdServer.cpp
    colorspaces/KoAlphaColorSpace.cpp
    colorspaces/KoLabColorSpace.cpp
    colorspaces/KoRgbU16ColorSpace.cpp
    colorspaces/KoRgbU8ColorSpace.cpp
    colorspaces/KoSimpleColorSpaceEngine.cpp
    compositeops/KoOptimizedCompositeOpFactory.cpp
    compositeops/KoOptimizedCompositeOpFactoryPerArch_Scalar.cpp
    ${__per_arch_factory_objs}
    colorprofiles/KoDummyColorProfile.cpp
    resources/KoAbstractGradient.cpp
    resources/KoColorSet.cpp
    resources/KoPattern.cpp
    resources/KoResource.cpp
    resources/KoMD5Generator.cpp
    resources/KoHashGeneratorProvider.cpp
    resources/KoStopGradient.cpp
    resources/KoSegmentGradient.cpp
)


set(PIGMENT_INSTALL_FILES
        ${CMAKE_CURRENT_BINARY_DIR}/pigment_export.h
        DebugPigment.h
        KoBasicHistogramProducers.h
        KoChannelInfo.h
        KoColor.h
        KoColorConversionTransformation.h
        KoColorConversionTransformationAbstractFactory.h
        KoColorConversionTransformationFactory.h
        KoColorModelStandardIds.h
        KoColorProfile.h
        KoColorSpace.h
        KoColorSpaceEngine.h
        KoColorSpaceFactory.h
        KoColorSpaceAbstract.h
        KoColorSpaceConstants.h
        KoColorSpaceMaths.h
        KoColorSpaceRegistry.h
        KoCmykColorSpaceTraits.h
        KoColorSpaceTraits.h
        KoGrayColorSpaceTraits.h
        KoLabColorSpaceTraits.h
        KoRgbColorSpaceTraits.h
        KoXyzColorSpaceTraits.h
        KoYcbcrColorSpaceTraits.h
        KoColorTransformation.h
        KoColorTransformationFactory.h
        KoColorTransformationFactoryRegistry.h
        KoCompositeOp.h
        KoConvolutionOp.h
        KoFallBackColorTransformation.h
        KoIntegerMaths.h
        KoLabDarkenColorTransformation.h
        KoMixColorsOp.h
        KoMixColorsOpImpl.h
        KoHistogramProducer.h
)

set (EXTRA_LIBRARIES ${LINK_OPENEXR_LIB} ${LINK_VC_LIB})

if(MSVC OR (WIN32 AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel"))
  # avoid "cannot open file 'LIBC.lib'" error
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:LIBC.LIB")
endif()

add_library(pigmentcms SHARED ${pigmentcms_SRCS})
generate_export_header(pigmentcms
    BASE_NAME pigment
    EXPORT_MACRO_NAME PIGMENTCMS_EXPORT
)

target_link_libraries(pigmentcms
    PUBLIC
        KF6::I18n
        Qt6::Xml
        Qt6::Gui
        ${EXTRA_LIBRARIES}
    PRIVATE
        koplugin
        ${WIN32_PLATFORM_NET_LIBS}
)

if (HAVE_VC AND NOT PACKAGERS_BUILD)
    set_property(TARGET pigmentcms APPEND PROPERTY COMPILE_OPTIONS "${Vc_ARCHITECTURE_FLAGS}")
endif()

set_target_properties(pigmentcms PROPERTIES
    VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION}
)

install(TARGETS pigmentcms  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

# TODO: with the new embedded JSON data for plugins there is no schema ATM to define extended properties
# pigment.desktop pigmentextension.desktop

if (SHOULD_BUILD_DEVEL_HEADERS)
install( FILES ${PIGMENT_INSTALL_FILES}
         DESTINATION ${KDE_INSTALL_INCLUDEDIR}/calligra COMPONENT Devel)
endif()
