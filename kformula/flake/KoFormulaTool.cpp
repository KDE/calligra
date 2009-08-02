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
#include <KAction>
#include <QPainter>
#include <kdebug.h>

#include <QFile>
#include <KFileDialog>
#include <KoShapeSavingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfStylesReader.h>
#include "FormulaCommand.h"
#include "FormulaCommandUpdate.h"
#include <KoXmlReader.h>
#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoEmbeddedDocumentSaver.h>



KoFormulaTool::KoFormulaTool( KoCanvasBase* canvas ) : KoTool( canvas ),
                                                       m_formulaShape( 0 ),
                                                       m_formulaCursor( 0 )
{ 
    setupActions();
}

KoFormulaTool::~KoFormulaTool()
{
    if( m_formulaCursor ) {
        m_cursorList.removeAll(m_formulaCursor);
        delete m_formulaCursor;
    }
    foreach (FormulaCursor* tmp, m_cursorList) {
        delete tmp;
    }
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
    m_formulaCursor=0;
    for (int i = 0; i < m_cursorList.count(); i++) {
        FormulaCursor* cursor = m_cursorList[i];
        FormulaData* formulaData=m_formulaShape->formulaData();
        if (cursor->formulaData() == formulaData) {
            //we have to check if the cursors current element is actually a 
            //child of the m_formulaShape->formulaData()
            m_cursorList.removeAll(cursor);
            if (formulaData->formulaElement()->hasDescendant(cursor->currentElement())) {
                if (cursor->isAccepted()) {
                    kDebug()<<"Found old cursor";
                    m_formulaCursor=cursor;      
                    break;
                }
            }
            delete cursor;
        }
    }
    if (m_formulaCursor==0) {
        m_formulaCursor = new FormulaCursor( m_formulaShape->formulaData()->formulaElement(),
                                             m_formulaShape->formulaData());
    }
    connect(m_formulaShape->formulaData(), SIGNAL(dataChanged(FormulaCommand*,bool)), this, SLOT(updateCursor(FormulaCommand*,bool)));
}

void KoFormulaTool::deactivate()
{
    disconnect(m_formulaShape->formulaData(),0,this,0);
    if (m_canvas) {
        m_cursorList.append(m_formulaCursor);
        kDebug()<<"Appending cursor";
    }
    if (m_cursorList.count() > 20) { // don't let it grow indefinitely
        //TODO: is this save?
        delete m_cursorList[0];
        m_cursorList.removeAt(0);
    }
    m_formulaShape = 0;
}


void KoFormulaTool::updateCursor(FormulaCommand* command, bool undo)
{
    if (command!=0) {
        kDebug()<<"Going to change cursor";
        command->changeCursor(m_formulaCursor,undo);
    } else {
        kDebug()<<"Going to reset cursor";
        resetFormulaCursor();
    }
    repaintCursor();
}


void KoFormulaTool::paint( QPainter &painter, const KoViewConverter &converter )
{
    painter.save();
    // transform painter from view coordinate system to document coordinate system
    // remember that matrix multiplication is not commutative so painter.matrix
    // has to come last
    painter.setMatrix( m_formulaShape->absoluteTransformation( &converter ) * painter.matrix());
    KoShape::applyConversion(painter,converter);
    //TODO: find out, how to adjust the painter, so that that this also works in 
    // rotated mode
    
    m_formulaCursor->paint( painter );
    painter.restore();
}

void KoFormulaTool::repaintCursor()
{
    canvas()->updateCanvas( m_formulaShape->boundingRect() );
}

void KoFormulaTool::mousePressEvent( KoPointerEvent *event )
{
    // Check if the event is valid means inside the shape 
    if(!m_formulaShape->boundingRect().contains( event->point )) {
        return;
    }
    // transform the global coordinates into shape coordinates
    QPointF p = m_formulaShape->absoluteTransformation(0).inverted().map( event->point );
    if (event->modifiers() & Qt::ShiftModifier) {
        m_formulaCursor->setSelecting(true);
    } else {
        m_formulaCursor->setSelecting(false);
    }
    // set the cursor to the element the user clicked on
    m_formulaCursor->setCursorTo( p );

    repaintCursor();
    event->accept();
}

void KoFormulaTool::mouseDoubleClickEvent( KoPointerEvent *event )
{
    if( !m_formulaShape->boundingRect().contains( event->point ) ) {
        return;
    }
    // transform the global coordinates into shape coordinates
    QPointF p = m_formulaShape->absoluteTransformation(0).inverted().map( event->point );
    
    //clear the current selection
    m_formulaCursor->setSelecting(false);
    //place the cursor
    m_formulaCursor->setCursorTo(p);
    m_formulaCursor->selectElement(m_formulaCursor->currentElement());
    repaintCursor();
    event->accept();
}

void KoFormulaTool::mouseMoveEvent( KoPointerEvent *event )
{
//     Q_UNUSED( event )
    if (!(event->buttons() & Qt::LeftButton)) {
	return;
    }
    // Check if the event is valid means inside the shape
    if( !m_formulaShape->boundingRect().contains( event->point ) )
        kDebug() << "Getting most probably invalid mouseMoveEvent";
    
    // transform the global coordinates into shape coordinates
    QPointF p = m_formulaShape->absoluteTransformation(0).inverted().map( event->point );
    //TODO Implement drag and drop of elements
    m_formulaCursor->setSelecting(true);
    m_formulaCursor->setCursorTo( p );
    repaintCursor();
    event->accept();
}

void KoFormulaTool::mouseReleaseEvent( KoPointerEvent *event )
{
    Q_UNUSED( event )

    // TODO Implement drag and drop
}

