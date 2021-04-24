/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KOSELECTIONPRIVATE_H
#define KOSELECTIONPRIVATE_H

#include "KoShape_p.h"

class KoShapeGroup;

class KoSelectionPrivate : public KoShapePrivate
{
public:
    explicit KoSelectionPrivate(KoSelection *parent)
        : KoShapePrivate(parent), eventTriggered(false), activeLayer(0), q(parent) {}
    QList<KoShape*> selectedShapes;
    bool eventTriggered;

    KoShapeLayer *activeLayer;

    void requestSelectionChangedEvent();
    void selectGroupChildren(KoShapeGroup *group);
    void deselectGroupChildren(KoShapeGroup *group);

    void selectionChangedEvent();

    QRectF sizeRect();

    KoSelection *q;
    QRectF globalBound;
};

#endif
