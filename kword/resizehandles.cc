/* This file is part of the KDE project
   Copyright (C) 2000 Thomas Zander <zander@kde.org>

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

#include "resizehandles.h"
#include "kwdoc.h"
#include "kwcanvas.h"
#include "kwviewmode.h"
#include "kwframe.h"
#include <kdebug.h>

/******************************************************************/
/* Class: KWResizeHandle                                          */
/******************************************************************/

KWResizeHandle::KWResizeHandle( KWCanvas * p, Direction d, KWFrame *frm )
    : QWidget( p->viewport() ), m_canvas( p ), direction( d ), frame( frm )
{
    mousePressed = FALSE;
    setMouseTracking( TRUE );
    setBackgroundMode( PaletteHighlight );

    switch ( direction ) {
    case LeftUp:
        setCursor( Qt::sizeFDiagCursor );
        break;
    case Up:
        setCursor( Qt::sizeVerCursor );
        break;
    case RightUp:
        setCursor( Qt::sizeBDiagCursor );
        break;
    case Right:
        setCursor( Qt::sizeHorCursor );
        break;
    case RightDown:
        setCursor( Qt::sizeFDiagCursor );
        break;
    case Down:
        setCursor( Qt::sizeVerCursor );
        break;
    case LeftDown:
        setCursor( Qt::sizeBDiagCursor );
        break;
    case Left:
        setCursor( Qt::sizeHorCursor );
        break;
    }

    updateGeometry();
    show();
}

void KWResizeHandle::mouseMoveEvent( QMouseEvent *e )
{
    if ( !mousePressed )
        return;

    bool shiftPressed = e->state() & ShiftButton;
    switch ( direction ) {
    case LeftUp:
        m_canvas->mmEditFrameResize( true, false, true, false, shiftPressed );
        break;
    case Up:
        m_canvas->mmEditFrameResize( true, false, false, false, shiftPressed );
        break;
    case RightUp:
        m_canvas->mmEditFrameResize( true, false, false, true, shiftPressed );
        break;
    case Right:
        m_canvas->mmEditFrameResize( false, false, false, true, shiftPressed );
        break;
    case RightDown:
        m_canvas->mmEditFrameResize( false, true, false, true, shiftPressed );
        break;
    case Down:
        m_canvas->mmEditFrameResize( false, true, false, false, shiftPressed );
        break;
    case LeftDown:
        m_canvas->mmEditFrameResize( false, true, true, false, shiftPressed );
        break;
    case Left:
        m_canvas->mmEditFrameResize( false, false, true, false, shiftPressed );
        break;
    }
}

void KWResizeHandle::mousePressEvent( QMouseEvent *e )
{
    KWFrameSet *fs = 0;
    KWFrame *frm = 0;

    // Deselect all other frames
    KWDocument * doc = frame->getFrameSet()->kWordDocument();
    for ( unsigned int i = 0; i < doc->getNumFrameSets(); ++i ) {
        fs = doc->getFrameSet( i );
        for ( unsigned int j = 0; j < fs->getNumFrames(); ++j ) {
            frm = fs->getFrame( j );
            if ( frame->isSelected() && frm != frame )
                frm->setSelected( FALSE );
        }
    }

    mousePressed = true;
    oldX = e->x();
    oldY = e->y();
    QPoint vPoint( x() + e->x(), y() + e->y() );
    QPoint nPoint = m_canvas->viewMode()->viewToNormal( vPoint );
    m_canvas->mpEditFrame( 0, nPoint );
}

void KWResizeHandle::mouseReleaseEvent( QMouseEvent *e )
{
    mousePressed = false;
    QPoint vPoint( x() + e->x(), y() + e->y() );
    QPoint nPoint = m_canvas->viewMode()->viewToNormal( vPoint );
    m_canvas->mrEditFrame( e, nPoint );
}

void KWResizeHandle::updateGeometry()
{
    //KWDocument * doc = frame->getFrameSet()->kWordDocument();
    QRect newRect( frame->outerRect() );
    QRect frameRect( m_canvas->viewMode()->normalToView( newRect ) );
    switch ( direction ) {
    case LeftUp:
        m_canvas->moveChild( this, frameRect.x(), frameRect.y() );
        break;
    case Up:
        m_canvas->moveChild( this, frameRect.x() + frameRect.width() / 2 - 3, frameRect.y() );
        break;
    case RightUp:
        m_canvas->moveChild( this, frameRect.x() + frameRect.width() - 6, frameRect.y() );
        break;
    case Right:
        m_canvas->moveChild( this, frameRect.x() + frameRect.width() - 6,
                             frameRect.y() + frameRect.height() / 2 - 3 );
        break;
    case RightDown:
        m_canvas->moveChild( this, frameRect.x() + frameRect.width() - 6,
                             frameRect.y() + frameRect.height() - 6 );
        break;
    case Down:
        m_canvas->moveChild( this, frameRect.x() + frameRect.width() / 2 - 3,
                             frameRect.y() + frameRect.height() - 5 );
        break;
    case LeftDown:
        m_canvas->moveChild( this, frameRect.x(), frameRect.y() + frameRect.height() - 6 );
        break;
    case Left:
        m_canvas->moveChild( this,frameRect.x(), frameRect.y() + frameRect.height() / 2 - 3 );
        break;
    }
    resize( 6, 6 );
}

#include "resizehandles.moc"
