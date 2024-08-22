/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Robert Knight <robertknight@gmail.com>
   SPDX-FileCopyrightText: 2006 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 1999-2002, 2004 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2002-2005 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 1999-2004 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2004-2005 Meni Livne <livne@kde.org>
   SPDX-FileCopyrightText: 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2003 Hamish Rodda <rodda@kde.org>
   SPDX-FileCopyrightText: 2003 Joseph Wenninger <jowenn@kde.org>
   SPDX-FileCopyrightText: 2003 Lukas Tinkl <lukas@kde.org>
   SPDX-FileCopyrightText: 2000-2002 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 2002 Harri Porten <porten@kde.org>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 2002 Daniel Naber <daniel.naber@t-online.de>
   SPDX-FileCopyrightText: 1999-2000 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 1999-2000 Stephan Kulow <coolo@kde.org>
   SPDX-FileCopyrightText: 2000 Bernd Wuebben <wuebben@kde.org>
   SPDX-FileCopyrightText: 2000 Wilco Greven <greven@kde.org>
   SPDX-FileCopyrightText: 2000 Simon Hausmann <hausmann@kde.org
   SPDX-FileCopyrightText: 1999 Michael Reiher <michael.reiher@gmx.de>
   SPDX-FileCopyrightText: 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>
   SPDX-FileCopyrightText: 1999 Reginald Stadlbauer <reggie@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "HeaderItems.h"
#include "CanvasBase.h"
#include "CanvasItem.h"
#include "Doc.h"

// Qt
#include <QApplication>
#include <QGraphicsSceneResizeEvent>
#include <QLabel>
#include <QStyleOptionGraphicsItem>
#include <QToolTip>

// KF5
#include <KLocalizedString>

// Calligra
#include <KoPointerEvent.h>
#include <KoToolProxy.h>
#include <KoUnit.h>
#include <KoZoomHandler.h>

// Sheets
#include "core/Sheet.h"

using namespace Calligra::Sheets;

/****************************************************************
 *
 * RowHeaderItem
 *
 ****************************************************************/

RowHeaderItem::RowHeaderItem(QGraphicsItem *_parent, CanvasItem *_canvas)
    : QGraphicsWidget(_parent)
    , RowHeader(_canvas)
{
    // setMouseTracking(true);
    setAcceptHoverEvents(true);

    // connect(m_pView, SIGNAL(autoScroll(QPoint)),
    // this, SLOT(slotAutoScroll(QPoint)));
    connect(m_pCanvas->toolProxy(), &KoToolProxy::toolChanged, this, &RowHeaderItem::toolChanged);

    setFlag(ItemClipsToShape, true);
}

RowHeaderItem::~RowHeaderItem() = default;

void RowHeaderItem::mousePressEvent(QGraphicsSceneMouseEvent *_ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mousePress(&pev);
}

void RowHeaderItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *_ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mouseRelease(&pev);
}

void RowHeaderItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *_ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mouseDoubleClick(&pev);
}

void RowHeaderItem::mouseMoveEvent(QGraphicsSceneMouseEvent *_ev)
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
void RowHeaderItem::wheelEvent(QGraphicsSceneWheelEvent *_ev)
{
    Q_UNUSED(_ev);
    // TODO XXX
}

void RowHeaderItem::paintSizeIndicator(int mouseY)
{
    Sheet *const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

    m_iResizePos = mouseY;

    // Don't make the row have a height < 2 pixel.
    double y = m_pCanvas->zoomHandler()->zoomItY(sheet->rowPosition(m_iResizedRow) - m_pCanvas->yOffset());
    if (m_iResizePos < y + 2)
        m_iResizePos = (int)y;

    // XXX: Port to QGraphicsView
    // if (!m_rubberband) {
    //    m_rubberband = new QRubberBand(QRubberBand::Line, m_pCanvas);
    //    m_rubberband->setGeometry(0, m_iResizePos, m_pCanvas->width(), 2);
    //    m_rubberband->show();
    //}
    // m_rubberband->move(0, m_iResizePos);

    QString tmpSize;
    double hh = m_pCanvas->zoomHandler()->unzoomItY(m_iResizePos - y);
    double hu = m_pCanvas->doc()->unit().toUserValue(hh);
    if (hu > 0.01)
        tmpSize = i18n("Height: %1 %2", hu, m_pCanvas->doc()->unit().symbol());
    else
        tmpSize = i18n("Hide Row");

    // if (!m_lSize) {
    //     int screenNo = 0; //QApplication::desktop()->screenNumber(topLevelWidget());
    //     m_lSize = new QLabel(qApp->primaryScreen()-> , Qt::ToolTip);
    //     m_lSize->setAlignment(Qt::AlignVCenter);
    //     m_lSize->setAutoFillBackground(true);
    //     m_lSize->setPalette(QToolTip::palette());
    //     m_lSize->setMargin(1 + style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, 0, m_lSize));
    //     m_lSize->setFrameShape(QFrame::Box);
    //     m_lSize->setIndent(1);
    // }

    // m_lSize->setText(tmpSize);
    // m_lSize->adjustSize();
    // QRectF rcf = static_cast<CanvasItem*>(m_pCanvas)->boundingRect();
    // QPoint pos = (sheet->layoutDirection() == Qt::RightToLeft) ? QPoint(rcf.width() - m_lSize->width() - 3, (int)y + 3) : QPoint(3, (int)y + 3);
    // pos -= QPoint(0, m_lSize->height());
    //// XXX: Port
    ////m_lSize->move(m_pCanvas->mapToGlobal(pos).x(), m_pCanvas->mapToGlobal(pos).y());
    // m_lSize->show();
}

