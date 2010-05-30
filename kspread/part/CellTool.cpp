/* This file is part of the KDE project
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
#include "CellTool.h"

#include <QPainter>

#include <KAction>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kmimetype.h>
#include <krun.h>
#include <knotification.h>
#include <KSelectAction>

#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoPointerEvent.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoViewConverter.h>

#include "Cell.h"
#include "CellView.h"
#include "Canvas.h"
#include "Canvas_p.h"
#include "Doc.h"
#include "Editors.h"
#include "Limits.h"
#include "PrintSettings.h"
#include "Selection.h"
#include "Sheet.h"
#include "SheetView.h"
#include "StyleManager.h"
#include "Util.h"
#include "View.h"

#include "commands/AutoFillCommand.h"
#include "commands/DefinePrintRangeCommand.h"

using namespace KSpread;

/**
 * \li 'normal' selection:
 *   \li marking, resizing (handle or Shift), extending (Ctrl)
 *   \li merging cells
 *   \li autofill
 *   \li pasting (middle)
 *   \li drag & drop
 * \li 'reference' selection:
 *   \li marking, resizing (handle or Shift), extending (Ctrl)
 */

class CellTool::Private
{
public:
    void processClickSelectionHandle(KoPointerEvent* event);
#if 0 // KSPREAD_MOUSE_STRATEGIES
    void processLeftClickAnchor();
#endif

public:
    Canvas* canvas;

    // If the user is dragging around with the mouse then this tells us what he is doing.
    // The user may want to mark cells or he started in the lower right corner
    // of the marker which is something special. The values for the 2 above
    // methods are called 'Mark' and 'Merge' or 'AutoFill' depending
    // on the mouse button used. By default this variable holds
    // the value 'None'.
enum { None, Mark, Merge, AutoFill, Resize } mouseAction : 3;

    // If we use the lower right corner of the marker to start autofilling, then this
    // rectangle conatins all cells that were already marker when the user started
    // to mark the rectangle which he wants to become autofilled.
    QRect autoFillSource;

    // Start coordinates for drag and drop
    QPoint dragStart;

    // If the mouse is over some anchor ( in the sense of HTML anchors )
    QString anchor;
};

CellTool::CellTool(KoCanvasBase* canvas)
        : CellToolBase(canvas)
        , d(new Private)
{
    d->canvas = static_cast<Canvas*>(canvas);
    d->dragStart = QPoint(-1, -1);
    d->mouseAction = Private::None;

    KAction* action = 0;

    // -- misc actions --

    action = new KAction(i18n("Define Print Range"), this);
    addAction("definePrintRange", action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(definePrintRange()));
    action->setToolTip(i18n("Define the print range in the current sheet"));
}

CellTool::~CellTool()
{
    delete d;
}

void CellTool::paint(QPainter &painter, const KoViewConverter &viewConverter)
{
    KoShape::applyConversion(painter, viewConverter);
    const double xOffset = viewConverter.viewToDocumentX(canvas()->canvasController()->canvasOffsetX());
    const double yOffset = viewConverter.viewToDocumentY(canvas()->canvasController()->canvasOffsetY());
    const QRectF paintRect = QRectF(QPointF(-xOffset, -yOffset), size());

    /* paint the selection */
    paintReferenceSelection(painter, paintRect);
    paintSelection(painter, paintRect);
}

