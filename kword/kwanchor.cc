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
#include "kwtextframeset.h"
#include "kwdoc.h"
#include "kwviewmode.h"
#include <kdebug.h>


KWAnchor::KWAnchor( KoTextDocument *textDocument, KWFrameSet * frameset, int frameNum )
    : KoTextCustomItem( textDocument),
      m_frameset( frameset ),
      m_frameNum( frameNum )
{
}

KWAnchor::~KWAnchor()
{
    kdDebug(32001) << "KWAnchor::~KWAnchor" << endl;
}

void KWAnchor::move( int x, int y )
{
    if ( m_deleted )
        return;

    // This test isn't enough. paragy may have changed. Or anything else
    // It's up to moveFloatingFrame to check if it really moved.
    //if ( x != xpos || y != ypos )

    int paragy = paragraph()->rect().y();
    xpos = x;
    ypos = y;
    kdDebug() << this << " KWAnchor::move " << x << "," << y << " paragy=" << paragy << endl;

    KWTextFrameSet * fs = static_cast<KWTextDocument *>(textDocument())->textFrameSet();
    KoPoint dPoint;
    if ( fs->internalToDocument( QPoint( x, y+paragy ), dPoint ) )
    {
        //kdDebug(32001) << "KWAnchor::move moving frame to [zoomed pos] " << nPoint.x() << "," << nPoint.y() << endl;
        // Move the frame to position nPoint.
        m_frameset->moveFloatingFrame( m_frameNum, dPoint );
    } else
    {
        // This can happen if the page hasn't been created yet
        kdDebug(32001) << "KWAnchor::move internalToDocument returned 0L for " << x << ", " << y+paragy << endl;
    }
}

//#define DEBUG_DRAWING

