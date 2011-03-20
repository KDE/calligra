/* This file is part of the KDE project
*
* Copyright (C) 2010 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
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

#include "KPrViewModeSlidesSorter.h"

#include <QtCore/QEvent>
#include <QtGui/QPainter>
#include <QVariant>
#include <QScrollBar>
#include <QMenu>
#include <QtCore/qmath.h>

#include "KPrSlidesSorterDocumentModel.h"
#include <KoResourceManager.h>
#include <KoRuler.h>
#include <KoSelection.h>
#include <KoShapeLayer.h>
#include <KoShapeManager.h>
#include <KoText.h>
#include <KoToolManager.h>
#include <KoToolProxy.h>
#include <KoZoomController.h>

#include <KoPACanvas.h>
#include <KoPADocument.h>
#include <KoPAPageBase.h>
#include <KoPAMasterPage.h>
#include <KoPAView.h>
#include <KPrView.h>
#include <KoPAPageMoveCommand.h>

#include <klocale.h>
#include <KDebug>

KPrViewModeSlidesSorter::KPrViewModeSlidesSorter(KoPAView *view, KoPACanvas *canvas)
    : KoPAViewMode( view, canvas )
    , m_slidesSorter( new KPrSlidesSorter(this, view->parentWidget()) )
    , m_documentModel(new KPrSlidesSorterDocumentModel(this, view->parentWidget()))
    , m_iconSize( QSize(200, 200) )
    , m_itemSize( QRect(0, 0, 0, 0) )
    , m_sortNeeded(false)
    , m_pageCount(m_view->kopaDocument()->pages().count())
    , m_dragingFlag(false)
    , m_lastItemNumber(-1)
{
    m_slidesSorter->hide();
    m_slidesSorter->setIconSize( m_iconSize );
}

KPrViewModeSlidesSorter::KPrSlidesSorter::~KPrSlidesSorter()
{
}

KPrViewModeSlidesSorter::~KPrViewModeSlidesSorter()
{
}

void KPrViewModeSlidesSorter::paint(KoPACanvasBase* /*canvas*/, QPainter& /*painter*/, const QRectF &/*paintRect*/)
{
}

void KPrViewModeSlidesSorter::KPrSlidesSorter::paintEvent( QPaintEvent* event )
{
    event->accept();
    QListView::paintEvent(event);

    // Paint the line where the slide should go
    int lastItemNumber = m_viewModeSlidesSorter->lastItemNumber();
    int currentItemNumber = lastItemNumber;

    //20 is for the rigth margin
    int slidesNumber = qFloor((contentsRect().width() - 20)/m_viewModeSlidesSorter->itemSize().width());

    if (m_viewModeSlidesSorter->isDraging() && currentItemNumber >= 0) {
        QSize size(m_viewModeSlidesSorter->itemSize().width(), m_viewModeSlidesSorter->itemSize().height());

        int numberMod = currentItemNumber % slidesNumber;

        //put line after last slide.
        if (numberMod == 0 && currentItemNumber == m_viewModeSlidesSorter->pageCount()) {
            numberMod = slidesNumber;
        }
        int verticalValue = (currentItemNumber - numberMod) / slidesNumber * size.height() - verticalScrollBar()->value();

        QPoint point1(numberMod * size.width(), verticalValue);
        QPoint point2(numberMod * size.width(), verticalValue + size.height());
        QLineF line(point1, point2);

        QPainter painter(this->viewport());
        painter.drawLine(line);
    }

}

void KPrViewModeSlidesSorter::KPrSlidesSorter::mouseDoubleClickEvent(QMouseEvent *event)
{
    event->accept();
    QListView::mouseDoubleClickEvent(event);
    m_viewModeSlidesSorter->activateNormalViewMode();
}

void KPrViewModeSlidesSorter::KPrSlidesSorter::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    menu.addAction(SmallIcon("document-new"), i18n("Add a new slide"), m_viewModeSlidesSorter, SLOT(addSlide()));
    menu.addAction(i18n("Delete current Slide"), m_viewModeSlidesSorter, SLOT(deleteSlide()));

    menu.addAction(i18n("Cut") ,m_viewModeSlidesSorter, SLOT(editCut()));
    menu.addAction(i18n("Copy"), m_viewModeSlidesSorter, SLOT(editCopy()));
    menu.addAction(i18n("Paste"), m_viewModeSlidesSorter, SLOT(editPaste()));

    menu.exec(event->globalPos());
}

void KPrViewModeSlidesSorter::KPrSlidesSorter::keyPressEvent(QKeyEvent *event)
{
    m_viewModeSlidesSorter->m_toolProxy->keyPressEvent(event);

    if (!event->isAccepted()) {
        event->accept();

        switch (event->key()) {
            case Qt::Key_Delete:
                m_viewModeSlidesSorter->deleteSlide();
                break;

            default:
                event->ignore();
                return;
        }
    }
}

void KPrViewModeSlidesSorter::deleteSlide()
{
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        view->deletePage();
    }
}

