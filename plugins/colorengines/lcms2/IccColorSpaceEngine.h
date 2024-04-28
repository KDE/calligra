/*
 *  SPDX-FileCopyrightText: 2008 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KO_ICC_COLOR_SPACE_ENGINE_H_
#define _KO_ICC_COLOR_SPACE_ENGINE_H_

#include <KoColorSpaceEngine.h>

class IccColorSpaceEngine : public KoColorSpaceEngine
{
public:
    IccColorSpaceEngine();
    ~IccColorSpaceEngine() override;
    void addProfile(const QString &filename) override;
    void removeProfile(const QString &filename) override;
    KoColorConversionTransformation *createColorTransformation(const KoColorSpace *srcColorSpace,
                                                               const KoColorSpace *dstColorSpace,
                                                               KoColorConversionTransformation::Intent renderingIntent,
                                                               KoColorConversionTransformation::ConversionFlags conversionFlags) const override;
    quint32 computeColorSpaceType(const KoColorSpace *cs) const;

private:
    struct Private;
    Private *const d;
};

#endif
