/* This file is part of the KDE project
 * Copyright ( C ) 2010 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <KoPAViewBase.h>
#include <KoZoomHandler.h>

class Q_DECL_HIDDEN KoPAViewBase::Private
{
public:
    KoZoomHandler zoomHandler;
    KoPAViewMode *viewMode;
};

KoPAViewBase::KoPAViewBase()
    : d(new Private)
{
    d->viewMode = nullptr;
    proxyObject = new KoPAViewProxyObject(this);
}

KoPAViewBase::~KoPAViewBase()
{
    delete d;
    delete proxyObject;
}

KoViewConverter *KoPAViewBase::viewConverter(KoPACanvasBase *canvas)
{
    Q_UNUSED(canvas);

    return &d->zoomHandler;
}

KoZoomHandler *KoPAViewBase::zoomHandler()
{
    return &d->zoomHandler;
}

KoViewConverter *KoPAViewBase::viewConverter() const
{
    return &d->zoomHandler;
}

KoZoomHandler *KoPAViewBase::zoomHandler() const
{
    return &d->zoomHandler;
}

void KoPAViewBase::setViewMode(KoPAViewMode *mode)
{
    Q_ASSERT(mode);
    if (!d->viewMode) {
        d->viewMode = mode;
    } else if (mode != d->viewMode) {
        KoPAViewMode *previousViewMode = d->viewMode;
        d->viewMode->deactivate();
        d->viewMode = mode;
        d->viewMode->activate(previousViewMode);
    }
}

KoPAViewMode *KoPAViewBase::viewMode() const
{
    return d->viewMode;
}

KoPAViewProxyObject::KoPAViewProxyObject(KoPAViewBase *parent)
{
    m_view = parent;
}