#if 0 // KSPREAD_MOUSE_STRATEGIES
void CellTool::mousePressEvent(KoPointerEvent* event)
{
    register Sheet * const sheet = d->canvas->activeSheet();
    if (!sheet)
        return;

    if (event->button() == Qt::LeftButton) {
        d->canvas->d->mousePressed = true;
        d->canvas->view()->enableAutoScroll();
    }

    // Get info about where the event occurred - this is duplicated
    // in ::mouseMoveEvent, needs to be separated into one function
    QPointF position = d->canvas->viewConverter()->viewToDocument(event->pos());
    if (sheet->layoutDirection() == Qt::RightToLeft)
        position.setX(d->canvas->viewConverter()->viewToDocumentX(d->canvas->width()) - position.x());
    position += QPointF(d->canvas->xOffset(), d->canvas->yOffset());

    // In which cell did the user click ?
    double xpos;
    double ypos;
    int col  = sheet->leftColumn(position.x(), xpos);
    int row  = sheet->topRow(position.y(), ypos);
    // you cannot move marker when col > KS_colMax or row > KS_rowMax
    if (col > KS_colMax || row > KS_rowMax) {
        kDebug(36005) << "Canvas::mousePressEvent: col or row is out of range:"
        << "col: " << col << " row: " << row << endl;
        return;
    }

    if (selection()->referenceSelectionMode() && d->canvas->highlightRangeSizeGripAt(position.x(), position.y())) {
        d->canvas->selection()->setActiveElement(QPoint(col, row), editor());
        d->mouseAction = Private::Resize;
        event->accept(); // MouseButtonPress
        return;
    }

    // We were editing a cell -> save value and get out of editing mode
    d->canvas->selection()->emitCloseEditor(true); // save changes

    // Did we click in the lower right corner of the marker/marked-area ?
    if (d->canvas->selection()->selectionHandleArea(d->canvas->viewConverter()).contains(QPointF(position.x(), position.y()))) {
        d->processClickSelectionHandle(event);
        event->accept(); // MouseButtonPress
        return;
    }

    // TODO Stefan: adapt to non-cont. selection
    {
        // start drag ?
        QRect rct(d->canvas->selection()->lastRange());

        QRect r1;
        QRect r2;
        {
            double lx = sheet->columnPosition(rct.left());
            double rx = sheet->columnPosition(rct.right() + 1);
            double ty = sheet->rowPosition(rct.top());
            double by = sheet->rowPosition(rct.bottom() + 1);

            r1.setLeft((int)(lx - 1));
            r1.setTop((int)(ty - 1));
            r1.setRight((int)(rx + 1));
            r1.setBottom((int)(by + 1));

            r2.setLeft((int)(lx + 1));
            r2.setTop((int)(ty + 1));
            r2.setRight((int)(rx - 1));
            r2.setBottom((int)(by - 1));
        }

        d->dragStart.setX(-1);

        if (r1.contains(QPoint((int) position.x(), (int) position.y()))
                && !r2.contains(QPoint((int) position.x(), (int) position.y()))) {
            d->dragStart.setX((int) position.x());
            d->dragStart.setY((int) position.y());
            event->accept(); // MouseButtonPress
            return;
        }
    }

//    kDebug(36005) <<"Clicked in cell" << col <<"," << row;

    // Extending an existing selection with the shift button ?
    if ((event->modifiers() & Qt::ShiftModifier) &&
            d->canvas->view()->koDocument()->isReadWrite() &&
            !d->canvas->selection()->isColumnOrRowSelected()) {
        d->canvas->selection()->update(QPoint(col, row));
        event->accept(); // MouseButtonPress
        return;
    }


    // Go to the upper left corner of the obscuring object if cells are merged
    Cell cell(sheet, col, row);
    if (cell.isPartOfMerged()) {
        cell = cell.masterCell();
        col = cell.column();
        row = cell.row();
    }

    switch (event->button()) {
    case Qt::LeftButton:
        event->accept();
        // Check, whether a filter button was hit.
        {
            QPointF p1(xpos, ypos);
            p1 -= QPointF(d->canvas->xOffset(), d->canvas->yOffset());
            QSizeF s1(cell.width(), cell.height());
            QRect cellRect = d->canvas->viewConverter()->documentToView(QRectF(p1, s1)).toRect();
            if (d->canvas->view()->sheetView(sheet)->cellView(col, row).hitTestFilterButton(cell, cellRect, event->pos())) {
                Database database = cell.database();
                database.showPopup(d->canvas, cell, cellRect);
                break;
            }
        }
        if (!d->anchor.isEmpty()) {
            // Hyperlink pressed
            d->processLeftClickAnchor();
        } else if (event->modifiers() & Qt::ControlModifier) {
            if (selection()->referenceSelectionMode()) {
#if 0 // TODO Stefan: remove for NCS of choices
                // Start a marking action
                d->mouseAction = Private::Mark;
                // extend the existing selection
                d->canvas->selection()->extend(QPoint(col, row), sheet);
#endif
            } else {
                // Start a marking action
                d->mouseAction = Private::Mark;
                // extend the existing selection
                d->canvas->selection()->extend(QPoint(col, row), sheet);
            }
            // TODO Stefan: simplification, if NCS of choices is working
            /*        d->canvas->selection()->extend(QPoint(col,row), sheet);*/
        } else {
            // Start a marking action
            d->mouseAction = Private::Mark;
            // reinitialize the selection
            d->canvas->selection()->initialize(QPoint(col, row), sheet);
        }
        break;
    case Qt::MidButton:
        // Paste operation with the middle button?
        if (d->canvas->view()->koDocument()->isReadWrite() && !sheet->isProtected()) {
            d->canvas->selection()->initialize(QPoint(col, row), sheet);
            sheet->paste(d->canvas->selection()->lastRange(), true, Paste::Normal,
                         Paste::OverWrite, false, 0, false, QClipboard::Selection);
            sheet->setRegionPaintDirty(*d->canvas->selection());
            event->accept(); // MouseButtonPress
        }
        break;
    case Qt::RightButton:
        if (!d->canvas->selection()->contains(QPoint(col, row))) {
            // No selection or the mouse press was outside of an existing selection?
            d->canvas->selection()->initialize(QPoint(col, row), sheet);
        }
        event->ignore(); // MouseButtonPress
        break;
    default:
        break;
    }

    d->canvas->scrollToCell(d->canvas->selection()->marker());
    CellToolBase::mousePressEvent(event);
}

