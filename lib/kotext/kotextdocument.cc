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
#include "kotextparag.h"
#include "kotextformatter.h"
#include <kdebug.h>
#include <kcommand.h>

KoTextDocument::KoTextDocument( KoZoomHandler *zoomHandler, KoTextFormatCollection *fc,
                                KoTextFormatter *formatter, bool createInitialParag )
    : QTextDocument( 0L /*we don't use parent documents */, fc ),
      m_zoomHandler( zoomHandler ),
      m_bDestroying( false )
{
    m_bDrawFormattingChars=false;
    setAddMargins( true );                 // top margin and bottom are added, not max'ed
    if ( !formatter )
        formatter = new KoTextFormatter;
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

Qt3::QTextParag * KoTextDocument::createParag( QTextDocument *d, Qt3::QTextParag *pr, Qt3::QTextParag *nx, bool updateIds )
{
    return new KoTextParag( static_cast<KoTextDocument *>(d), static_cast<KoTextParag *>(pr), static_cast<KoTextParag *>(nx), updateIds );
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

bool KoTextDocument::visitFromTo( Qt3::QTextParag *firstParag, int firstIndex, Qt3::QTextParag* lastParag, int lastIndex, KoParagVisitor* visitor, bool forw )
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

        Qt3::QTextParag* currentParag = forw ? firstParag->next() : lastParag->prev();
        Qt3::QTextParag * endParag = forw ? lastParag : firstParag;
        while ( currentParag && currentParag != endParag )
        {
            ret = visitor->visit( currentParag, 0, currentParag->length() - 1 );
            if (!ret) return false;
            currentParag = forw ? currentParag->next() : currentParag->prev();
        }
        Q_ASSERT( currentParag );
        Q_ASSERT( endParag == currentParag );
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
	int w = QABS( s.width() );
	int h = QABS( s.height() );
	ko_buf_pixmap = new QPixmap( w, h );
    } else {
	if ( ko_buf_pixmap->width() < s.width() ||
	     ko_buf_pixmap->height() < s.height() ) {
	    ko_buf_pixmap->resize( QMAX( s.width(), ko_buf_pixmap->width() ),
				QMAX( s.height(), ko_buf_pixmap->height() ) );
	}
    }

    return ko_buf_pixmap;
}

void KoTextDocument::drawWithoutDoubleBuffer( QPainter *p, const QRect &cr, const QColorGroup &cg,
                                              KoZoomHandler* zoomHandler, const QBrush *paper )
{
    if ( !firstParag() )
	return;

    if ( paper ) {
	p->setBrushOrigin( -(int)p->translationX(),
			   -(int)p->translationY() );
	p->fillRect( cr, *paper );
    }

    Qt3::QTextParag *parag = firstParag();
    while ( parag ) {
	if ( !parag->isValid() )
	    parag->format();

	QRect pr( static_cast<KoTextParag *>(parag)->pixelRect( zoomHandler ) );
        pr.setLeft( 0 );
        pr.setWidth( QWIDGETSIZE_MAX );

	if ( !cr.isNull() && !cr.intersects( pr ) ) {
	    parag = parag->next();
	    continue;
	}
	p->translate( 0, pr.y() );
        //p->setBrushOrigin( p->brushOrigin() + QPoint( 0, pr.y() ) );
	parag->paint( *p, cg, 0, FALSE );
	p->translate( 0, -pr.y() );
        //p->setBrushOrigin( p->brushOrigin() - QPoint( 0, pr.y() ) );
	parag = parag->next();
    }
}

