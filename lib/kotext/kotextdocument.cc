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

#include "kotextdocument.h"
#include "kozoomhandler.h"
#include "kotextformat.h"
#include "kotextparag.h"
#include "kotextformatter.h"
#include <kdebug.h>

KoTextDocument::KoTextDocument( KoZoomHandler * zoomHandler, QTextDocument *p, KoTextFormatCollection *fc )
    : QTextDocument( p, fc ), m_zoomHandler( zoomHandler ), m_bDestroying( false )
{
    setAddMargins( true );                 // top margin and bottom are added, not max'ed
    QTextFormatter * formatter = new KoTextFormatter;
    formatter->setAllowBreakInWords( true ); // Necessary for lines without a single space
    setFormatter( formatter );

    setY( 0 );
    setLeftMargin( 0 );
    setRightMargin( 0 );
    ko_buf_pixmap = 0;

    // TODO clear(true) here if kpresenter uses kotextdocument directly,
    // otherwise in the derived constructor [virtual method call is the problem]
}

KoTextDocument::~KoTextDocument()
{
    m_bDestroying = true;
    delete ko_buf_pixmap;
    clear( false );
}

QTextParag * KoTextDocument::createParag( QTextDocument *d, QTextParag *pr, QTextParag *nx, bool updateIds )
{
    return new KoTextParag( d, pr, nx, updateIds );
}

bool KoTextDocument::visitSelection( int selectionId, KoParagVisitor* visitor, bool forward )
{
    QTextCursor c1 = selectionStartCursor( selectionId );
    QTextCursor c2 = selectionEndCursor( selectionId );
    if ( c1 == c2 )
        return true;
    return visitFromTo( c1.parag(), c1.index(), c2.parag(), c2.index(), visitor, forward );
}

bool KoTextDocument::visitDocument( KoParagVisitor *visitor, bool forward )
{
    return visitFromTo( firstParag(), 0, lastParag(), lastParag()->length()-1, visitor, forward );
}

bool KoTextDocument::visitFromTo( QTextParag *firstParag, int firstIndex, QTextParag* lastParag, int lastIndex, KoParagVisitor* visitor, bool forw )
{
    if ( firstParag == lastParag )
    {
        return visitor->visit( firstParag, firstIndex, lastIndex );
    }
    else
    {
        bool ret = true;
        if ( forw )
        {
            // the -1 is for the trailing space
            ret = visitor->visit( firstParag, firstIndex, firstParag->length() - 1 );
            if (!ret) return false;
        }
        else
        {
            ret = visitor->visit( lastParag, 0, lastIndex );
            if (!ret) return false;
        }

        QTextParag* currentParag = forw ? firstParag->next() : lastParag->prev();
        QTextParag * endParag = forw ? lastParag : firstParag;
        while ( currentParag && currentParag != endParag )
        {
            ret = visitor->visit( currentParag, 0, currentParag->length() - 1 );
            if (!ret) return false;
            currentParag = forw ? currentParag->next() : currentParag->prev();
        }
        ASSERT( currentParag );
        ASSERT( endParag == currentParag );
        if ( forw )
            ret = visitor->visit( lastParag, 0, lastIndex );
        else
            ret = visitor->visit( currentParag, firstIndex, currentParag->length() - 1 );
        return ret;
    }
}

// SYNC start - from here until end mark, modified copies of QTextDocument methods
static bool is_printer( QPainter *p )
{
    return p && p->device() && p->device()->devType() == QInternal::Printer;
}

QPixmap *KoTextDocument::bufferPixmap( const QSize &s )
{
    if ( !ko_buf_pixmap ) {
	ko_buf_pixmap = new QPixmap( s );
    } else {
	if ( ko_buf_pixmap->width() < s.width() ||
	     ko_buf_pixmap->height() < s.height() ) {
	    ko_buf_pixmap->resize( QMAX( s.width(), ko_buf_pixmap->width() ),
				QMAX( s.height(), ko_buf_pixmap->height() ) );
	}
    }

    return ko_buf_pixmap;
}

void KoTextDocument::drawWithoutDoubleBuffer( QPainter *p, const QRect &cr, const QColorGroup &cg, const QBrush *paper )
{
    if ( !firstParag() )
	return;

    if ( paper ) {
//QT2HACK
//	p->setBrushOrigin( -(int)p->translationX(),
//			   -(int)p->translationY() );
	p->setBrushOrigin( -(int)p->worldMatrix().dx(),
			   -(int)p->worldMatrix().dy() );
	p->fillRect( cr, *paper );
    }

    QTextParag *parag = firstParag();
    while ( parag ) {
	if ( !parag->isValid() )
	    parag->format();

	QRect pr( 0, m_zoomHandler->layoutUnitToPixelY( parag->rect().y() ),
                  QWIDGETSIZE_MAX, m_zoomHandler->layoutUnitToPixelY( parag->rect().height() ) );
	if ( !cr.isNull() && !cr.intersects( pr ) ) {
	    parag = parag->next();
	    continue;
	}
	p->translate( 0, m_zoomHandler->layoutUnitToPixelY( pr.y() ) );
	parag->paint( *p, cg, 0, FALSE );
	p->translate( 0, -m_zoomHandler->layoutUnitToPixelY( pr.y() ) );
	parag = parag->next();
    }
}

