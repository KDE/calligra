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
    if ( m_deleted )
        return;

    KWTextFrameSet * fs = static_cast<KWTextDocument *>(textDocument())->textFrameSet();
    KoZoomHandler* zh = fs->textDocument()->paintingZoomHandler();
    int paragy = /*zh->layoutUnitToPixelY*/( paragraph()->rect().y() );
#ifdef DEBUG_DRAWING
    kdDebug(32001) << "KWAnchor::draw x:" << x << ", y:" << y << " paragy=" << paragy
                   << "  cliprect(LU)" << DEBUGRECT( QRect( cx,cy,cw,ch ) ) << endl;
#endif

    if ( x != xpos || y != ypos ) { // shouldn't happen I guess ?
        kdDebug() << "rectifying position to " << x << "," << y << endl;
        move( x, y );
    }

    p->save();
    // Determine crect in view coords

    // 1 - in internal (LU) coords
    QRect crect_lu;
    if ( cx == -1 && cy+paragy == -1 && cw == -1 && ch == -1 )
        crect_lu = QRect( x, y+paragy, width, height );
    else
        crect_lu = QRect( cx > 0 ? cx : 0, cy+paragy, cw, ch );
#ifdef DEBUG_DRAWING
    kdDebug() << "KWAnchor::draw crect ( in internal coords; LU ) = " << DEBUGRECT( crect_lu ) << endl;
#endif
    // 2 - convert to view coords, first topleft then bottomright
    QPoint cnPoint = crect_lu.topLeft(); //fallback
    KoPoint dPoint;
    if ( fs->internalToDocument( crect_lu.topLeft(), dPoint ) )
        cnPoint = zh->zoomPoint( dPoint );
    else
        kdDebug() << "KWAnchor::draw internalToNormal returned 0L for topLeft of crect!" << endl;
#ifdef DEBUG_DRAWING
    kdDebug() << "KWAnchor::draw cnPoint in normal coordinates " << cnPoint.x() << "," << cnPoint.y() << endl;
#endif
    cnPoint = fs->currentViewMode()->normalToView( cnPoint );
    //kdDebug() << "KWAnchor::draw cnPoint in view coordinates " << cnPoint.x() << "," << cnPoint.y() << endl;
    QRect crect;
    crect.setLeft( cnPoint.x() );
    crect.setTop( cnPoint.y() );
    QPoint brnPoint; // bottom right in normal coords
    if ( fs->internalToDocument( crect_lu.bottomRight(), dPoint ) )
    {
        brnPoint = zh->zoomPoint( dPoint );
#ifdef DEBUG_DRAWING
        kdDebug() << "KWAnchor::draw brnPoint in normal coordinates " << brnPoint.x() << "," << brnPoint.y() << endl;
#endif
        brnPoint = fs->currentViewMode()->normalToView( brnPoint );
        crect.setRight( brnPoint.x() );
        crect.setBottom( brnPoint.y() );
    }
    else
        kdWarning() << "internalToNormal returned 0L for bottomRight=" << crect.right() << "," << crect.bottom() << endl;
#ifdef DEBUG_DRAWING
    kdDebug() << "KWAnchor::draw crect (in view coords) = " << DEBUGRECT( crect ) << endl;
#endif

    KWFrame *frame = fs->currentDrawnFrame();
    if ( frame->isCopy() )
    {
        // Find last real frame, in case we are in a copied frame
        QPtrListIterator<KWFrame> frameIt( fs->frameIterator() );
        frameIt.toLast(); // from the end to avoid a 2*N in the worst case
        while ( !frameIt.atFirst() && frameIt.current() != frame ) // look for 'frame'
            --frameIt;
        if ( frameIt.atFirst() && frameIt.current() != frame )
            kdWarning() << "KWAnchor::draw: frame not found " << frame << endl;
        while ( !frameIt.atFirst() && frameIt.current()->isCopy() ) // go back to last non-copy
            --frameIt;
        frame = frameIt.current();
        //kdDebug() << "KWAnchor::draw frame=" << frame << endl;
    }

    // and make painter go back to view coord system
    // (this is exactly the opposite of the code in KWFrameSet::drawContents)
    // (It does translate(view - internal), so we do translate(internal - view))

    // The amount we have to move depends on the frame we are positioned in, as we don't know this
    // lets find out via our containting frameset.
    KoPoint topLeft = KoPoint(
        zh->unzoomItX(m_frameset->frame(m_frameNum)->outerRect().x()) + 2, // we add one to x and y to 
        zh->unzoomItY(m_frameset->frame(m_frameNum)->outerRect().y()) + 2);// compensate for rounding errors.

    QPoint dud;
    KWFrame *containingFrame=fs->documentToInternal(topLeft, dud, false);

    if(containingFrame==0L) {
        kdDebug() << "KWAnchor::paint Hmm? it seems my frame is positioned outside all text areas!!, aboring draw\n";
        return;
    }
    // left side from the containing frameset and we can get the top from the m_frameset frame.
    topLeft=zh->zoomPoint(containingFrame->topLeft());

#ifdef DEBUG_DRAWING
    kdDebug() << "KWAnchor::draw translating by " << 0-topLeft.x() << "," << -1-m_frameset->frame(m_frameNum)->outerRect().y() << endl;
#endif
    p->translate( 0-topLeft.x(), -1 - m_frameset->frame(m_frameNum)->outerRect().y());
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
