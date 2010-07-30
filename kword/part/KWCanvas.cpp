/* This file is part of the KDE project
 * Copyright (C) 2002-2006 David Faure <faure@kde.org>
 * Copyright (C) 2005-2006 Thomas Zander <zander@kde.org>
 * Copyright (C) 2009 Inge Wallin <inge@lysator.liu.se>
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

// #define DEBUG_REPAINT


KWCanvas::KWCanvas(const QString &viewMode, KWDocument *document, KWView *view, KWGui *parent)
        : QWidget(parent),
        KoCanvasBase(document),
        m_document(document),
        m_view(view),
        m_viewMode(0)
{
    m_shapeManager = new KoShapeManager(this);
    m_viewMode = KWViewMode::create(viewMode, document, this);
    setFocusPolicy(Qt::StrongFocus);

    connect(document, SIGNAL(pageSetupChanged()), this, SLOT(pageSetupChanged()));

    m_toolProxy = new KoToolProxy(this, this);
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_InputMethodEnabled, true);
}

KWCanvas::~KWCanvas()
{
    delete m_shapeManager;
    m_shapeManager = 0;
    delete m_viewMode;
    m_viewMode = 0;
    m_toolProxy = 0;
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

void KWCanvas::gridSize(qreal *horizontal, qreal *vertical) const
{
    *horizontal = m_document->gridData().gridX();
    *vertical = m_document->gridData().gridY();
}

bool KWCanvas::snapToGrid() const
{
    return m_view->snapToGrid();
}

void KWCanvas::addCommand(QUndoCommand *command)
{
    m_document->addCommand(command);
}

void KWCanvas::updateCanvas(const QRectF &rc)
{
    QRectF zoomedRect = m_viewMode->documentToView(rc);
    QList<KWViewMode::ViewMap> map = m_viewMode->clipRectToDocument(zoomedRect.toRect());
    foreach (KWViewMode::ViewMap vm, map) {
        vm.clipRect.adjust(-2, -2, 2, 2); // grow for anti-aliasing
        QRect finalClip((int)(vm.clipRect.x() + vm.distance.x() - m_documentOffset.x()),
                        (int)(vm.clipRect.y() + vm.distance.y() - m_documentOffset.y()),
                        vm.clipRect.width(), vm.clipRect.height());
        update(finalClip);
    }
}

const KoViewConverter *KWCanvas::viewConverter() const
{
    return m_view->viewConverter();
}

void KWCanvas::clipToDocument(const KoShape *shape, QPointF &move) const
{
    Q_ASSERT(shape);
    const QPointF absPos = shape->absolutePosition();
    const QPointF destination = absPos + move;
    qreal bottomOfPage = 0.0;
    KWPage page;
    foreach (const KWPage &p, m_document->pageManager()->pages()) {
        bottomOfPage += p.height();
        if (bottomOfPage >= absPos.y())
            page = p;
        if (bottomOfPage >= destination.y()) {
            page = p;
            break;
        }
    }
    if (!page.isValid()) { // shape was not in any page to begin with, can't propose anything sane...
        move.setX(0);
        move.setY(0);
        return;
    }
    QRectF pageRect(page.rect().adjusted(5, 5, -5, -5));
    QPainterPath path(shape->absoluteTransformation(0).map(shape->outline()));
    QRectF shapeBounds = path.boundingRect();
    shapeBounds.moveTopLeft(shapeBounds.topLeft() + move);
    if (!shapeBounds.intersects(pageRect)) {
        if (shapeBounds.left() > pageRect.right()) // need to move to the left some
            move.setX(move.x() + (pageRect.right() - shapeBounds.left()));
        else if (shapeBounds.right() < pageRect.left()) // need to move to the right some
            move.setX(move.x() + pageRect.left() - shapeBounds.right());

        if (shapeBounds.top() > pageRect.bottom()) // need to move up some
            move.setY(move.y() + (pageRect.bottom() - shapeBounds.top()));
        else if (shapeBounds.bottom() < pageRect.top()) // need to move down some
            move.setY(move.y() + pageRect.top() - shapeBounds.bottom());
    }
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

void KWCanvas::updateInputMethodInfo()
{
    updateMicroFocus();
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

KoGuidesData *KWCanvas::guidesData()
{
    return &m_document->guidesData();
}

void KWCanvas::ensureVisible(const QRectF &rect)
{
    QRectF viewRect = m_viewMode->documentToView(rect);
    canvasController()->ensureVisible(viewRect);
}

#ifdef DEBUG_REPAINT
# include <stdlib.h>
#endif
void KWCanvas::paintEvent(QPaintEvent * ev)
{
    QPainter painter(this);
    painter.eraseRect(ev->rect());
    painter.translate(-m_documentOffset);

    if (m_viewMode->hasPages()) {
        int pageContentArea = 0;

        // Create a list of clipRects in the document space from the
        // current view. Each rect corresponds to a part of a page
        // that is shown on the canvas.
        //
        // Then go through them and paint each one.
        QList<KWViewMode::ViewMap> map = m_viewMode->clipRectToDocument(ev->rect().translated(m_documentOffset));
        foreach (KWViewMode::ViewMap vm, map) {
            painter.save();

            // Set up the painter to clip the part of the canvas that contains the rect.
            painter.translate(vm.distance.x(), vm.distance.y());
            vm.clipRect = vm.clipRect.adjusted(-1, -1, 1, 1);
            painter.setClipRect(vm.clipRect);

            // Paint the background of the page.
            QColor color = Qt::white; // TODO paper background
#ifdef DEBUG_REPAINT
            color = QColor(random() % 255, random() % 255, random() % 255);
#endif
            painter.fillRect(vm.clipRect, QBrush(color));

            // Paint the page decorations: border, shadow, etc.
            paintPageDecorations(painter, vm);

            // Paint the contents of the page.
            painter.setRenderHint(QPainter::Antialiasing);
            m_shapeManager->paint(painter, *(viewConverter()), false);

            // Paint the grid
            painter.save();
            painter.translate(-vm.distance.x(), -vm.distance.y());
            painter.setRenderHint(QPainter::Antialiasing, false);
            const QRectF clipRect = viewConverter()->viewToDocument(vm.clipRect);
            document()->gridData().paintGrid(painter, *(viewConverter()), clipRect);
            document()->guidesData().paintGuides(painter, *(viewConverter()), clipRect);
            painter.restore();

            // paint whatever the tool wants to paint
            m_toolProxy->paint(painter, *(viewConverter()));
            painter.restore();

            int contentArea = qRound(vm.clipRect.width() * vm.clipRect.height());
            if (contentArea > pageContentArea)
                pageContentArea = contentArea;
        }
    } else {
        // TODO paint the main-text-flake directly
        kWarning(32003) << "Non-page painting not implemented yet!";
    }

    painter.end();
}

void KWCanvas::paintPageDecorations(QPainter &painter, KWViewMode::ViewMap &viewMap)
{
    painter.save();

    const QRectF       pageRect = viewMap.page.rect();
    const KoPageLayout pageLayout = viewMap.page.pageStyle().pageLayout();

    // Get the coordinates of the border rect in view coordinates.
    QPointF topLeftCorner = viewConverter()->documentToView(pageRect.topLeft()
                                                            + QPointF(pageLayout.leftMargin,
                                                                      pageLayout.topMargin));
    QPointF bottomRightCorner = viewConverter()->documentToView(pageRect.bottomRight()
                                                                + QPointF(-pageLayout.rightMargin,
                                                                          -pageLayout.bottomMargin));
    QRectF borderRect = QRectF(topLeftCorner, bottomRightCorner);

    // Actually paint the border
    paintBorder(painter, pageLayout.border, borderRect);

    painter.restore();
}

void KWCanvas::paintBorder(QPainter &painter, const KoBorder &border, const QRectF &borderRect) const
{
    // Get the zoom.
    qreal zoomX;
    qreal zoomY;
    viewConverter()->zoom(&zoomX, &zoomY);

    KoBorder::BorderData borderSide = border.leftBorderData();
    painter.save();
    paintBorderSide(painter, borderSide, borderRect.topLeft(), borderRect.bottomLeft(),
                    zoomX, 1, 0);
    borderSide = border.topBorderData();
    painter.restore();
    painter.save();
    paintBorderSide(painter, borderSide, borderRect.topLeft(), borderRect.topRight(),
                    zoomY, 0, 1);

    borderSide = border.rightBorderData();
    painter.restore();
    painter.save();
    paintBorderSide(painter, borderSide, borderRect.topRight(), borderRect.bottomRight(),
                    zoomX, -1, 0);

    borderSide = border.bottomBorderData();
    painter.restore();
    painter.save();
    paintBorderSide(painter, borderSide, borderRect.bottomLeft(), borderRect.bottomRight(),
                    zoomY, 0, -1);
    painter.restore();
}

void KWCanvas::paintBorderSide(QPainter &painter, const KoBorder::BorderData &borderData,
                               const QPointF &lineStart, const QPointF &lineEnd, qreal zoom,
                               int inwardsX, int inwardsY) const
{
    // Return if nothing to paint
    if (borderData.style == KoBorder::BorderNone)
        return;

    // Set up the painter and inner and outer pens.
    QPen pen = painter.pen();
    // Line color
    pen.setColor(borderData.color);

    // Line style
    switch (borderData.style) {
    case KoBorder::BorderNone: break; // No line
    case KoBorder::BorderDotted: pen.setStyle(Qt::DotLine); break;
    case KoBorder::BorderDashed: pen.setStyle(Qt::DashLine); break;
    case KoBorder::BorderSolid: pen.setStyle(Qt::SolidLine); break;
    case KoBorder::BorderDouble: pen.setStyle(Qt::SolidLine); break; // Handled separately
    case KoBorder::BorderGroove: pen.setStyle(Qt::SolidLine); break; // FIXME
    case KoBorder::BorderRidge: pen.setStyle(Qt::SolidLine); break; // FIXME
    case KoBorder::BorderInset: pen.setStyle(Qt::SolidLine); break; // FIXME
    case KoBorder::BorderOutset: pen.setStyle(Qt::SolidLine); break; // FIXME

    case KoBorder::BorderDashDotPattern: pen.setStyle(Qt::DashDotLine); break;
    case KoBorder::BorderDashDotDotPattern: pen.setStyle(Qt::DashDotDotLine); break;
    }

    if (borderData.style == KoBorder::BorderDouble) {
        // outerWidth is the width of the outer line.  The offsets
        // are the distances from the center line of the whole
        // border to the centerlines of the outer and inner
        // borders respectively.
        qreal outerWidth = borderData.width - borderData.innerWidth - borderData.spacing;
        qreal outerOffset = borderData.width / 2.0 + outerWidth / 2.0;
        qreal innerOffset = borderData.width / 2.0 - borderData.innerWidth / 2.0;

        QPointF outerOffset2D(-inwardsX * outerOffset, -inwardsY * outerOffset);
        QPointF innerOffset2D(inwardsX * innerOffset, inwardsY * innerOffset);

        // Draw the outer line.
        pen.setWidthF(zoom * outerWidth);
        painter.setPen(pen);
        painter.drawLine(lineStart + outerOffset2D, lineEnd + outerOffset2D);

        // Draw the inner line
        pen.setWidthF(zoom * borderData.innerWidth);
        painter.setPen(pen);
        painter.drawLine(lineStart + innerOffset2D, lineEnd + innerOffset2D);
    }
    else {
        pen.setWidthF(zoom * borderData.width);
        painter.setPen(pen);
        painter.drawLine(lineStart, lineEnd);
    }
}

QCursor KWCanvas::setCursor(const QCursor &cursor)
{
    QCursor oldCursor = QWidget::cursor();
    QWidget::setCursor(cursor);
    return oldCursor;
}
