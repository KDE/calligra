/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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

#include "kwanchor.h"
#include "kwcommand.h"
#include "kwtextdocument.h"
#include "kwtextframeset.h"
#include "kwdoc.h"
#include "kwviewmode.h"
#include <kdebug.h>
#include <qdom.h>

KWAnchor::KWAnchor( KWTextDocument *textdoc, KWFrameSet * frameset, int frameNum )
    : KWTextCustomItem( textdoc ),
      m_frameset( frameset ),
      m_frameNum( frameNum )
{
}

KWAnchor::~KWAnchor()
{
    kdDebug() << "KWAnchor::~KWAnchor" << endl;
}

void KWAnchor::move( int x, int y )
{
    // This test isn't enough. paragy may have changed. Or anything else
    // It's up to moveFloatingFrame to check if it really moved.
    //if ( x != xpos || y != ypos )

    int paragy = paragraph()->rect().y();
    xpos = x;
    ypos = y;
    kdDebug() << this << " KWAnchor::move " << x << "," << y << " paragy=" << paragy << endl;
    KWTextFrameSet * fs = textDocument()->textFrameSet();
    QPoint nPoint;
    if ( fs->internalToNormal( QPoint( x, y+paragy ), nPoint ) )
    {
        //kdDebug(32001) << "KWAnchor::move moving frame to [zoomed pos] " << nPoint.x() << "," << nPoint.y() << endl;
        // Move the frame to position nPoint.
        m_frameset->moveFloatingFrame( m_frameNum, nPoint );
    } else
    {
        // This can happen if the page hasn't been created yet
        kdDebug(32001) << "KWAnchor::move internalToNormal returned 0L for " << x << ", " << y+paragy << endl;
    }
}

void KWAnchor::draw( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected )
{
    ASSERT( !m_deleted );
    if ( m_deleted ) // can't happen !
        return;

    int paragy = paragraph()->rect().y();
    //kdDebug(32001) << "KWAnchor::draw " << x << "," << y << " paragy=" << paragy
    //               << "  " << DEBUGRECT( QRect( cx,cy,cw,ch ) ) << endl;
    KWTextFrameSet * fs = textDocument()->textFrameSet();

    if ( x != xpos || y != ypos ) { // shouldn't happen I guess ?
        kdDebug() << "rectifying position to " << x << "," << y << endl;
        move( x, y );
    }

    p->save();
    // Determine crect in view coords
    QRect crect;
    if ( cx == -1 && cy+paragy == -1 && cw == -1 && ch == -1 )
        crect = QRect( x, y+paragy, width, height );
    else
        crect = QRect( cx > 0 ? cx : 0, cy+paragy, cw, ch );
    //kdDebug() << "KWAnchor::draw crect ( in internal coords ) = " << DEBUGRECT( crect ) << endl;
    QPoint cnPoint = crect.topLeft(); //fallback
    (void) fs->internalToNormal( crect.topLeft(), cnPoint );
    //kdDebug() << "KWAnchor::draw cnPoint " << cnPoint.x() << "," << cnPoint.y() << endl;
    crect.moveTopLeft( fs->currentViewMode()->normalToView( cnPoint ) );
    QPoint brnPoint; // bottom right in normal coords
    if ( fs->internalToNormal( crect.bottomRight(), brnPoint ) )
    {
        brnPoint = fs->currentViewMode()->normalToView( brnPoint );
        crect.setRight( brnPoint.x() );
        crect.setBottom( brnPoint.y() );
    }
    //kdDebug() << "KWAnchor::draw crect ( in view coords ) = " << DEBUGRECT( crect ) << endl;

    // and make painter go back to view coord system
    // (this is exactly the opposite of the code in KWFrameSet::drawContents)
    QPoint frameTopLeft = fs->kWordDocument()->zoomPoint( fs->currentDrawnFrame()->topLeft() );
    QPoint iPoint;
    if ( fs->normalToInternal( frameTopLeft, iPoint ) )
    {
        QPoint vPoint = fs->currentViewMode()->normalToView( frameTopLeft );
        p->translate( iPoint.x() - vPoint.x(), iPoint.y() - vPoint.y() - paragy );
    } else
        kdWarning() << "normalToInternal returned 0L in KWAnchor::draw - shouldn't happen. "
                    << frameTopLeft.x() << "," << frameTopLeft.y() << endl;
    // Draw the frame
    QColorGroup cg2( cg );
    m_frameset->drawContents( p, crect, cg2, false, true, 0L, fs->currentViewMode(), fs->currentDrawnCanvas() );

    if ( selected && placement() == PlaceInline && p->device()->devType() != QInternal::Printer ) {
        QPoint nPoint;
        if ( fs->internalToNormal( QPoint( x, y+paragy ), nPoint ) )
        {
            QPoint vPoint = fs->currentViewMode()->normalToView( nPoint );
            p->fillRect( QRect( vPoint.x(), vPoint.y(), width, height ), QBrush( cg.highlight(), QBrush::Dense4Pattern) );
        }
    }

    p->restore();
    //kdDebug() << "KWAnchor::draw done" << endl;
}

QSize KWAnchor::size() const
{
    return m_frameset->floatingFrameSize( m_frameNum );
}


int KWAnchor::ascent() const
{
    int baseline = m_frameset->floatingFrameBaseline( m_frameNum );
    return ( baseline == -1 ) ? height : baseline;
}

void KWAnchor::resize()
{
    if ( m_deleted )
        return;
    QSize s = size();
    if ( width != s.width() || height != s.height() )
    {
        width = s.width();
        height = s.height();
        //kdDebug(32001) << "KWAnchor::resize " << width << "x" << height << endl;
        QTextParag * parag = paragraph();
        if ( parag )
            parag->invalidate( 0 );
    }
}

KCommand * KWAnchor::createCommand()
{
    kdDebug(32001) << "KWAnchor::addCreateCommand" << endl;
    return m_frameset->anchoredObjectCreateCommand( m_frameNum );
}

KCommand * KWAnchor::deleteCommand()
{
    kdDebug(32001) << "KWAnchor::addDeleteCommand" << endl;
    return m_frameset->anchoredObjectDeleteCommand( m_frameNum );
}

void KWAnchor::setDeleted( bool b )
{
    kdDebug() << "KWAnchor::setDeleted " << b << endl;
    if ( b )
        m_frameset->setAnchored( 0L );
    else
        m_frameset->setAnchored( textDocument()->textFrameSet() );
    KWTextCustomItem::setDeleted( b );
}

void KWAnchor::save( QDomElement &formatElem )
{
    formatElem.setAttribute( "id", 6 ); // code for an anchor
    QDomElement anchorElem = formatElem.ownerDocument().createElement( "ANCHOR" );
    formatElem.appendChild( anchorElem );
    anchorElem.setAttribute( "type", "frameset" ); // the only possible value currently
    //KWDocument * doc = textDocument()->textFrameSet()->kWordDocument();
    // ## TODO save the frame number as well ? Only the first frame ? to be determined
    // ## or maybe use len=<number of frames>. Difficult :}
    anchorElem.setAttribute( "instance", m_frameset->getName() );
}

bool KWAnchor::ownLine() const
{
    return false;
    //commented out, since it prevents multiple tables on the same line, alignment etc.
    //return m_frameset->type() == FT_TABLE;
}
