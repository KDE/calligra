/* This file is part of the KDE project
   Copyright (C) 2006-2007 Thorsten Zachmann <zachmann@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#include "KoPACanvas.h"

#include <KoShapeManager.h>
#include <KoToolProxy.h>
#include <KoUnit.h>

#include "Document.h"
#include "KoPAView.h"
#include "../Section.h"

#include <kxmlguifactory.h>

#include <KAction>
#include <QMenu>
#include <QMouseEvent>

KoPACanvas::KoPACanvas( KoPAView * view, Document * doc )
: QWidget( view )
, KoCanvasBase( doc )
, m_view( view )
, m_doc( doc )
{
    m_shapeManager = new KoShapeManager( this );
    m_toolProxy = new KoToolProxy( this );
    setFocusPolicy( Qt::StrongFocus );
    // this is much faster than painting it in the paintevent
    setBackgroundRole( QPalette::Base );
    setAutoFillBackground( true );
    setAttribute(Qt::WA_InputMethodEnabled, true);
}

KoPACanvas::~KoPACanvas()
{
    delete m_toolProxy;
    delete m_shapeManager;
}

void KoPACanvas::setDocumentOffset(const QPoint &offset) {
    m_documentOffset = offset;
}

void KoPACanvas::gridSize( qreal *horizontal, qreal *vertical ) const
{
    *horizontal = m_doc->gridData().gridX();
    *vertical = m_doc->gridData().gridY();
}

bool KoPACanvas::snapToGrid() const
{
    return m_doc->gridData().snapToGrid();
}

void KoPACanvas::addCommand( QUndoCommand *command )
{
    m_doc->addCommand( command );
}

KoShapeManager * KoPACanvas::shapeManager() const
{
    return m_shapeManager;
}

void KoPACanvas::updateCanvas( const QRectF& rc )
{
    QRect clipRect( viewConverter()->documentToView( rc ).toRect() );
    clipRect.adjust( -2, -2, 2, 2 ); // Resize to fit anti-aliasing
    clipRect.moveTopLeft( clipRect.topLeft() - m_documentOffset);
    update( clipRect );

    emit canvasUpdated();
}

const KoViewConverter * KoPACanvas::viewConverter() const
{
    return m_view->viewConverter( const_cast<KoPACanvas *>( this ) );
}

KoUnit KoPACanvas::unit() const
{
    return m_doc->unit();
}

const QPoint & KoPACanvas::documentOffset() const
{
    return m_documentOffset;
}

void KoPACanvas::paintEvent( QPaintEvent *event )
{
    QPainter painter( this );
    painter.translate( -documentOffset() );
    painter.setRenderHint( QPainter::Antialiasing );
    QRectF clipRect = event->rect().translated( documentOffset() );
    painter.setClipRect( clipRect );

    KoViewConverter * converter = m_view->viewConverter( this );
    shapeManager()->paint( painter, *converter, false );
    painter.setRenderHint( QPainter::Antialiasing, false );

    QRectF updateRect = converter->viewToDocument( clipRect );
    document()->gridData().paintGrid( painter, *converter, updateRect );
    document()->guidesData().paintGuides( painter, *converter, updateRect );

    painter.setRenderHint( QPainter::Antialiasing );
    m_toolProxy->paint( painter, *converter );
    
}

void KoPACanvas::tabletEvent( QTabletEvent *event )
{
    m_toolProxy->tabletEvent( event, viewConverter()->viewToDocument( event->pos() + m_documentOffset ) );
}

void KoPACanvas::mousePressEvent( QMouseEvent *event )
{
    m_toolProxy->mousePressEvent( event, viewConverter()->viewToDocument( event->pos() + m_documentOffset ) );

    if(!event->isAccepted() && event->button() == Qt::RightButton)
    {
        showContextMenu( event->globalPos(), toolProxy()->popupActionList() );
        event->setAccepted( true );
    }
}

void KoPACanvas::mouseDoubleClickEvent( QMouseEvent *event )
{
    m_toolProxy->mouseDoubleClickEvent( event, viewConverter()->viewToDocument( event->pos() + m_documentOffset ) );
}

void KoPACanvas::mouseMoveEvent( QMouseEvent *event )
{
    m_toolProxy->mouseMoveEvent( event, viewConverter()->viewToDocument( event->pos() + m_documentOffset ) );
}

void KoPACanvas::mouseReleaseEvent( QMouseEvent *event )
{
    m_toolProxy->mouseReleaseEvent( event, viewConverter()->viewToDocument( event->pos() + m_documentOffset ) );
}

void KoPACanvas::keyPressEvent( QKeyEvent *event )
{
    m_toolProxy->keyPressEvent( event );

    if ( ! event->isAccepted() ) {
        event->accept();

        switch ( event->key() )
        {
            case Qt::Key_Home:
                m_view->navigatePage( KoPageApp::PageFirst );
                break;
            case Qt::Key_PageUp:
                m_view->navigatePage( KoPageApp::PagePrevious );
                break;
            case Qt::Key_PageDown:
                m_view->navigatePage( KoPageApp::PageNext );
                break;
            case Qt::Key_End:
                m_view->navigatePage( KoPageApp::PageLast );
                break;
            default:
                event->ignore();
                break;
        }
    }
    if (! event->isAccepted()) {
        if (event->key() == Qt::Key_Backtab
                || (event->key() == Qt::Key_Tab && (event->modifiers() & Qt::ShiftModifier)))
            focusNextPrevChild(false);
        else if (event->key() == Qt::Key_Tab)
            focusNextPrevChild(true);
    }
}

void KoPACanvas::keyReleaseEvent( QKeyEvent *event )
{
    m_toolProxy->keyReleaseEvent( event );
}

void KoPACanvas::wheelEvent ( QWheelEvent * event )
{
    m_toolProxy->wheelEvent( event, viewConverter()->viewToDocument( event->pos() + m_documentOffset ) );
}

void KoPACanvas::closeEvent( QCloseEvent * event )
{
    event->ignore();
}

void KoPACanvas::updateInputMethodInfo()
{
    updateMicroFocus();
}

QVariant KoPACanvas::inputMethodQuery(Qt::InputMethodQuery query) const
{
    return m_toolProxy->inputMethodQuery(query, *(viewConverter()) );
}

void KoPACanvas::inputMethodEvent(QInputMethodEvent *event)
{
    m_toolProxy->inputMethodEvent(event);
}

void KoPACanvas::resizeEvent( QResizeEvent * event )
{
    emit sizeChanged( event->size() );
}

void KoPACanvas::showContextMenu( const QPoint& globalPos, const QList<QAction*>& actionList )
{
    m_view->unplugActionList( "toolproxy_action_list" );
    m_view->plugActionList( "toolproxy_action_list", actionList );
    QMenu *menu = dynamic_cast<QMenu*>( m_view->factory()->container( "default_canvas_popup", m_view ) );

    if( menu )
        menu->exec( globalPos );
}

KoGuidesData * KoPACanvas::guidesData()
{
    return &m_doc->guidesData();
}

#include "KoPACanvas.moc"
