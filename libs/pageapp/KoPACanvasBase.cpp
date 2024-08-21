/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoPACanvasBase.h"

#include <KoGridData.h>
#include <KoShapeManager.h>
#include <KoText.h>
#include <KoToolProxy.h>
#include <KoUnit.h>

#include "KoPADocument.h"
#include "KoPAPageBase.h"
#include "KoPAPageProvider.h"
#include "KoPAViewBase.h"

class Q_DECL_HIDDEN KoPACanvasBase::Private
{
public:
    Private(KoPADocument *doc)
        : view(nullptr)
        , doc(doc)
        , shapeManager(nullptr)
        , masterShapeManager(nullptr)
        , toolProxy(nullptr)
        , showPageMargins(false)
    {
    }

    ~Private()
    {
        delete toolProxy;
        delete masterShapeManager;
        delete shapeManager;
    }

    ///< the origin of the page rect inside the canvas in document points
    QPointF origin() const
    {
        return view->viewMode()->origin();
    }

    KoPAViewBase *view;
    KoPADocument *doc;
    KoShapeManager *shapeManager;
    KoShapeManager *masterShapeManager;
    KoToolProxy *toolProxy;
    QPoint documentOffset;
    bool showPageMargins;
};

KoPACanvasBase::KoPACanvasBase(KoPADocument *doc)
    : KoCanvasBase(doc)
    , d(new Private(doc))
{
    d->shapeManager = new KoShapeManager(this);
    d->masterShapeManager = new KoShapeManager(this);
    d->toolProxy = new KoToolProxy(this);
}

KoPACanvasBase::~KoPACanvasBase()
{
    delete d;
}

void KoPACanvasBase::setView(KoPAViewBase *view)
{
    d->view = view;
}

KoPADocument *KoPACanvasBase::document() const
{
    return d->doc;
}

KoToolProxy *KoPACanvasBase::toolProxy() const
{
    return d->toolProxy;
}

KoPAViewBase *KoPACanvasBase::koPAView() const
{
    return d->view;
}

QPoint KoPACanvasBase::documentOrigin() const
{
    return viewConverter()->documentToView(d->origin()).toPoint();
}

void KoPACanvasBase::setDocumentOrigin(const QPointF &o)
{
    d->view->viewMode()->setOrigin(o);
}

void KoPACanvasBase::gridSize(qreal *horizontal, qreal *vertical) const
{
    *horizontal = d->doc->gridData().gridX();
    *vertical = d->doc->gridData().gridY();
}

bool KoPACanvasBase::snapToGrid() const
{
    return d->doc->gridData().snapToGrid();
}

void KoPACanvasBase::addCommand(KUndo2Command *command)
{
    d->doc->addCommand(command);
}

KoShapeManager *KoPACanvasBase::shapeManager() const
{
    return d->shapeManager;
}

KoShapeManager *KoPACanvasBase::masterShapeManager() const
{
    return d->masterShapeManager;
}

KoViewConverter *KoPACanvasBase::viewConverter() const
{
    return d->view->viewMode()->viewConverter(const_cast<KoPACanvasBase *>(this));
}

KoUnit KoPACanvasBase::unit() const
{
    return d->doc->unit();
}

const QPoint &KoPACanvasBase::documentOffset() const
{
    return d->documentOffset;
}

void KoPACanvasBase::setDocumentOffset(const QPoint &offset)
{
    d->documentOffset = offset;
}

QPoint KoPACanvasBase::widgetToView(const QPoint &p) const
{
    return p - viewConverter()->documentToView(d->origin()).toPoint();
}

QRect KoPACanvasBase::widgetToView(const QRect &r) const
{
    return r.translated(viewConverter()->documentToView(-d->origin()).toPoint());
}

QPoint KoPACanvasBase::viewToWidget(const QPoint &p) const
{
    return p + viewConverter()->documentToView(d->origin()).toPoint();
}

QRect KoPACanvasBase::viewToWidget(const QRect &r) const
{
    return r.translated(viewConverter()->documentToView(d->origin()).toPoint());
}

KoGuidesData *KoPACanvasBase::guidesData()
{
    return &d->doc->guidesData();
}

void KoPACanvasBase::paint(QPainter &painter, const QRectF &paintRect)
{
    KoPAPageBase *activePage(d->view->activePage());
    if (d->view->activePage()) {
        int pageNumber = d->doc->pageIndex(d->view->activePage()) + 1;
        QVariant var = d->doc->resourceManager()->resource(KoText::PageProvider);
        static_cast<KoPAPageProvider *>(var.value<void *>())->setPageData(pageNumber, activePage);
        d->view->viewMode()->paint(this, painter, paintRect);
    }
}

void KoPACanvasBase::setShowPageMargins(bool state)
{
    d->showPageMargins = state;
}

bool KoPACanvasBase::showPageMargins() const
{
    return d->showPageMargins;
}
