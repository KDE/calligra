/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <iostream>

#include <qpainter.h>

#include <kapp.h>
#include <kdebug.h>

#include "basicelement.h"
#include "formulacursor.h"
#include "formulaelement.h"
#include "kformulacontainer.h"
#include "kformuladocument.h"
#include "kformulaview.h"

KFORMULA_NAMESPACE_BEGIN

struct View::View_Impl {

    View_Impl(Container* doc, View* view)
            : smallCursor(false), cursorVisible(false), cursorHasChanged(true),
              document(doc)
    {
        connect(document, SIGNAL(elementWillVanish(BasicElement*)),
                view, SLOT(slotElementWillVanish(BasicElement*)));
        connect(document, SIGNAL(formulaLoaded(FormulaElement*)),
                view, SLOT(slotFormulaLoaded(FormulaElement*)));
        connect(document, SIGNAL(cursorMoved(FormulaCursor*)),
                view, SLOT(slotCursorMoved(FormulaCursor*)));

        cursor = document->createCursor();
    }

    ~View_Impl()
    {
        delete cursor;
    }

    /**
     * If set the cursor will never be bigger that the formula.
     */
    bool smallCursor;

    /**
     * Whether you can see the cursor. This has to be kept
     * in sync with reality.
     */
    bool cursorVisible;

    /**
     * Whether the cursor changed since the last time
     * we emitted a cursorChanged signal.
     */
    bool cursorHasChanged;

    /**
     * The area that needs an update because the cursor moved.
     */
    QRect dirtyArea;

    /**
     * The formula we show.
     */
    Container* document;

    /**
     * Out cursor.
     */
    FormulaCursor* cursor;
};


FormulaCursor* View::cursor() const        { return impl->cursor; }
bool& View::cursorHasChanged()             { return impl->cursorHasChanged; }
bool& View::cursorVisible()                { return impl->cursorVisible; }
bool& View::smallCursor()                  { return impl->smallCursor; }
Container* View::container() const { return impl->document; }

QRect View::getDirtyArea() const { return impl->dirtyArea; }


View::View(Container* doc)
{
    impl = new View_Impl(doc, this);
    cursor()->calcCursorSize( contextStyle(), smallCursor() );
}

View::~View()
{
    delete impl;
}


QPoint View::getCursorPoint() const
{
    return contextStyle().layoutUnitToPixel( cursor()->getCursorPoint() );
}

void View::setReadOnly(bool ro)
{
    cursor()->setReadOnly(ro);
}


void View::draw(QPainter& painter, const QRect& rect, const QColorGroup& cg)
{
//     kdDebug( 40000 ) << "View::draw: " << rect.x() << " " << rect.y() << " "
//                      << rect.width() << " " << rect.height() << endl;
    container()->draw( painter, rect, cg );
    if ( cursorVisible() ) {
        cursor()->draw( painter, contextStyle(), smallCursor() );
    }
}

void View::keyPressEvent( QKeyEvent* event )
{
    if ( !container()->input( event ) ) {
        int action = event->key();
        int state = event->state();
	MoveFlag flag = movementFlag(state);

	switch (action) {
	case Qt::Key_Left:
            slotMoveLeft(flag);
            break;
        case Qt::Key_Right:
            slotMoveRight(flag);
            break;
        case Qt::Key_Up:
            slotMoveUp(flag);
            break;
        case Qt::Key_Down:
            slotMoveDown(flag);
            break;
        case Qt::Key_Home:
            slotMoveHome(flag);
            break;
        case Qt::Key_End:
            slotMoveEnd(flag);
            break;
        case Qt::Key_Return:
            container()->addLineBreak();
            break;
        }
    }
}


void View::focusInEvent(QFocusEvent*)
{
    //cerr << "void View::focusInEvent(QFocusEvent*): " << cursorVisible() << " " << hasFocus() << endl;
    container()->setActiveCursor(cursor());
    cursorHasChanged() = true;
    cursorVisible() = true;
    emitCursorChanged();
}

void View::focusOutEvent(QFocusEvent*)
{
    //cerr << "void View::focusOutEvent(QFocusEvent*): " << cursorVisible() << " " << hasFocus() << endl;
    container()->setActiveCursor(0);
    cursorHasChanged() = true;
    cursorVisible() = false;
    emitCursorChanged();
}

void View::mousePressEvent( QMouseEvent* event )
{
    const ContextStyle& context = contextStyle();
    mousePressEvent( event, context.pixelToLayoutUnit( event->pos() ) );
}

void View::mouseReleaseEvent( QMouseEvent* event )
{
    const ContextStyle& context = contextStyle();
    mouseReleaseEvent( event, context.pixelToLayoutUnit( event->pos() ) );
}

void View::mouseDoubleClickEvent( QMouseEvent* event )
{
    const ContextStyle& context = contextStyle();
    mouseDoubleClickEvent( event, context.pixelToLayoutUnit( event->pos() ) );
}

void View::mouseMoveEvent( QMouseEvent* event )
{
    const ContextStyle& context = contextStyle();
    mouseMoveEvent( event, context.pixelToLayoutUnit( event->pos() ) );
}

