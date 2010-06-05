/* This file is part of the KDE project
   Copyright 2009 Thomas Zander <zander@kde.org>
   Copyright 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2006 Robert Knight <robertknight@gmail.com>
   Copyright 2006 Inge Wallin <inge@lysator.liu.se>
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
#include "Canvas.h"
#include "Canvas_p.h"

// std
#include <assert.h>
#include <float.h>
#include <stdlib.h>

// Qt
#include <QApplication>
#include <QBuffer>
#include <QByteArray>
#include <QClipboard>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QList>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QPoint>
#include <QScrollBar>
#include <QTextStream>
#include <QToolTip>
#include <QWidget>

// KDE
#include <kcursor.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <krun.h>
#include <kmimetype.h>
#include <ksharedptr.h>
#include <kwordwrap.h>
#include <kxmlguifactory.h>

// KOffice
#include <KoCanvasController.h>
#include <KoShapeManager.h>
#include <KoStore.h>
#include <KoToolManager.h>
#include <KoToolProxy.h>
#include <KoXmlWriter.h>
#include <KoZoomHandler.h>

// KSpread
#include "CellStorage.h"
#include "CellView.h"
#include "Doc.h"
#include "Editors.h"
#include "Global.h"
#include "Headers.h"
#include "Localization.h"
#include "Map.h"
#include "RowColumnFormat.h"
#include "Selection.h"
#include "Sheet.h"
#include "SheetView.h"
#include "Util.h"
#include "Validity.h"
#include "View.h"

// commands
#include "commands/DeleteCommand.h"
#include "commands/StyleCommand.h"
#include "commands/Undo.h"

#define MIN_SIZE 10

using namespace KSpread;

/****************************************************************
 *
 * Canvas
 *
 ****************************************************************/

Canvas::Canvas(View *view)
        : QWidget(view)
        , KoCanvasBase(0)
        , d(new Private)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_StaticContents);
    setBackgroundRole(QPalette::Base);

    d->validationInfo = 0;

    QWidget::setFocusPolicy(Qt::StrongFocus);

    d->offset = QPointF(0.0, 0.0);

    d->view = view;

    setMouseTracking(true);
    d->mousePressed = false;
    d->dragging = false;

    connect(d->view, SIGNAL(autoScroll(const QPoint &)),
            this, SLOT(slotAutoScroll(const QPoint &)));

    installEventFilter(this);   // for TAB key processing, otherwise focus change
    setAcceptDrops(true);
    setAttribute(Qt::WA_InputMethodEnabled, true); // ensure using the InputMethod

    // flake
    d->shapeManager = new KoShapeManager(this);
    d->toolProxy = new KoToolProxy(this);
}

Canvas::~Canvas()
{
    delete d->shapeManager;
    delete d->toolProxy;
    delete d->validationInfo;
    delete d;
}

View* Canvas::view() const
{
    return d->view;
}

Doc* Canvas::doc() const
{
    return d->view->doc();
}

void Canvas::gridSize(qreal* horizontal, qreal* vertical) const
{
    *horizontal = doc()->map()->defaultColumnFormat()->width();
    *vertical = doc()->map()->defaultRowFormat()->height();
}

bool Canvas::snapToGrid() const
{
    return false; // FIXME
}

void Canvas::addCommand(QUndoCommand* command)
{
    doc()->addCommand(command);
}

KoShapeManager* Canvas::shapeManager() const
{
    return d->shapeManager;
}

void Canvas::updateCanvas(const QRectF& rc)
{
    QRect clipRect(viewConverter()->documentToView(rc.translated(-offset())).toRect());
    clipRect.adjust(-2, -2, 2, 2);   // Resize to fit anti-aliasing
    update(clipRect);
}

const KoViewConverter* Canvas::viewConverter() const
{
    return view()->zoomHandler();
}

KoUnit Canvas::unit() const
{
    return doc()->unit();
}

KoToolProxy* Canvas::toolProxy() const
{
    return d->toolProxy;
}

QPointF Canvas::offset() const
{
    return d->offset;
}

double Canvas::xOffset() const
{
    return d->offset.x();
}

