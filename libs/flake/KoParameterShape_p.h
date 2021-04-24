/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2007, 2009 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPARAMETERSHAPE_P_H
#define KOPARAMETERSHAPE_P_H

#include "KoPathShape_p.h"

#include <QList>
#include <QPointF>

class KoParameterShapePrivate : public KoPathShapePrivate
{
public:
    explicit KoParameterShapePrivate(KoParameterShape *shape)
        : KoPathShapePrivate(shape),
        parametric(true)
    {
    }

    bool parametric;

    /// the handles that the user can grab and change
    QVector<QPointF> handles;
};

#endif
