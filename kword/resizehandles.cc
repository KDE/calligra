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
    //kdDebug() << "KWResizeHandle::KWResizeHandle this=" << this << " frame=" << frm << endl;
    Q_ASSERT( frame );
    mousePressed = FALSE;
    setMouseTracking( TRUE );
    //setBackgroundMode( PaletteHighlight );
    if ( isResizingEnabled() )
    {
        applyCursorType();
    }

    updateGeometry();
    show();
}

KWResizeHandle::~KWResizeHandle()
{
    //kdDebug() << "KWResizeHandle::~KWResizeHandle " << this << endl;
}

void KWResizeHandle::applyCursorType()
{
    if ( frame->frameSet()->isProtectSize() )  {
        setCursor( Qt::forbiddenCursor );
        return;
    }
    switch ( direction )
    {
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
}

void KWResizeHandle::mouseMoveEvent( QMouseEvent *e )
{
    if ( !mousePressed || !m_canvas->kWordDocument()->isReadWrite() )
        return;
    if ( !isResizingEnabled() )
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
    if ( e->button() != LeftButton )
    {
        e->ignore(); // let KWCanvas handle it
        return;
    }
    KWFrameSet *fs = 0;
    KWFrame *frm = 0;

    // Deselect all other frames
    KWDocument * doc = frame->frameSet()->kWordDocument();
    for ( unsigned int i = 0; i < doc->numFrameSets(); ++i ) {
        fs = doc->frameSet( i );
        for ( unsigned int j = 0; j < fs->getNumFrames(); ++j ) {
            frm = fs->frame( j );
            if ( frame->isSelected() && frm != frame )
                frm->setSelected( FALSE );
        }
    }

    mousePressed = true;
    oldX = e->x();
    oldY = e->y();
    QPoint vPoint( x() + e->x(), y() + e->y() );
    QPoint nPoint = m_canvas->viewMode()->viewToNormal( vPoint );

    MouseMeaning meaning = doc->getMouseMeaning( nPoint, e->state() );
    Q_ASSERT( meaning >= MEANING_TOPLEFT ); // had be better be about resizing...
    if ( meaning < MEANING_TOPLEFT )
        kdDebug(32001) << "KWResizeHandle: got unexpected meaning " << meaning << endl;

    m_canvas->mpEditFrame( 0, nPoint, meaning );
}

void KWResizeHandle::mouseReleaseEvent( QMouseEvent *e )
{
    if ( e->button() != LeftButton )
    {
        e->ignore(); // let KWCanvas handle it
        return;
    }
    mousePressed = false;
    QPoint vPoint( x() + e->x(), y() + e->y() );
    QPoint nPoint = m_canvas->viewMode()->viewToNormal( vPoint );
    m_canvas->mrEditFrame( e, nPoint );
}

void KWResizeHandle::updateGeometry()
{
    //KWDocument * doc = frame->frameSet()->kWordDocument();
    QRect newRect( frame->outerRect( m_canvas->viewMode() ) );
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

bool KWResizeHandle::isResizingEnabled() const
{
    KWFrameSet *fs = frame->frameSet();
    if ( !fs )
    {
        kdWarning() << "KWResizeHandle: Frame has been deleted !  " << frame << endl;
        return false;
    }

    if ( fs->isMainFrameset() || fs->isProtectSize())
        return false;

    // Headers and footer are resizable only in some directions
    // and only if not in auto-resize mode
    if ( fs->isAHeader() &&
         ( frame->frameBehavior() == KWFrame::AutoExtendFrame ||
           ( direction != Down && direction != LeftDown && direction != RightDown ) ) )
        return false;

    if ( fs->isAFooter() &&
         ( frame->frameBehavior() == KWFrame::AutoExtendFrame ||
           ( direction != Up && direction != LeftUp && direction != RightUp ) ) )
        return false;

    if ( fs->isFootEndNote() &&
         ( frame->frameBehavior() == KWFrame::AutoExtendFrame ||
           ( direction != Up && direction != LeftUp && direction != RightUp ) ) )
        return false;

    return true;
}

void KWResizeHandle::paintEvent( QPaintEvent * )
{
    QPainter p;
    p.begin( this );
    if ( isResizingEnabled() )
        p.fillRect( 0, 0, 6, 6, colorGroup().brush( QColorGroup::Highlight ) );
    else
    {
        p.setPen( colorGroup().color( QColorGroup::Highlight ) );
        p.drawRect( 0, 0, 6, 6 );
        p.fillRect( 1, 1, 4, 4, colorGroup().brush( QColorGroup::Base ) );
    }
    p.end();
}

#include "resizehandles.moc"
