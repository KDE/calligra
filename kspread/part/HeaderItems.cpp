/* This file is part of the KDE project
   Copyright 2006 Robert Knight <robertknight@gmail.com>
   Copyright 2006 Inge Wallin <inge@lysator.liu.se>
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 1999-2002,2004 Laurent Montel <montel@kde.org>
   Copyright 2002-2005 Ariya Hidayat <ariya@kde.org>
   Copyright 1999-2004 David Faure <faure@kde.org>
   Copyright 2004-2005 Meni Livne <livne@kde.org>
   Copyright 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2003 Hamish Rodda <rodda@kde.org>
   Copyright 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright 2003 Lukas Tinkl <lukas@kde.org>
   Copyright 2000-2002 Werner Trobin <trobin@kde.org>
   Copyright 2002 Harri Porten <porten@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 2002 Daniel Naber <daniel.naber@t-online.de>
   Copyright 1999-2000 Torben Weis <weis@kde.org>
   Copyright 1999-2000 Stephan Kulow <coolo@kde.org>
   Copyright 2000 Bernd Wuebben <wuebben@kde.org>
   Copyright 2000 Wilco Greven <greven@kde.org>
   Copyright 2000 Simon Hausmann <hausmann@kde.org
   Copyright 1999 Michael Reiher <michael.reiher@gmx.de>
   Copyright 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>
   Copyright 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Local
#include "HeaderItems.h"

// Qt
#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QPainter>
#include <QRubberBand>
#include <QStyle>
#include <QTextLayout>
#include <QToolTip>
#include <QScrollBar>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneWheelEvent>

// KDE
#include <klocale.h>
#include <kmessagebox.h>
#include <kwordwrap.h>

// KOffice
#include <KoCanvasController.h>
#include <KoToolProxy.h>
#include <KoZoomHandler.h>
#include <KoDpi.h>
#include <KoPointerEvent.h>

// KSpread
#include "CanvasItem.h"
#include "Cell.h"
#include "Doc.h"
#include "kspread_limits.h"
#include "RowColumnFormat.h"
#include "Sheet.h"
#include "Selection.h"

// commands
#include "commands/RowColumnManipulators.h"

using namespace KSpread;

/****************************************************************
 *
 * RowHeaderItem
 *
 ****************************************************************/

RowHeaderItem::RowHeaderItem(QGraphicsItem *_parent, CanvasItem *_canvas)
        : QGraphicsWidget(_parent), RowHeader(_canvas)
{
    setAttribute(Qt::WA_StaticContents);

    //setMouseTracking(true);
    setAcceptHoverEvents(true);

    //connect(m_pView, SIGNAL(autoScroll(const QPoint &)),
            //this, SLOT(slotAutoScroll(const QPoint &)));
    connect(m_pCanvas->toolProxy(), SIGNAL(toolChanged(const QString&)),
            this, SLOT(toolChanged(const QString&)));
}


RowHeaderItem::~RowHeaderItem()
{
}

void RowHeaderItem::mousePressEvent(QGraphicsSceneMouseEvent * _ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mousePress(&pev);
}

void RowHeaderItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * _ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mouseRelease(&pev);
}

void RowHeaderItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mouseDoubleClick(&pev);
}


void RowHeaderItem::mouseMoveEvent(QGraphicsSceneMouseEvent * _ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mouseMove(&pev);
}
/*
void RowHeaderItem::slotAutoScroll(const QPoint& scrollDistance)
{
    // NOTE Stefan: This slot is triggered by the same signal as
    //              Canvas::slotAutoScroll and ColumnHeaderItem::slotAutoScroll.
    //              Therefore, nothing has to be done except the scrolling was
    //              initiated in this header.
    if (!m_bMousePressed)
        return;
    if (scrollDistance.y() == 0)
        return;
    const QPoint offset = m_pCanvas->viewConverter()->documentToView(m_pCanvas->offset()).toPoint();
    if (offset.y() + scrollDistance.y() < 0)
        return;
    m_pCanvas->setDocumentOffset(offset + QPoint(0, scrollDistance.y()));
    // XXX: Port!
    //QGraphicsSceneMouseEvent event(QEvent::MouseMove, mapFromGlobal(QCursor::pos()),
    //                  Qt::NoButton, Qt::NoButton, QApplication::keyboardModifiers());
    //QApplication::sendEvent(this, &event);
    m_pCanvas->update();
}
*/
void RowHeaderItem::wheelEvent(QGraphicsSceneWheelEvent* _ev)
{
    KoPointerEvent pev(_ev, QPointF());
    wheel(&pev);
}

