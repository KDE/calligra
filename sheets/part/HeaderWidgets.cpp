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
#include "HeaderWidgets.h"
#include "Canvas.h"
#include "Doc.h"
#include "View.h"

// Qt
#include <QApplication>
#include <QLabel>
#include <QPainter>
#include <QRubberBand>
#include <QStyle>
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
 * RowHeaderWidget
 *
 ****************************************************************/

RowHeaderWidget::RowHeaderWidget(QWidget *_parent, Canvas *_canvas, View *_view)
    : QWidget(_parent)
    , RowHeader(_canvas)
    , m_rubberband(nullptr)
{
    setAttribute(Qt::WA_StaticContents);
    setMouseTracking(true);

    connect(_view, &View::autoScroll, this, &RowHeaderWidget::slotAutoScroll);
    connect(m_pCanvas->toolProxy(), &KoToolProxy::toolChanged, this, &RowHeaderWidget::toolChanged);
}

RowHeaderWidget::~RowHeaderWidget() = default;

void RowHeaderWidget::mousePressEvent(QMouseEvent *_ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mousePress(&pev);
}

void RowHeaderWidget::mouseReleaseEvent(QMouseEvent *_ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mouseRelease(&pev);
}

void RowHeaderWidget::mouseDoubleClickEvent(QMouseEvent *_ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mouseDoubleClick(&pev);
}

void RowHeaderWidget::mouseMoveEvent(QMouseEvent *_ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mouseMove(&pev);
}

void RowHeaderWidget::slotAutoScroll(const QPoint &scrollDistance)
{
    // NOTE Stefan: This slot is triggered by the same signal as
    //              Canvas::slotAutoScroll and ColumnHeaderWidget::slotAutoScroll.
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
    QMouseEvent event(QEvent::MouseMove, mapFromGlobal(QCursor::pos()), Qt::NoButton, Qt::NoButton, QApplication::keyboardModifiers());
    QApplication::sendEvent(this, &event);
    m_pCanvas->update();
}

void RowHeaderWidget::wheelEvent(QWheelEvent *_ev)
{
    QApplication::sendEvent(static_cast<Canvas *>(m_pCanvas), _ev);
}

void RowHeaderWidget::paintSizeIndicator(int mouseY)
{
    Sheet *const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

    m_iResizePos = mouseY;

    // Don't make the row have a height < 2 pixel.
    double y = m_pCanvas->zoomHandler()->zoomItY(sheet->rowPosition(m_iResizedRow) - m_pCanvas->yOffset());
    if (m_iResizePos < y + 2)
        m_iResizePos = (int)y;

    if (!m_rubberband) {
        m_rubberband = new QRubberBand(QRubberBand::Line, static_cast<Canvas *>(m_pCanvas));
        m_rubberband->setGeometry(0, m_iResizePos, m_pCanvas->width(), 2);
        m_rubberband->show();
    }
    m_rubberband->move(0, m_iResizePos);

    QString tmpSize;
    double hh = m_pCanvas->zoomHandler()->unzoomItY(m_iResizePos - y);
    double hu = m_pCanvas->doc()->unit().toUserValue(hh);
    if (hu > 0.01)
        tmpSize = i18n("Height: %1 %2", hu, m_pCanvas->doc()->unit().symbol());
    else
        tmpSize = i18n("Hide Row");

    // TODO port to qt6 and wayland
    // if (!m_lSize) {
    //     int screenNo = QApplication::desktop()->screenNumber(this);
    //     m_lSize = new QLabel(QApplication::desktop()->screen(screenNo) , Qt::ToolTip);
    //     m_lSize->setAlignment(Qt::AlignVCenter);
    //     m_lSize->setAutoFillBackground(true);
    //     m_lSize->setPalette(QToolTip::palette());
    //     m_lSize->setMargin(1 + style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, 0, m_lSize));
    //     m_lSize->setFrameShape(QFrame::Box);
    //     m_lSize->setIndent(1);
    // }

    // m_lSize->setText(tmpSize);
    // m_lSize->adjustSize();
    // QPoint pos = (sheet->layoutDirection() == Qt::RightToLeft) ? QPoint(m_pCanvas->width() - m_lSize->width() - 3, (int)y + 3) :
    //              QPoint(3, (int)y + 3);
    // pos -= QPoint(0, m_lSize->height());
    // m_lSize->move(m_pCanvas->mapToGlobal(pos).x(), m_pCanvas->mapToGlobal(pos).y());
    // m_lSize->show();
}

void RowHeaderWidget::removeSizeIndicator()
{
    delete m_rubberband;
    m_rubberband = nullptr;
}

void RowHeaderWidget::updateRows(int from, int to)
{
    Sheet *const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

    double y0 = m_pCanvas->zoomHandler()->zoomItY(sheet->rowPosition(from));
    double y1 = m_pCanvas->zoomHandler()->zoomItY(sheet->rowPosition(to + 1));
    QWidget::update(0, (int)y0, QWidget::width(), (int)(y1 - y0));
}

void RowHeaderWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    paint(&painter, event->rect());
}

void RowHeaderWidget::focusOutEvent(QFocusEvent *_ev)
{
    focusOut(_ev);
}

void RowHeaderWidget::toolChanged(const QString &toolId)
{
    doToolChanged(toolId);
}

/****************************************************************
 *
 * ColumnHeaderWidget
 *
 ****************************************************************/

ColumnHeaderWidget::ColumnHeaderWidget(QWidget *_parent, Canvas *_canvas, View *_view)
    : QWidget(_parent)
    , ColumnHeader(_canvas)
    , m_rubberband(nullptr)
{
    setAttribute(Qt::WA_StaticContents);
    setMouseTracking(true);

    connect(_view, &View::autoScroll, this, &ColumnHeaderWidget::slotAutoScroll);
    connect(m_pCanvas->toolProxy(), &KoToolProxy::toolChanged, this, &ColumnHeaderWidget::toolChanged);
}

