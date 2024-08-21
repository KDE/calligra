/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2009 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2006 Robert Knight <robertknight@gmail.com>
   SPDX-FileCopyrightText: 2006 Inge Wallin <inge@lysator.liu.se>
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
#include "CanvasBase.h"
#include "CanvasBase_p.h"

#include "Doc.h"
#include "Headers.h"

// Qt
#include <QMimeData>
#include <QPainter>
#include <QToolTip>

// Calligra
#include <KoPointerEvent.h>
#include <KoShapeManager.h>
#include <KoToolProxy.h>
#include <KoUnit.h>
#include <KoZoomHandler.h>

// Sheets
#include "core/Cell.h"
#include "core/ColFormatStorage.h"
#include "core/Map.h"
#include "core/RowFormatStorage.h"
#include "core/Sheet.h"

// ui
#include "ui/CellView.h"
#include "ui/Selection.h"
#include "ui/SheetView.h"
#include "ui/commands/PasteCommand.h"

#define MIN_SIZE 10

using namespace Calligra::Sheets;

/****************************************************************
 *
 * CanvasBase
 *
 ****************************************************************/

CanvasBase::CanvasBase(Doc *doc)
    : KoCanvasBase(nullptr)
    , d(new Private)
{
    d->offset = QPointF(0.0, 0.0);
    d->doc = doc;

    // flake
    d->shapeManager = new KoShapeManager(this);
    d->toolProxy = new KoToolProxy(this);
}

CanvasBase::~CanvasBase()
{
    delete d->shapeManager;
    delete d->toolProxy;
    delete d;
}

Doc *CanvasBase::doc() const
{
    return d->doc;
}

void CanvasBase::gridSize(qreal *horizontal, qreal *vertical) const
{
    *horizontal = doc()->map()->defaultColumnFormat().width;
    *vertical = doc()->map()->defaultRowFormat().height;
}

bool CanvasBase::snapToGrid() const
{
    return false; // FIXME
}

void CanvasBase::addCommand(KUndo2Command *command)
{
    doc()->addCommand(command);
}

KoShapeManager *CanvasBase::shapeManager() const
{
    return d->shapeManager;
}

void CanvasBase::updateCanvas(const QRectF &rc)
{
    QRectF clipRect(viewConverter()->documentToView(rc.translated(-offset())));
    clipRect.adjust(-2, -2, 2, 2); // Resize to fit anti-aliasing
    update(clipRect);
}

KoUnit CanvasBase::unit() const
{
    return doc()->unit();
}

KoToolProxy *CanvasBase::toolProxy() const
{
    return d->toolProxy;
}

QPointF CanvasBase::offset() const
{
    return d->offset;
}

double CanvasBase::xOffset() const
{
    return d->offset.x();
}

double CanvasBase::yOffset() const
{
    return d->offset.y();
}

bool CanvasBase::eventFilter(QObject *o, QEvent *e)
{
    /* this canvas event filter acts on events sent to the line edit as well
       as events to this filter itself.
    */
    if (!o || !e)
        return true;
    switch (e->type()) {
    case QEvent::KeyPress: {
        QKeyEvent *keyev = static_cast<QKeyEvent *>(e);
        if ((keyev->key() == Qt::Key_Tab) || (keyev->key() == Qt::Key_Backtab)) {
            keyPressed(keyev);
            return true;
        }
        break;
    }
    case QEvent::InputMethod: {
        // QIMEvent * imev = static_cast<QIMEvent *>(e);
        // processIMEvent( imev );
        break;
    }
    case QEvent::ToolTip: {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(e);
        showToolTip(helpEvent->pos());
    }
    default:
        break;
    }
    return false;
}

void CanvasBase::setDocumentOffset(const QPoint &offset)
{
    const QPoint delta = offset - viewConverter()->documentToView(d->offset).toPoint();
    d->offset = viewConverter()->viewToDocument(offset);

    ColumnHeader *ch = columnHeader();
    if (ch)
        ch->scroll(-delta.x(), 0);
    RowHeader *rh = rowHeader();
    if (rh)
        rh->scroll(0, -delta.y());
}

void CanvasBase::setDocumentSize(const QSizeF &size)
{
    const QSize s = viewConverter()->documentToView(size).toSize();
    documentSizeChanged(s);
}

