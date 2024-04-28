/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef CALLOUTCONTAINERMODEL_H
#define CALLOUTCONTAINERMODEL_H

#include "KoShapeContainerDefaultModel.h"

#include <QHash>
#include <QPointF>
#include <QSizeF>
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
    QHash<KoShape *, bool> m_ignore;
};

#endif /* CALLOUTCONTAINERMODEL_H */
