/* This file is part of the KDE project
 * Copyright (C) 2002-2006 David Faure <faure@kde.org>
 * Copyright (C) 2005-2006 Thomas Zander <zander@kde.org>
 * Copyright (C) 2009 Inge Wallin <inge@lysator.liu.se>
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
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

// kword includes
#include "KWCanvas.h"
#include "KWGui.h"
#include "KWView.h"
#include "KWViewMode.h"
#include "KWPage.h"

// koffice libs includes
#include <KoShapeManager.h>
#include <KoPointerEvent.h>
#include <KoToolManager.h>
#include <KoCanvasController.h>
#include <KoToolProxy.h>
#include <KoGridData.h>

// KDE + Qt includes
#include <KDebug>
#include <QBrush>
#include <QPainter>
#include <QPainterPath>

KWCanvas::KWCanvas(const QString &viewMode, KWDocument *document, KWView *view, KWGui *parent)
        : QWidget(parent),
        KWCanvasBase(document, this),
        m_view(view)
{
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_InputMethodEnabled, true);
    setFocusPolicy(Qt::StrongFocus);
    connect(document, SIGNAL(pageSetupChanged()), this, SLOT(pageSetupChanged()));
    m_viewConverter = m_view->viewConverter();
    m_viewMode = KWViewMode::create(viewMode, document, this);
}

KWCanvas::~KWCanvas()
{
}

void KWCanvas::pageSetupChanged()
{
    m_viewMode->pageSetupChanged();
    updateSize();
}

void KWCanvas::updateSize()
{
    resourceManager()->setResource(KWord::CurrentPageCount, m_document->pageCount());
    emit documentSize(m_viewMode->contentsSize());
}

void KWCanvas::setDocumentOffset(const QPoint &offset)
{
    m_documentOffset = offset;
}

bool KWCanvas::snapToGrid() const
{
    return m_view->snapToGrid();
}

void KWCanvas::mouseMoveEvent(QMouseEvent *e)
{
    m_toolProxy->mouseMoveEvent(e, m_viewMode->viewToDocument(e->pos() + m_documentOffset));
}

void KWCanvas::mousePressEvent(QMouseEvent *e)
{
    m_toolProxy->mousePressEvent(e, m_viewMode->viewToDocument(e->pos() + m_documentOffset));
    if (!e->isAccepted() && e->button() == Qt::RightButton) {
        m_view->popupContextMenu(e->globalPos(), m_toolProxy->popupActionList());
        e->setAccepted(true);
    }
}

void KWCanvas::mouseReleaseEvent(QMouseEvent *e)
{
    m_toolProxy->mouseReleaseEvent(e, m_viewMode->viewToDocument(e->pos() + m_documentOffset));
}

void KWCanvas::mouseDoubleClickEvent(QMouseEvent *e)
{
    m_toolProxy->mouseDoubleClickEvent(e, m_viewMode->viewToDocument(e->pos() + m_documentOffset));
}

bool KWCanvas::event(QEvent *e)
{
    if(toolProxy()) {
        toolProxy()->processEvent(e);
    }
    return QWidget::event(e);
}

void KWCanvas::keyPressEvent(QKeyEvent *e)
{
    m_toolProxy->keyPressEvent(e);
    if (! e->isAccepted()) {
        if (e->key() == Qt::Key_Backtab
                || (e->key() == Qt::Key_Tab && (e->modifiers() & Qt::ShiftModifier)))
            focusNextPrevChild(false);
        else if (e->key() == Qt::Key_Tab)
            focusNextPrevChild(true);
    }
}

QVariant KWCanvas::inputMethodQuery(Qt::InputMethodQuery query) const
{
    return m_toolProxy->inputMethodQuery(query, *(viewConverter()));
}

void KWCanvas::keyReleaseEvent(QKeyEvent *e)
{
#ifndef NDEBUG
    // Debug keys
    if ((e->modifiers() & (Qt::AltModifier | Qt::ControlModifier | Qt::ShiftModifier))) {
        if (e->key() == Qt::Key_F) {
            document()->printDebug();
            e->accept();
            return;
        }
        if (e->key() == Qt::Key_M) {
            const QDateTime dtMark(QDateTime::currentDateTime());
            kDebug(32001) << "Developer mark:" << dtMark.toString("yyyy-MM-dd hh:mm:ss,zzz");
            e->accept();
            return;
        }
    }
#endif
    m_toolProxy->keyReleaseEvent(e);
}

void KWCanvas::tabletEvent(QTabletEvent *e)
{
    m_toolProxy->tabletEvent(e, m_viewMode->viewToDocument(e->pos() + m_documentOffset));
}

void KWCanvas::wheelEvent(QWheelEvent *e)
{
    m_toolProxy->wheelEvent(e, m_viewMode->viewToDocument(e->pos() + m_documentOffset));
}

void KWCanvas::inputMethodEvent(QInputMethodEvent *event)
{
    m_toolProxy->inputMethodEvent(event);
}

void KWCanvas::paintEvent(QPaintEvent *ev)
{
    QPainter painter(this);
    painter.eraseRect(ev->rect());
    paint(painter, ev->rect());
    painter.end();
}

void KWCanvas::setCursor(const QCursor &cursor)
{
    QWidget::setCursor(cursor);
}

void KWCanvas::updateInputMethodInfo()
{
    updateMicroFocus();
}