void KWAnchor::draw( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected )
{
    // (x,y) is the position of the inline item (in Layout Units)
    // (cx,cy,cw,ch) is the rectangle to be painted, in layout units too

    if ( m_deleted )
        return;

    if ( x != xpos || y != ypos ) { // shouldn't happen I guess ?
        kdDebug() << "rectifying position to " << x << "," << y << endl;
        move( x, y );
    }

    // The containing text-frameset.
    KWTextFrameSet * fs = static_cast<KWTextDocument *>(textDocument())->textFrameSet();
    KoZoomHandler* zh = fs->textDocument()->paintingZoomHandler();
    int paragy = paragraph()->rect().y();
#ifdef DEBUG_DRAWING
    kdDebug(32001) << "KWAnchor::draw x:" << x << ", y:" << y << " paragy=" << paragy
                   << "  cliprect(LU)" << DEBUGRECT( QRect( cx,cy,cw,ch ) ) << endl;
#endif

#if 0
    QPoint topLeftLU( QMAX(cx > 0 ? cx : 0,x), QMAX(y+paragy,-cy) );
    QPoint bottomRightLU( leftLU + QMIN(width,cw), paragy + QMIN(y + height, cy + ch) );
#ifdef DEBUG_DRAWING
    //kdDebug(32001) << "KWAnchor::draw x1: " << leftLU << ", y1: " << topLU << ", x2: " << rightLU << ", y2: " << bottomLU << endl;
#endif
#endif

    QRect crectLU;
    // Special case: QRichText calls us with (-1,-1,-1,-1) to mean "draw it all"
    if ( cx == -1 && cy+paragy == -1 && cw == -1 && ch == -1 )
        crectLU = QRect( x, y+paragy, width, height );
    else // otherwise just use the passed data as crect
        crectLU = QRect( cx > 0 ? cx : 0, cy+paragy, cw, ch );
#ifdef DEBUG_DRAWING
    kdDebug() << "KWAnchor::draw crect in LU coordinates:                   " << DEBUGRECT( crectLU ) << endl;
#endif

    KWFrame* containingFrame = fs->currentDrawnFrame();
    // Intersect with containing frame - in case the inline item is
    // bigger, it shouldn't go out of it!
    QRect frameRectLU( 0, containingFrame->internalY(),
                     zh->ptToLayoutUnitPixX( containingFrame->width() ),
                     zh->ptToLayoutUnitPixY( containingFrame->height() ) );
    crectLU &= frameRectLU;
#ifdef DEBUG_DRAWING
    kdDebug() << "KWAnchor::draw crect LU, after intersect with framerect : " << DEBUGRECT( crectLU ) << endl;
#endif

    QPoint topLeftLU = crectLU.topLeft();
    QPoint bottomRightLU = crectLU.bottomRight();

    // Convert crect to document coordinates, first topleft then bottomright
    KoPoint topLeftPt;
    if ( ! fs->internalToDocument( topLeftLU, topLeftPt ))
    {
        kdDebug() << "KWAnchor::paint Hmm? it seems my frame is positioned outside all text areas!!, aboring draw\n";
        return;
    }
    KoPoint bottomRightPt;
    if ( ! fs->internalToDocument( bottomRightLU, bottomRightPt ) )
    {
        kdWarning() << "internalToDocument returned 0L for bottomRightLU=" << bottomRightLU.x() << "," << bottomRightLU.y() << endl;
        return;
    }
    KoRect crectPt( topLeftPt, bottomRightPt );

    // Convert crect to view coords
    QRect crect = fs->currentViewMode()->normalToView( zh->zoomRect( crectPt ) );
#ifdef DEBUG_DRAWING
    kdDebug() << "KWAnchor::draw crect in view coordinates (pixel) : " << DEBUGRECT( crect ) << endl;
#endif

    // Ok, we finally have our crect in view coordinates!
    // Now ensure the containing frame is the one actually containing our text
    // (for copies, e.g. headers and footers, we need to go back until finding a real frame)

    if ( containingFrame->isCopy() )
    {
        // Find last real frame, in case we are in a copied frame
        QPtrListIterator<KWFrame> frameIt( fs->frameIterator() );
        frameIt.toLast(); // from the end to avoid a 2*N in the worst case
        while ( !frameIt.atFirst() && frameIt.current() != containingFrame ) // look for 'containingFrame'
            --frameIt;
        if ( frameIt.atFirst() && frameIt.current() != containingFrame )
            kdWarning() << "KWAnchor::draw: containingFrame not found " << containingFrame << endl;
        while ( !frameIt.atFirst() && frameIt.current()->isCopy() ) // go back to last non-copy
            --frameIt;
        containingFrame = frameIt.current();
        //kdDebug() << "KWAnchor::draw frame=" << containingFrame << endl;
    }

#if 0 // Why this code? Why draw from x=0 if the crect is positionned correctly? (DF)
    // left side from the containing frameset and we can get the top from the calculated point of our parag. (TZ)
    // ## Looks wrong to me. The translation can't depend on the crect.... (DF)
    QPoint topLeft( zh->zoomItX(containingFrame->x()), cnpoint.y() );
#endif
    QPoint topLeft = fs->currentViewMode()->normalToView( zh->zoomPoint( containingFrame->topLeft() ) );

    // Finally, make the painter go back to view coord system
    // (this is exactly the opposite of the code in KWFrameSet::drawContents)
    // (It does translate(view - internal), so we do translate(internal - view) - e.g. with (0,0) for internal)
    p->save();
    p->translate( -topLeft.x(), -topLeft.y() );
#ifdef DEBUG_DRAWING
    kdDebug() << "KWAnchor::draw translating by " << -topLeft.x() << "," << -topLeft.y() << endl;
#endif
    QColorGroup cg2( cg );
    m_frameset->drawContents( p, crect, cg2, false, true, 0L, fs->currentViewMode(), fs->currentDrawnCanvas() );

    if( selected && placement() == PlaceInline && p->device()->devType() != QInternal::Printer )
    {
        p->fillRect( crect, QBrush( cg.highlight(), QBrush::Dense4Pattern) );
    }

    p->restore();
#ifdef DEBUG_DRAWING
    kdDebug() << "KWAnchor::draw done" << endl;
#endif
}

QSize KWAnchor::size() const
{
    QSize sz = m_frameset->floatingFrameSize( m_frameNum );
    if ( sz.isNull() ) // for some reason, we don't know the size yet
        sz = QSize( width, height );
    // Convert to LU
    KoZoomHandler * zh = textDocument()->formattingZoomHandler();
    return QSize( zh->pixelToLayoutUnitX( sz.width() ), zh->pixelToLayoutUnitY( sz.height() ) );
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
        kdDebug(32001) << "KWAnchor::resize " << width << "x" << height << endl;
        Qt3::QTextParag * parag = paragraph();
        if ( parag )
        {
            kdDebug(32001) << "KWAnchor::resize invalidating parag " << parag->paragId() << endl;
            parag->invalidate( 0 );
        }
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
        m_frameset->setAnchored( static_cast<KWTextDocument *>(textDocument())->textFrameSet() );
    KoTextCustomItem::setDeleted( b );
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
