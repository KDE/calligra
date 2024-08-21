/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006-2007, 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOINTERACTIONTOOLPRIVATE_H
#define KOINTERACTIONTOOLPRIVATE_H

#include "KoInteractionStrategy.h"
#include "KoToolBase_p.h"

class KoInteractionToolPrivate : public KoToolBasePrivate
{
public:
    KoInteractionToolPrivate(KoToolBase *qq, KoCanvasBase *canvas)
        : KoToolBasePrivate(qq, canvas)
        , currentStrategy(nullptr)
    {
    }

    ~KoInteractionToolPrivate()
    {
        delete currentStrategy;
    }

    QPointF lastPoint;
    KoInteractionStrategy *currentStrategy;
};

#endif
