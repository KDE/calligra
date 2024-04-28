/*
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KO_COLOR_CONVERSION_ALPHA_TRANSFORMATION_H_
#define _KO_COLOR_CONVERSION_ALPHA_TRANSFORMATION_H_

#include "KoColorConversionTransformation.h"
#include "KoColorConversionTransformationFactory.h"

/**
 * Create converter from the alpha color space to any color space
 * This class is for use by the KoColorConversionSystemn, no reason
 * to use it directly.
 */
class KoColorConversionFromAlphaTransformationFactory : public KoColorConversionTransformationFactory
{
public:
    KoColorConversionFromAlphaTransformationFactory(const QString &_dstModelId, const QString &_dstDepthId, const QString &_dstProfileName);
    KoColorConversionTransformation *createColorTransformation(const KoColorSpace *srcColorSpace,
                                                               const KoColorSpace *dstColorSpace,
                                                               KoColorConversionTransformation::Intent renderingIntent,
                                                               KoColorConversionTransformation::ConversionFlags conversionFlags) const override;
    bool conserveColorInformation() const override;
    bool conserveDynamicRange() const override;
};

/**
 * Create converter to the alpha color space to any color space
 * This class is for use by the KoColorConversionSystemn, no reason
 * to use it directly.
 */
class KoColorConversionToAlphaTransformationFactory : public KoColorConversionTransformationFactory
{
public:
    KoColorConversionToAlphaTransformationFactory(const QString &_dstModelId, const QString &_dstDepthId, const QString &_srcProfileName);
    KoColorConversionTransformation *createColorTransformation(const KoColorSpace *srcColorSpace,
                                                               const KoColorSpace *dstColorSpace,
                                                               KoColorConversionTransformation::Intent renderingIntent,
                                                               KoColorConversionTransformation::ConversionFlags conversionFlags) const override;
    bool conserveColorInformation() const override;
    bool conserveDynamicRange() const override;
};

#endif