double Canvas::yOffset() const
{
    return d->offset.y();
}

bool Canvas::eventFilter(QObject *o, QEvent *e)
{
    /* this canvas event filter acts on events sent to the line edit as well
       as events to this filter itself.
    */
    if (!o || !e)
        return true;
    switch (e->type()) {
    case QEvent::KeyPress: {
        QKeyEvent * keyev = static_cast<QKeyEvent *>(e);
        if ((keyev->key() == Qt::Key_Tab) || (keyev->key() == Qt::Key_Backtab)) {
            keyPressEvent(keyev);
            return true;
        }
        break;
    }
    case QEvent::InputMethod: {
        //QIMEvent * imev = static_cast<QIMEvent *>(e);
        //processIMEvent( imev );
        //break;
    }
    case QEvent::ToolTip: {
        QHelpEvent* helpEvent = static_cast<QHelpEvent*>(e);
        showToolTip(helpEvent->pos());
    }
    default:
        break;
    }
    return false;
}

Selection* Canvas::selection() const
{
    return d->view->selection();
}

ColumnHeader* Canvas::columnHeader() const
{
    return d->view->columnHeader();
}

RowHeader* Canvas::rowHeader() const
{
    return d->view->rowHeader();
}

QScrollBar* Canvas::horzScrollBar() const
{
    return d->view->horzScrollBar();
}

QScrollBar* Canvas::vertScrollBar() const
{
    return d->view->vertScrollBar();
}

Sheet* Canvas::activeSheet() const
{
    return d->view->activeSheet();
}

void Canvas::validateSelection()
{
    register Sheet * const sheet = activeSheet();
    if (!sheet)
        return;

    if (selection()->isSingular()) {
        int col = selection()->marker().x();
        int row = selection()->marker().y();
        Cell cell(sheet, col, row);
        Validity validity = cell.validity();
        if (validity.displayValidationInformation()) {
            QString title = validity.titleInfo();
            QString message = validity.messageInfo();
            if (title.isEmpty() && message.isEmpty())
                return;

            if (!d->validationInfo)
                d->validationInfo = new QLabel(this);
            kDebug(36001) << " display info validation";
            double u = cell.width();
            double v = cell.height();
            double xpos = sheet->columnPosition(selection()->marker().x()) - xOffset();
            double ypos = sheet->rowPosition(selection()->marker().y()) - yOffset();
            // Special treatment for obscured cells.
            if (cell.isPartOfMerged()) {
                cell = cell.masterCell();
                int moveX = cell.column();
                int moveY = cell.row();

                // Use the obscuring cells dimensions
                u = cell.width();
                v = cell.height();
                xpos = sheet->columnPosition(moveX);
                ypos = sheet->rowPosition(moveY);
            }
            //d->validationInfo->setGeometry( 3, y + 3, len + 2, hei + 2 );
            d->validationInfo->setAlignment(Qt::AlignVCenter);
            QPainter painter;
            painter.begin(this);
            int len = 0;
            int hei = 0;
            QString resultText;
            if (!title.isEmpty()) {
                len = painter.fontMetrics().width(title);
                hei = painter.fontMetrics().height();
                resultText = title + '\n';
            }
            if (!message.isEmpty()) {
                int i = 0;
                int pos = 0;
                QString t;
                do {
                    i = message.indexOf("\n", pos);
                    if (i == -1)
                        t = message.mid(pos, message.length() - pos);
                    else {
                        t = message.mid(pos, i - pos);
                        pos = i + 1;
                    }
                    hei += painter.fontMetrics().height();
                    len = qMax(len, painter.fontMetrics().width(t));
                } while (i != -1);
                resultText += message;
            }
            painter.end();
            d->validationInfo->setText(resultText);

            QRectF unzoomedMarker(xpos - xOffset() + u,
                                  ypos - yOffset() + v,
                                  len,
                                  hei);
            QRectF marker(viewConverter()->documentToView(unzoomedMarker));

            d->validationInfo->setGeometry(marker.toRect());
            d->validationInfo->show();
        } else {
            delete d->validationInfo;
            d->validationInfo = 0;
        }
    } else {
        delete d->validationInfo;
        d->validationInfo = 0;
    }
}


