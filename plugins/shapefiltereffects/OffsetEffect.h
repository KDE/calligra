/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef OFFSETEFFECT_H
#define OFFSETEFFECT_H

#include "KoFilterEffect.h"
#include <QPointF>

#define OffsetEffectId "feOffset"

/// An image offset effect
class OffsetEffect : public KoFilterEffect
{
public:
    OffsetEffect();

    QPointF offset() const;
    void setOffset(const QPointF &offset);

    /// reimplemented from KoFilterEffect
    QImage processImage(const QImage &image, const KoFilterEffectRenderContext &context) const override;
    /// reimplemented from KoFilterEffect
    bool load(const KoXmlElement &element, const KoFilterEffectLoadingContext &context) override;
    /// reimplemented from KoFilterEffect
    void save(KoXmlWriter &writer) override;

private:
    QPointF m_offset;
};

#endif // OFFSETEFFECT_H