void KoTextDocument::drawParagWYSIWYG( QPainter *p, KoTextParag *parag, int cx, int cy, int cw, int ch,
                                       QPixmap *&doubleBuffer, const QColorGroup &cg,
                                       KoZoomHandler* zoomHandler, bool drawCursor,
                                       QTextCursor *cursor, bool resetChanged, bool drawFormattingChars )
{
#ifdef DEBUG_PAINTING
    kdDebug() << "drawParagWYSIWYG " << (void*)parag << " id:" << parag->paragId() << endl;
#endif
    m_bDrawFormattingChars=drawFormattingChars;
    int sx = 0, sy = 0;
    if ( parag->shadowDistance() )
    {
        sx = parag->shadowX( zoomHandler );
        sy = parag->shadowY( zoomHandler );
    }
    QPainter *painter = 0;
    if ( resetChanged )
	parag->setChanged( FALSE );
    QRect rect = static_cast<KoTextParag *>(parag)->pixelRect( zoomHandler );
    rect.rRight() += sx;
    rect.rBottom() += sy;
    QRect ir( rect );
    QRect crect( cx, cy, cw, ch ); // in pixels
    QBrush brush = parag->backgroundColor() ? *parag->backgroundColor() : cg.brush( QColorGroup::Base );

#ifdef DEBUG_PAINTING
    kdDebug() << "KoTextDocument::drawParagWYSIWYG parag->rect=" << DEBUGRECT( parag->rect() )
              << " pixelRect()=" << DEBUGRECT(ir)
              << " crect (pixels)=" << DEBUGRECT(crect) << endl;
#endif

    bool useDoubleBuffer = !parag->document()->parent();
    // no parent docs in libkotext (and no access to nextDoubleBuffered)
    //if ( !useDoubleBuffer && parag->document()->nextDoubleBuffered )
    //	useDoubleBuffer = TRUE;
    if ( p->device()->devType() == QInternal::Printer )
	useDoubleBuffer = FALSE;
    // Can't handle transparency using double-buffering, in case of rotation/scaling (due to bitBlt)
    if ( !p->worldMatrix().isIdentity() && brush == Qt::NoBrush )
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

    // Cumulate ir.x(), ir.y() with the current brush origin
    //painter->setBrushOrigin( painter->brushOrigin() + ir.topLeft() );

    if ( useDoubleBuffer || is_printer( painter ) ) {
        // Transparent -> grab background from p's device
        if ( brush == Qt::NoBrush ) {
            bitBlt( doubleBuffer, 0, 0, p->device(),
                    ir.x() + (int)p->translationX(), ir.y() + (int)p->translationY(),
                    ir.width(), ir.height() );
        }
	else
        {
	    painter->fillRect( QRect( 0, 0, ir.width(), ir.height() ), brush );
        }
    } else {
	if ( cursor && cursor->parag() == parag ) {
            painter->fillRect( QRect( zoomHandler->layoutUnitToPixelX( parag->at( cursor->index() )->x ), 0, 2, ir.height() ),
                               brush );
	}
    }

    painter->translate( rect.x() - ir.x(), rect.y() - ir.y() );
#ifdef DEBUG_PAINTING
    kdDebug() << "KoTextDocument::drawParagWYSIWYG translate " << rect.x() - ir.x() << "," << rect.y() - ir.y() << endl;
#endif
    //painter->setBrushOrigin( painter->brushOrigin() + rect.topLeft() - ir.topLeft() );

    // The cliprect is checked in layout units, in QTextParag::paint
    QRect crect_lu( zoomHandler->pixelToLayoutUnit( crect ) );
#ifdef DEBUG_PAINTING
    kdDebug() << "KoTextDocument::drawParagWYSIWYG crect_lu=" << DEBUGRECT( crect_lu ) << endl;
#endif

    if ( sx != 0 || sy != 0 )
    {
        painter->save();
        painter->translate( sx, sy );
        m_bDrawingShadow = true;
        parag->paint( *painter, cg, drawCursor ? cursor : 0, TRUE,
                      crect_lu.x(), crect_lu.y(), crect_lu.width(), crect_lu.height() );
        painter->restore();
    }
    m_bDrawingShadow = false;

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
/* // for debug!
        p->save();
        p->setPen( Qt::blue );
        p->drawRect( ir.x(), ir.y(), ir.width(), ir.height() );
        p->restore();
*/
    } else {
	painter->translate( -ir.x(), -ir.y() );
        //painter->setBrushOrigin( painter->brushOrigin() - ir.topLeft() );
    }

    int docright = zoomHandler->layoutUnitToPixelX( parag->document()->x() + parag->document()->width() );
    if ( useDoubleBuffer ) {
        if ( rect.x() + rect.width() < docright ) {
            p->fillRect( rect.x() + rect.width(), rect.y(),
                         docright - ( rect.x() + rect.width() ),
                         rect.height(), cg.brush( QColorGroup::Base ) );
        }
    }

    //parag->document()->nextDoubleBuffered = FALSE;
}