void Canvas::scrollToCell(const QPoint& location) const
{
    register Sheet * const sheet = activeSheet();
    if (!sheet)
        return;
    if (d->view->isLoading())
        return;

    // Adjust the maximum accessed column and row for the scrollbars.
    view()->sheetView(sheet)->updateAccessedCellRange(location);

    // The cell geometry expanded by the size of one column or one row, resp., in each direction.
    const Cell cell = Cell(sheet, location).masterCell();
    const double xpos = sheet->columnPosition(cell.cellPosition().x());
    const double ypos = sheet->rowPosition(cell.cellPosition().y());
    const double width = sheet->map()->defaultColumnFormat()->width();
    const double height = sheet->map()->defaultRowFormat()->height();
    QRectF rect(xpos, ypos, cell.width(), cell.height());
    rect.adjust(-width - 2, -height - 2, width + 2, height + 2);
    rect = rect & QRectF(QPointF(0.0, 0.0), sheet->documentSize());

    d->view->canvasController()->ensureVisible(rect, true);
}

void Canvas::setDocumentOffset(const QPoint& offset)
{
    const QPoint delta = offset - viewConverter()->documentToView(d->offset).toPoint();
    d->offset = viewConverter()->viewToDocument(offset);

    columnHeader()->scroll(delta.x(), 0);
    rowHeader()->scroll(0, delta.y());
}

void Canvas::setDocumentSize(const QSizeF& size)
{
    const QSize s = viewConverter()->documentToView(size).toSize();
    emit documentSizeChanged(s);
}

#if 0
void Canvas::slotScrollHorz(int _value)
{
    register Sheet * const sheet = activeSheet();
    if (!sheet)
        return;

    kDebug(36005) << "slotScrollHorz: value =" << _value;
    //kDebug(36005) << kBacktrace();

    if (sheet->layoutDirection() == Qt::RightToLeft)
        _value = horzScrollBar()->maximum() - _value;

    if (_value < 0) {
        kDebug(36001)
        << "Canvas::slotScrollHorz: value out of range (_value: "
        << _value << ')' << endl;
        _value = 0;
    }

    double xpos = sheet->columnPosition(qMin(KS_colMax, sheet->maxColumn() + 10)) - d->xOffset;
    if (_value > (xpos + d->xOffset))
        _value = (int)(xpos + d->xOffset);

    // Relative movement
    // NOTE Stefan: Always scroll by whole pixels, otherwise we'll get offsets.
    int dx = qRound(viewConverter()->documentToViewX(d->xOffset - _value));

    // New absolute position
    // NOTE Stefan: Always store whole pixels, otherwise we'll get offsets.
    d->xOffset -=  viewConverter()->viewToDocumentX(dx);
    if (d->xOffset < 0.05)
        d->xOffset = 0.0;

    // scrolling the widgets in the right direction
    if (sheet->layoutDirection() == Qt::RightToLeft)
        dx = -dx;
    scroll(dx, 0);
    columnHeader()->scroll(dx, 0);
}

void Canvas::slotScrollVert(int _value)
{
    register Sheet * const sheet = activeSheet();
    if (!sheet)
        return;

    if (_value < 0) {
        _value = 0;
        kDebug(36001) << "Canvas::slotScrollVert: value out of range (_value:" <<
        _value << ')' << endl;
    }

    double ypos = sheet->rowPosition(qMin(KS_rowMax, sheet->maxRow() + 10));
    if (_value > ypos)
        _value = (int) ypos;

    // Relative movement
    // NOTE Stefan: Always scroll by whole pixels, otherwise we'll get offsets.
    int dy = qRound(viewConverter()->documentToViewY(d->yOffset - _value));
    scroll(0, dy);
    rowHeader()->scroll(0, dy);

    // New absolute position
    // NOTE Stefan: Always store whole pixels, otherwise we'll get offsets.
    d->yOffset -= viewConverter()->viewToDocumentY(dy);
    if (d->yOffset < 0.05)
        d->yOffset = 0.0;
}