void CellTool::mouseReleaseEvent(KoPointerEvent*)
{
    register Sheet * const sheet = d->canvas->activeSheet();
    if (!sheet)
        return;

    d->canvas->d->mousePressed = false;
    d->canvas->view()->disableAutoScroll();

    const Selection* selection = d->canvas->view()->selection();

    // The user started the drag in the lower right corner of the marker ?
    if (d->mouseAction == Private::Merge && !sheet->isProtected()) {
        sheet->mergeCells(Region(selection->lastRange()));
        this->selection()->emitModified();
    } else if (d->mouseAction == Private::AutoFill && !sheet->isProtected()) {
        AutoFillCommand* command = new AutoFillCommand();
        command->setSheet(sheet);
        command->setSourceRange(d->autoFillSource);
        command->setTargetRange(selection->lastRange());
        command->execute();
    }
    // The user started the drag in the middle of a cell ?
    else if (d->mouseAction == Private::Mark && !d->canvas->selection()->referenceSelectionMode()) {
        this->selection()->emitModified();
    }

    d->mouseAction = Private::None;
    d->canvas->d->dragging = false;
    d->dragStart.setX(-1);
}

void CellTool::mouseMoveEvent(KoPointerEvent* event)
{
    // Don't allow modifications, if document is readonly. Selecting is no modification
    if ((!d->canvas->view()->doc()->isReadWrite()) && (d->mouseAction != Private::Mark))
        return;

    if (d->canvas->d->dragging)
        return;

    if (d->dragStart.x() != -1) {
        QPoint p((int) event->pos().x() + (int) d->canvas->xOffset(),
                 (int) event->pos().y() + (int) d->canvas->yOffset());

        if ((d->dragStart - p).manhattanLength() > 4) {
            d->canvas->d->dragging = true;
            d->canvas->startTheDrag();
            d->dragStart.setX(-1);
        }
        d->canvas->d->dragging = false;
        return;
    }

    // Get info about where the event occurred - this is duplicated
    // in ::mousePressEvent, needs to be separated into one function
    register Sheet * const sheet = d->canvas->activeSheet();
    if (!sheet)
        return;

    QPointF position = d->canvas->viewConverter()->viewToDocument(event->pos());
    if (sheet->layoutDirection() == Qt::RightToLeft)
        position.setX(d->canvas->viewConverter()->viewToDocumentX(d->canvas->width()) - position.x());
    position += QPointF(d->canvas->xOffset(), d->canvas->yOffset());

    // In which cell did the user click ?
    double xpos;
    double ypos;
    int col = sheet->leftColumn(position.x(), xpos);
    int row  = sheet->topRow(position.y(), ypos);

    // you cannot move marker when col > KS_colMax or row > KS_rowMax
    if (col > KS_colMax || row > KS_rowMax) {
        kDebug(36005) << "Canvas::mouseMoveEvent: col or row is out of range:"
        << "col: " << col << " row: " << row << endl;
        return;
    }


    //*** Highlighted Range Resize Handling ***
    if (d->mouseAction == Private::Resize) {
        d->canvas->selection()->update(QPoint(col, row));
        return;
    }

    //Check to see if the mouse is over a highlight range size grip and if it is, change the cursor
    //shape to a resize arrow
    if (d->canvas->highlightRangeSizeGripAt(position.x(), position.y())) {
        if (sheet->layoutDirection() == Qt::RightToLeft)
            d->canvas->setCursor(Qt::SizeBDiagCursor);
        else
            d->canvas->setCursor(Qt::SizeFDiagCursor);
        return;
    }

    QRect rct(d->canvas->selection()->lastRange());

    QRect r1;
    QRect r2;

    double lx = sheet->columnPosition(rct.left());
    double rx = sheet->columnPosition(rct.right() + 1);
    double ty = sheet->rowPosition(rct.top());
    double by = sheet->rowPosition(rct.bottom() + 1);

    r1.setLeft((int)(lx - 1));
    r1.setTop((int)(ty - 1));
    r1.setRight((int)(rx + 1));
    r1.setBottom((int)(by + 1));

    r2.setLeft((int)(lx + 1));
    r2.setTop((int)(ty + 1));
    r2.setRight((int)(rx - 1));
    r2.setBottom((int)(by - 1));

    // Test whether the mouse is over some anchor
    {
        Cell cell = Cell(sheet, col, row).masterCell();
        QString anchor;
        if (sheet->layoutDirection() == Qt::RightToLeft) {
            CellView cellView = d->canvas->view()->sheetView(sheet)->cellView(col, row);
            anchor = cellView.testAnchor(cell, cell.width() - position.x() + xpos, position.y() - ypos);
        } else {
            CellView cellView = d->canvas->view()->sheetView(sheet)->cellView(col, row);
            anchor = cellView.testAnchor(cell, position.x() - xpos, position.y() - ypos);
        }
        if (!anchor.isEmpty() && anchor != d->anchor) {
            d->canvas->setCursor(Qt::PointingHandCursor);
        }

        d->anchor = anchor;
    }

    // Test whether mouse is over the Selection.handle
    const QRectF selectionHandle = d->canvas->view()->selection()->selectionHandleArea(d->canvas->viewConverter());
    if (selectionHandle.contains(QPointF(position.x(), position.y()))) {
        //If the cursor is over the handle, than it might be already on the next cell.
        //Recalculate the cell position!
        col  = sheet->leftColumn(position.x() - d->canvas->viewConverter()->viewToDocumentX(2), xpos);
        row  = sheet->topRow(position.y() - d->canvas->viewConverter()->viewToDocumentY(2), ypos);

        if (!sheet->isProtected()) {
            if (sheet->layoutDirection() == Qt::RightToLeft)
                d->canvas->setCursor(Qt::SizeBDiagCursor);
            else
                d->canvas->setCursor(Qt::SizeFDiagCursor);
        }
    } else if (!d->anchor.isEmpty()) {
        if (!sheet->isProtected())
            d->canvas->setCursor(Qt::PointingHandCursor);
    } else if (r1.contains(QPoint((int) position.x(), (int) position.y()))
               && !r2.contains(QPoint((int) position.x(), (int) position.y()))) {
        d->canvas->setCursor(Qt::PointingHandCursor);
    } else if (selection()->referenceSelectionMode()) {
        //Visual cue to indicate that the user can drag-select the selection selection
        d->canvas->setCursor(Qt::CrossCursor);
    } else {
        //Nothing special is happening, use a normal arrow cursor
        d->canvas->setCursor(Qt::ArrowCursor);
    }

    // No marking, selecting etc. in progress? Then quit here.
    if (d->mouseAction == Private::None)
        return;

    // Set the new extent of the selection
    d->canvas->selection()->update(QPoint(col, row));
}