void KoTextDocument::drawParagWYSIWYG( QPainter *p, QTextParag *parag, int cx, int cy, int cw, int ch,
                                       QPixmap *&doubleBuffer, const QColorGroup &cg,
                                       bool drawCursor, QTextCursor *cursor, bool resetChanged )
{
    //qDebug( "drawParagWYSIWYG %p %d", parag, parag->paragId() );
    QPainter *painter = 0;
    if ( resetChanged )
	parag->setChanged( FALSE );
    QRect rect( m_zoomHandler->layoutUnitToPixel( parag->rect() ) );
    QRect ir( rect );
    //kdDebug() << "KWTextDocument::drawParagWYSIWYG parag->rect=" << DEBUGRECT( parag->rect() )
    //          << " ir=" << DEBUGRECT(ir) << endl;
    bool useDoubleBuffer = !parag->document()->parent();
    // no parent docs in kword (and no access to nextDoubleBuffered)
    //if ( !useDoubleBuffer && parag->document()->nextDoubleBuffered )
    //	useDoubleBuffer = TRUE;
    if ( p->device()->devType() == QInternal::Printer )
	useDoubleBuffer = FALSE;

    if ( useDoubleBuffer  ) {
	painter = new QPainter;
	if ( cx >= 0 && cy >= 0 )
	    ir = ir.intersect( QRect( cx, cy, cw, ch ) );
	if ( !doubleBuffer ||
	     ir.width() > doubleBuffer->width() ||
	     ir.height() > doubleBuffer->height() ) {
	    doubleBuffer = bufferPixmap( ir.size() );
	    painter->begin( doubleBuffer );
	} else {
	    painter->begin( doubleBuffer );
	}
    } else {
	painter = p;
	painter->translate( ir.x(), ir.y() );
    }

    painter->setBrushOrigin( -ir.x(), -ir.y() );

    if ( useDoubleBuffer || is_printer( painter ) ) {
	if ( !parag->backgroundColor() )
	    painter->fillRect( QRect( 0, 0, ir.width(), ir.height() ),
			       cg.brush( QColorGroup::Base ) );
	else
	    painter->fillRect( QRect( 0, 0, ir.width(), ir.height() ),
			       *parag->backgroundColor() );
    } else {
	if ( cursor && cursor->parag() == parag ) {
	    if ( !parag->backgroundColor() )
		painter->fillRect( QRect( m_zoomHandler->layoutUnitToPixelX( parag->at( cursor->index() )->x ), 0, 2, ir.height() ),
				   cg.brush( QColorGroup::Base ) );
	    else
		painter->fillRect( QRect( m_zoomHandler->layoutUnitToPixelX( parag->at( cursor->index() )->x ), 0, 2, ir.height() ),
				   *parag->backgroundColor() );
	}
    }

    painter->translate( -( ir.x() - m_zoomHandler->layoutUnitToPixelX( parag->rect().x() ) ),
		       -( ir.y() - m_zoomHandler->layoutUnitToPixelY( parag->rect().y() ) ) );

    // The cliprect is checked in layout units, in QTextParag::paint
    QRect lu_crect( m_zoomHandler->pixelToLayoutUnit( QRect( cx, cy, cw, ch ) ) );
    parag->paint( *painter, cg, drawCursor ? cursor : 0, TRUE,
                  lu_crect.x(), lu_crect.y(), lu_crect.width(), lu_crect.height() );

/*  if ( !flow()->isEmpty() ) {
	painter->translate( 0, -parag->rect().y() );
	QRect cr( cx, cy, cw, ch );
	cr = cr.intersect( QRect( 0, parag->rect().y(), parag->rect().width(), parag->rect().height() ) );
	flow()->drawFloatingItems( painter, cr.x(), cr.y(), cr.width(), cr.height(), cg, FALSE );
	painter->translate( 0, +parag->rect().y() );
    } */

    if ( useDoubleBuffer ) {
	delete painter;
	painter = 0;
	p->drawPixmap( ir.topLeft(), *doubleBuffer, QRect( QPoint( 0, 0 ), ir.size() ) );
    } else {
	painter->translate( -ir.x(), -ir.y() );
    }

    int docright = m_zoomHandler->layoutUnitToPixelX( parag->document()->x() + parag->document()->width() );
    if ( rect.x() + rect.width() < docright ) {
	p->fillRect( rect.x() + rect.width(), rect.y(),
		     docright - ( rect.x() + rect.width() ),
		     rect.height(), cg.brush( QColorGroup::Base ) );
    }

    //if ( verticalBreak() && parag->lastInFrame && parag->document()->flow() )
    //    parag->document()->flow()->eraseAfter( parag, p, cg );

    //parag->document()->nextDoubleBuffered = FALSE;
}

