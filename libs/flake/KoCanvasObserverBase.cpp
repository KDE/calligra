/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoCanvasObserverBase.h"

#include <QString>

class KoCanvasObserverBasePrivate
{
public:
    KoCanvasObserverBasePrivate()
        : canvas(nullptr)
    {
    }

    ~KoCanvasObserverBasePrivate() = default;

    KoCanvasBase *canvas;
};

KoCanvasObserverBase::KoCanvasObserverBase()
    : d(new KoCanvasObserverBasePrivate)
{
}

KoCanvasObserverBase::~KoCanvasObserverBase()
{
    delete d;
}

QString KoCanvasObserverBase::observerName() const
{
    return QString();
}

void KoCanvasObserverBase::setObservedCanvas(KoCanvasBase *canvas)
{
    d->canvas = canvas;
    setCanvas(canvas);
}

void KoCanvasObserverBase::unsetObservedCanvas()
{
    d->canvas = nullptr;
    unsetCanvas();
}

KoCanvasBase *KoCanvasObserverBase::observedCanvas() const
{
    return d->canvas;
}