void CanvasBase::mousePressed(KoPointerEvent *event)
{
    KoPointerEvent *const origEvent = event;
    QPointF documentPosition;
    if (layoutDirection() == Qt::LeftToRight) {
        documentPosition = viewConverter()->viewToDocument(event->position()) + offset();
    } else {
        const QPoint position(width() - event->x(), event->y());
        const QPointF offset(this->offset().x(), this->offset().y());
        documentPosition = viewConverter()->viewToDocument(position) + offset;
        /*XXX TODO
        debugSheets << "----------------------------";
        debugSheets << "event->pos():" << event->pos();
        debugSheets << "event->globalPos():" << event->globalPos();
        debugSheets << "position:" << position;
        debugSheets << "offset:" << offset;
        debugSheets << "documentPosition:" << documentPosition;
        event = new QMouseEvent(QEvent::MouseButtonPress, position, mapToGlobal(position), event->button(), event->buttons(), event->modifiers());
        debugSheets << "newEvent->pos():" << event->pos();
        debugSheets << "newEvent->globalPos():" << event->globalPos();*/
    }

    event = new KoPointerEvent(event, documentPosition);

    // flake
    if (d->toolProxy) {
        d->toolProxy->mousePressEvent(event);
        if (!event->isAccepted() && event->button() == Qt::RightButton) {
            showContextMenu(origEvent->globalPosition().toPoint());
            origEvent->accept();
        }
    }
    if (layoutDirection() == Qt::RightToLeft) {
        // delete event;
    }
    delete event;
}

void CanvasBase::mouseReleased(KoPointerEvent *event)
{
    QPointF documentPosition;
    if (layoutDirection() == Qt::LeftToRight) {
        documentPosition = viewConverter()->viewToDocument(event->position()) + offset();
    } else {
        const QPoint position(width() - event->x(), event->y());
        const QPointF offset(this->offset().x(), this->offset().y());
        documentPosition = viewConverter()->viewToDocument(position) + offset;
        // XXX TODO event = new QMouseEvent(QEvent::MouseButtonRelease, position, mapToGlobal(position), event->button(), event->buttons(), event->modifiers());
    }

    event = new KoPointerEvent(event, documentPosition);

    // flake
    if (d->toolProxy)
        d->toolProxy->mouseReleaseEvent(event);

    if (layoutDirection() == Qt::RightToLeft) {
        // delete event;
    }
    delete event;
}

void CanvasBase::mouseMoved(KoPointerEvent *event)
{
    QPointF documentPosition;
    if (layoutDirection() == Qt::LeftToRight) {
        documentPosition = viewConverter()->viewToDocument(event->position()) + offset();
    } else {
        const QPoint position(width() - event->x(), event->y());
        const QPointF offset(this->offset().x(), this->offset().y());
        documentPosition = viewConverter()->viewToDocument(position) + offset;
        // XXX TODO event = new QMouseEvent(QEvent::MouseMove, position, mapToGlobal(position), event->button(), event->buttons(), event->modifiers());
    }

    event = new KoPointerEvent(event, documentPosition);

    // flake
    if (d->toolProxy)
        d->toolProxy->mouseMoveEvent(event);

    if (layoutDirection() == Qt::RightToLeft) {
        // delete event;
    }
    delete event;
}

void CanvasBase::mouseDoubleClicked(KoPointerEvent *event)
{
    QPointF documentPosition;
    if (layoutDirection() == Qt::LeftToRight) {
        documentPosition = viewConverter()->viewToDocument(event->position()) + offset();
    } else {
        const QPoint position(width() - event->x(), event->y());
        const QPointF offset(this->offset().x(), this->offset().y());
        documentPosition = viewConverter()->viewToDocument(position) + offset;
        // XXX TODO event = new QMouseEvent(QEvent::MouseButtonDblClick, position, mapToGlobal(position), event->button(), event->buttons(),
        // event->modifiers());
    }

    event = new KoPointerEvent(event, documentPosition);

    // flake
    if (d->toolProxy)
        d->toolProxy->mouseDoubleClickEvent(event);

    if (layoutDirection() == Qt::RightToLeft) {
        // delete event;
    }
    delete event;
}

void CanvasBase::keyPressed(QKeyEvent *event)
{
    // flake
    if (d->toolProxy)
        d->toolProxy->keyPressEvent(event);
}

void CanvasBase::tabletEvent(QTabletEvent *e)
{
    // flake
    if (d->toolProxy)
        d->toolProxy->tabletEvent(e, viewConverter()->viewToDocument(e->pos() + offset()));
}

QVariant CanvasBase::inputMethodQuery(Qt::InputMethodQuery query) const
{
    // flake
    return d->toolProxy ? d->toolProxy->inputMethodQuery(query, *(viewConverter())) : 0;
}

