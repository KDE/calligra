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

#include <KoPAPageMoveCommand.h>

#include <KDebug>

KPrViewModeSlidesSorter::KPrViewModeSlidesSorter(KoPAView *view, KoPACanvas *canvas)
    : KoPAViewMode( view, canvas )
    , m_slidesSorter( new KPrSlidesSorter(this, view->parentWidget()) )
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

KPrViewModeSlidesSorter::~KPrViewModeSlidesSorter()
{
}

void KPrViewModeSlidesSorter::paint(KoPACanvasBase* /*canvas*/, QPainter& /*painter*/, const QRectF &/*paintRect*/)
{
}

void KPrViewModeSlidesSorter::KPrSlidesSorter::paintEvent( QPaintEvent* event )
{
    event->accept();
    QListWidget::paintEvent(event);

    // Paint the line where the slide should go
    int currentItemNumber = m_viewModeSlidesSorter->lastItemNumber();
    if (m_viewModeSlidesSorter->isDraging() && currentItemNumber >= 0) {
        QSize size(m_viewModeSlidesSorter->itemSize().width(), m_viewModeSlidesSorter->itemSize().height());

        int numberMod = currentItemNumber%4 > 0 ? currentItemNumber%4 : 0;
        int verticalValue = (currentItemNumber - numberMod) / 4 * size.height() - verticalScrollBar()->value();
        QPoint point1(numberMod * size.width(), verticalValue );
        QPoint point2(numberMod * size.width(), verticalValue + size.height() );
        QLineF line(point1, point2);

        QPainter painter(this->viewport());
        painter.drawLine(line);
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
    Q_UNUSED( previousViewMode );
    populate();
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        view->hide();
    }
    m_slidesSorter->show();
    m_slidesSorter->setFocus(Qt::ActiveWindowFocusReason);
}

void KPrViewModeSlidesSorter::deactivate()
{
    m_slidesSorter->hide();
    // Give the ressources back to the canvas
    m_canvas->resourceManager()->setResource(KoText::ShowTextFrames, 0);
    // Active the view as a basic but active one
    m_view->setActionEnabled( KoPAView::AllActions, true );
    m_view->doUpdateActivePage(m_view->activePage());
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        view->show();
    }
}

void KPrViewModeSlidesSorter::updateActivePage( KoPAPageBase *page )
{
    Q_UNUSED(page);
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
    QAbstractItemView::startDrag( Qt::MoveAction );
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

    QListWidgetItem * itemNew = itemAt(ev->pos());
    if (itemNew)
    {
        // Normal case
        newIndex = row(itemNew);
    } else {
        // In case you point the end (no slides under the pointer)
        newIndex = m_viewModeSlidesSorter->pageCount() - 1;
    }
    if (oldIndex != newIndex) {
        m_viewModeSlidesSorter->movePage(oldIndex, newIndex);
        QListWidgetItem *sourceItem = takeItem(oldIndex);
        insertItem(newIndex, sourceItem);
    }
}

QMimeData* KPrViewModeSlidesSorter::KPrSlidesSorter::mimeData(const QList<QListWidgetItem*> items) const
{
    QListWidgetItem* page = items.first();

    QByteArray ssData = QVariant(row(page)).toByteArray();

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-koffice-sliderssorter", ssData);

    return mimeData;
}

QStringList KPrViewModeSlidesSorter::KPrSlidesSorter::mimeTypes() const
{
    return QStringList() << "application/x-koffice-sliderssorter";
}

int KPrViewModeSlidesSorter::KPrSlidesSorter::pageBefore(QPoint point)
{
    QListWidgetItem *item = itemAt(point);
    if (item) {
        m_viewModeSlidesSorter->setLastItemNumber(row(item) + 1);
        return row(item) + 1;
    }
    int lastPage = m_viewModeSlidesSorter->pageCount();
    m_viewModeSlidesSorter->setLastItemNumber(lastPage);
    return lastPage;
}

void KPrViewModeSlidesSorter::populate()
{
    int currentPage = 0;
    m_slidesSorter->clear();

    QListWidgetItem * item = 0;

    //Load the available slides
    foreach( KoPAPageBase* page, m_view->kopaDocument()->pages() )
    {
        QString slideName = page->name().isEmpty() ? i18n("Slide %1", ++currentPage) : page->name();
        item = new QListWidgetItem( QIcon( page->thumbnail( m_iconSize ) ), slideName, m_slidesSorter );
        item->setFlags((item->flags() | Qt::ItemIsDragEnabled ) & ~Qt::ItemIsDropEnabled);
    }
    if (item) {
        setItemSize(m_slidesSorter->visualItemRect(item));
    }
}

void KPrViewModeSlidesSorter::movePage(int pageNumber, int pageAfterNumber)
{
    KoPAPageBase * page = 0;
    KoPAPageBase * pageAfter = 0;

    page = m_view->kopaDocument()->pageByIndex(pageNumber,false);
    pageAfter = m_view->kopaDocument()->pageByIndex(pageAfterNumber,false);

    if (page && pageAfter) {
        KoPAPageMoveCommand *command = new KoPAPageMoveCommand( m_view->kopaDocument(), page, pageAfter );
        m_view->kopaDocument()->addCommand( command );
    }
}

int KPrViewModeSlidesSorter::pageCount() const
{
    return m_pageCount;
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