void KPrViewModeSlidesSorter::paintEvent( KoPACanvas * canvas, QPaintEvent* event )
{
    Q_UNUSED(canvas);
    Q_UNUSED(event);
    Q_ASSERT( m_canvas == canvas );
}

void KPrViewModeSlidesSorter::tabletEvent(QTabletEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeSlidesSorter::mousePressEvent(QMouseEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeSlidesSorter::mouseDoubleClickEvent(QMouseEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeSlidesSorter::mouseMoveEvent(QMouseEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeSlidesSorter::mouseReleaseEvent(QMouseEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeSlidesSorter::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

void KPrViewModeSlidesSorter::keyReleaseEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

void KPrViewModeSlidesSorter::wheelEvent(QWheelEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeSlidesSorter::activate(KoPAViewMode *previousViewMode)
{
    Q_UNUSED(previousViewMode);
    populate();
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        view->hide();
    }
    m_slidesSorter->show();
    m_slidesSorter->setFocus(Qt::ActiveWindowFocusReason);
    updateToActivePageIndex();

    connect(m_slidesSorter, SIGNAL(pressed(QModelIndex)), this, SLOT(itemClicked(const QModelIndex)));
    connect(this, SIGNAL(pageChanged(KoPAPageBase*)), m_view->proxyObject, SLOT(updateActivePage(KoPAPageBase*)));
    connect(m_view->proxyObject, SIGNAL(activePageChanged()), this, SLOT(updatePageAdded()));
    connect(m_view->kopaDocument(),SIGNAL(pageAdded(KoPAPageBase*)),this, SLOT(updateModel()));
    connect(m_view->kopaDocument(),SIGNAL(pageRemoved(KoPAPageBase*)),this, SLOT(updateModel()));
    connect(m_view->zoomController(), SIGNAL(zoomChanged(KoZoomMode::Mode,qreal)), this, SLOT(updateZoom(KoZoomMode::Mode,qreal)));
}

void KPrViewModeSlidesSorter::deactivate()
{
    m_slidesSorter->hide();
    // Give the ressources back to the canvas
    m_canvas->resourceManager()->setResource(KoText::ShowTextFrames, 0);
    // Active the view as a basic but active one
    m_view->setActionEnabled(KoPAView::AllActions, true);
    m_view->doUpdateActivePage(m_view->activePage());
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        view->show();
    }
    m_view->setActivePage(m_view->kopaDocument()->pageByIndex(m_slidesSorter->currentIndex().row(), false));
}
void KPrViewModeSlidesSorter::updateModel()
{
    m_documentModel->update();
    updateToActivePageIndex();
}
void KPrViewModeSlidesSorter::updatePageAdded()
{
    m_documentModel->update();
    int row = m_view->kopaDocument()->pageIndex(m_view->activePage());
    QModelIndex index = m_documentModel->index(row, 0);
    m_slidesSorter->setCurrentIndex(index);
}

void KPrViewModeSlidesSorter::updateActivePage( KoPAPageBase *page )
{
    if (m_view->activePage() != page) {
        m_view->setActivePage(page);
    }
    updateToActivePageIndex();
}

void KPrViewModeSlidesSorter::updateToActivePageIndex()
{
    int row = m_view->kopaDocument()->pageIndex(m_view->activePage());
    QModelIndex index = m_documentModel->index(row, 0);
    m_slidesSorter->setCurrentIndex(index);
}

void KPrViewModeSlidesSorter::updateDocumentDock()
{
    QModelIndex c_index = m_slidesSorter->currentIndex();
    m_view->setActivePage(m_view->kopaDocument()->pageByIndex(c_index.row(), false));
}

void KPrViewModeSlidesSorter::addShape( KoShape *shape )
{
    Q_UNUSED(shape);
}

void KPrViewModeSlidesSorter::removeShape( KoShape *shape )
{
    Q_UNUSED(shape);
}

void KPrViewModeSlidesSorter::KPrSlidesSorter::startDrag ( Qt::DropActions supportedActions )
{
    Q_UNUSED(supportedActions);
    QAbstractItemView::startDrag(Qt::MoveAction);
}


void KPrViewModeSlidesSorter::KPrSlidesSorter::dragMoveEvent(QDragMoveEvent* ev)
{
    ev->accept();
    m_viewModeSlidesSorter->setDragingFlag();
    pageBefore(ev->pos());
    viewport()->update();
}

void KPrViewModeSlidesSorter::KPrSlidesSorter::dropEvent(QDropEvent* ev)
{
    m_viewModeSlidesSorter->setDragingFlag(false);
    ev->setDropAction(Qt::IgnoreAction);
    ev->accept();

    int newIndex;
    QByteArray ssData = ev->mimeData()->data("application/x-koffice-sliderssorter");
    int oldIndex = ssData.toInt();

    QModelIndex itemNew = indexAt(ev->pos());
    if (itemNew.row() >= 0)
    {
        // Normal case
        newIndex = itemNew.row();
    } else {
        // In case you point the end (no slides under the pointer)
        newIndex = m_viewModeSlidesSorter->pageCount() - 1;
        itemNew = currentIndex().model()->index(newIndex,0);
    }

    if (oldIndex != newIndex) {
        if (oldIndex > newIndex) {
            m_viewModeSlidesSorter->movePage(oldIndex, newIndex - 1);
        } else {
            m_viewModeSlidesSorter->movePage(oldIndex, newIndex);
        }

        // This selection helps the user
        clearSelection();
        setCurrentIndex(itemNew);
        m_viewModeSlidesSorter->updateDocumentDock();
    }
}

int KPrViewModeSlidesSorter::KPrSlidesSorter::pageBefore(QPoint point)
{
    QModelIndex item = indexAt(point);
    int pageBeforeNumber = -1;
    if (item.row()>=0) {
        //normal case
        pageBeforeNumber = item.row();
    } else {
        //after the last slide
        pageBeforeNumber = m_viewModeSlidesSorter->pageCount();
    }
    m_viewModeSlidesSorter->setLastItemNumber(pageBeforeNumber);
    return pageBeforeNumber;
}

void KPrViewModeSlidesSorter::populate()
{
    m_documentModel->setDocument(m_view->kopaDocument());
    m_slidesSorter->setModel(m_documentModel);
    m_slidesSorter->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_slidesSorter->setSelectionMode(QAbstractItemView::SingleSelection);
    m_slidesSorter->setDragDropMode(QAbstractItemView::InternalMove);
    QModelIndex item = m_documentModel->index(0,0);
    setItemSize(m_slidesSorter->visualRect(item));
}

void KPrViewModeSlidesSorter::movePage(int pageNumber, int pageAfterNumber)
{
    KoPAPageBase * page = 0;
    KoPAPageBase * pageAfter = 0;

    if (pageNumber >= 0) {
        page = m_view->kopaDocument()->pageByIndex(pageNumber,false);
    }
    if (pageAfterNumber >= 0) {
        pageAfter = m_view->kopaDocument()->pageByIndex(pageAfterNumber,false);
    }

    if (page) {
        KoPAPageMoveCommand *command = new KoPAPageMoveCommand(m_view->kopaDocument(), page, pageAfter);
        m_view->kopaDocument()->addCommand(command);
    }
}

int KPrViewModeSlidesSorter::pageCount() const
{
    return m_view->kopaDocument()->pages().count();
}

QSize KPrViewModeSlidesSorter::iconSize() const
{
    return m_iconSize;
}

QRect KPrViewModeSlidesSorter::itemSize() const
{
    return m_itemSize;
}

void KPrViewModeSlidesSorter::setItemSize(QRect size)
{
    m_itemSize = size;
}

bool KPrViewModeSlidesSorter::isDraging() const
{
    return m_dragingFlag;
}

void KPrViewModeSlidesSorter::setDragingFlag(bool flag)
{
    m_dragingFlag = flag;
}

int KPrViewModeSlidesSorter::lastItemNumber() const
{
    return m_lastItemNumber;
}

void KPrViewModeSlidesSorter::setLastItemNumber(int number)
{
    m_lastItemNumber = number;
}

void KPrViewModeSlidesSorter::activateNormalViewMode()
{
    KPrView *view = static_cast<KPrView *>(m_view);
    view->showNormal();
}

void KPrViewModeSlidesSorter::itemClicked(const QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }

    KoPAPageBase *page = m_view->kopaDocument()->pageByIndex(index.row(), false);

    if (page) {
        m_view->setActivePage(page);
    }
}

void KPrViewModeSlidesSorter::KPrSlidesSorter::dragEnterEvent(QDragEnterEvent *event)
{
    event->setDropAction(Qt::MoveAction);
    event->accept();
}

void KPrViewModeSlidesSorter::addSlide()
{
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        view->insertPage();
    }
}

void KPrViewModeSlidesSorter::updateDocumentModel()
{
    m_documentModel->update();
}

void KPrViewModeSlidesSorter::editCut()
{
    editCopy();
    deleteSlide();
}

void KPrViewModeSlidesSorter::editCopy()
{
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        view->copyPage();
    }
}

void KPrViewModeSlidesSorter::editPaste()
{
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        view->pagePaste();
    }
}

void KPrViewModeSlidesSorter::updateZoom(KoZoomMode::Mode mode, qreal zoom)
{
    Q_UNUSED(mode);

    //at zoom 100%, iconSize is set in 200 x 200
    //KPrSlidesSorterDocumentModel uses iconSize function in decorate Role.
    setIconSize(QSize(qRound(zoom*200),qRound(zoom*200)));
    m_slidesSorter->setIconSize(iconSize());

    //update item size
    QModelIndex item = m_documentModel->index(0,0);
    setItemSize(m_slidesSorter->visualRect(item));
}

void KPrViewModeSlidesSorter::setIconSize(QSize size)
{
    if (size != m_iconSize) {
        m_iconSize = size;
    }

}
