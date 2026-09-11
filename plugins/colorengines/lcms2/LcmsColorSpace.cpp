/*
 *  SPDX-FileCopyrightText: 2002 Patrick Julien <freak@codepimps.org>
 *  SPDX-FileCopyrightText: 2005-2006 C. Boemann <cbo@boemann.dk>
 *  SPDX-FileCopyrightText: 2004, 2006-2007 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "LcmsColorSpace.h"
#include "KoColorConversionTransformationFactory.h"
#include "KoColorModelStandardIds.h"

#include "QDebug"

cmsHPROFILE KoLcmsDefaultTransformations::s_RGBProfile = nullptr;
QMap<QString, QMap<LcmsColorProfileContainer *, KoLcmsDefaultTransformations *>> KoLcmsDefaultTransformations::s_transformations;

namespace
{
struct DefaultTransformationsCleanup {
    ~DefaultTransformationsCleanup()
    {
        for (auto &transformations : KoLcmsDefaultTransformations::s_transformations) {
            for (KoLcmsDefaultTransformations *transformation : transformations) {
                if (transformation->fromRGB) {
                    cmsDeleteTransform(transformation->fromRGB);
                }
                if (transformation->toRGB) {
                    cmsDeleteTransform(transformation->toRGB);
                }
                delete transformation;
            }
        }
        KoLcmsDefaultTransformations::s_transformations.clear();

        if (KoLcmsDefaultTransformations::s_RGBProfile) {
            cmsCloseProfile(KoLcmsDefaultTransformations::s_RGBProfile);
            KoLcmsDefaultTransformations::s_RGBProfile = nullptr;
        }
    }
};

DefaultTransformationsCleanup s_defaultTransformationsCleanup;
}

// -- LcmsColorSpaceFactory --
QList<KoColorConversionTransformationFactory *> LcmsColorSpaceFactory::colorConversionLinks() const
{
    return QList<KoColorConversionTransformationFactory *>();
}

KoColorProfile *LcmsColorSpaceFactory::createColorProfile(const QByteArray &rawData) const
{
    return new IccColorProfile(rawData);
}
