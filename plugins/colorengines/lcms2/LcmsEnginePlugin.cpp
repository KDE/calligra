/*
 *  SPDX-FileCopyrightText: 2003 Patrick Julien <freak@codepimps.org>
 *  SPDX-FileCopyrightText: 2004, 2010 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2011 Srikanth Tiyyagura <srikanth.tulasiram@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#include "LcmsEnginePlugin.h"

#include <QHash>

#include <QDir>
#include <QStringList>

#include <KLocalizedString>
#include <KPluginFactory>
#include <KoResourcePaths.h>
#include <QDebug>

#include <KoBasicHistogramProducers.h>
#include <KoColorSpace.h>
#include <KoColorSpaceEngine.h>
#include <KoColorSpaceRegistry.h>

#include "IccColorSpaceEngine.h"
#include "colorprofiles/LcmsColorProfileContainer.h"

#include "colorspaces/cmyk_f32/CmykF32ColorSpace.h"
#include "colorspaces/cmyk_u16/CmykU16ColorSpace.h"
#include "colorspaces/cmyk_u8/CmykU8ColorSpace.h"

#include "colorspaces/gray_f32/GrayF32ColorSpace.h"
#include "colorspaces/gray_u16/GrayU16ColorSpace.h"
#include "colorspaces/gray_u8/GrayU8ColorSpace.h"

#include "colorspaces/lab_f32/LabF32ColorSpace.h"
#include "colorspaces/lab_u16/LabColorSpace.h"
#include "colorspaces/lab_u8/LabU8ColorSpace.h"

#include "colorspaces/xyz_f32/XyzF32ColorSpace.h"
#include "colorspaces/xyz_u16/XyzU16ColorSpace.h"
#include "colorspaces/xyz_u8/XyzU8ColorSpace.h"

#include "colorspaces/rgb_f32/RgbF32ColorSpace.h"
#include "colorspaces/rgb_u16/RgbU16ColorSpace.h"
#include "colorspaces/rgb_u8/RgbU8ColorSpace.h"

#include "colorspaces/ycbcr_f32/YCbCrF32ColorSpace.h"
#include "colorspaces/ycbcr_u16/YCbCrU16ColorSpace.h"
#include "colorspaces/ycbcr_u8/YCbCrU8ColorSpace.h"

#include <KoConfig.h>
#ifdef HAVE_OPENEXR
#include <half.h>

#include "colorspaces/gray_f16/GrayF16ColorSpace.h"
#include "colorspaces/rgb_f16/RgbF16ColorSpace.h"
#include "colorspaces/xyz_f16/XyzF16ColorSpace.h"

#endif
void lcms2LogErrorHandlerFunction(cmsContext /*ContextID*/, cmsUInt32Number ErrorCode, const char *Text)
{
    qCritical() << "Lcms2 error: " << ErrorCode << Text;
}

K_PLUGIN_FACTORY_WITH_JSON(PluginFactory, "kolcmsengine.json", registerPlugin<LcmsEnginePlugin>();)

LcmsEnginePlugin::LcmsEnginePlugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    // Set the lmcs error reporting function
    cmsSetLogErrorHandler(&lcms2LogErrorHandlerFunction);

    KoColorSpaceRegistry *registry = KoColorSpaceRegistry::instance();

    // Initialise color engine
    KoColorSpaceEngineRegistry::instance()->add(new IccColorSpaceEngine);

    // prepare a list of the ICC profiles
    KoResourcePaths::addResourceType("icc_profiles", "data", "color/icc");

    QStringList profileFilenames;
    profileFilenames += KoResourcePaths::findAllResources("icc_profiles", "*.icm", KoResourcePaths::Recursive | KoResourcePaths::NoDuplicates);
    profileFilenames += KoResourcePaths::findAllResources("icc_profiles", "*.ICM", KoResourcePaths::Recursive | KoResourcePaths::NoDuplicates);
    profileFilenames += KoResourcePaths::findAllResources("icc_profiles", "*.ICC", KoResourcePaths::Recursive | KoResourcePaths::NoDuplicates);
    profileFilenames += KoResourcePaths::findAllResources("icc_profiles", "*.icc", KoResourcePaths::Recursive | KoResourcePaths::NoDuplicates);

    // Load the profiles
    if (!profileFilenames.empty()) {
        KoColorProfile *profile = nullptr;
        for (QStringList::Iterator it = profileFilenames.begin(); it != profileFilenames.end(); ++it) {
            profile = new IccColorProfile(*it);
            Q_CHECK_PTR(profile);

            profile->load();
            if (profile->valid()) {
                // qDebug() << "Valid profile : " << profile->fileName() << profile->name();
                registry->addProfileToMap(profile);
            } else {
                qDebug() << "Invalid profile : " << profile->fileName() << profile->name();
                delete profile;
            }
        }
    }

    // ------------------- LAB ---------------------------------

    KoColorProfile *labProfile = LcmsColorProfileContainer::createFromLcmsProfile(cmsCreateLab2Profile(nullptr));
    registry->addProfile(labProfile);

    registry->add(new LabU8ColorSpaceFactory());
    registry->add(new LabU16ColorSpaceFactory());
    registry->add(new LabF32ColorSpaceFactory());

    KoHistogramProducerFactoryRegistry::instance()->add(
        new KoBasicHistogramProducerFactory<KoBasicU8HistogramProducer>(KoID("LABAU8HISTO", i18n("L*a*b* Histogram")),
                                                                        LABAColorModelID.id(),
                                                                        Integer8BitsColorDepthID.id()));

    KoHistogramProducerFactoryRegistry::instance()->add(
        new KoBasicHistogramProducerFactory<KoBasicU16HistogramProducer>(KoID("LABAU16HISTO", i18n("L*a*b* Histogram")),
                                                                         LABAColorModelID.id(),
                                                                         Integer16BitsColorDepthID.id()));

    KoHistogramProducerFactoryRegistry::instance()->add(
        new KoBasicHistogramProducerFactory<KoBasicF32HistogramProducer>(KoID("LABAF32HISTO", i18n("L*a*b* Histogram")),
                                                                         LABAColorModelID.id(),
                                                                         Float32BitsColorDepthID.id()));

    // ------------------- RGB ---------------------------------

    KoColorProfile *rgbProfile = LcmsColorProfileContainer::createFromLcmsProfile(cmsCreate_sRGBProfile());
    registry->addProfile(rgbProfile);

    registry->add(new RgbU8ColorSpaceFactory());
    registry->add(new RgbU16ColorSpaceFactory());
