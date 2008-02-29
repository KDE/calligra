/* This file is part of the KDE project
   Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net>

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

#include "KoFormulaTool.h"
#include "KoFormulaShape.h"
#include "FormulaCursor.h"
#include "FormulaToolWidget.h"
#include "BasicElement.h"
#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <kiconloader.h>
#include <klocale.h>
#include <QKeyEvent>
#include <QPainter>
#include "KoFormulaTool.moc"

KoFormulaTool::KoFormulaTool( KoCanvasBase* canvas ) : KoTool( canvas ),
                                                       m_formulaShape( 0 ),
                                                       m_formulaCursor( 0 )
{
}

KoFormulaTool::~KoFormulaTool()
{
    if( m_formulaCursor )
        delete m_formulaCursor;
}

void KoFormulaTool::activate( bool temporary )
{
    Q_UNUSED(temporary);
    KoSelection* selection = m_canvas->shapeManager()->selection();
    foreach( KoShape* shape, selection->selectedShapes() )
    {
        m_formulaShape = dynamic_cast<KoFormulaShape*>( shape );
        if( m_formulaShape )
            break;
    }
    if( m_formulaShape == 0 )  // none found
    {
        emit done();
        return;
    }
    
    useCursor( Qt::IBeamCursor, true );
    m_formulaCursor = new FormulaCursor( m_formulaShape->formulaElement() );
}

void KoFormulaTool::deactivate()
{
    m_formulaShape = 0;
    delete m_formulaCursor;
    m_formulaCursor = 0;
}

void KoFormulaTool::paint( QPainter &painter, const KoViewConverter &converter)
{
    painter.setMatrix( painter.matrix() *
                       m_formulaShape->absoluteTransformation( &converter ) );
    double zoomX, zoomY;              // apply view conversions for painting
    converter.zoom(&zoomX, &zoomY);
    painter.scale(zoomX, zoomY);
    
    m_formulaCursor->paint( painter );
}

void KoFormulaTool::mousePressEvent( KoPointerEvent *event )
{
    Q_UNUSED( event )

// TODO implement the action and the elementAt method in FormulaShape
//   m_formulaCursor->setCursorTo( m_formulaShape->elementAt( ) );
//
//
//   from the old FormulaCursor implementation
/*
    FormulaElement* formula = getElement()->formula();
    formula->goToPos( this, pos );

    setCursorToElement( m_container->childElementAt( pos ) );
    if (flag & SelectMovement) {
        setSelection(true);
        if (getMark() == -1) {
            setMark(getPos());
        }
    }
    else {
        setSelection(false);
        setMark(getPos());
    }
*/
}

void KoFormulaTool::mouseDoubleClickEvent( KoPointerEvent *event )
{
    Q_UNUSED( event )

    // TODO select whole element
}

void KoFormulaTool::mouseMoveEvent( KoPointerEvent *event )
{
    Q_UNUSED( event )

    if( !m_formulaCursor->hasSelection() )
        return;

    //TODO Implement drag and drop of elements
    //TODO Implement selecting via mouse
}

void KoFormulaTool::mouseReleaseEvent( KoPointerEvent *event )
{
    Q_UNUSED( event )

    // TODO Implement drag and drop
}

void KoFormulaTool::keyPressEvent( QKeyEvent *event )
{
    if( !m_formulaCursor )
        return;

    m_formulaCursor->setSelecting( event->modifiers() & Qt::ShiftModifier );

    switch( event->key() )                           // map key to movement or action
    {
        case Qt::Key_Backspace:
            remove( true );
            break;
        case Qt::Key_Delete:
	    remove( false );
            break;
        case Qt::Key_Left:
	    m_formulaCursor->move( MoveLeft );
            break;
        case Qt::Key_Up:
            m_formulaCursor->move( MoveUp );
            break;
        case Qt::Key_Right:
	    m_formulaCursor->move( MoveRight );
            break;
        case Qt::Key_Down:
	    m_formulaCursor->move( MoveDown );
            break;
        case Qt::Key_End:
	    m_formulaCursor->moveEnd();
            break;
        case Qt::Key_Home:
	    m_formulaCursor->moveHome();
            break;
        default:
            if( event->text().length() == 0 )
                return;
            m_formulaCursor->insertText( event->text() );
            m_formulaShape->update();
    }

    event->accept();
}

void KoFormulaTool::keyReleaseEvent( QKeyEvent *event )
{
    event->accept();
}

void KoFormulaTool::remove( bool backSpace )
{
    Q_UNUSED( backSpace )
/*
    if( m_formulaCursor->hasSelection() )  // remove the selection
    {
	 // TODO set the cursor according to backSpace
//        m_formulaCursor->setCursorTo( );
    }
    else         */                         // remove only the current element
        m_formulaCursor->remove( backSpace );
}

void KoFormulaTool::insertAtCursor( BasicElement* element )
{
}

void KoFormulaTool::insertAtCursor( QList<BasicElement*> elements )
{
}

QWidget* KoFormulaTool::createOptionWidget()
{
    FormulaToolWidget* options = new FormulaToolWidget();
    options->setFormulaTool( this );
    return options;
}

KoFormulaShape* KoFormulaTool::shape()
{
    return m_formulaShape;
}