void Canvas::slotMaxColumn(int _max_column)
{
    register Sheet * const sheet = activeSheet();
    if (!sheet)
        return;

    int oldValue = horzScrollBar()->maximum() - horzScrollBar()->value();
    double xpos = sheet->columnPosition(qMin(KS_colMax, _max_column + 10)) - xOffset();
    double unzoomWidth = viewConverter()->viewToDocumentX(width());

    //Don't go beyond the maximum column range (KS_colMax)
    double sizeMaxX = sheet->documentSize().width();
    if (xpos > sizeMaxX - xOffset() - unzoomWidth)
        xpos = sizeMaxX - xOffset() - unzoomWidth;

    horzScrollBar()->setRange(0, (int)(xpos + xOffset()));

    if (sheet->layoutDirection() == Qt::RightToLeft)
        horzScrollBar()->setValue(horzScrollBar()->maximum() - oldValue);
}

void Canvas::slotMaxRow(int _max_row)
{
    register Sheet * const sheet = activeSheet();
    if (!sheet)
        return;

    double ypos = sheet->rowPosition(qMin(KS_rowMax, _max_row + 10)) - yOffset();
    double unzoomHeight = viewConverter()->viewToDocumentY(height());

    //Don't go beyond the maximum row range (KS_rowMax)
    double sizeMaxY = sheet->documentSize().height();
    if (ypos > sizeMaxY - yOffset() - unzoomHeight)
        ypos = sizeMaxY - yOffset() - unzoomHeight;

    vertScrollBar()->setRange(0, (int)(ypos + yOffset()));
}
#endif

