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
#include "FormulaToolWidget.h"
#include "BasicElement.h"
#include "FormulaCursor.h"
#include <KoCanvasBase.h>
#include <KoPointerEvent.h>
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
    setupActions();
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

void KoFormulaTool::paint( QPainter &painter, const KoViewConverter &converter )
{
    painter.setMatrix( painter.matrix() *
                       m_formulaShape->absoluteTransformation( &converter ) );
    double zoomX, zoomY;              // apply view conversions for painting
    converter.zoom(&zoomX, &zoomY);
    painter.scale(zoomX, zoomY);
    
    m_formulaCursor->paint( painter );
}

void KoFormulaTool::repaintCursor()
{
    canvas()->updateCanvas( m_formulaShape->boundingRect() );
}

void KoFormulaTool::mousePressEvent( KoPointerEvent *event )
{
    // Check if the event is valid means inside the shape
    if( !m_formulaShape->boundingRect().contains( event->point ) )
        return;

    // transform the global coordinates into shape coordinates
    QPointF p = m_formulaShape->absoluteTransformation(0).inverted().map( event->point );

    // set the cursor to element the user clicked to
    m_formulaCursor->setCursorTo( p );

    repaintCursor();
    event->accept();
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
            if( event->text().length() != 0 ) {
                m_formulaCursor->insertText( event->text() );
                m_formulaShape->update();
            }
    }
    repaintCursor();
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

void KoFormulaTool::insert( QAction* action )
{
    m_formulaCursor->insertData( action->data().toString() );
}

QWidget* KoFormulaTool::createOptionWidget()
{
    FormulaToolWidget* options = new FormulaToolWidget( this );
    options->setFormulaTool( this );
    return options;
}

KoFormulaShape* KoFormulaTool::shape()
{
    return m_formulaShape;
}

void KoFormulaTool::setupActions()
{
    QAction* action;

    action = new QAction( i18n( "Insert fence" ), this );
    action->setData( QString( "mfenced" ) ); 
    addAction( "insert_fence", action );

    action = new QAction( i18n( "Insert root" ), this );
    action->setData( QString( "mroot" ) ); 
    addAction( "insert_root", action );

    action = new QAction( i18n( "Insert square root" ), this );
    action->setData( QString( "msqrt" ) ); 
    addAction( "insert_sqrt", action );

    action = new QAction( i18n( "Insert fraction" ), this );
    action->setData( QString( "mfrac" ) ); 
    addAction( "insert_fraction", action );

    action = new QAction( i18n( "Insert 3x3 table" ), this );
    action->setData( QString( "mtable" ) ); 
    addAction( "insert_33table", action );

    action = new QAction( i18n( "Insert 2x2 table" ), this );
    action->setData( QString( "mtable" ) ); 
    addAction( "insert_22table", action );

    action = new QAction( i18n( "Insert 3 dimensional vector" ), this );
    action->setData( QString( "mtable" ) ); 
    addAction( "insert_31table", action );

    action = new QAction( i18n( "Insert 2 dimensional vector" ), this );
    action->setData( QString( "mtable" ) ); 
    addAction( "insert_21table", action );

    action = new QAction( i18n( "Insert table row" ), this );
    action->setData( QString( "mtr" ) ); 
    addAction( "insert_tablerow", action );

    action = new QAction( i18n( "Insert table column" ), this );
    action->setData( QString( "mtd" ) ); 
    addAction( "insert_tablecol", action );

    action = new QAction( i18n( "Insert subscript" ), this );
    action->setData( QString( "msub" ) ); 
    addAction( "insert_subscript", action );

    action = new QAction( i18n( "Insert superscript" ), this );
    action->setData( QString( "msup" ) ); 
    addAction( "insert_supscript", action );

    action = new QAction( i18n( "Insert sub- and superscript" ), this );
    action->setData( QString( "msubsup" ) ); 
    addAction( "insert_subsupscript", action );
}