void CanvasBase::inputMethodEvent(QInputMethodEvent *event)
{
    // flake
    if (d->toolProxy)
        d->toolProxy->inputMethodEvent(event);
}

void CanvasBase::paint(QPainter *painter, const QRectF &painterRect)
{
    if (doc()->map()->isLoading() || isViewLoading())
        return;

    Sheet *const sheet = activeSheet();
    if (!sheet)
        return;

    //     ElapsedTime et("Painting cells", ElapsedTime::PrintOnlyTime);

    painter->setClipRect(painterRect);
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter->save();

    // After the scaling, the painter methods need document coordinates!
    qreal zoomX, zoomY;
    viewConverter()->zoom(&zoomX, &zoomY);
    painter->scale(zoomX, zoomY);

    const bool layoutReversed = sheet->layoutDirection() == Qt::RightToLeft;
    const QPointF offset(layoutReversed ? -this->offset().x() : this->offset().x(), this->offset().y());
    painter->translate(-offset);

    // erase background
    const QRectF paintRect(viewConverter()->viewToDocument(rect()).translated(offset));
    painter->fillRect(paintRect, painter->background());

    // paint visible cells
    const QRect visibleRect = visibleCells();
    const QPointF topLeft(sheet->columnPosition(visibleRect.left()), sheet->rowPosition(visibleRect.top()));
    sheetView(sheet)->setPaintCellRange(visibleRect);
    sheetView(sheet)->paintCells(*painter, paintRect, topLeft, this);

    // flake
    painter->restore();
    // d->offset is the negated CanvasController offset in document coordinates.
    //     painter.save();
    painter->translate(-viewConverter()->documentToView(offset));
    d->shapeManager->paint(*painter, *viewConverter(), false);
    //     painter.restore();
    //     const QPointF p = -viewConverter()->documentToView(this->offset());
    //     painter.translate(p.x() /*+ width()*/, p.y());
    painter->setRenderHint(QPainter::Antialiasing, false);
    if (d->toolProxy)
        d->toolProxy->paint(*painter, *viewConverter());
}

void CanvasBase::focusIn(QFocusEvent *event)
{
    Q_UNUSED(event);
    // If we are in editing mode, we redirect the
    // focus to the CellEditor or ExternalEditor.
    // Using a focus proxy does not work here, because in reference selection
    // mode clicking on the canvas to select a reference should end up in the
    // editor, which got the focus before. This is determined by storing the
    // last editor with focus. It is set by the editors on getting focus by user
    // interaction. Setting a focus proxy would always result in the proxy being
    // the last editor, because clicking the canvas is a user interaction.
    // This screws up <Tab> though (David)
    selection()->emitRequestFocusEditor();
    // XXX TODO QWidget::focusInEvent(event);
}

bool CanvasBase::dragEnter(const QMimeData *mimeData)
{
    if (mimeData->hasText() || mimeData->hasFormat("application/x-calligra-sheets-snippet")) {
        return true;
    }
    return false;
}