void Canvas::mousePressEvent(QMouseEvent* event)
{
    // flake
    if(d->toolProxy) {
        d->toolProxy->mousePressEvent(event, viewConverter()->viewToDocument(event->pos()) + offset());

        if (!event->isAccepted() && event->button() == Qt::RightButton) {
            d->view->unplugActionList("toolproxy_action_list");
            d->view->plugActionList("toolproxy_action_list", toolProxy()->popupActionList());
            QMenu* menu = dynamic_cast<QMenu*>(d->view->factory()->container("default_canvas_popup", d->view));
            // Only show the menu, if there are items. The plugged action list counts as one action.
            if (menu && menu->actions().count() > 1) {
                menu->exec(event->globalPos());
            }
            event->setAccepted(true);
        }
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
    // flake
    if(d->toolProxy)
        d->toolProxy->mouseReleaseEvent(event, viewConverter()->viewToDocument(event->pos()) + offset());
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    // flake
    if(d->toolProxy)
        d->toolProxy->mouseMoveEvent(event, viewConverter()->viewToDocument(event->pos()) + offset());
}

void Canvas::mouseDoubleClickEvent(QMouseEvent* event)
{
    // flake
    if(d->toolProxy)
        d->toolProxy->mouseDoubleClickEvent(event, viewConverter()->viewToDocument(event->pos()) + offset());
}

void Canvas::keyPressEvent(QKeyEvent* event)
{
    // flake
    if(d->toolProxy)
        d->toolProxy->keyPressEvent(event);
}

void Canvas::tabletEvent(QTabletEvent *e)
{
    // flake
    if(d->toolProxy)
        d->toolProxy->tabletEvent(e, viewConverter()->viewToDocument(e->pos() + offset()));
}

QVariant Canvas::inputMethodQuery(Qt::InputMethodQuery query) const
{
    // flake
    return d->toolProxy ? d->toolProxy->inputMethodQuery(query, *(viewConverter())) : 0;
}

void Canvas::inputMethodEvent(QInputMethodEvent *event)
{
    // flake
    if(d->toolProxy)
        d->toolProxy->inputMethodEvent(event);
}

bool Canvas::highlightRangeSizeGripAt(double x, double y)
{
    if (!selection()->referenceSelectionMode())
        return false;

    Region::ConstIterator end = selection()->constEnd();
    for (Region::ConstIterator it = selection()->constBegin(); it != end; ++it) {
        // TODO Stefan: adapt to Selection::selectionHandleArea
        QRectF visibleRect = activeSheet()->cellCoordinatesToDocument((*it)->rect());

        QPoint bottomRight((int) visibleRect.right(), (int) visibleRect.bottom());
        QRect handle(((int) bottomRight.x() - 6),
                     ((int) bottomRight.y() - 6),
                     (6),
                     (6));

        if (handle.contains(QPoint((int) x, (int) y))) {
            return true;
        }
    }

    return false;
}

void Canvas::startTheDrag()
{
    register Sheet * const sheet = activeSheet();
    if (!sheet)
        return;

    // right area for start dragging
    setCursor(Qt::PointingHandCursor);

    QDomDocument doc = sheet->saveCellRegion(*selection(), true);

    // Save to buffer
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    QTextStream str(&buffer);
    str.setCodec("UTF-8");
    str << doc;
    buffer.close();

    QMimeData* mimeData = new QMimeData();
    mimeData->setText(sheet->copyAsText(selection()));
    mimeData->setData("application/x-kspread-snippet", buffer.buffer());

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->start();

    setCursor(Qt::ArrowCursor);
}

void Canvas::paintEvent(QPaintEvent* event)
{
    if (d->view->doc()->isLoading() || d->view->isLoading())
        return;

    register Sheet * const sheet = activeSheet();
    if (!sheet)
        return;

//     ElapsedTime et("Painting cells", ElapsedTime::PrintOnlyTime);

    QPainter painter(this);
    const QPointF offset = viewConverter()->documentToView(this->offset());
    painter.translate(-offset);
    painter.setClipRegion(event->region().translated(offset.x(), offset.y()));
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.save();

    qreal zoomX, zoomY;
    viewConverter()->zoom(&zoomX, &zoomY);
    painter.scale(zoomX, zoomY);

    // erase background
    const QRectF paintRect = viewConverter()->viewToDocument(rect()).translated(this->offset());
    painter.fillRect(paintRect, painter.background());

    // paint visible cells
    const QRect visibleRect = visibleCells();
    const QPointF topLeft(sheet->columnPosition(visibleRect.left()), sheet->rowPosition(visibleRect.top()));
    view()->sheetView(sheet)->setPaintCellRange(visibleRect);
    view()->sheetView(sheet)->paintCells(this, painter, paintRect, topLeft);

    // flake
    painter.restore();
    d->shapeManager->paint(painter, *viewConverter(), false);
    painter.setRenderHint(QPainter::Antialiasing, false);
    if(d->toolProxy)
        d->toolProxy->paint(painter, *viewConverter());

    event->accept();
}

void Canvas::focusInEvent(QFocusEvent*)
{
    // If we are in editing mode, we redirect the
    // focus to the CellEditor or ExternalEditor.
    // This screws up <Tab> though (David)
    selection()->emitRequestFocusEditor();
}

void Canvas::focusOutEvent(QFocusEvent*)
{
    d->mousePressed = false;
    d->view->disableAutoScroll();
}

void Canvas::dragEnterEvent(QDragEnterEvent* event)
{
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasText() ||
            mimeData->hasFormat("application/x-kspread-snippet")) {
        event->acceptProposedAction();
    }
}

void Canvas::dragMoveEvent(QDragMoveEvent* event)
{
    register Sheet * const sheet = activeSheet();
    if (!sheet) {
        event->ignore();
        return;
    }

    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasText() || mimeData->hasFormat("application/x-kspread-snippet")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
        return;
    }
#if 0 // TODO Stefan: implement drag marking rectangle
    QRect dragMarkingRect;
    if (mimeData->hasFormat("application/x-kspread-snippet")) {
        if (event->source() == this) {
            kDebug(36005) << "source == this";
            dragMarkingRect = selection()->boundingRect();
        } else {
            kDebug(36005) << "source != this";
            QByteArray data = mimeData->data("application/x-kspread-snippet");
            QString errorMsg;
            int errorLine;
            int errorColumn;
            QDomDocument doc;
            if (!doc.setContent(data, false, &errorMsg, &errorLine, &errorColumn)) {
                // an error occurred
                kDebug(36005) << "Canvas::daragMoveEvent: an error occurred" << endl
                << "line: " << errorLine << " col: " << errorColumn
                << ' ' << errorMsg << endl;
                dragMarkingRect = QRect(1, 1, 1, 1);
            } else {
                QDomElement root = doc.documentElement(); // "spreadsheet-snippet"
                dragMarkingRect = QRect(1, 1,
                                        root.attribute("columns").toInt(),
                                        root.attribute("rows").toInt());
            }
        }
    } else { // if ( mimeData->hasText() )
        kDebug(36005) << "has text";
        dragMarkingRect = QRect(1, 1, 1, 1);
    }