Qt3::QTextParag *KoTextDocument::drawWYSIWYG( QPainter *p, int cx, int cy, int cw, int ch, const QColorGroup &cg,
                                              KoZoomHandler* zoomHandler, bool onlyChanged,
                                              bool drawCursor, QTextCursor *cursor,
                                              bool resetChanged, bool drawFormattingChars )
{
    m_bDrawFormattingChars=drawFormattingChars;
    if ( isWithoutDoubleBuffer() /* || par && par->withoutDoubleBuffer */ ) {
	//setWithoutDoubleBuffer( TRUE );
	QRect crect( cx, cy, cw, ch );
	drawWithoutDoubleBuffer( p, crect, cg, zoomHandler );
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

    Qt3::QTextParag *lastFormatted = 0;
    KoTextParag *parag = static_cast<KoTextParag *>( firstParag() );

    QPixmap *doubleBuffer = 0;
    QPainter painter;
    QRect crect( cx, cy, cw, ch );
    //kdDebug() << "KoTextDocument::drawWYSIWYG crect=" << DEBUGRECT(crect) << endl;

    // Space above first parag
    QRect pixelRect = parag->pixelRect( zoomHandler );
    if ( isPageBreakEnabled() && parag && cy <= pixelRect.y() && pixelRect.y() > 0 ) {
        QRect r( 0, 0,
                 zoomHandler->layoutUnitToPixelX( parag->document()->x() + parag->document()->width() ),
                 pixelRect.y() );
        r &= crect;
        if ( !r.isEmpty() )
            p->fillRect( r, cg.brush( QColorGroup::Base ) );
    }

    while ( parag ) {
	lastFormatted = parag;
	if ( !parag->isValid() )
	    parag->format();

	QRect ir = parag->pixelRect( zoomHandler );
        //kdDebug() << "KoTextDocument::drawWYSIWYG ir=" << DEBUGRECT(ir) << endl;
	if ( isPageBreakEnabled() && parag->next() )
        {
            int nexty = static_cast<KoTextParag *>(parag->next())->pixelRect(zoomHandler).y();
	    if ( ir.y() + ir.height() < nexty ) {
		QRect r( 0, ir.y() + ir.height(),
			 zoomHandler->layoutUnitToPixelX( parag->document()->x() + parag->document()->width() ),
			 nexty - ( ir.y() + ir.height() ) );
		r &= crect;
		if ( !r.isEmpty() )
		    p->fillRect( r, cg.brush( QColorGroup::Base ) );
	    }
        }
	if ( !ir.intersects( crect ) ) {
	    ir.setWidth( zoomHandler->layoutUnitToPixelX( parag->document()->width() ) );
	    if ( ir.intersects( crect ) )
		p->fillRect( ir.intersect( crect ), cg.brush( QColorGroup::Base ) );
	    if ( ir.y() > cy + ch ) {
		//tmpCursor = 0;
		if ( ko_buf_pixmap && ko_buf_pixmap->height() > 300 ) {
		    delete ko_buf_pixmap;
		    ko_buf_pixmap = 0;
		}
		return lastFormatted;
	    }
	}
        else if ( parag->hasChanged() || !onlyChanged ) {
            drawParagWYSIWYG( p, parag, cx, cy, cw, ch, doubleBuffer, cg,
                              zoomHandler, drawCursor, cursor, resetChanged, drawFormattingChars  );
        }

	parag = static_cast<KoTextParag *>( parag->next() );
    }

    parag = static_cast<KoTextParag *>( lastParag() );
    pixelRect = parag->pixelRect(zoomHandler);
    int docheight = zoomHandler->layoutUnitToPixelY( parag->document()->height() );
    if ( pixelRect.y() + pixelRect.height() < docheight ) {
        int docwidth = zoomHandler->layoutUnitToPixelX( parag->document()->width() );
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

    //tmpCursor = 0;
    return lastFormatted;
}
// SYNC end - end of modified copies of QTextDocument methods



int KoTextCustomItem::index() const
{
    Q_ASSERT( paragraph() );
    KoTextParag * parag = static_cast<KoTextParag *>( paragraph() );
    return parag->findCustomItem( this );
}

QTextFormat * KoTextCustomItem::format() const
{
    Qt3::QTextParag * parag = paragraph();
    //kdDebug() << "KoTextCustomItem::format index=" << index() << " format=" << parag->at( index() )->format() << endl;
    return parag->at( index() )->format();
}


void KoTextCustomItem::draw(QPainter* p, int x, int _y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected )
{
    KoZoomHandler *zh=textDocument()->paintingZoomHandler();
    //kdDebug()<<" x :"<<x<<" y :"<<y<<" cx :"<<cx<<" cy :"<<cy<<" ch :"<<ch<<" cw :"<<cw<<endl;

    x=zh->layoutUnitToPixelX(x) + paragraph()->at( index() )->pixelxadj;
    int y=zh->layoutUnitToPixelY(_y);
    cx=zh->layoutUnitToPixelX(cx);
    cy=zh->layoutUnitToPixelY(_y,cy);
    ch=zh->layoutUnitToPixelY(_y,ch);
    cw=zh->layoutUnitToPixelX(cw);
    //kdDebug()<<"After  x :"<<x<<" y :"<<y<<" cx :"<<cx<<" cy :"<<cy<<" ch :"<<ch<<" cw :"<<cw<<endl;

    KoTextFormat * fmt = static_cast<KoTextFormat *>( format() );

    //bool forPrint = ( p->device()->devType() == QInternal::Printer );
    p->setFont( fmt->screenFont( zh ) );

    int offset=0;
    if ( fmt->vAlign() == QTextFormat::AlignSuperScript )
    {
        int h = zh->layoutUnitToPixelY( _y, height );
        offset =- ( h - p->fontMetrics().height() );
    }

    drawCustomItem(p, x, y, cx, cy, cw, ch, cg, selected, offset);
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