void RowHeaderItem::paintSizeIndicator(int mouseY)
{
    register Sheet * const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

    m_iResizePos = mouseY;

    // Don't make the row have a height < 2 pixel.
    double y = m_pCanvas->zoomHandler()->zoomItY(sheet->rowPosition(m_iResizedRow) - m_pCanvas->yOffset());
    if (m_iResizePos < y + 2)
        m_iResizePos = (int) y;

    // XXX: Port to QGraphicsView
    //if (!m_rubberband) {
    //    m_rubberband = new QRubberBand(QRubberBand::Line, m_pCanvas);
    //    m_rubberband->setGeometry(0, m_iResizePos, m_pCanvas->width(), 2);
    //    m_rubberband->show();
    //}
    //m_rubberband->move(0, m_iResizePos);

    QString tmpSize;
    double hh = m_pCanvas->zoomHandler()->unzoomItY(m_iResizePos - y);
    double hu = m_pCanvas->doc()->unit().toUserValue(hh);
    if (hu > 0.01)
        tmpSize = i18n("Height: %1 %2", hu, KoUnit::unitName(m_pCanvas->doc()->unit()));
    else
        tmpSize = i18n("Hide Row");

    if (!m_lSize) {
        int screenNo = 0; //QApplication::desktop()->screenNumber(topLevelWidget());
        m_lSize = new QLabel(QApplication::desktop()->screen(screenNo) , Qt::ToolTip);
        m_lSize->setAlignment(Qt::AlignVCenter);
        m_lSize->setAutoFillBackground(true);
        m_lSize->setPalette(QToolTip::palette());
        m_lSize->setMargin(1 + style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, 0, m_lSize));
        m_lSize->setFrameShape(QFrame::Box);
        m_lSize->setIndent(1);
    }

    m_lSize->setText(tmpSize);
    m_lSize->adjustSize();
    QRectF rcf = static_cast<CanvasItem*>(m_pCanvas)->boundingRect();
    QPoint pos = (sheet->layoutDirection() == Qt::RightToLeft) ? QPoint(rcf.width() - m_lSize->width() - 3, (int)y + 3) : QPoint(3, (int)y + 3);
    pos -= QPoint(0, m_lSize->height());
    // XXX: Port
    //m_lSize->move(m_pCanvas->mapToGlobal(pos).x(), m_pCanvas->mapToGlobal(pos).y());
    m_lSize->show();
}

void RowHeaderItem::removeSizeIndicator()
{
    // XXX TODO
}

void RowHeaderItem::updateRows(int from, int to)
{
    register Sheet * const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

    double y0 = m_pCanvas->zoomHandler()->zoomItY(sheet->rowPosition(from));
    double y1 = m_pCanvas->zoomHandler()->zoomItY(sheet->rowPosition(to + 1));
    QGraphicsItem::update(0, (int) y0, boundingRect().width(), (int)(y1 - y0));
}

void RowHeaderItem::paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget);
    RowHeader::paint(painter, option->exposedRect);
}


void RowHeaderItem::focusOutEvent(QFocusEvent* _ev)
{
    focusOut(_ev);
}

void RowHeaderItem::toolChanged(const QString& toolId)
{
    doToolChanged(toolId);
}

/****************************************************************
 *
 * ColumnHeaderItem
 *
 ****************************************************************/

