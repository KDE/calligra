/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef MERGEEFFECT_H
#define MERGEEFFECT_H

#include "KoFilterEffect.h"

#define MergeEffectId "feMerge"

/// A gaussian blur effect
class MergeEffect : public KoFilterEffect
{
public:
    MergeEffect();

    /// reimplemented from KoFilterEffect
    QImage processImage(const QImage &image, const KoFilterEffectRenderContext &context) const override;
    /// reimplemented from KoFilterEffect
    QImage processImages(const QVector<QImage> &images, const KoFilterEffectRenderContext &context) const override;
    /// reimplemented from KoFilterEffect
    bool load(const KoXmlElement &element, const KoFilterEffectLoadingContext &context) override;
    /// reimplemented from KoFilterEffect
    void save(KoXmlWriter &writer) override;
};

#endif // MERGEEFFECT_H