void CellTool::mouseDoubleClickEvent(KoPointerEvent* event)
{
    Q_UNUSED(event);
    if (d->canvas->view()->doc()->isReadWrite() && d->canvas->activeSheet())
        createEditor(false /* keep content */);
}

KoInteractionStrategy* CellTool::createStrategy(KoPointerEvent* event)
{
    Q_UNUSED(event)
    return 0;
}
#endif

void CellTool::activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes)
{
    canvas()->shapeManager()->selection()->deselectAll();
    CellToolBase::activate(toolActivation, shapes);
}

KSpread::Selection* CellTool::selection()
{
    return d->canvas->selection();
}

QPointF CellTool::offset() const
{
    return QPointF(0.0, 0.0);
}

QSizeF CellTool::size() const
{
    return canvas()->viewConverter()->viewToDocument(d->canvas->size());
}

int CellTool::maxCol() const
{
    return KS_colMax;
}

int CellTool::maxRow() const
{
    return KS_rowMax;
}

SheetView* CellTool::sheetView(const Sheet* sheet) const
{
    return d->canvas->view()->sheetView(sheet);
}

void CellTool::definePrintRange()
{
    DefinePrintRangeCommand* command = new DefinePrintRangeCommand();
    command->setSheet(selection()->activeSheet());
    command->add(*selection());
    d->canvas->view()->doc()->addCommand(command);
}

