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

void KWAnchor::draw( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected )
{
    ASSERT( !m_deleted );
    if ( m_deleted ) // can't happen !
        return;

    if ( placement() != PlaceInline ) {
        x = xpos;
        y = ypos;
    }

    int paragy = paragraph()->rect().y();
    //kdDebug(32001) << "KWAnchor::draw " << x << "," << y << " paragy=" << paragy
    //               << "  " << DEBUGRECT( QRect( cx,cy,cw,ch ) ) << endl;
    KWDocument * doc = m_frameset->kWordDocument();
    KWTextFrameSet * fs = textDocument()->textFrameSet();

    // 1 - move frame. We have to do this here since QTextCustomItem doesn't
    // have a way to tell us when we are placed (during formatting)
    QPoint nPoint;
    if ( fs->internalToNormal( QPoint( x, y+paragy ), nPoint ) )
    {
        //kdDebug(32001) << "KWAnchor::draw moving frame to [zoomed pos] " << nPoint.x() << "," << nPoint.y() << endl;
        // Move the frame to position x,y.
        m_frameset->moveFloatingFrame( m_frameNum, KoPoint( nPoint.x() / doc->zoomedResolutionX(), nPoint.y() / doc->zoomedResolutionY() ) );
    }

    // 2 - draw

    p->save();
    // Determine crect in contents coords
    QRect crect( cx > 0 ? cx : 0, cy+paragy, cw, ch );
    QPoint cnPoint = crect.topLeft(); //fallback
    (void) fs->internalToNormal( crect.topLeft(), cnPoint );
    crect.moveTopLeft( fs->currentViewMode()->normalToView( cnPoint ) );
    // and go back to contents coord system
    QPoint iPoint( 0, paragy );
    if ( fs->internalToNormal( iPoint, cnPoint ) )
    {
        QPoint cPoint = fs->currentViewMode()->normalToView( cnPoint );
        //kdDebug(32002) << "translate " << -cPoint.x() << "," << -cPoint.y() << endl;
        p->translate( -cPoint.x(), -cPoint.y() );
    }
    // Draw the frame
    QColorGroup cg2( cg );
    m_frameset->drawContents( p, crect, cg2, false /*?*/, false /*?*/, 0L, fs->currentViewMode() );

    if ( selected && placement() == PlaceInline && p->device()->devType() != QInternal::Printer ) {
        QPoint vPoint = fs->currentViewMode()->normalToView( nPoint );
	p->fillRect( QRect( vPoint.x(), vPoint.y(), width, height ), QBrush( cg.highlight(), QBrush::Dense4Pattern) );
    }

    p->restore();
}

QSize KWAnchor::size() const
{
    KWDocument * doc = m_frameset->kWordDocument();
    return m_frameset->floatingFrameSize( m_frameNum );
}

void KWAnchor::resize()
{
    if ( m_deleted )
        return;
    QSize s = size();
    width = s.width();
    height = s.height();
    //kdDebug(32001) << "KWAnchor::resize " << width << "x" << height << endl;
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
