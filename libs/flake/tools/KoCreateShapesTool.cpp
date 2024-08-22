/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoCreateShapesTool.h"
#include "KoCreateShapeStrategy.h"
#include "KoInteractionStrategy.h"
#include "KoInteractionTool_p.h"
#include "KoPointerEvent.h"

#include <QMouseEvent>
#include <QPainter>

class KoCreateShapesToolPrivate : public KoInteractionToolPrivate
{
public:
    KoCreateShapesToolPrivate(KoToolBase *qq, KoCanvasBase *canvas)
        : KoInteractionToolPrivate(qq, canvas)
        , newShapeProperties(nullptr)
    {
    }

    QString shapeId;
    const KoProperties *newShapeProperties;
};

KoCreateShapesTool::KoCreateShapesTool(KoCanvasBase *canvas)
    : KoInteractionTool(*(new KoCreateShapesToolPrivate(this, canvas)))
{
}

KoCreateShapesTool::~KoCreateShapesTool() = default;

void KoCreateShapesTool::paint(QPainter &painter, const KoViewConverter &converter)
{
    if (currentStrategy())
        currentStrategy()->paint(painter, converter);
}

void KoCreateShapesTool::mouseReleaseEvent(KoPointerEvent *event)
{
    KoInteractionTool::mouseReleaseEvent(event);
    Q_EMIT KoToolBase::done();
}

void KoCreateShapesTool::activate(ToolActivation, const QSet<KoShape *> &)
{
    useCursor(Qt::ArrowCursor);
}

void KoCreateShapesTool::setShapeId(const QString &id)
{
    Q_D(KoCreateShapesTool);
    d->shapeId = id;
}

QString KoCreateShapesTool::shapeId() const
{
    Q_D(const KoCreateShapesTool);
    return d->shapeId;
}

void KoCreateShapesTool::setShapeProperties(const KoProperties *properties)
{
    Q_D(KoCreateShapesTool);
    d->newShapeProperties = properties;
}

const KoProperties *KoCreateShapesTool::shapeProperties()
{
    Q_D(KoCreateShapesTool);
    return d->newShapeProperties;
}

KoInteractionStrategy *KoCreateShapesTool::createStrategy(KoPointerEvent *event)
{
    return new KoCreateShapeStrategy(this, event->point);
}