void View::wheelEvent( QWheelEvent* event )
{
    const ContextStyle& context = contextStyle();
    wheelEvent( event, context.pixelToLayoutUnit( event->pos() ) );
}

void View::mousePressEvent( QMouseEvent* event, const PtPoint& pos )
{
    const ContextStyle& context = contextStyle();
    mousePressEvent( event, context.ptToLayoutUnitPix( pos ) );
}

void View::mouseReleaseEvent( QMouseEvent* event, const PtPoint& pos )
{
    const ContextStyle& context = contextStyle();
    mouseReleaseEvent( event, context.ptToLayoutUnitPix( pos ) );
}

void View::mouseDoubleClickEvent( QMouseEvent* event, const PtPoint& pos )
{
    const ContextStyle& context = contextStyle();
    mouseDoubleClickEvent( event, context.ptToLayoutUnitPix( pos ) );
}

void View::mouseMoveEvent( QMouseEvent* event, const PtPoint& pos )
{
    const ContextStyle& context = contextStyle();
    mouseMoveEvent( event, context.ptToLayoutUnitPix( pos ) );
}

void View::wheelEvent( QWheelEvent* event, const PtPoint& pos )
{
    const ContextStyle& context = contextStyle();
    wheelEvent( event, context.ptToLayoutUnitPix( pos ) );
}


void View::mousePressEvent( QMouseEvent* event, const LuPixelPoint& pos )
{
    int flags = movementFlag( event->state() );
    cursor()->mousePress( pos, flags );
    emitCursorChanged();
}

void View::mouseReleaseEvent( QMouseEvent* event, const LuPixelPoint& pos )
{
    int flags = movementFlag( event->state() );
    cursor()->mouseRelease( pos, flags );
    emitCursorChanged();
}

void View::mouseDoubleClickEvent( QMouseEvent*, const LuPixelPoint& )
{
    cursor()->moveRight( WordMovement );
    cursor()->moveLeft( SelectMovement | WordMovement );
    emitCursorChanged();
}

void View::mouseMoveEvent( QMouseEvent* event, const LuPixelPoint& pos )
{
    int flags = movementFlag( event->state() );
    cursor()->mouseMove( pos, flags );
    emitCursorChanged();
}

void View::wheelEvent( QWheelEvent*, const LuPixelPoint& )
{
}


void View::slotCursorMoved(FormulaCursor* c)
{
    if (c == cursor()) {
        cursorHasChanged() = true;
        emitCursorChanged();
    }
}

// void View::slotCursorChanged(FormulaCursor* c)
// {
//     if ( c == cursor() ) {
//         C->calcCursorSize( contextStyle(), smallCursor() );
//     }
// }

void View::slotFormulaLoaded(FormulaElement* formula)
{
    cursor()->formulaLoaded(formula);
}

void View::slotElementWillVanish(BasicElement* element)
{
    cursor()->elementWillVanish(element);
}

void View::slotSelectAll()
{
    cursor()->moveHome(WordMovement);
    cursor()->moveEnd(SelectMovement | WordMovement);
    emitCursorChanged();
}


void View::slotMoveLeft(MoveFlag flag)
{
    cursor()->moveLeft(flag);
    emitCursorChanged();
}

void View::slotMoveRight(MoveFlag flag)
{
    cursor()->moveRight(flag);
    emitCursorChanged();
}

void View::slotMoveUp(MoveFlag flag)
{
    cursor()->moveUp(flag);
    emitCursorChanged();
}

void View::slotMoveDown(MoveFlag flag)
{
    cursor()->moveDown(flag);
    emitCursorChanged();
}

void View::slotMoveHome(MoveFlag flag)
{
    cursor()->moveHome(flag);
    emitCursorChanged();
}

void View::slotMoveEnd(MoveFlag flag)
{
    cursor()->moveEnd(flag);
    emitCursorChanged();
}


MoveFlag View::movementFlag(int state)
{
    int flag = NormalMovement;

    if (state & Qt::ControlButton)
        flag |= WordMovement;

    if (state & Qt::ShiftButton)
        flag |= SelectMovement;

    return static_cast<MoveFlag>(flag);
}

void View::setSmallCursor(bool small)
{
    smallCursor() = small;
}

bool View::isHome() const
{
    return cursor()->isHome();
}

bool View::isEnd() const
{
    return cursor()->isEnd();
}

void View::emitCursorChanged()
{
    if (cursor()->hasChanged() || cursorHasChanged()) {
        const ContextStyle& context = contextStyle();

        cursor()->clearChangedFlag();
        cursorHasChanged() = false;

        impl->dirtyArea = context.layoutUnitToPixel( cursor()->getCursorSize() );
        cursor()->calcCursorSize( contextStyle(), smallCursor() );
        impl->dirtyArea |= context.layoutUnitToPixel( cursor()->getCursorSize() );

        emit cursorChanged(cursorVisible(), cursor()->isSelection());
    }
}

const ContextStyle& View::contextStyle() const
{
    return container()->document()->getContextStyle();
}

KFORMULA_NAMESPACE_END

using namespace KFormula;
#include "kformulaview.moc"