#endif
    const QPoint dragAnchor = selection()->boundingRect().topLeft();
    double xpos = sheet->columnPosition(dragAnchor.x());
    double ypos = sheet->rowPosition(dragAnchor.y());
    double width  = sheet->columnFormat(dragAnchor.x())->width();
    double height = sheet->rowFormat(dragAnchor.y())->height();

    // consider also the selection rectangle
    const QRectF noGoArea(xpos - 1, ypos - 1, width + 3, height + 3);

    // determine the current position
    double eventPosX;
    if (sheet->layoutDirection() == Qt::RightToLeft) {
        eventPosX = this->width() - viewConverter()->viewToDocumentX(event->pos().x()) + xOffset();
    } else {
        eventPosX = viewConverter()->viewToDocumentX(event->pos().x()) + xOffset();
    }
    double eventPosY = viewConverter()->viewToDocumentY(event->pos().y()) + yOffset();

    if (noGoArea.contains(QPointF(eventPosX, eventPosY))) {
        event->ignore(noGoArea.toRect());
        return;
    }

#if 0 // TODO Stefan: implement drag marking rectangle
    // determine the cell position under the mouse
    double tmp;
    const int col = sheet->leftColumn(eventPosX, tmp);
    const int row = sheet->topRow(eventPosY, tmp);
    dragMarkingRect.moveTo(QPoint(col, row));
    kDebug(36005) << "MARKING RECT =" << dragMarkingRect;
#endif
}

void Canvas::dragLeaveEvent(QDragLeaveEvent *)
{
}

void Canvas::dropEvent(QDropEvent * _ev)
{
    d->dragging = false;
    d->view->disableAutoScroll();
    register Sheet * const sheet = activeSheet();
    if (!sheet || sheet->isProtected()) {
        _ev->ignore();
        return;
    }

    double xpos = sheet->columnPosition(selection()->lastRange().left());
    double ypos = sheet->rowPosition(selection()->lastRange().top());
    double width  = sheet->columnFormat(selection()->lastRange().left())->width();
    double height = sheet->rowFormat(selection()->lastRange().top())->height();

    const QRectF noGoArea(xpos - 1, ypos - 1, width + 3, height + 3);

    double ev_PosX;
    if (sheet->layoutDirection() == Qt::RightToLeft)
        ev_PosX = this->width() - viewConverter()->viewToDocumentX(_ev->pos().x()) + xOffset();
    else
        ev_PosX = viewConverter()->viewToDocumentX(_ev->pos().x()) + xOffset();

    double ev_PosY = viewConverter()->viewToDocumentY(_ev->pos().y()) + yOffset();

    if (noGoArea.contains(QPointF(ev_PosX, ev_PosY))) {
        _ev->ignore();
        return;
    } else
        _ev->setAccepted(true);

    double tmp;
    int col = sheet->leftColumn(ev_PosX, tmp);
    int row = sheet->topRow(ev_PosY, tmp);

    const QMimeData* mimeData = _ev->mimeData();
    if (!mimeData->hasText() && !mimeData->hasFormat("application/x-kspread-snippet")) {
        _ev->ignore();
        return;
    }

    QByteArray b;

    bool makeUndo = true;

    if (mimeData->hasFormat("application/x-kspread-snippet")) {
        if (_ev->source() == this) {
            UndoDragDrop * undo
            = new UndoDragDrop(sheet, *selection(),
                               Region(QRect(col, row,
                                            selection()->boundingRect().width(),
                                            selection()->boundingRect().height())),
                               selection());
            d->view->doc()->addCommand(undo);
            makeUndo = false;

            DeleteCommand* command = new DeleteCommand();
            command->setSheet(activeSheet());
            command->add(*selection());
            command->setRegisterUndo(false);
            command->execute();
        }


        b = mimeData->data("application/x-kspread-snippet");
        sheet->paste(b, QRect(col, row, 1, 1), makeUndo);

        //Select the pasted cells
        selection()->initialize(QRect(col, row,
                                      selection()->boundingRect().width(),
                                      selection()->boundingRect().height()), sheet);

        _ev->setAccepted(true);
    } else {
        QString text = mimeData->text();
        sheet->pasteTextPlain(text, QRect(col, row, 1, 1));
        _ev->setAccepted(true);
        return;
    }
}

