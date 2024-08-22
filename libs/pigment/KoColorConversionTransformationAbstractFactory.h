/*
 *  SPDX-FileCopyrightText: 2008 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KO_COLOR_CONVERSION_TRANSFORMATION_ABSTRACT_FACTORY_H_
#define _KO_COLOR_CONVERSION_TRANSFORMATION_ABSTRACT_FACTORY_H_

#include "pigment_export.h"

#include <KoColorConversionTransformation.h>

class PIGMENTCMS_EXPORT KoColorConversionTransformationAbstractFactory
{
public:
    KoColorConversionTransformationAbstractFactory() = default;
    virtual ~KoColorConversionTransformationAbstractFactory() = default;
    /**
     * Creates a color transformation between the source color space and the destination
     * color space.
     *
     * @param srcColorSpace source color space
     * @param dstColorSpace destination color space
     * @param renderingIntent the rendering intent
     * @param conversionFlags the conversion flags
     */
    virtual KoColorConversionTransformation *createColorTransformation(const KoColorSpace *srcColorSpace,
                                                                       const KoColorSpace *dstColorSpace,
                                                                       KoColorConversionTransformation::Intent renderingIntent,
                                                                       KoColorConversionTransformation::ConversionFlags conversionFlags) const = 0;
};

#endif