QTextParag *KoTextDocument::drawWYSIWYG( QPainter *p, int cx, int cy, int cw, int ch, const QColorGroup &cg,
				 bool onlyChanged, bool drawCursor, QTextCursor *cursor, bool resetChanged )
{
    if ( isWithoutDoubleBuffer() /* || par && par->withoutDoubleBuffer */ ) {
	//setWithoutDoubleBuffer( TRUE );
	QRect crect( cx, cy, cw, ch );
	drawWithoutDoubleBuffer( p, crect, cg );
	return 0;
    }
    setWithoutDoubleBuffer( FALSE );

    if ( !firstParag() )
	return 0;

    //if ( drawCursor && cursor )
    //    tmpCursor = cursor;
/*    if ( cx < 0 && cy < 0 ) {
	cx = 0;
	cy = 0;
	cw = width();
	ch = height();
        }*/

    QTextParag *lastFormatted = 0;
    QTextParag *parag = firstParag();

    QPixmap *doubleBuffer = 0;
    QPainter painter;
    QRect crect( cx, cy, cw, ch );
    //kdDebug() << "KWTextDocument::drawKW crect=" << DEBUGRECT(crect) << endl;

    // Space above first parag
    int paragy = m_zoomHandler->layoutUnitToPixelY( parag->rect().y() );
    if ( parag && cy <= paragy && paragy > 0 ) {
        QRect r( 0, 0,
                 m_zoomHandler->layoutUnitToPixelX( parag->document()->x() + parag->document()->width() ),
                 paragy );
        r &= crect;
        if ( !r.isEmpty() )
            p->fillRect( r, cg.brush( QColorGroup::Base ) );
    }

    while ( parag ) {
	lastFormatted = parag;
	if ( !parag->isValid() )
	    parag->format();

	QRect ir = m_zoomHandler->layoutUnitToPixel( parag->rect() );
        //kdDebug() << "KWTextDocument::drawKW ir=" << DEBUGRECT(ir) << endl;
	if ( verticalBreak() && parag->next() )
	    if ( ir.y() + ir.height() < m_zoomHandler->layoutUnitToPixelY( parag->next()->rect().y() ) ) {
		QRect r( 0, ir.y() + ir.height(),
			 m_zoomHandler->layoutUnitToPixelX( parag->document()->x() + parag->document()->width() ),
			 m_zoomHandler->layoutUnitToPixelY( parag->next()->rect().y() ) - ( ir.y() + ir.height() ) );
		r &= crect;
		if ( !r.isEmpty() )
		    p->fillRect( r, cg.brush( QColorGroup::Base ) );
	    }

	if ( !ir.intersects( crect ) ) {
	    ir.setWidth( m_zoomHandler->layoutUnitToPixelX( parag->document()->width() ) );
	    if ( ir.intersects( crect ) )
		p->fillRect( ir.intersect( crect ), cg.brush( QColorGroup::Base ) );
	    if ( ir.y() > cy + ch ) {
		//tmpCursor = 0;
		if ( ko_buf_pixmap && ko_buf_pixmap->height() > 300 ) {
		    delete ko_buf_pixmap;
		    ko_buf_pixmap = 0;
		}
		//if ( verticalBreak() && flow() )
		//    flow()->draw( p, cx, cy, cw, ch );

		return lastFormatted;
	    }
	    parag = parag->next();
	    continue;
	}

	if ( !parag->hasChanged() && onlyChanged ) {
	    //qDebug( "skipping unchanged parag %p %d", parag, parag->paragId() );
	    parag = parag->next();
	    continue;
	}

	drawParagWYSIWYG( p, parag, cx, cy, cw, ch, doubleBuffer, cg, drawCursor, cursor, resetChanged );
	parag = parag->next();
    }

    parag = lastParag();
    paragy = m_zoomHandler->layoutUnitToPixelY( parag->rect().y() );
    int paragheight = m_zoomHandler->layoutUnitToPixelY( parag->rect().height() );
    int docheight = m_zoomHandler->layoutUnitToPixelY( parag->document()->height() );
    if ( paragy + paragheight < docheight ) {
        int docwidth = m_zoomHandler->layoutUnitToPixelX( parag->document()->width() );
	p->fillRect( 0, paragy + paragheight, docwidth, docheight - ( paragy + paragheight ),
		     cg.brush( QColorGroup::Base ) );
	if ( !flow()->isEmpty() ) {
	    QRect cr( cx, cy, cw, ch );
	    cr = cr.intersect( QRect( 0, paragy + paragheight, docwidth,
				      docheight - ( paragy + paragheight ) ) );
	    flow()->drawFloatingItems( p, cr.x(), cr.y(), cr.width(), cr.height(), cg, FALSE );
	}
    }

    if ( ko_buf_pixmap && ko_buf_pixmap->height() > 300 ) {
	delete ko_buf_pixmap;
	ko_buf_pixmap = 0;
    }

    //if ( verticalBreak() && flow() )
    //	flow()->draw( p, cx, cy, cw, ch );

    //tmpCursor = 0;
    return lastFormatted;
}
// SYNC end - end of modified copies of QTextDocument methods





#include "kotextdocument.moc"