#ifdef HAVE_OPENEXR
    registry->add(new RgbF16ColorSpaceFactory());
#endif
    registry->add(new RgbF32ColorSpaceFactory());

    KoHistogramProducerFactoryRegistry::instance()->add(
        new KoBasicHistogramProducerFactory<KoBasicU8HistogramProducer>(KoID("RGBU8HISTO", i18n("RGB8 Histogram")),
                                                                        RGBAColorModelID.id(),
                                                                        Integer8BitsColorDepthID.id()));

    KoHistogramProducerFactoryRegistry::instance()->add(
        new KoBasicHistogramProducerFactory<KoBasicU16HistogramProducer>(KoID("RGBU16HISTO", i18n("RGB16 Histogram")),
                                                                         RGBAColorModelID.id(),
                                                                         Integer16BitsColorDepthID.id()));

#ifdef HAVE_OPENEXR
    KoHistogramProducerFactoryRegistry::instance()->add(
        new KoBasicHistogramProducerFactory<KoBasicF16HalfHistogramProducer>(KoID("RGBF16HISTO", i18n("RGBF16 Histogram")),
                                                                             RGBAColorModelID.id(),
                                                                             Float16BitsColorDepthID.id()));
#endif

    KoHistogramProducerFactoryRegistry::instance()->add(
        new KoBasicHistogramProducerFactory<KoBasicF32HistogramProducer>(KoID("RGF328HISTO", i18n("RGBF32 Histogram")),
                                                                         RGBAColorModelID.id(),
                                                                         Float32BitsColorDepthID.id()));

    // ------------------- GRAY ---------------------------------

    cmsToneCurve *Gamma = cmsBuildGamma(nullptr, 2.2);
    cmsHPROFILE hProfile = cmsCreateGrayProfile(cmsD50_xyY(), Gamma);
    cmsFreeToneCurve(Gamma);
    KoColorProfile *defProfile = LcmsColorProfileContainer::createFromLcmsProfile(hProfile);
    registry->addProfile(defProfile);

    registry->add(new GrayAU8ColorSpaceFactory());
    registry->add(new GrayAU16ColorSpaceFactory());
#ifdef HAVE_OPENEXR
    registry->add(new GrayF16ColorSpaceFactory());
#endif
    registry->add(new GrayF32ColorSpaceFactory());

    KoHistogramProducerFactoryRegistry::instance()->add(
        new KoBasicHistogramProducerFactory<KoBasicU8HistogramProducer>(KoID("GRAYA8HISTO", i18n("GRAY/Alpha8 Histogram")),
                                                                        GrayAColorModelID.id(),
                                                                        Integer8BitsColorDepthID.id()));

    KoHistogramProducerFactoryRegistry::instance()->add(
        new KoBasicHistogramProducerFactory<KoBasicU16HistogramProducer>(KoID("GRAYA16HISTO", i18n("GRAY/Alpha16 Histogram")),
                                                                         GrayAColorModelID.id(),
                                                                         Integer16BitsColorDepthID.id()));
#ifdef HAVE_OPENEXR
    KoHistogramProducerFactoryRegistry::instance()->add(
        new KoBasicHistogramProducerFactory<KoBasicF16HalfHistogramProducer>(KoID("GRAYF16HISTO", i18n("GRAYF16 Histogram")),
                                                                             GrayAColorModelID.id(),
                                                                             Float16BitsColorDepthID.id()));
