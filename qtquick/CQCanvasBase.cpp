/*
 * This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "CQCanvasBase.h"

class CQCanvasBase::Private
{
public:
    Private() : canvasController(0), zoomController(0) { }

    QString source;
    CQCanvasController *canvasController;
    KoZoomController* zoomController;
};

CQCanvasBase::CQCanvasBase(QDeclarativeItem* parent)
    : QDeclarativeItem(parent), d(new Private)
{
}

CQCanvasBase::~CQCanvasBase()
{
    delete d;
}

CQCanvasController* CQCanvasBase::canvasController() const
{
    return d->canvasController;
}

KoZoomController* CQCanvasBase::zoomController() const
{
    return d->zoomController;
}

QString CQCanvasBase::source() const
{
    return d->source;
}

void CQCanvasBase::setSource(const QString& source)
{
    if (source != d->source) {
        d->source = source;
        openFile(d->source);
        emit sourceChanged();
    }
}

qreal CQCanvasBase::shapeTransparency() const
{
    return 0;
}

void CQCanvasBase::setShapeTransparency(qreal newTransparency)
{
    Q_UNUSED(newTransparency);
    emit shapeTransparencyChanged();
}

void CQCanvasBase::setCanvasController(CQCanvasController* controller)
{
    if (d->canvasController != controller) {
        d->canvasController = controller;
        emit canvasControllerChanged();
    }
}

void CQCanvasBase::setZoomController(KoZoomController* controller)
{
    d->zoomController = controller;
}
