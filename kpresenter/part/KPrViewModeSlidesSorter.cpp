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
#include "KPrFactory.h"
#include "KPrSlidesManagerView.h"
#include "KPrSelectionManager.h"

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
#include <KoPAPageDeleteCommand.h>
#include <KoPAOdfPageSaveHelper.h>
#include <KoDrag.h>

#include <klocale.h>
#include <KDebug>
#include <kconfiggroup.h>

KPrViewModeSlidesSorter::KPrViewModeSlidesSorter(KoPAView *view, KoPACanvas *canvas)
    : KoPAViewMode( view, canvas )
    //, m_slidesSorter( new KPrSlidesSorter(this, view->parentWidget()) )
    , m_slidesSorter( new KPrSlidesManagerView(m_toolProxy, view->parentWidget()))
    , m_documentModel(new KPrSlidesSorterDocumentModel(this, view->parentWidget()))
    , m_iconSize( QSize(200, 200) )
{
    m_slidesSorter->hide();
    m_slidesSorter->setIconSize( m_iconSize );

    connect(m_slidesSorter, SIGNAL(requestContextMenu(QContextMenuEvent*)), this, SLOT(slidesSorterContextMenu(QContextMenuEvent*)));
    connect(m_slidesSorter, SIGNAL(slideDblClick()), this, SLOT(activateNormalViewMode()));

    m_slidesSorter->installEventFilter(this);

    //install selection manager for Slides Sorter View
    m_selectionManagerSlidesSorter = new KPrSelectionManager(m_slidesSorter, m_view->kopaDocument());
}

KPrViewModeSlidesSorter::~KPrViewModeSlidesSorter()
{
    //save zoom value
    saveZoomConfig(zoom());
}

