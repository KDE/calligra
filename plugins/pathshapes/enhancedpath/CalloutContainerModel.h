/* This file is part of the KDE project
 * Copyright (C) 2018 Dag Andersen <danders@get2net.dk>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef CALLOUTCONTAINERMODEL_H
#define CALLOUTCONTAINERMODEL_H

#include "KoShapeContainerDefaultModel.h"

#include <QHash>
#include <QSizeF>
#include <QPointF>
#include <QTransform>

class KoShapeContainer;
class CalloutShape;
class PathShape;
class KoShape;

class CalloutContainerModel : public KoShapeContainerDefaultModel
{
public:
    CalloutContainerModel();

    void containerChanged(KoShapeContainer *container, KoShape::ChangeType type) override;

    void childChanged(KoShape *shape, KoShape::ChangeType type) override;

    bool isChildLocked(const KoShape *child) const override;

    void setIgnore(KoShape *shape, bool state);

    bool ignore(KoShape *shape) const;

    void resizePath(PathShape *path, const QPointF &newPos, const QSizeF &newSize);

private:
    QTransform m_prevTrans;
    QSizeF m_prevSize;
    QPointF m_prevPosition;
    bool m_resizing;
    QHash<KoShape*, bool> m_ignore;
};

#endif /* CALLOUTCONTAINERMODEL_H */