ColumnHeaderWidget::~ColumnHeaderWidget() = default;

void ColumnHeaderWidget::mousePressEvent(QMouseEvent *_ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mousePress(&pev);
}

void ColumnHeaderWidget::mouseReleaseEvent(QMouseEvent *_ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mouseRelease(&pev);
}

void ColumnHeaderWidget::mouseDoubleClickEvent(QMouseEvent *_ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mouseDoubleClick(&pev);
}

void ColumnHeaderWidget::mouseMoveEvent(QMouseEvent *_ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mouseMove(&pev);
}

void ColumnHeaderWidget::slotAutoScroll(const QPoint &scrollDistance)
{
    // NOTE Stefan: This slot is triggered by the same signal as
    //              Canvas::slotAutoScroll and RowHeaderWidget::slotAutoScroll.
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
    QMouseEvent event(QEvent::MouseMove, mapFromGlobal(QCursor::pos()), Qt::NoButton, Qt::NoButton, QApplication::keyboardModifiers());
    QApplication::sendEvent(this, &event);
    m_pCanvas->update();
}

void ColumnHeaderWidget::wheelEvent(QWheelEvent *_ev)
{
    QApplication::sendEvent(static_cast<Canvas *>(m_pCanvas), _ev);
}

void ColumnHeaderWidget::resizeEvent(QResizeEvent *_ev)
{
    ColumnHeader::resize(_ev->size(), _ev->oldSize());
}

void ColumnHeaderWidget::paintSizeIndicator(int mouseX)
{
    Sheet *const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

    if (sheet->layoutDirection() == Qt::RightToLeft)
        m_iResizePos = mouseX + m_pCanvas->width() - QWidget::width();
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

    if (!m_rubberband) {
        m_rubberband = new QRubberBand(QRubberBand::Line, static_cast<Canvas *>(m_pCanvas));
        m_rubberband->setGeometry(m_iResizePos, 0, 2, m_pCanvas->height());
        m_rubberband->show();
    }
    m_rubberband->move(m_iResizePos, 0);

    QString tmpSize;
    double ww = m_pCanvas->zoomHandler()->unzoomItX((sheet->layoutDirection() == Qt::RightToLeft) ? x - m_iResizePos : m_iResizePos - x);
    double wu = m_pCanvas->doc()->unit().toUserValue(ww);
    if (wu > 0.01)
        tmpSize = i18n("Width: %1 %2", wu, m_pCanvas->doc()->unit().symbol());
    else
        tmpSize = i18n("Hide Column");

    // if (!m_lSize) {
    //     int screenNo = QApplication::desktop()->screenNumber(this);
    //     m_lSize = new QLabel(QApplication::desktop()->screen(screenNo) , Qt::ToolTip);
    //     m_lSize->setAlignment(Qt::AlignVCenter);
    //     m_lSize->setAutoFillBackground(true);
    //     m_lSize->setPalette(QToolTip::palette());
    //     m_lSize->setMargin(1 + style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, 0, m_lSize));
    //     m_lSize->setFrameShape(QFrame::Box);
    //     m_lSize->setIndent(1);
    // }

    // m_lSize->setText(tmpSize);
    // m_lSize->adjustSize();
    // QPoint pos = (sheet->layoutDirection() == Qt::RightToLeft) ? QPoint((int) x - 3 - m_lSize->width(), 3) :
    //              QPoint((int) x + 3, 3);
    // pos -= QPoint(0, m_lSize->height());
    // m_lSize->move(m_pCanvas->mapToGlobal(pos).x(), mapToGlobal(pos).y());
    // m_lSize->show();
}

void ColumnHeaderWidget::removeSizeIndicator()
{
    delete m_rubberband;
    m_rubberband = nullptr;
}

void ColumnHeaderWidget::updateColumns(int from, int to)
{
    Sheet *const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

    double x0 = m_pCanvas->zoomHandler()->zoomItX(sheet->columnPosition(from));
    double x1 = m_pCanvas->zoomHandler()->zoomItX(sheet->columnPosition(to + 1));
    QWidget::update((int)x0, 0, (int)(x1 - x0), QWidget::height());
}

void ColumnHeaderWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    paint(&painter, event->rect());
}

void ColumnHeaderWidget::focusOutEvent(QFocusEvent *_ev)
{
    focusOut(_ev);
}

void ColumnHeaderWidget::toolChanged(const QString &toolId)
{
    doToolChanged(toolId);
}

/****************************************************************
 *
 * SelectAllButtonWidget
 *
 ****************************************************************/

SelectAllButtonWidget::SelectAllButtonWidget(CanvasBase *canvasBase)
    : QWidget(canvasBase->canvasWidget())
    , SelectAllButton(canvasBase)
{
    connect(canvasBase->toolProxy(), &KoToolProxy::toolChanged, this, &SelectAllButtonWidget::toolChanged);
}

SelectAllButtonWidget::~SelectAllButtonWidget() = default;

void SelectAllButtonWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    paint(&painter, event->rect());
}

void SelectAllButtonWidget::mousePressEvent(QMouseEvent *_ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mousePress(&pev);
}

void SelectAllButtonWidget::mouseReleaseEvent(QMouseEvent *_ev)
{
    KoPointerEvent pev(_ev, QPointF());
    mouseRelease(&pev);
}

void SelectAllButtonWidget::wheelEvent(QWheelEvent *_ev)
{
    QApplication::sendEvent(static_cast<Canvas *>(m_canvasBase), _ev);
}

void SelectAllButtonWidget::toolChanged(const QString &toolId)
{
    doToolChanged(toolId);
}