void KoFormulaTool::keyPressEvent( QKeyEvent *event )
{
    FormulaCommand *command=0;
    if( !m_formulaCursor )
        return;

    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right || 
        event->key() == Qt::Key_Up || event->key() == Qt::Key_Down ||
        event->key() == Qt::Key_Home || event->key() == Qt::Key_End ) {
        if (event->modifiers() & Qt::ShiftModifier) {
            m_formulaCursor->setSelecting(true);
        } else {
            m_formulaCursor->setSelecting(false);
        }
    }
    switch( event->key() )                           // map key to movement or action
    {
        case Qt::Key_Backspace:
            m_formulaShape->update();
            command=m_formulaCursor->remove( true );;
            m_formulaShape->updateLayout();
            m_formulaShape->update();
            break;
        case Qt::Key_Delete:
            m_formulaShape->update();
            command=m_formulaCursor->remove( false );;
            m_formulaShape->updateLayout();
            m_formulaShape->update();
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
                command=m_formulaCursor->insertText( event->text() );
            }
    }
    if (command!=0) {
        m_canvas->addCommand(new FormulaCommandUpdate(m_formulaShape,command));
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
    m_formulaShape->update();
    m_formulaCursor->remove( backSpace );
    m_formulaShape->updateLayout();
    m_formulaShape->update();
}

void KoFormulaTool::insert( QAction* action )
{
    FormulaCommand *command;
    m_formulaShape->update();
    command=m_formulaCursor->insertData( action->data().toString() );
    if (command!=0) {
        m_canvas->addCommand(new FormulaCommandUpdate(m_formulaShape, command));
    }
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


FormulaCursor* KoFormulaTool::formulaCursor()
{
    return m_formulaCursor;
}


void KoFormulaTool::resetFormulaCursor() {
    m_formulaCursor->setData(m_formulaShape->formulaData());
    m_formulaCursor->setCurrentElement(m_formulaShape->formulaData()->formulaElement());
    m_formulaCursor->setPosition(0);
    m_formulaCursor->setSelecting(false);
    m_formulaCursor->setSelectionStart(0);
    //we don't know if this cursor is allowed there, so we move it right
    if ( !m_formulaCursor->isAccepted() ) {
        m_formulaCursor->move(MoveRight);
    }
}

void KoFormulaTool::loadFormula()
{
    // get an url
    KUrl url = KFileDialog::getOpenUrl();
    if( url.isEmpty() || !shape() )
        return;

    // open the file the url points to
    QFile file( url.path() );
    if( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
        return;

    KoOdfStylesReader stylesReader;
    KoOdfLoadingContext odfContext( stylesReader, 0 );
    QMap<QString, KoDataCenter *> dataCenterMap;
    KoShapeLoadingContext shapeContext( odfContext, dataCenterMap );

    // setup a DOM structure and start the actual loading process
    KoXmlDocument tmpDocument;
    tmpDocument.setContent( &file, false, 0, 0, 0 );
    FormulaElement* formulaElement = new FormulaElement();     // create a new root element
    formulaElement->readMathML( tmpDocument.documentElement() );     // and load the new formula
    FormulaCommand* command=new FormulaCommandLoad(m_formulaShape->formulaData(),formulaElement);
    m_canvas->addCommand(new FormulaCommandUpdate(m_formulaShape, command));
}

void KoFormulaTool::saveFormula()
{
    KUrl url = KFileDialog::getSaveUrl();
    if( url.isEmpty() || !shape() )
        return;

    QFile file( url.path() );
    KoXmlWriter writer( &file );
    KoGenStyles styles;
    KoEmbeddedDocumentSaver embeddedSaver;
    KoShapeSavingContext shapeSavingContext( writer, styles, embeddedSaver );

    // TODO this should not use saveOdf
    shape()->saveOdf( shapeSavingContext );
}

void KoFormulaTool::setupActions()
{
    KAction* action;

    action = new KAction( i18n( "Insert fence" ), this );
    action->setData( QString( "mfenced" ) ); 
    addAction( "insert_fence", action );

    action = new KAction( i18n( "Insert root" ), this );
    action->setData( QString( "mroot" ) ); 
    addAction( "insert_root", action );

    action = new KAction( i18n( "Insert square root" ), this );
    action->setData( QString( "msqrt" ) ); 
    addAction( "insert_sqrt", action );

    action = new KAction( i18n( "Insert fraction" ), this );
    action->setData( QString( "mfrac" ) ); 
    addAction( "insert_fraction", action );

    action = new KAction( i18n( "Insert 3x3 table" ), this );
    action->setData( QString( "mtable" ) ); 
    addAction( "insert_33table", action );

    action = new KAction( i18n( "Insert 2x2 table" ), this );
    action->setData( QString( "mtable" ) ); 
    addAction( "insert_22table", action );

    action = new KAction( i18n( "Insert 3 dimensional vector" ), this );
    action->setData( QString( "mtable" ) ); 
    addAction( "insert_31table", action );

    action = new KAction( i18n( "Insert 2 dimensional vector" ), this );
    action->setData( QString( "mtable" ) ); 
    addAction( "insert_21table", action );

    action = new KAction( i18n( "Insert table row" ), this );
    action->setData( QString( "mtr" ) ); 
    addAction( "insert_tablerow", action );

    action = new KAction( i18n( "Insert table column" ), this );
    action->setData( QString( "mtd" ) ); 
    addAction( "insert_tablecol", action );

    action = new KAction( i18n( "Insert subscript" ), this );
    action->setData( QString( "msub" ) ); 
    addAction( "insert_subscript", action );

    action = new KAction( i18n( "Insert superscript" ), this );
    action->setData( QString( "msup" ) ); 
    addAction( "insert_supscript", action );

    action = new KAction( i18n( "Insert sub- and superscript" ), this );
    action->setData( QString( "msubsup" ) ); 
    addAction( "insert_subsupscript", action );
}