void CellTool::Private::processClickSelectionHandle(KoPointerEvent* event)
{
    // Auto fill ? That is done using the left mouse button.
    if (event->button() == Qt::LeftButton) {
        mouseAction = Private::AutoFill;
        autoFillSource = canvas->selection()->lastRange();
    }
    // Merge a cell (done with the right mouse button) ?
    // But for that to work there must not be a selection.
    else if (event->button() == Qt::MidButton && canvas->selection()->isSingular()) {
        mouseAction = Private::Merge;
    }
    return;
}

#if 0 // KSPREAD_MOUSE_STRATEGIES
void CellTool::Private::processLeftClickAnchor()
{
    KNotification *notify = new KNotification("LinkActivated");
    notify->setText(i18n("Link <i>%1</i> activated", anchor));
    notify->addContext("anchor", anchor);

    KUrl url(anchor);
    if (! url.isValid() || url.isRelative()) {
        Region r(anchor, canvas->view()->doc()->map(), canvas->activeSheet());
        if (r.isValid()) {
            if (r.firstSheet() != canvas->view()->activeSheet())
                canvas->view()->setActiveSheet(r.firstSheet());
            canvas->selection()->initialize(r);

            if (! r.firstRange().isNull()) {
                QPoint p(r.firstRange().topLeft());
                notify->addContext("region", Cell(r.firstSheet(), p.x(), p.y()).fullName());
            }
        }
    } else {
        const QString type = KMimeType::findByUrl(url, 0, url.isLocalFile())->name();
        notify->addContext("type", type);
        if (! Util::localReferenceAnchor(anchor)) {
            const bool executable = KRun::isExecutableFile(url, type);
            notify->addContext("executable", QVariant(executable).toString());
            if (executable) {
                QString question = i18n("This link points to the program or script '%1'.\n"
                                        "Malicious programs can harm your computer. "
                                        "Are you sure that you want to run this program?", anchor);
                // this will also start local programs, so adding a "don't warn again"
                // checkbox will probably be too dangerous
                int selection = KMessageBox::warningYesNo(canvas, question, i18n("Open Link?"));
                if (selection != KMessageBox::Yes)
                    return;
            }
            new KRun(url, canvas, 0, url.isLocalFile());
        }
    }

    QTimer::singleShot(0, notify, SLOT(sendEvent()));
}
#endif

#include "CellTool.moc"