#endif

    KoHistogramProducerFactoryRegistry::instance()->add(
        new KoBasicHistogramProducerFactory<KoBasicF32HistogramProducer>(KoID("GRAYAF32HISTO", i18n("GRAY/Alpha 32 float Histogram")),
                                                                         GrayAColorModelID.id(),
                                                                         Float32BitsColorDepthID.id()));

    // ------------------- CMYK ---------------------------------

    registry->add(new CmykU8ColorSpaceFactory());
    registry->add(new CmykU16ColorSpaceFactory());
    registry->add(new CmykF32ColorSpaceFactory());

    KoHistogramProducerFactoryRegistry::instance()->add(
        new KoBasicHistogramProducerFactory<KoBasicU8HistogramProducer>(KoID("CMYK8HISTO", i18n("CMYK8 Histogram")),
                                                                        CMYKAColorModelID.id(),
                                                                        Integer8BitsColorDepthID.id()));

    KoHistogramProducerFactoryRegistry::instance()->add(
        new KoBasicHistogramProducerFactory<KoBasicU16HistogramProducer>(KoID("CMYK16HISTO", i18n("CMYK16 Histogram")),
                                                                         CMYKAColorModelID.id(),
                                                                         Integer16BitsColorDepthID.id()));

    KoHistogramProducerFactoryRegistry::instance()->add(
        new KoBasicHistogramProducerFactory<KoBasicF32HistogramProducer>(KoID("CMYKF32HISTO", i18n("CMYK F32 Histogram")),
                                                                         CMYKAColorModelID.id(),
                                                                         Float32BitsColorDepthID.id()));

    // ------------------- XYZ ---------------------------------

    KoColorProfile *xyzProfile = LcmsColorProfileContainer::createFromLcmsProfile(cmsCreateXYZProfile());
    registry->addProfile(xyzProfile);

    registry->add(new XyzU8ColorSpaceFactory());
    registry->add(new XyzU16ColorSpaceFactory());
#ifdef HAVE_OPENEXR
    registry->add(new XyzF16ColorSpaceFactory());
#endif
    registry->add(new XyzF32ColorSpaceFactory());

    KoHistogramProducerFactoryRegistry::instance()->add(
        new KoBasicHistogramProducerFactory<KoBasicU8HistogramProducer>(KoID("XYZ8HISTO", i18n("XYZ8 Histogram")),
                                                                        XYZAColorModelID.id(),
                                                                        Integer8BitsColorDepthID.id()));

    KoHistogramProducerFactoryRegistry::instance()->add(
        new KoBasicHistogramProducerFactory<KoBasicU16HistogramProducer>(KoID("XYZ16HISTO", i18n("XYZ16 Histogram")),
                                                                         XYZAColorModelID.id(),
                                                                         Integer16BitsColorDepthID.id()));

#ifdef HAVE_OPENEXR
    KoHistogramProducerFactoryRegistry::instance()->add(
        new KoBasicHistogramProducerFactory<KoBasicF32HistogramProducer>(KoID("XYZF16HISTO", i18n("XYZF16 Histogram")),
                                                                         XYZAColorModelID.id(),
                                                                         Float16BitsColorDepthID.id()));
#endif

    KoHistogramProducerFactoryRegistry::instance()->add(
        new KoBasicHistogramProducerFactory<KoBasicF32HistogramProducer>(KoID("XYZF32HISTO", i18n("XYZF32 Histogram")),
                                                                         XYZAColorModelID.id(),
                                                                         Float32BitsColorDepthID.id()));

    // ------------------- YCBCR ---------------------------------

    //    KoColorProfile *yCbCrProfile = LcmsColorProfileContainer::createFromLcmsProfile(cmsCreateYCBCRProfile());
    //    registry->addProfile(yCbCrProfile);

    registry->add(new YCbCrU8ColorSpaceFactory());
    registry->add(new YCbCrU16ColorSpaceFactory());
    registry->add(new YCbCrF32ColorSpaceFactory());

    KoHistogramProducerFactoryRegistry::instance()->add(
        new KoBasicHistogramProducerFactory<KoBasicU8HistogramProducer>(KoID("YCBCR8HISTO", i18n("YCBCR8 Histogram")),
                                                                        YCbCrAColorModelID.id(),
                                                                        Integer8BitsColorDepthID.id()));

    KoHistogramProducerFactoryRegistry::instance()->add(
        new KoBasicHistogramProducerFactory<KoBasicU16HistogramProducer>(KoID("YCBCR16HISTO", i18n("YCBCR16 Histogram")),
                                                                         YCbCrAColorModelID.id(),
                                                                         Integer16BitsColorDepthID.id()));

    KoHistogramProducerFactoryRegistry::instance()->add(
        new KoBasicHistogramProducerFactory<KoBasicF32HistogramProducer>(KoID("YCBCRF32HISTO", i18n("YCBCRF32 Histogram")),
                                                                         YCbCrAColorModelID.id(),
                                                                         Float32BitsColorDepthID.id()));

    // Add profile alias for default profile from lcms1
    registry->addProfileAlias("sRGB built-in - (lcms internal)", "sRGB built-in");
    registry->addProfileAlias("gray built-in - (lcms internal)", "gray built-in");
    registry->addProfileAlias("Lab identity built-in - (lcms internal)", "Lab identity built-in");
    registry->addProfileAlias("XYZ built-in - (lcms internal)", "XYZ identity built-in");
}

#include <LcmsEnginePlugin.moc>