ColumnHeaderItem::ColumnHeaderItem(QGraphicsItem *_parent, CanvasItem *_canvas)
        : QGraphicsWidget(_parent), ColumnHeader(_canvas)
{
    setAttribute(Qt::WA_StaticContents);

    //setMouseTracking(true);
    setAcceptHoverEvents(true);

    //connect(_view, SIGNAL(autoScroll(const QPoint &)),
            //this, SLOT(slotAutoScroll(const QPoint &)));
    connect(_canvas->toolProxy(), SIGNAL(toolChanged(const QString&)),
            this, SLOT(toolChanged(const QString&)));
}


ColumnHeaderItem::~ColumnHeaderItem()
{
}

void ColumnHeaderItem::mousePressEvent(QGraphicsSceneMouseEvent * _ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mousePress(&pev);
}

void ColumnHeaderItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * _ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mouseRelease(&pev);
}

void ColumnHeaderItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mouseDoubleClick(&pev);
}

void ColumnHeaderItem::mouseMoveEvent(QGraphicsSceneMouseEvent * _ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mouseMove(&pev);
}
/*
void ColumnHeaderItem::slotAutoScroll(const QPoint& scrollDistance)
{
    // NOTE Stefan: This slot is triggered by the same signal as
    //              Canvas::slotAutoScroll and RowHeaderItem::slotAutoScroll.
    //              Therefore, nothing has to be done except the scrolling was
    //              initiated in this header.
    if (!m_bMousePressed)
        return;
    if (scrollDistance.x() == 0)
        return;
    const QPoint offset = m_pCanvas->viewConverter()->documentToView(m_pCanvas->offset()).toPoint();
    if (offset.x() + scrollDistance.x() < 0)
        return;
    m_pCanvas->setDocumentOffset(offset + QPoint(scrollDistance.x(), 0));
    //QGraphicsSceneMouseEvent event(QEvent::MouseMove, mapFromGlobal(QCursor::pos()),
    //                  Qt::NoButton, Qt::NoButton, QApplication::keyboardModifiers());
    //QApplication::sendEvent(this, &event);
    m_pCanvas->update();
}
*/
void ColumnHeaderItem::wheelEvent(QGraphicsSceneWheelEvent* _ev)
{
    KoPointerEvent pev(_ev, QPointF());
    wheel(&pev);
}

void ColumnHeaderItem::resizeEvent(QGraphicsSceneResizeEvent* _ev)
{
    ColumnHeader::resize(_ev->newSize(), _ev->oldSize());
}

void ColumnHeaderItem::paintSizeIndicator(int mouseX)
{
    register Sheet * const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

    if (sheet->layoutDirection() == Qt::RightToLeft)
        m_iResizePos = mouseX + m_pCanvas->width() - boundingRect().width();
    else
        m_iResizePos = mouseX;

    // Don't make the column have a width < 2 pixels.
    double x = m_pCanvas->zoomHandler()->zoomItX(sheet->columnPosition(m_iResizedColumn) - m_pCanvas->xOffset());

    if (sheet->layoutDirection() == Qt::RightToLeft) {
        x = m_pCanvas->width() - x;

        if (m_iResizePos > x - 2)
            m_iResizePos = (int) x;
    } else {
        if (m_iResizePos < x + 2)
            m_iResizePos = (int) x;
    }
// XXX: Port
//    if (!m_rubberband) {
//        m_rubberband = new QRubberBand(QRubberBand::Line, m_pCanvas);
//        m_rubberband->setGeometry(m_iResizePos, 0, 2, m_pCanvas->height());
//        m_rubberband->show();
//    }
//    m_rubberband->move(m_iResizePos, 0);

    QString tmpSize;
    double ww = m_pCanvas->zoomHandler()->unzoomItX((sheet->layoutDirection() == Qt::RightToLeft) ? x - m_iResizePos : m_iResizePos - x);
    double wu = m_pCanvas->doc()->unit().toUserValue(ww);
    if (wu > 0.01)
        tmpSize = i18n("Width: %1 %2", wu, KoUnit::unitName(m_pCanvas->doc()->unit()));
    else
        tmpSize = i18n("Hide Column");

// XXX: Port
//    if (!m_lSize) {
//        int screenNo = 0; // QApplication::desktop()->screenNumber(this);
//        m_lSize = new QLabel(QApplication::desktop()->screen(screenNo) , Qt::ToolTip);
//        m_lSize->setAlignment(Qt::AlignVCenter);
//        m_lSize->setAutoFillBackground(true);
//        m_lSize->setPalette(QToolTip::palette());
//        m_lSize->setMargin(1 + style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, 0, m_lSize));
//        m_lSize->setFrameShape(QFrame::Box);
//        m_lSize->setIndent(1);
//    }
//
//    m_lSize->setText(tmpSize);
//    m_lSize->adjustSize();
//    QPoint pos = (sheet->layoutDirection() == Qt::RightToLeft) ? QPoint((int) x - 3 - m_lSize->width(), 3) :
//                 QPoint((int) x + 3, 3);
//    pos -= QPoint(0, m_lSize->height());
//    m_lSize->move(m_pCanvas->mapToGlobal(pos).x(), mapToGlobal(pos).y());
//    m_lSize->show();
}