void RowHeaderItem::removeSizeIndicator()
{
    // XXX TODO
}

void RowHeaderItem::updateRows(int from, int to)
{
    Sheet *const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

    double y0 = m_pCanvas->zoomHandler()->zoomItY(sheet->rowPosition(from));
    double y1 = m_pCanvas->zoomHandler()->zoomItY(sheet->rowPosition(to + 1));
    QGraphicsItem::update(0, (int)y0, boundingRect().width(), (int)(y1 - y0));
}

void RowHeaderItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    RowHeader::paint(painter, option->exposedRect);
}

void RowHeaderItem::focusOutEvent(QFocusEvent *_ev)
{
    focusOut(_ev);
}

void RowHeaderItem::toolChanged(const QString &toolId)
{
    doToolChanged(toolId);
}

/****************************************************************
 *
 * ColumnHeaderItem
 *
 ****************************************************************/

ColumnHeaderItem::ColumnHeaderItem(QGraphicsItem *_parent, CanvasItem *_canvas)
    : QGraphicsWidget(_parent)
    , ColumnHeader(_canvas)
{
    // setMouseTracking(true);
    setAcceptHoverEvents(true);

    // connect(_view, SIGNAL(autoScroll(QPoint)),
    // this, SLOT(slotAutoScroll(QPoint)));
    connect(_canvas->toolProxy(), &KoToolProxy::toolChanged, this, &ColumnHeaderItem::toolChanged);

    setFlag(ItemClipsToShape, true);
}

ColumnHeaderItem::~ColumnHeaderItem() = default;

void ColumnHeaderItem::mousePressEvent(QGraphicsSceneMouseEvent *_ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mousePress(&pev);
}

void ColumnHeaderItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *_ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mouseRelease(&pev);
}

void ColumnHeaderItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *_ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mouseDoubleClick(&pev);
}

void ColumnHeaderItem::mouseMoveEvent(QGraphicsSceneMouseEvent *_ev)
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
void ColumnHeaderItem::wheelEvent(QGraphicsSceneWheelEvent *_ev)
{
    Q_UNUSED(_ev);
    // TODO XXX
}

void ColumnHeaderItem::resizeEvent(QGraphicsSceneResizeEvent *_ev)
{
    ColumnHeader::resize(_ev->newSize(), _ev->oldSize());
}

void ColumnHeaderItem::paintSizeIndicator(int mouseX)
{
    Sheet *const sheet = m_pCanvas->activeSheet();
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
            m_iResizePos = (int)x;
    } else {
        if (m_iResizePos < x + 2)
            m_iResizePos = (int)x;
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
        tmpSize = i18n("Width: %1 %2", wu, m_pCanvas->doc()->unit().symbol());
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
    Sheet *const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

    double x0 = m_pCanvas->zoomHandler()->zoomItX(sheet->columnPosition(from));
    double x1 = m_pCanvas->zoomHandler()->zoomItX(sheet->columnPosition(to + 1));
    QGraphicsItem::update((int)x0, 0, (int)(x1 - x0), boundingRect().height());
}

void ColumnHeaderItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    ColumnHeader::paint(painter, option->exposedRect);
}

void ColumnHeaderItem::focusOutEvent(QFocusEvent *_ev)
{
    focusOut(_ev);
}

void ColumnHeaderItem::toolChanged(const QString &toolId)
{
    doToolChanged(toolId);
}

void ColumnHeaderItem::scroll(qreal x, qreal y)
{
    if (m_pCanvas->layoutDirection() == Qt::RightToLeft) {
        QGraphicsWidget::scroll(-x, y);
    } else {
        QGraphicsWidget::scroll(x, y);
    }
}

/****************************************************************
 *
 * SelectAllButtonItem
 *
 ****************************************************************/

SelectAllButtonItem::SelectAllButtonItem(QGraphicsItem *_parent, CanvasBase *canvasBase)
    : QGraphicsWidget(_parent)
    , SelectAllButton(canvasBase)
{
    connect(canvasBase->toolProxy(), &KoToolProxy::toolChanged, this, &SelectAllButtonItem::toolChanged);
}

SelectAllButtonItem::~SelectAllButtonItem() = default;

void SelectAllButtonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    SelectAllButton::paint(painter, option->exposedRect);
}

void SelectAllButtonItem::mousePressEvent(QGraphicsSceneMouseEvent *_ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mousePress(&pev);
}

void SelectAllButtonItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *_ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mouseRelease(&pev);
}

void SelectAllButtonItem::wheelEvent(QGraphicsSceneWheelEvent *_ev)
{
    Q_UNUSED(_ev);
    // TODO XXX
}

void SelectAllButtonItem::toolChanged(const QString &toolId)
{
    doToolChanged(toolId);
}