void KPrViewModeSlidesSorter::paint(KoPACanvasBase* /*canvas*/, QPainter& /*painter*/, const QRectF &/*paintRect*/)
{
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

    connect(m_slidesSorter,SIGNAL(indexChanged(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
    connect(m_slidesSorter, SIGNAL(pressed(QModelIndex)), this, SLOT(itemClicked(const QModelIndex)));
    connect(m_view->proxyObject, SIGNAL(activePageChanged()), this, SLOT(updateToActivePageIndex()));
    connect(m_view->kopaDocument(),SIGNAL(pageAdded(KoPAPageBase*)),this, SLOT(updateSlidesSorterDocumentModel()));
    connect(m_view->kopaDocument(),SIGNAL(pageRemoved(KoPAPageBase*)),this, SLOT(updateSlidesSorterDocumentModel()));

    //change zoom saving slot
    connect(m_view->zoomController(), SIGNAL(zoomChanged(KoZoomMode::Mode, qreal)), this, SLOT(updateZoom(KoZoomMode::Mode, qreal)));

    KPrView *kPrview = dynamic_cast<KPrView *>(m_view);
    if (kPrview) {
        disconnect(kPrview->zoomController(), SIGNAL(zoomChanged(KoZoomMode::Mode, qreal)), kPrview, SLOT(zoomChanged(KoZoomMode::Mode, qreal)));
        m_view->zoomController()->zoomAction()->setZoomModes(KoZoomMode::ZOOM_CONSTANT);
        loadZoomConfig();
    }

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

    //save zoom value
    saveZoomConfig(zoom());

    //change zoom saving slot and restore normal view zoom values
    disconnect(m_view->zoomController(), SIGNAL(zoomChanged(KoZoomMode::Mode, qreal)), this, SLOT(updateZoom(KoZoomMode::Mode, qreal)));

    m_view->zoomController()->zoomAction()->setZoomModes(KoZoomMode::ZOOM_PAGE | KoZoomMode::ZOOM_WIDTH);

    m_view->setActivePage(m_view->kopaDocument()->pageByIndex(m_slidesSorter->currentIndex().row(), false));

    KPrView *kPrview = dynamic_cast<KPrView *>(m_view);
    if (kPrview) {
        kPrview->restoreZoomConfig();
        connect(kPrview->zoomController(), SIGNAL(zoomChanged(KoZoomMode::Mode, qreal)), kPrview, SLOT(zoomChanged(KoZoomMode::Mode, qreal)));
    }


}

void KPrViewModeSlidesSorter::updateSlidesSorterDocumentModel()
{
    m_documentModel->update();
    updateToActivePageIndex();
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
    QModelIndex index = m_documentModel->index(row, 0, QModelIndex());
    m_slidesSorter->setCurrentIndex(index);
}

void KPrViewModeSlidesSorter::updateActivePageToCurrentIndex()
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

void KPrViewModeSlidesSorter::populate()
{
    m_documentModel->setDocument(m_view->kopaDocument());
    m_slidesSorter->setModel(m_documentModel);

    m_slidesSorter->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_slidesSorter->setSelectionMode(QAbstractItemView::ExtendedSelection);

    m_slidesSorter->setDragDropMode(QAbstractItemView::InternalMove);
    QModelIndex item = m_documentModel->index(0, 0, QModelIndex());
    m_slidesSorter->setItemSize (m_slidesSorter->visualRect(item));
}

QSize KPrViewModeSlidesSorter::iconSize() const
{
    return m_iconSize;
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

    //Avoid deselect slides when dragging
    if (m_slidesSorter->selectionModel()->selectedIndexes().length () > 1) {
        return;
    }

    KoPAPageBase *page = m_view->kopaDocument()->pageByIndex(index.row(), false);

    if (page) {
        m_view->setActivePage(page);
    }
}

QList<KoPAPageBase *> KPrViewModeSlidesSorter::extractSelectedSlides()
{
    QList<KoPAPageBase *> slides;

    QModelIndexList selectedItems = m_slidesSorter->selectionModel()->selectedIndexes();
    if (selectedItems.count() == 0) {
        return slides;
    }

    foreach (const QModelIndex & index, selectedItems) {
        KoPAPageBase * page = m_view->kopaDocument()->pageByIndex(index.row (), false);
        if (page) {
            slides.append(page);
        }
    }

    qSort(slides.begin(), slides.end());

    return slides;
}

void KPrViewModeSlidesSorter::deleteSlide()
{
    // create a list with all selected slides
    QList<KoPAPageBase*> selectedSlides = extractSelectedSlides();

    if (!selectedSlides.empty() && m_view->kopaDocument()->pages().count() > selectedSlides.count()) {
         QUndoCommand *cmd = new KoPAPageDeleteCommand(m_view->kopaDocument(), selectedSlides);
        if (cmd) {
            m_view->kopaDocument()->addCommand(cmd);
        }
    }
}

void KPrViewModeSlidesSorter::addSlide()
{
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        view->insertPage();
    }
}

void KPrViewModeSlidesSorter::editCut()
{
    editCopy();
    deleteSlide();
}

void KPrViewModeSlidesSorter::editCopy()
{
    // separate selected layers and selected shapes
    QList<KoPAPageBase*> slides = extractSelectedSlides();;

    if (!slides.empty()) {
        // Copy Pages
        KoPAOdfPageSaveHelper saveHelper(m_view->kopaDocument (), slides);
        KoDrag drag;
        drag.setOdf(KoOdf::mimeType(m_view->kopaDocument()->documentType()), saveHelper);
        drag.addToClipboard();
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
    QModelIndex item = m_documentModel->index(0, 0, QModelIndex());
    m_slidesSorter->setItemSize(m_slidesSorter->visualRect(item));

    setZoom(qRound(zoom * 100.));
}

void KPrViewModeSlidesSorter::setIconSize(QSize size)
{
    if (size != m_iconSize) {
        m_iconSize = size;
    }

}

void KPrViewModeSlidesSorter::loadZoomConfig()
{
    KSharedConfigPtr config = KPrFactory::componentData().config();
    int s_zoom = 100;

    if (config->hasGroup("Interface")) {
        const KConfigGroup interface = config->group("Interface");
        s_zoom = interface.readEntry("ZoomSlidesSorter", s_zoom);
    }
    m_view->zoomController()->setZoom(KoZoomMode::ZOOM_CONSTANT, s_zoom/100.);
}

void KPrViewModeSlidesSorter::saveZoomConfig(int zoom)
{
    KSharedConfigPtr config = KPrFactory::componentData().config();
    KConfigGroup interface = config->group("Interface");
    interface.writeEntry("ZoomSlidesSorter", zoom);
}

void KPrViewModeSlidesSorter::setZoom(int zoom)
{
    m_zoom = zoom;
}

int KPrViewModeSlidesSorter::zoom()
{
    return m_zoom;
}

void KPrViewModeSlidesSorter::slidesSorterContextMenu(QContextMenuEvent *event)
{
    QMenu menu(m_slidesSorter);

    // Not connected yet
    menu.addAction(SmallIcon("document-new"), i18n("Add a new slide"), this, SLOT(addSlide()));
    menu.addAction(i18n("Delete selected slides"), this, SLOT(deleteSlide()));

    menu.addAction(i18n( "Cut" ) ,this,  SLOT(editCut()));
    menu.addAction(i18n( "Copy" ), this,  SLOT(editCopy()));
    menu.addAction(i18n( "Paste" ), this, SLOT(editPaste()));

    menu.exec(event->globalPos());
}

bool KPrViewModeSlidesSorter::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_slidesSorter) {
        switch (event->type()) {
            case QEvent::KeyPress: {
                QKeyEvent *keyEv = static_cast<QKeyEvent *>(event);
                switch (keyEv->key()) {
                    case Qt::Key_Delete: {
                        deleteSlide();
                        break;
                    }

                    default:
                       break;
                }
                break;
            }

            default:
                break;
        }
    } //endIf

    return QObject::eventFilter(watched, event);
}