void ColumnHeaderItem::removeSizeIndicator()
{
    // XXX TODO
}

void ColumnHeaderItem::updateColumns(int from, int to)
{
    register Sheet * const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

    double x0 = m_pCanvas->zoomHandler()->zoomItX(sheet->columnPosition(from));
    double x1 = m_pCanvas->zoomHandler()->zoomItX(sheet->columnPosition(to + 1));
    QGraphicsItem::update((int) x0, 0, (int)(x1 - x0), boundingRect().height());
}

void ColumnHeaderItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget);
    ColumnHeader::paint(painter, option->exposedRect);
}


void ColumnHeaderItem::focusOutEvent(QFocusEvent* _ev)
{
    focusOut(_ev);
}

void ColumnHeaderItem::toolChanged(const QString& toolId)
{
    doToolChanged(toolId);
}


/****************************************************************
 *
 * SelectAllButtonItem
 *
 ****************************************************************/

SelectAllButtonItem::SelectAllButtonItem(QGraphicsItem *_parent, KoCanvasBase* canvasBase, Selection* selection)
        : QGraphicsWidget(_parent)
        , m_canvasBase(canvasBase)
        , m_selection(selection)
        , m_mousePressed(false)
{
    m_cellToolIsActive = true;
    connect(m_canvasBase->toolProxy(), SIGNAL(toolChanged(const QString&)),
            this, SLOT(toolChanged(const QString&)));
}

SelectAllButtonItem::~SelectAllButtonItem()
{
}

void SelectAllButtonItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget);
    // the painter
    painter->setClipRect(option->exposedRect);

    // if all cells are selected
    if (m_selection->isAllSelected() &&
            !m_selection->referenceSelectionMode() && m_cellToolIsActive) {
        // selection brush/color
        QColor selectionColor(palette().highlight().color());
        selectionColor.setAlpha(127);
        const QBrush selectionBrush(selectionColor);

        painter->setPen(selectionColor.dark(150));
        painter->setBrush(selectionBrush);
    } else {
        // background brush/color
        const QBrush backgroundBrush(palette().window());
        const QColor backgroundColor(backgroundBrush.color());

        painter->setPen(backgroundColor.dark(150));
        painter->setBrush(backgroundBrush);
    }
    painter->drawRect(rect().adjusted(0, 0, -1, -1));
}

void SelectAllButtonItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (!m_cellToolIsActive)
        return;
    if (event->button() == Qt::LeftButton)
        m_mousePressed = true;
}

void SelectAllButtonItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (!m_cellToolIsActive)
        return;
    Q_UNUSED(event);
    if (!m_mousePressed)
        return;
    m_mousePressed = false;
    m_selection->selectAll();
}

void SelectAllButtonItem::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    QWheelEvent ev(event->pos().toPoint(), event->delta(), event->buttons(), event->modifiers(), event->orientation());
    QApplication::sendEvent(m_canvasBase->canvasWidget(), &ev);
}

void SelectAllButtonItem::toolChanged(const QString& toolId)
{
    m_cellToolIsActive = toolId.startsWith("KSpread");
    update();
}

#include "HeaderItems.moc"
