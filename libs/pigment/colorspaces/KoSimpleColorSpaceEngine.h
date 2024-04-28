/*
 *  SPDX-FileCopyrightText: 2009 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KO_SIMPLE_COLOR_SPACE_ENGINE_H_
#define _KO_SIMPLE_COLOR_SPACE_ENGINE_H_

#include <KoColorSpaceEngine.h>

class KoSimpleColorSpaceEngine : public KoColorSpaceEngine
{
public:
    KoSimpleColorSpaceEngine();
    ~KoSimpleColorSpaceEngine() override;
    KoColorConversionTransformation *createColorTransformation(const KoColorSpace *srcColorSpace,
                                                               const KoColorSpace *dstColorSpace,
                                                               KoColorConversionTransformation::Intent renderingIntent,
                                                               KoColorConversionTransformation::ConversionFlags conversionFlags) const override;
    void addProfile(const QString &profile) override
    {
        Q_UNUSED(profile);
    }
    void removeProfile(const QString &profile) override
    {
        Q_UNUSED(profile);
    }
};

#endif