bool CanvasBase::dragMove(const QMimeData *mimeData, const QPointF &eventPos, const QObject *source)
{
    Sheet *const sheet = activeSheet();
    if (!sheet) {
        return false;
    }

    if (mimeData->hasText() || mimeData->hasFormat("application/x-calligra-sheets-snippet")) {
        // acceptProposedAction
    } else {
        return false;
    }
#if 0 // TODO Stefan: implement drag marking rectangle
    QRect dragMarkingRect;
    if (mimeData->hasFormat("application/x-calligra-sheets-snippet")) {
        if (source == canvasWidget()) {
            debugSheetsUI << "source == this";
            dragMarkingRect = selection()->boundingRect();
        } else {
            debugSheetsUI << "source != this";
            QByteArray data = mimeData->data("application/x-calligra-sheets-snippet");
            QString errorMsg;
            int errorLine;
            int errorColumn;
            QDomDocument doc;
            if (!doc.setContent(data, false, &errorMsg, &errorLine, &errorColumn)) {
                // an error occurred
                debugSheetsUI << "CanvasBase::daragMoveEvent: an error occurred" << endl
                << "line: " << errorLine << " col: " << errorColumn
                << ' ' << errorMsg << Qt::endl;
                dragMarkingRect = QRect(1, 1, 1, 1);
            } else {
                QDomElement root = doc.documentElement(); // "spreadsheet-snippet"
                dragMarkingRect = QRect(1, 1,
                                        root.attribute("columns").toInt(),
                                        root.attribute("rows").toInt());
            }
        }
    } else { // if ( mimeData->hasText() )
        debugSheetsUI << "has text";
        dragMarkingRect = QRect(1, 1, 1, 1);
    }
#else
    Q_UNUSED(source);
#endif
    const QPoint dragAnchor = selection()->boundingRect().topLeft();
    double xpos = sheet->columnPosition(dragAnchor.x());
    double ypos = sheet->rowPosition(dragAnchor.y());
    double width = sheet->columnFormats()->colWidth(dragAnchor.x());
    double height = sheet->rowFormats()->rowHeight(dragAnchor.y());

    // consider also the selection rectangle
    const QRectF noGoArea(xpos - 1, ypos - 1, width + 3, height + 3);

    // determine the current position
    double eventPosX;
    if (sheet->layoutDirection() == Qt::RightToLeft) {
        eventPosX = viewConverter()->viewToDocumentX(this->width() - eventPos.x()) + xOffset();
    } else {
        eventPosX = viewConverter()->viewToDocumentX(eventPos.x()) + xOffset();
    }
    double eventPosY = viewConverter()->viewToDocumentY(eventPos.y()) + yOffset();

    if (noGoArea.contains(QPointF(eventPosX, eventPosY))) {
        return false;
        // XXX TODO event->ignore(noGoArea.toRect());
    }

#if 0 // TODO Stefan: implement drag marking rectangle
    // determine the cell position under the mouse
    qreal tmp;
    const int col = sheet->leftColumn(eventPosX, tmp);
    const int row = sheet->topRow(eventPosY, tmp);
    dragMarkingRect.moveTo(QPoint(col, row));
    debugSheetsUI << "MARKING RECT =" << dragMarkingRect;
#endif
    return true;
}

void CanvasBase::dragLeave()
{
}

bool CanvasBase::drop(const QMimeData *mimeData, const QPointF &eventPos, const QObject *source)
{
    Sheet *const sheet = activeSheet();
    // FIXME Sheet protection: Not all cells have to be protected.
    if (!sheet || sheet->isProtected()) {
        return false;
    }

    if (!PasteCommand::supports(mimeData)) {
        return false;
    }

    // Do not allow dropping onto the same position.
    const QPoint topLeft(selection()->boundingRect().topLeft());
    const double xpos = sheet->columnPosition(topLeft.x());
    const double ypos = sheet->rowPosition(topLeft.y());
    const double width = sheet->columnFormats()->colWidth(topLeft.x());
    const double height = sheet->rowFormats()->rowHeight(topLeft.y());

    const QRectF noGoArea(xpos - 1, ypos - 1, width + 3, height + 3);

    double ev_PosX;
    if (sheet->layoutDirection() == Qt::RightToLeft) {
        ev_PosX = viewConverter()->viewToDocumentX(this->width() - eventPos.x()) + xOffset();
    } else {
        ev_PosX = viewConverter()->viewToDocumentX(eventPos.x()) + xOffset();
    }
    double ev_PosY = viewConverter()->viewToDocumentY(eventPos.y()) + yOffset();

    if (noGoArea.contains(QPointF(ev_PosX, ev_PosY))) {
        return false;
    }

    // The destination cell location.
    qreal tmp;
    const int col = sheet->leftColumn(ev_PosX, tmp);
    const int row = sheet->topRow(ev_PosY, tmp);

    PasteCommand *const command = new PasteCommand();
    command->setSheet(sheet);
    command->add(Region(col, row, 1, 1, sheet));
    command->setMimeData(mimeData, (source == canvasWidget()));

    if (source == canvasWidget())
        command->setCutMode(true);

    command->execute();

    // Select the pasted cells
    const int columns = selection()->boundingRect().width();
    const int rows = selection()->boundingRect().height();
    selection()->initialize(QRect(col, row, columns, rows), sheet);

    return true;
}

QRect CanvasBase::viewToCellCoordinates(const QRectF &viewRect) const
{
    Sheet *const sheet = activeSheet();
    if (!sheet)
        return QRect();

    // NOTE Stefan: Do not consider the layout direction in this case.
    const QRectF rect = viewConverter()->viewToDocument(viewRect.normalized()).translated(offset());

    qreal tmp;
    const int left = sheet->leftColumn(rect.left(), tmp);
    const int right = sheet->rightColumn(rect.right());
    const int top = sheet->topRow(rect.top(), tmp);
    const int bottom = sheet->bottomRow(rect.bottom());
    return QRect(left, top, right - left + 1, bottom - top + 1);
}

QRect CanvasBase::visibleCells() const
{
    return viewToCellCoordinates(rect());
}

