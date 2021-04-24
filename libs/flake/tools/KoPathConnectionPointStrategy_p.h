/* This file is part of the KDE project
 * 
 * SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@kde.org>
 * SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPATHCONNECTIONPOINTSTRATEGYPRIVATE_H
#define KOPATHCONNECTIONPOINTSTRATEGYPRIVATE_H

#include "KoParameterChangeStrategy_p.h"
#include "KoConnectionShape.h"

class KoPathConnectionPointStrategyPrivate : public KoParameterChangeStrategyPrivate
{
public:
    KoPathConnectionPointStrategyPrivate(KoToolBase* owner, KoConnectionShape* connectionShape, int handle)
        : KoParameterChangeStrategyPrivate(owner, connectionShape, handle)
        , connectionShape(connectionShape)
        , oldConnectionShape(0), oldConnectionId(-1)
        , newConnectionShape(0), newConnectionId(-1)
    {
        if (handleId == 0) {
            oldConnectionShape = connectionShape->firstShape();
            oldConnectionId = connectionShape->firstConnectionId();
        } else {
            oldConnectionShape = connectionShape->secondShape();
            oldConnectionId = connectionShape->secondConnectionId();
        }
    }

    KoConnectionShape *connectionShape; ///< the parametric shape we are working on
    KoShape *oldConnectionShape;
    int oldConnectionId;
    KoShape *newConnectionShape;
    int newConnectionId;
};

#endif // KOPATHCONNECTIONPOINTSTRATEGYPRIVATE_H
