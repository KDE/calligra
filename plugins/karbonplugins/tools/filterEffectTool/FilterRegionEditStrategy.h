/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef FILTERREGIONEDITSTRATEGY_H
#define FILTERREGIONEDITSTRATEGY_H

#include "KarbonFilterEffectsTool.h"
#include <KoInteractionStrategy.h>

#include <QRectF>

class KoShape;
class KoFilterEffect;

class FilterRegionEditStrategy : public KoInteractionStrategy
{
public:
    FilterRegionEditStrategy(KoToolBase *parent, KoShape *shape, KoFilterEffect *effect, KarbonFilterEffectsTool::EditMode mode);

    // reimplemented from KoInteractionStrategy
    void handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers) override;
    // reimplemented from KoInteractionStrategy
    KUndo2Command *createCommand() override;
    // reimplemented from KoInteractionStrategy
    void finishInteraction(Qt::KeyboardModifiers modifiers) override;
    // reimplemented from KoInteractionStrategy
    void paint(QPainter &painter, const KoViewConverter &converter) override;

private:
    KoFilterEffect *m_effect;
    KoShape *m_shape;
    QRectF m_sizeRect;
    QRectF m_filterRect;
    KarbonFilterEffectsTool::EditMode m_editMode;
    QPointF m_lastPosition;
};

#endif // FILTERREGIONEDITSTRATEGY_H
