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
#include <kcommand.h>

KoTextDocument::KoTextDocument( KoZoomHandler * zoomHandler, QTextDocument *p, KoTextFormatCollection *fc, bool createInitialParag )
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

    // Delete the QTextParag created by QTextDocument, and re-create a KoTextParag
    // unless createInitialParag is false.
    clear( createInitialParag );
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

	QRect pr( static_cast<KoTextParag *>(parag)->pixelRect() );
        pr.setLeft( 0 );
        pr.setWidth( QWIDGETSIZE_MAX );

	if ( !cr.isNull() && !cr.intersects( pr ) ) {
	    parag = parag->next();
	    continue;
	}
	p->translate( 0, pr.y() );
	parag->paint( *p, cg, 0, FALSE );
	p->translate( 0, -pr.y() );
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
    QRect rect = static_cast<KoTextParag *>(parag)->pixelRect();
    QRect ir( rect );
    QRect crect( cx, cy, cw, ch );

    //kdDebug() << "KoTextDocument::drawParagWYSIWYG parag->rect=" << DEBUGRECT( parag->rect() )
    //          << " ir=" << DEBUGRECT(ir) << endl;
    bool useDoubleBuffer = !parag->document()->parent();
    // no parent docs in libkotext (and no access to nextDoubleBuffered)
    //if ( !useDoubleBuffer && parag->document()->nextDoubleBuffered )
    //	useDoubleBuffer = TRUE;
    if ( p->device()->devType() == QInternal::Printer )
	useDoubleBuffer = FALSE;

    if ( useDoubleBuffer  ) {
	painter = new QPainter;
	if ( cx >= 0 && cy >= 0 )
	    ir = ir.intersect( crect );
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

    painter->translate( rect.x() - ir.x(), rect.y() - ir.y() );

    // The cliprect is checked in layout units, in QTextParag::paint
    QRect crect_lu( m_zoomHandler->pixelToLayoutUnit( crect ) );
    parag->paint( *painter, cg, drawCursor ? cursor : 0, TRUE,
                  crect_lu.x(), crect_lu.y(), crect_lu.width(), crect_lu.height() );

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
    KoTextParag *parag = static_cast<KoTextParag *>( firstParag() );

    QPixmap *doubleBuffer = 0;
    QPainter painter;
    QRect crect( cx, cy, cw, ch );
    //kdDebug() << "KoTextDocument::drawWYSIWYG crect=" << DEBUGRECT(crect) << endl;

    // Space above first parag
    QRect pixelRect = parag->pixelRect();
    if ( parag && cy <= pixelRect.y() && pixelRect.y() > 0 ) {
        QRect r( 0, 0,
                 m_zoomHandler->layoutUnitToPixelX( parag->document()->x() + parag->document()->width() ),
                 pixelRect.y() );
        r &= crect;
        if ( !r.isEmpty() )
            p->fillRect( r, cg.brush( QColorGroup::Base ) );
    }

    while ( parag ) {
	lastFormatted = parag;
	if ( !parag->isValid() )
	    parag->format();

	QRect ir = parag->pixelRect();
        //kdDebug() << "KoTextDocument::drawWYSIWYG ir=" << DEBUGRECT(ir) << endl;
	if ( verticalBreak() && parag->next() )
        {
            int nexty = static_cast<KoTextParag *>(parag->next())->pixelRect().y();
	    if ( ir.y() + ir.height() < nexty ) {
		QRect r( 0, ir.y() + ir.height(),
			 m_zoomHandler->layoutUnitToPixelX( parag->document()->x() + parag->document()->width() ),
			 nexty - ( ir.y() + ir.height() ) );
		r &= crect;
		if ( !r.isEmpty() )
		    p->fillRect( r, cg.brush( QColorGroup::Base ) );
	    }
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
	}
        else if ( parag->hasChanged() || !onlyChanged ) {
            drawParagWYSIWYG( p, parag, cx, cy, cw, ch, doubleBuffer, cg, drawCursor, cursor, resetChanged );
        }

	parag = static_cast<KoTextParag *>( parag->next() );
    }

    parag = static_cast<KoTextParag *>( lastParag() );
    pixelRect = parag->pixelRect();
    int docheight = m_zoomHandler->layoutUnitToPixelY( parag->document()->height() );
    if ( pixelRect.y() + pixelRect.height() < docheight ) {
        int docwidth = m_zoomHandler->layoutUnitToPixelX( parag->document()->width() );
	p->fillRect( 0, pixelRect.y() + pixelRect.height(),
                     docwidth, docheight - ( pixelRect.y() + pixelRect.height() ),
		     cg.brush( QColorGroup::Base ) );
	if ( !flow()->isEmpty() ) {
	    QRect cr( cx, cy, cw, ch );
	    cr = cr.intersect( QRect( 0, pixelRect.y() + pixelRect.height(), docwidth,
				      docheight - ( pixelRect.y() + pixelRect.height() ) ) );
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



int KoTextCustomItem::index() const
{
    ASSERT( paragraph() );
    KoTextParag * parag = static_cast<KoTextParag *>( paragraph() );
    return parag->findCustomItem( this );
}

QTextFormat * KoTextCustomItem::format() const
{
    QTextParag * parag = paragraph();
    //kdDebug() << "KoTextCustomItem::format index=" << index() << " format=" << parag->at( index() )->format() << endl;
    return parag->at( index() )->format();
}


void KoTextCustomItem::draw(QPainter* p, int x, int _y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected )
{
    KoZoomHandler *zh=textDocument()->zoomHandler();
    //kdDebug()<<" x :"<<x<<" y :"<<y<<" cx :"<<cx<<" cy :"<<cy<<" ch :"<<ch<<" cw :"<<cw<<endl;
    x=zh->layoutUnitToPixelX(x);
    int y=zh->layoutUnitToPixelY(_y);
    cx=zh->layoutUnitToPixelX(cx);
    cy=zh->layoutUnitToPixelY(_y,cy);
    ch=zh->layoutUnitToPixelY(_y,ch);
    cw=zh->layoutUnitToPixelX(cw);
    //kdDebug()<<"After  x :"<<x<<" y :"<<y<<" cx :"<<cx<<" cy :"<<cy<<" ch :"<<ch<<" cw :"<<cw<<endl;

    QTextFormat * f = format();

    QFont newFont(f->font());

    bool forPrint = ( p->device()->devType() == QInternal::Printer );
    newFont.setPointSizeFloat( zh->layoutUnitToFontSize( newFont.pointSize(), forPrint ) );

    p->setFont( newFont );
    int offset=0;
    //code from qt3stuff
    if ( f->vAlign() == QTextFormat::AlignSuperScript )
    {
        newFont.setPointSize( ( newFont.pointSize() * 2 ) / 3 );
        p->setFont( newFont );

        int h = zh->layoutUnitToPixelY( _y, height );
        offset =- ( h - p->fontMetrics().height() );
    }
    else if ( f->vAlign() == QTextFormat::AlignSubScript )
        newFont.setPointSize( ( newFont.pointSize() * 2 ) / 3 );

    drawCustomItem(p, x, y, cx, cy, cw, ch, cg, selected, newFont, offset);
}

void CustomItemsMap::insertItems( const QTextCursor & startCursor, int size )
{
    if ( isEmpty() )
        return;

    QTextCursor cursor( startCursor );
    for ( int i = 0; i < size; ++i )
    {
        CustomItemsMap::Iterator it = find( i );
        if ( it != end() )
        {
            kdDebug() << "CustomItemsMap::insertItems setting custom item " << it.data() << endl;
            static_cast<KoTextParag *>(cursor.parag())->setCustomItem( cursor.index(), it.data(), 0 );
            it.data()->setDeleted( false );
        }
        cursor.gotoRight();
    }
}

void CustomItemsMap::deleteAll( KMacroCommand *macroCmd )
{
    Iterator it = begin();
    for ( ; it != end(); ++it )
    {
        KoTextCustomItem * item = it.data();
        KCommand * itemCmd = item->deleteCommand();
        if ( itemCmd && macroCmd )
        {
            macroCmd->addCommand( itemCmd );
            itemCmd->execute(); // the item-specific delete stuff hasn't been done
        }
        item->setDeleted( true );
    }
}

#include "kotextdocument.moc"