//---------------------------------------------
//
// Drawing Engine
//
//---------------------------------------------

QRectF CanvasBase::cellCoordinatesToView(const QRect &cellRange) const
{
    Sheet *const sheet = activeSheet();
    if (!sheet)
        return QRectF();

    QRectF rect = sheet->cellCoordinatesToDocument(cellRange);
    // apply scrolling offset
    rect.translate(-xOffset(), -yOffset());
    // convert it to view coordinates
    rect = viewConverter()->documentToView(rect);
    // apply layout direction
    if (sheet->layoutDirection() == Qt::RightToLeft) {
        const double left = rect.left();
        const double right = rect.right();
        rect.setLeft(width() - right);
        rect.setRight(width() - left);
    }
    return rect;
}

void CanvasBase::showToolTip(const QPoint &p)
{
    Sheet *const sheet = activeSheet();
    if (!sheet)
        return;
    SheetView *const sheetView = this->sheetView(sheet);

    // Over which cell is the mouse ?
    qreal ypos, xpos;
    qreal dwidth = viewConverter()->viewToDocumentX(width());
    int col;
    if (sheet->layoutDirection() == Qt::RightToLeft)
        col = sheet->leftColumn((dwidth - viewConverter()->viewToDocumentX(p.x()) + xOffset()), xpos);
    else
        col = sheet->leftColumn((viewConverter()->viewToDocumentX(p.x()) + xOffset()), xpos);

    int row = sheet->topRow((viewConverter()->viewToDocumentY(p.y()) + yOffset()), ypos);

    Cell cell = Cell(sheet, col, row).masterCell();
    const CellView &baseCellView = sheetView->cellView(cell.column(), cell.row());
    const bool baseIsObscured = sheetView->isObscured(cell.cellPosition());
    const QPoint cellPos = baseIsObscured ? sheetView->obscuringCell(cell.cellPosition()) : cell.cellPosition();
    const CellView &cellView = baseIsObscured ? sheetView->cellView(cellPos) : baseCellView;
    if (sheetView->isObscured(cellPos)) {
        cell = Cell(sheet, sheetView->obscuringCell(cellPos));
    }

    // displayed tool tip, which has the following priorities:
    //  - cell content if the cell dimension is too small
    //  - cell comment
    //  - hyperlink
    // Ensure that it is plain text.
    // Not funny if (intentional or not) <a> appears as hyperlink.
    QString tipText;
    // If cell is too small, show the content
    if (!cellView.dimensionFits())
        tipText = cell.displayText().replace('<', "&lt;");

    // Show hyperlink, if any
    if (tipText.isEmpty())
        tipText = cell.link().replace('<', "&lt;");

    // Nothing to display, bail out
    if (tipText.isEmpty() && cell.comment().isEmpty())
        return;

    // Cut if the tip is ridiculously long
    const int maxLen = 256;
    if (tipText.length() > maxLen)
        tipText = tipText.left(maxLen).append("...");

    // Determine position and width of the current cell.
    const double cellWidth = cellView.cellWidth();
    const double cellHeight = cellView.cellHeight();

    // Get the cell dimensions
    QRect cellRect;
    bool insideCellRect = false;
    if (sheet->layoutDirection() == Qt::RightToLeft) {
        const QRectF rect(dwidth - cellWidth - xpos + xOffset(), ypos - yOffset(), cellWidth, cellHeight);
        cellRect = viewConverter()->documentToView(rect).toRect();
        insideCellRect = cellRect.contains(p);
    } else {
        QRectF rect(xpos - xOffset(), ypos - yOffset(), cellWidth, cellHeight);
        cellRect = viewConverter()->documentToView(rect).toRect();
        insideCellRect = cellRect.contains(p);
    }

    // No use if mouse is somewhere else
    if (!insideCellRect)
        return;

    // Show comment, if any.
    if (tipText.isEmpty())
        tipText = cell.comment().replace('<', "&lt;");
    else if (!cell.comment().isEmpty())
        tipText += "</p><h4>" + i18n("Comment:") + "</h4><p>" + cell.comment().replace('<', "&lt;");

    // Now we show the tip
    QToolTip::showText(mapToGlobal(cellRect.bottomRight()), "<p>" + tipText.replace('\n', "<br>") + "</p>");
    // TODO XXX this, cellRect.translated(-mapToGlobal(cellRect.topLeft())));
}

void CanvasBase::updateInputMethodInfo()
{
    updateMicroFocus();
}

KoViewConverter *CanvasBase::viewConverter() const
{
    return zoomHandler();
}
