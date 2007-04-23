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

#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QSpinBox>

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kmimetype.h>
#include <krun.h>

#include <KoCanvasBase.h>
#include <KoPointerEvent.h>
#include <KoSelection.h>
#include <KoShapeManager.h>

#include "Cell.h"
#include "CellView.h"
#include "Canvas.h"
#include "CanvasPrivate.h"
#include "Doc.h"
#include "Global.h"
#include "Selection.h"
#include "Sheet.h"
#include "SheetView.h"
#include "Util.h"
#include "View.h"

#include "DefaultTool.h"

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

class DefaultTool::Private
{
public:
    void processClickSelectionHandle( KoPointerEvent* event );
    void processLeftClickAnchor();

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

DefaultTool::DefaultTool( KoCanvasBase* canvas )
    : KoTool( canvas )
    , d( new Private )
{
    d->canvas = static_cast<Canvas*>( canvas );
    d->dragStart = QPoint( -1, -1 );
    d->mouseAction = Private::None;
}

DefaultTool::~DefaultTool()
{
    delete d;
}

void DefaultTool::paint( QPainter& painter, KoViewConverter& viewConverter )
{
    const QRectF paintRect = viewConverter.viewToDocument( d->canvas->rect() );

    /* paint the selection */
    d->canvas->paintHighlightedRanges(painter, paintRect);
    d->canvas->paintNormalMarker(painter, paintRect);
}

void DefaultTool::mousePressEvent( KoPointerEvent* event )
{
    register Sheet * const sheet = d->canvas->activeSheet();
    if (!sheet)
        return;

    if ( event->button() == Qt::LeftButton )
    {
        d->canvas->d->mousePressed = true;
        d->canvas->view()->enableAutoScroll();
    }

    // Get info about where the event occurred - this is duplicated
    // in ::mouseMoveEvent, needs to be separated into one function
    QPointF position = d->canvas->viewConverter()->viewToDocument( event->pos() );
    if ( sheet->layoutDirection() == Qt::RightToLeft )
        position.setX( d->canvas->viewConverter()->viewToDocumentX( d->canvas->width() ) - position.x() );
    position += QPointF( d->canvas->xOffset(), d->canvas->yOffset() );

    // In which cell did the user click ?
    double xpos;
    double ypos;
    int col  = sheet->leftColumn( position.x(), xpos );
    int row  = sheet->topRow( position.y(), ypos );
    // you cannot move marker when col > KS_colMax or row > KS_rowMax
    if ( col > KS_colMax || row > KS_rowMax )
    {
        kDebug(36005) << "Canvas::mousePressEvent: col or row is out of range: "
                << "col: " << col << " row: " << row << endl;
        return;
    }

    if (d->canvas->chooseMode() && d->canvas->highlightRangeSizeGripAt(position.x(),position.y()))
    {
        d->canvas->choice()->setActiveElement( QPoint( col, row ), d->canvas->editor() );
        d->mouseAction = Private::Resize;
        return;
    }

    // We were editing a cell -> save value and get out of editing mode
    if ( d->canvas->editor() && !d->canvas->chooseMode() )
    {
        d->canvas->deleteEditor( true ); // save changes
    }

// FIXME Stefan: Still needed?
//     d->scrollTimer->start( 50 );

    // Did we click in the lower right corner of the marker/marked-area ?
    if ( d->canvas->selection()->selectionHandleArea(d->canvas->view()->zoomHandler()).contains( QPointF( position.x(), position.y() ) ) )
    {
        d->processClickSelectionHandle( event );
        return;
    }


    // TODO Stefan: adapt to non-cont. selection
    {
        // start drag ?
        QRect rct( d->canvas->selection()->lastRange() );

        QRect r1;
        QRect r2;
        {
            double lx = sheet->columnPosition( rct.left() );
            double rx = sheet->columnPosition( rct.right() + 1 );
            double ty = sheet->rowPosition( rct.top() );
            double by = sheet->rowPosition( rct.bottom() + 1 );

            r1.setLeft( (int) (lx - 1) );
            r1.setTop( (int) (ty - 1) );
            r1.setRight( (int) (rx + 1) );
            r1.setBottom( (int) (by + 1) );

            r2.setLeft( (int) (lx + 1) );
            r2.setTop( (int) (ty + 1) );
            r2.setRight( (int) (rx - 1) );
            r2.setBottom( (int) (by - 1) );
        }

        d->dragStart.setX( -1 );

        if ( r1.contains( QPoint( (int) position.x(), (int) position.y() ) )
             && !r2.contains( QPoint( (int) position.x(), (int) position.y() ) ) )
        {
            d->dragStart.setX( (int) position.x() );
            d->dragStart.setY( (int) position.y() );

            return;
        }
    }

//    kDebug(36005) << "Clicked in cell " << col << ", " << row << endl;

    // Extending an existing selection with the shift button ?
    if ((event->modifiers() & Qt::ShiftModifier) &&
         d->canvas->view()->koDocument()->isReadWrite() &&
         !d->canvas->selection()->isColumnOrRowSelected())
    {
        (d->canvas->chooseMode() ? d->canvas->choice() : d->canvas->selection())->update(QPoint(col,row));
        return;
    }


    // Go to the upper left corner of the obscuring object if cells are merged
    Cell cell( sheet, col, row );
    if (cell.isPartOfMerged())
    {
        cell = cell.masterCell();
        col = cell.column();
        row = cell.row();
    }

    switch (event->button())
    {
        case Qt::LeftButton:
            if (!d->anchor.isEmpty())
            {
                // Hyperlink pressed
                d->processLeftClickAnchor();
            }
            else if ( event->modifiers() & Qt::ControlModifier )
            {
                if (d->canvas->chooseMode())
                {
#if 0 // TODO Stefan: remove for NCS of choices
                    // Start a marking action
                    d->mouseAction = Private::Mark;
                    // extend the existing selection
                    d->canvas->choice()->extend(QPoint(col,row), sheet);
#endif
                }
                else
                {
                    // Start a marking action
                    d->mouseAction = Private::Mark;
                    // extend the existing selection
                    d->canvas->selection()->extend(QPoint(col,row), sheet);
                }
                // TODO Stefan: simplification, if NCS of choices is working
                /*        (d->canvas->chooseMode() ? d->canvas->choice() : d->canvas->selection())->extend(QPoint(col,row), sheet);*/
            }
            else
            {
                // Start a marking action
                d->mouseAction = Private::Mark;
                // reinitialize the selection
                (d->canvas->chooseMode() ? d->canvas->choice() : d->canvas->selection())->initialize(QPoint(col,row), sheet);
            }
            break;
        case Qt::MidButton:
            // Paste operation with the middle button?
            if ( d->canvas->view()->koDocument()->isReadWrite() && !sheet->isProtected() )
            {
                (d->canvas->chooseMode() ? d->canvas->choice() : d->canvas->selection())->initialize( QPoint( col, row ), sheet );
                sheet->paste(d->canvas->selection()->lastRange(), true, Paste::Normal,
                             Paste::OverWrite, false, 0, false, QClipboard::Selection);
                sheet->setRegionPaintDirty(*d->canvas->selection());
            }
            break;
        case Qt::RightButton:
            if (!d->canvas->selection()->contains( QPoint( col, row ) ))
            {
                // No selection or the mouse press was outside of an existing selection?
                (d->canvas->chooseMode() ? d->canvas->choice() : d->canvas->selection())->initialize(QPoint(col,row), sheet);
            }
            break;
        default:
            break;
    }

    d->canvas->scrollToCell(d->canvas->selection()->marker());
    if ( !d->canvas->chooseMode() )
    {
        d->canvas->view()->updateEditWidgetOnPress();
    }
    d->canvas->updatePosWidget();

    // Context menu?
    if ( event->button() == Qt::RightButton )
    {
        QPoint p = d->canvas->mapToGlobal( event->pos() );
        d->canvas->view()->openPopupMenu( p );
    }
}

void DefaultTool::mouseReleaseEvent( KoPointerEvent* )
{
    register Sheet * const sheet = d->canvas->activeSheet();
    if (!sheet)
        return;

// TODO Stefan: Still needed?
//   if ( d->scrollTimer->isActive() )
//     d->scrollTimer->stop();

    d->canvas->d->mousePressed = false;
    d->canvas->view()->disableAutoScroll();

    const Selection* selection = d->canvas->view()->selection();

    // The user started the drag in the lower right corner of the marker ?
    if ( d->mouseAction == Private::Merge && !sheet->isProtected() )
    {
        sheet->mergeCells(Region(selection->lastRange()));
        d->canvas->view()->updateEditWidget();
    }
    else if ( d->mouseAction == Private::AutoFill && !sheet->isProtected() )
    {
        sheet->autofill( d->autoFillSource, selection->lastRange() );
        d->canvas->view()->updateEditWidget();
    }
    // The user started the drag in the middle of a cell ?
    else if ( d->mouseAction == Private::Mark && !d->canvas->chooseMode() )
    {
        d->canvas->view()->updateEditWidget();
    }

    d->mouseAction = Private::None;
    d->canvas->d->dragging = false;
    d->dragStart.setX( -1 );
}

void DefaultTool::mouseMoveEvent( KoPointerEvent* event )
{
    // Dont allow modifications, if document is readonly. Selecting is no modification
    if ( (!d->canvas->view()->doc()->isReadWrite()) && (d->mouseAction!=Private::Mark))
        return;

    if ( d->canvas->d->dragging )
        return;

    if ( d->dragStart.x() != -1 )
    {
        QPoint p( (int) event->pos().x() + (int) d->canvas->xOffset(),
                  (int) event->pos().y() + (int) d->canvas->yOffset() );

        if ( ( d->dragStart - p ).manhattanLength() > 4 )
        {
            d->canvas->d->dragging = true;
            d->canvas->startTheDrag();
            d->dragStart.setX( -1 );
        }
        d->canvas->d->dragging = false;
        return;
    }

    // Get info about where the event occurred - this is duplicated
    // in ::mousePressEvent, needs to be separated into one function
    register Sheet * const sheet = d->canvas->activeSheet();
    if (!sheet)
        return;

    QPointF position = d->canvas->viewConverter()->viewToDocument( event->pos() );
    if ( sheet->layoutDirection() == Qt::RightToLeft )
        position.setX( d->canvas->viewConverter()->viewToDocumentX( d->canvas->width() ) - position.x() );
    position += QPointF( d->canvas->xOffset(), d->canvas->yOffset() );

    // In which cell did the user click ?
    double xpos;
    double ypos;
    int col = sheet->leftColumn( position.x(), xpos );
    int row  = sheet->topRow( position.y(), ypos );

    // you cannot move marker when col > KS_colMax or row > KS_rowMax
    if ( col > KS_colMax || row > KS_rowMax )
    {
        kDebug(36005) << "Canvas::mouseMoveEvent: col or row is out of range: "
                << "col: " << col << " row: " << row << endl;
        return;
    }


    //*** Highlighted Range Resize Handling ***
    if (d->mouseAction == Private::Resize)
    {
        d->canvas->choice()->update(QPoint(col,row));
        return;
    }

    //Check to see if the mouse is over a highlight range size grip and if it is, change the cursor
    //shape to a resize arrow
    if (d->canvas->highlightRangeSizeGripAt(position.x(),position.y()))
    {
        if ( sheet->layoutDirection() == Qt::RightToLeft )
            d->canvas->setCursor( Qt::SizeBDiagCursor );
        else
            d->canvas->setCursor( Qt::SizeFDiagCursor );
        return;
    }

    QRect rct( (d->canvas->chooseMode() ? d->canvas->choice() : d->canvas->selection())->lastRange() );

    QRect r1;
    QRect r2;

    double lx = sheet->columnPosition( rct.left() );
    double rx = sheet->columnPosition( rct.right() + 1 );
    double ty = sheet->rowPosition( rct.top() );
    double by = sheet->rowPosition( rct.bottom() + 1 );

    r1.setLeft( (int) (lx - 1) );
    r1.setTop( (int) (ty - 1) );
    r1.setRight( (int) (rx + 1) );
    r1.setBottom( (int) (by + 1) );

    r2.setLeft( (int) (lx + 1) );
    r2.setTop( (int) (ty + 1) );
    r2.setRight( (int) (rx - 1) );
    r2.setBottom( (int) (by - 1) );

    // Test whether the mouse is over some anchor
    {
        Cell cell = Cell( sheet, col, row ).masterCell();
        QString anchor;
        if ( sheet->layoutDirection() == Qt::RightToLeft )
        {
            CellView cellView = d->canvas->view()->sheetView( sheet )->cellView( col, row );
            anchor = cellView.testAnchor( cell, cell.width() - position.x() + xpos, position.y() - ypos );
        }
        else
        {
            CellView cellView = d->canvas->view()->sheetView( sheet )->cellView( col, row );
            anchor = cellView.testAnchor( cell, position.x() - xpos, position.y() - ypos );
        }
        if ( !anchor.isEmpty() && anchor != d->anchor )
        {
            d->canvas->setCursor( Qt::PointingHandCursor );
        }

        d->anchor = anchor;
    }

    // Test wether mouse is over the Selection.handle
    const QRectF selectionHandle = d->canvas->view()->selection()->selectionHandleArea(d->canvas->view()->zoomHandler());
    if ( selectionHandle.contains( QPointF( position.x(), position.y() ) ) )
    {
        //If the cursor is over the handle, than it might be already on the next cell.
        //Recalculate the cell position!
        col  = sheet->leftColumn( position.x() - d->canvas->viewConverter()->viewToDocumentX( 2 ), xpos );
        row  = sheet->topRow( position.y() - d->canvas->viewConverter()->viewToDocumentY( 2 ), ypos );

        if ( !sheet->isProtected() )
        {
            if ( sheet->layoutDirection() == Qt::RightToLeft )
                d->canvas->setCursor( Qt::SizeBDiagCursor );
            else
                d->canvas->setCursor( Qt::SizeFDiagCursor );
        }
    }
    else if ( !d->anchor.isEmpty() )
    {
        if ( !sheet->isProtected() )
            d->canvas->setCursor( Qt::PointingHandCursor );
    }
    else if ( r1.contains( QPoint( (int) position.x(), (int) position.y() ) )
              && !r2.contains( QPoint( (int) position.x(), (int) position.y() ) ) )
    {
        d->canvas->setCursor( Qt::PointingHandCursor );
    }
    else if ( d->canvas->chooseMode() )
    {
        //Visual cue to indicate that the user can drag-select the choice selection
        d->canvas->setCursor( Qt::CrossCursor );
    }
    else
    {
        //Nothing special is happening, use a normal arrow cursor
        d->canvas->setCursor( Qt::ArrowCursor );
    }

    // No marking, selecting etc. in progress? Then quit here.
    if ( d->mouseAction == Private::None )
        return;

    // Set the new extent of the selection
    (d->canvas->chooseMode() ? d->canvas->choice() : d->canvas->selection())->update(QPoint(col,row));
}

void DefaultTool::mouseDoubleClickEvent( KoPointerEvent* event )
{
    Q_UNUSED( event );
    if ( d->canvas->view()->doc()->isReadWrite() && d->canvas->activeSheet() )
        d->canvas->createEditor( false /* keep content */);
}

void DefaultTool::keyPressEvent( QKeyEvent* event )
{
    register Sheet * const sheet = d->canvas->activeSheet();

    if ( !sheet || d->canvas->formatKeyPress( event ) )
        return;

    // Dont handle the remaining special keys.
    if ( event->modifiers() & ( Qt::AltModifier | Qt::ControlModifier ) &&
         (event->key() != Qt::Key_Down) &&
         (event->key() != Qt::Key_Up) &&
         (event->key() != Qt::Key_Right) &&
         (event->key() != Qt::Key_Left) &&
         (event->key() != Qt::Key_Home) &&
         (event->key() != Qt::Key_Enter) &&
         (event->key() != Qt::Key_Return) &&
         (event->key() != KGlobalSettings::contextMenuKey()))
    {
        d->canvas->QWidget::keyPressEvent( event );
        return;
    }

    // Always accept so that events are not
    // passed to the parent.
    event->setAccepted(true);

    d->canvas->view()->doc()->emitBeginOperation(false);
    if ( event->key() == KGlobalSettings::contextMenuKey() ) {
        int row = d->canvas->selection()->marker().y();
        int col = d->canvas->selection()->marker().x();
        QPointF p( sheet->columnPosition(col), sheet->rowPosition(row) );
        d->canvas->view()->openPopupMenu( d->canvas->mapToGlobal(p.toPoint()) );
    }
    switch( event->key() )
    {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            d->canvas->processEnterKey( event );
            return;
            break;
        case Qt::Key_Down:
        case Qt::Key_Up:
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Tab: /* a tab behaves just like a right/left arrow */
        case Qt::Key_Backtab:  /* and so does Shift+Tab */
            if (event->modifiers() & Qt::ControlModifier)
            {
                if ( !d->canvas->processControlArrowKey( event ) )
                    return;
            }
            else
            {
                d->canvas->processArrowKey( event );
                return;
            }
            break;

        case Qt::Key_Escape:
            d->canvas->processEscapeKey( event );
            return;
            break;

        case Qt::Key_Home:
            if ( !d->canvas->processHomeKey( event ) )
                return;
            break;

        case Qt::Key_End:
            if ( !d->canvas->processEndKey( event ) )
                return;
            break;

        case Qt::Key_PageUp:  /* Page Up */
            if ( !d->canvas->processPriorKey( event ) )
                return;
            break;

        case Qt::Key_PageDown:   /* Page Down */
            if ( !d->canvas->processNextKey( event ) )
                return;
            break;

        case Qt::Key_Delete:
            d->canvas->processDeleteKey( event );
            return;
            break;

        case Qt::Key_F2:
            d->canvas->processF2Key( event );
            return;
            break;

        case Qt::Key_F4:
            d->canvas->processF4Key( event );
            return;
            break;

        default:
            d->canvas->processOtherKey( event );
            return;
            break;
    }

    //most process*Key methods call emitEndOperation, this only gets called in some situations
    // (after some move operations)
    d->canvas->view()->doc()->emitEndOperation( Region( d->canvas->visibleCells() ) );
    return;
}

void DefaultTool::activate( bool temporary )
{
    Q_UNUSED( temporary );

    m_canvas->shapeManager()->selection()->deselectAll();
    useCursor( Qt::ArrowCursor, true );
}

void DefaultTool::deactivate()
{
}

void DefaultTool::Private::processClickSelectionHandle( KoPointerEvent* event )
{
    // Auto fill ? That is done using the left mouse button.
    if ( event->button() == Qt::LeftButton )
    {
        mouseAction = Private::AutoFill;
        autoFillSource = canvas->selection()->lastRange();
    }
    // Merge a cell (done with the right mouse button) ?
    // But for that to work there must not be a selection.
    else if ( event->button() == Qt::MidButton && canvas->selection()->isSingular())
    {
        mouseAction = Private::Merge;
    }
    return;
}

void DefaultTool::Private::processLeftClickAnchor()
{
    bool isRefLink = Util::localReferenceAnchor( anchor );
    bool isLocalLink = (anchor.indexOf("file:") == 0);
    if ( !isRefLink )
    {
        QString type = KMimeType::findByUrl(anchor, 0, isLocalLink)->name();

        if ( KRun::isExecutableFile( anchor, type ) )
        {
            QString question = i18n("This link points to the program or script '%1'.\n"
                                    "Malicious programs can harm your computer. "
                                    "Are you sure that you want to run this program?", anchor);
            // this will also start local programs, so adding a "don't warn again"
            // checkbox will probably be too dangerous
            int choice = KMessageBox::warningYesNo(canvas, question, i18n("Open Link?"));
            if ( choice != KMessageBox::Yes )
                return;
        }
        new KRun(anchor, canvas);
    }
    else
    {
        canvas->selection()->initialize(Region(canvas->view()->doc()->map(), anchor, canvas->activeSheet()));
    }
}

#include "DefaultTool.moc"