void Canvas::slotAutoScroll(const QPoint &scrollDistance)
{
    // NOTE Stefan: This slot is triggered by the same signal as
    //              ColumnHeader::slotAutoScroll and RowHeader::slotAutoScroll.
    //              Therefore, nothing has to be done except the scrolling was
    //              initiated in the canvas.
    if (!d->mousePressed)
        return;
    d->view->canvasController()->scrollContentsBy(scrollDistance.x(), scrollDistance.y());
}

QRect Canvas::viewToCellCoordinates(const QRectF& viewRect) const
{
    register Sheet * const sheet = activeSheet();
    if (!sheet)
        return QRect();

    const QRectF rect = d->view->zoomHandler()->viewToDocument(viewRect).translated(offset());

    double tmp;
    const int left = sheet->leftColumn(rect.left(), tmp);
    const int right = sheet->rightColumn(rect.right());
    const int top = sheet->topRow(rect.top(), tmp);
    const int bottom = sheet->bottomRow(rect.bottom());

    return QRect(left, top, right - left + 1, bottom - top + 1);
}

QRect Canvas::visibleCells() const
{
    return viewToCellCoordinates(rect());
}

//---------------------------------------------
//
// Drawing Engine
//
//---------------------------------------------

QRectF Canvas::cellCoordinatesToView(const QRect& cellRange) const
{
    register Sheet * const sheet = activeSheet();
    if (!sheet)
        return QRectF();

    QRectF rect = sheet->cellCoordinatesToDocument(cellRange);
    // apply scrolling offset
    rect.translate(-xOffset(), -yOffset());
    // convert it to view coordinates
    rect = d->view->zoomHandler()->documentToView(rect);
    // apply layout direction
    if (sheet->layoutDirection() == Qt::RightToLeft) {
        const double left = rect.left();
        const double right = rect.right();
        rect.setLeft(width() - right);
        rect.setRight(width() - left);
    }
    return rect;
}

void Canvas::showToolTip(const QPoint& p)
{
    register Sheet * const sheet = activeSheet();
    if (!sheet)
        return;

    // Over which cell is the mouse ?
    double ypos, xpos;
    double dwidth = d->view->zoomHandler()->viewToDocumentX(width());
    int col;
    if (sheet->layoutDirection() == Qt::RightToLeft)
        col = sheet->leftColumn((dwidth - d->view->zoomHandler()->viewToDocumentX(p.x()) +
                                 xOffset()), xpos);
    else
        col = sheet->leftColumn((d->view->zoomHandler()->viewToDocumentX(p.x()) +
                                 xOffset()), xpos);


    int row = sheet->topRow((d->view->zoomHandler()->viewToDocumentY(p.y()) +
                             yOffset()), ypos);

    Cell cell = Cell(sheet, col, row).masterCell();
    CellView cellView = view()->sheetView(sheet)->cellView(cell.column(), cell.row());
    if (cellView.isObscured()) {
        cell = Cell(sheet, cellView.obscuringCell());
        cellView = view()->sheetView(sheet)->cellView(cellView.obscuringCell().x(), cellView.obscuringCell().y());
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
    QToolTip::showText(mapToGlobal(cellRect.bottomRight()),
                       "<p>" + tipText.replace('\n', "<br>") + "</p>",
                       this, cellRect.translated(-mapToGlobal(cellRect.topLeft())));
}

void Canvas::updateInputMethodInfo()
{
    updateMicroFocus();
}

#include "Canvas.moc"
