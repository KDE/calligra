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
#include "kotextformatter.h"
#include <kdebug.h>
#include <kdeversion.h>
#if ! KDE_IS_VERSION(3,1,90)
#include <kdebugclasses.h>
#endif
#include "kocommand.h"

//#define DEBUG_PAINTING

//// Note that many methods are implemented in korichtext.cpp
//// Those are the ones that come from Qt, and that mostly work :)

KoTextDocument::KoTextDocument( KoZoomHandler *zoomHandler, KoTextFormatCollection *fc,
                                KoTextFormatter *formatter, bool createInitialParag )
    : m_zoomHandler( zoomHandler ),
      m_bDestroying( false ),
#ifdef QTEXTTABLE_AVAILABLE
      par( 0L /*we don't use parent documents */ ),
      tc( 0 ),
#endif
      tArray( 0 ), tStopWidth( 0 )
{
    fCollection = fc;
    init(); // see korichtext.cpp

    m_drawingFlags = 0;
    setAddMargins( true );                 // top margin and bottom are added, not max'ed
    if ( !formatter )
        formatter = new KoTextFormatter;
    setFormatter( formatter );

    setY( 0 );
    setLeftMargin( 0 );
    setRightMargin( 0 );

    // Delete the KoTextParag created by KoTextDocument::init() if createInitialParag is false.
    if ( !createInitialParag )
        clear( false );
}

bool KoTextDocument::visitSelection( int selectionId, KoParagVisitor* visitor, bool forward )
{
    KoTextCursor c1 = selectionStartCursor( selectionId );
    KoTextCursor c2 = selectionEndCursor( selectionId );
    if ( c1 == c2 )
        return true;
    return visitFromTo( c1.parag(), c1.index(), c2.parag(), c2.index(), visitor, forward );
}

bool KoTextDocument::hasSelection( int id, bool visible ) const
{
    return ( selections.find( id ) != selections.end() &&
             ( !visible ||
               ( (KoTextDocument*)this )->selectionStartCursor( id ) !=
               ( (KoTextDocument*)this )->selectionEndCursor( id ) ) );
}

void KoTextDocument::setSelectionStart( int id, KoTextCursor *cursor )
{
    KoTextDocumentSelection sel;
    sel.startCursor = *cursor;
    sel.endCursor = *cursor;
    sel.swapped = FALSE;
    selections[ id ] = sel;
}

KoTextParag *KoTextDocument::paragAt( int i ) const
{
    KoTextParag *s = fParag;
    while ( s ) {
	if ( s->paragId() == i )
	    return s;
	s = s->next();
    }
    return 0;
}

bool KoTextDocument::visitDocument( KoParagVisitor *visitor, bool forward )
{
    return visitFromTo( firstParag(), 0, lastParag(), lastParag()->length()-1, visitor, forward );
}

bool KoTextDocument::visitFromTo( KoTextParag *firstParag, int firstIndex, KoTextParag* lastParag, int lastIndex, KoParagVisitor* visitor, bool forw )
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

        KoTextParag* currentParag = forw ? firstParag->next() : lastParag->prev();
        KoTextParag * endParag = forw ? lastParag : firstParag;
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

static bool is_printer( QPainter *p )
{
    return p && p->device() && p->device()->devType() == QInternal::Printer;
}

KoTextParag *KoTextDocument::drawWYSIWYG( QPainter *p, int cx, int cy, int cw, int ch, const QColorGroup &cg,
                                          KoZoomHandler* zoomHandler, bool onlyChanged,
                                          bool drawCursor, KoTextCursor *cursor,
                                          bool resetChanged, uint drawingFlags )
{
    m_drawingFlags = drawingFlags;
    if ( is_printer( p ) ) {
    // This stuff relies on doLayout()... simpler to just test for Printer.
    // If someone understand doLayout() please tell me (David)
    /*if ( isWithoutDoubleBuffer() || par && par->withoutDoubleBuffer ) { */
	//setWithoutDoubleBuffer( TRUE );
	QRect crect( cx, cy, cw, ch );
	drawWithoutDoubleBuffer( p, crect, cg, zoomHandler );
	return 0;
    }
    //setWithoutDoubleBuffer( FALSE );

    if ( !firstParag() )
        return 0;

    KoTextParag *lastFormatted = 0;
    KoTextParag *parag = firstParag();

    QPixmap *doubleBuffer = 0;
    QPainter painter;
    // All the coordinates in this method are in view pixels
    QRect crect( cx, cy, cw, ch );
#ifdef DEBUG_PAINTING
    kdDebug(32500) << "\nKoTextDocument::drawWYSIWYG crect=" << crect << endl;
#endif

    // Space above first parag
    QRect pixelRect = parag->pixelRect( zoomHandler );
    if ( isPageBreakEnabled() && parag && cy <= pixelRect.y() && pixelRect.y() > 0 ) {
        QRect r( 0, 0,
                 zoomHandler->layoutUnitToPixelX( parag->document()->x() + parag->document()->width() ),
                 pixelRect.y() );
        r &= crect;
        if ( !r.isEmpty() ) {
#ifdef DEBUG_PAINTING
            kdDebug(32500) << " drawWYSIWYG: space above first parag: " << r << " (pixels)" << endl;
            p->fillRect( r, cg.brush( QColorGroup::Base ) );
#endif
        }
    }

    while ( parag ) {
	lastFormatted = parag;
	if ( !parag->isValid() )
	    parag->format();

	QRect ir = parag->pixelRect( zoomHandler );
#ifdef DEBUG_PAINTING
        kdDebug(32500) << " drawWYSIWYG: ir=" << ir << endl;
#endif
	if ( isPageBreakEnabled() && parag->next() )
        {
            int nexty = parag->next()->pixelRect(zoomHandler).y();
            // Test ir.y+ir.height, which is the first pixel _under_ the parag
            // (as opposed ir.bottom() which is the last pixel of the parag)
	    if ( ir.y() + ir.height() < nexty ) {
		QRect r( 0, ir.y() + ir.height(),
			 zoomHandler->layoutUnitToPixelX( parag->document()->x() + parag->document()->width() ),
			 nexty - ( ir.y() + ir.height() ) );
		r &= crect;
		if ( !r.isEmpty() )
                {
#ifdef DEBUG_PAINTING
                    kdDebug(32500) << " drawWYSIWYG: space between parag " << parag->paragId() << " and " << parag->next()->paragId() << " : " << r << " (pixels)" << endl;
#endif
		    p->fillRect( r, cg.brush( QColorGroup::Base ) );
                }
	    }
        }
	if ( !ir.intersects( crect ) ) {
            // Paragraph is not in the crect - but let's check if the area on its right is.
	    ir.setWidth( zoomHandler->layoutUnitToPixelX( parag->document()->width() ) );
	    if ( ir.intersects( crect ) )
		p->fillRect( ir.intersect( crect ), cg.brush( QColorGroup::Base ) );
	    if ( ir.y() > cy + ch ) {
		//tmpCursor = 0;
                goto floating;
	    }
	}
        else if ( parag->hasChanged() || !onlyChanged ) {
            // lineChanged() only makes sense if we're drawing with onlyChanged=true
            // otherwise, call setChanged() to make sure we'll paint it all (lineChanged=-1).
            // (this avoids having to send onlyChanged to drawParagWYSIWYG)
            if ( !onlyChanged && parag->lineChanged() > 0 )
                parag->setChanged( false );
            drawParagWYSIWYG( p, parag, cx, cy, cw, ch, doubleBuffer, cg,
                              zoomHandler, drawCursor, cursor, resetChanged, drawingFlags );
        }

	parag = parag->next();
    }

    parag = lastParag();

floating:
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

    if ( buf_pixmap && buf_pixmap->height() > 300 ) {
	delete buf_pixmap;
	buf_pixmap = 0;
    }

    //tmpCursor = 0;
    return lastFormatted;
}

void KoTextDocument::drawWithoutDoubleBuffer( QPainter *p, const QRect &cr, const QColorGroup &cg,
                                              KoZoomHandler* zoomHandler, const QBrush *paper )
{
    if ( !firstParag() )
	return;

    Q_ASSERT( (m_drawingFlags & DrawSelections) == 0 );
    if (m_drawingFlags & DrawSelections)
           kdDebug() << kdBacktrace();
    if ( paper ) {
	p->setBrushOrigin( -(int)p->translationX(),
			   -(int)p->translationY() );
	p->fillRect( cr, *paper );
    }

    KoTextParag *parag = firstParag();
    while ( parag ) {
	if ( !parag->isValid() )
	    parag->format();

	QRect pr( parag->pixelRect( zoomHandler ) );
        pr.setLeft( 0 );
        pr.setWidth( QWIDGETSIZE_MAX );
        // The cliprect is checked in layout units, in KoTextParag::paint
        QRect crect_lu( parag->rect() );

	if ( !cr.isNull() && !cr.intersects( pr ) ) {
	    parag = parag->next();
	    continue;
	}
	p->translate( 0, pr.y() );
        QBrush brush = /*parag->backgroundColor() ? *parag->backgroundColor() :*/
            cg.brush( QColorGroup::Base );
        if ( brush != Qt::NoBrush )
	    p->fillRect( QRect( 0, 0, pr.width(), pr.height() ), brush );
        //p->setBrushOrigin( p->brushOrigin() + QPoint( 0, pr.y() ) );
	parag->paint( *p, cg, 0, FALSE,
                      crect_lu.x(), crect_lu.y(), crect_lu.width(), crect_lu.height() );
	p->translate( 0, -pr.y() );
        //p->setBrushOrigin( p->brushOrigin() - QPoint( 0, pr.y() ) );
	parag = parag->next();
    }
}

// Called by drawWYSIWYG and the app's drawCursor
void KoTextDocument::drawParagWYSIWYG( QPainter *p, KoTextParag *parag, int cx, int cy, int cw, int ch,
                                       QPixmap *&doubleBuffer, const QColorGroup &cg,
                                       KoZoomHandler* zoomHandler, bool drawCursor,
                                       KoTextCursor *cursor, bool resetChanged, uint drawingFlags )
{
    if ((cw == 0) || (ch == 0)) return;

#ifdef DEBUG_PAINTING
    kdDebug(32500) << "KoTextDocument::drawParagWYSIWYG " << (void*)parag << " id:" << parag->paragId() << endl;
#endif
    m_drawingFlags = drawingFlags;
    QPainter *painter = 0;
    // Those three rects are in pixels, in the document coordinates (0,0 == topleft of first parag)
    QRect rect = parag->pixelRect( zoomHandler ); // the parag rect

    int offsetY = 0;
    // Start painting from a given line number.
    if ( parag->lineChanged() > -1 )
    {
        offsetY = zoomHandler->layoutUnitToPixelY( parag->lineY( parag->lineChanged() ) - parag->topMargin() );
#ifdef DEBUG_PAINTING
        kdDebug(32500) << " Repainting from lineChanged=" << parag->lineChanged() << " -> adding " << offsetY << " to rect" << endl;
#endif
        // Skip the lines that are not repainted by moving Top. The bottom doesn't change.
        rect.rTop() += offsetY;
    }

    QRect crect( cx, cy, cw, ch ); // the overall crect
    QRect ir( rect ); // will be the rect to be repainted
    QBrush brush = /*parag->backgroundColor() ? *parag->backgroundColor() :*/
        cg.brush( QColorGroup::Base );
    // No need to brush plain white on a printer. Brush all other cases (except "full transparent" case).
    bool needBrush = brush.style() != Qt::NoBrush &&
                     !(brush.style() == Qt::SolidPattern && brush.color() == Qt::white && is_printer(p));

    bool useDoubleBuffer = !parag->document()->parent();
    if ( is_printer(p) )
	useDoubleBuffer = FALSE;
    // Can't handle transparency using double-buffering, in case of rotation/scaling (due to bitBlt)
    // The test on mat is almost like isIdentity(), but allows for translation.
    //// ##### The way to fix this: initialize the pixmap to be fully transparent instead
    // of being white.
    QWMatrix mat = p->worldMatrix();
    if ( ( mat.m11() != 1.0 || mat.m22() != 1.0 || mat.m12() != 0.0 || mat.m21() != 0.0 )
         && brush.style() != Qt::SolidPattern )
        useDoubleBuffer = FALSE;

#ifdef DEBUG_PAINTING
    kdDebug(32500) << "KoTextDocument::drawParagWYSIWYG parag->rect=" << parag->rect()
                   << " pixelRect(ir)=" << ir
                   << " crect (pixels)=" << crect
                   << " useDoubleBuffer=" << useDoubleBuffer << endl;
#endif

    if ( useDoubleBuffer  ) {
	painter = new QPainter;
	if ( cx >= 0 && cy >= 0 )
	    ir = ir.intersect( crect );
	if ( !doubleBuffer ||
	     ir.width() > doubleBuffer->width() ||
	     ir.height() > doubleBuffer->height() )
        {
	    doubleBuffer = bufferPixmap( ir.size() );
        }
        painter->begin( doubleBuffer );

    } else {
        p->save();
	painter = p;
	painter->translate( ir.x(), ir.y() );
    }
    // Until the next translate(), (0,0) in the painter will be at ir.topLeft() in reality
    //kdDebug() << "KoTextDocument::drawParagWYSIWYG ir=" << ir << endl;


    // Cumulate ir.x(), ir.y() with the current brush origin
    //painter->setBrushOrigin( painter->brushOrigin() + ir.topLeft() );

    if ( useDoubleBuffer || is_printer( painter ) ) {
        // Transparent -> grab background from p's device
        if ( brush.style() != Qt::SolidPattern ) {
            bitBlt( doubleBuffer, 0, 0, p->device(),
                    ir.x() + (int)p->translationX(), ir.y() + (int)p->translationY(),
                    ir.width(), ir.height() );
        }
    }
    if ( needBrush )
        painter->fillRect( QRect( 0, 0, ir.width(), ir.height() ), brush );

    // Now revert the previous painter translation, and instead make (0,0) the topleft of the PARAGRAPH
    painter->translate( rect.x() - ir.x(), rect.y() - ir.y() );
#ifdef DEBUG_PAINTING
    kdDebug(32500) << "KoTextDocument::drawParagWYSIWYG translate " << rect.x() - ir.x() << "," << rect.y() - ir.y() << endl;
#endif
    //painter->setBrushOrigin( painter->brushOrigin() + rect.topLeft() - ir.topLeft() );

    // The cliprect is checked in layout units, in KoTextParag::paint
    QRect crect_lu( zoomHandler->pixelToLayoutUnit( crect ) );
#ifdef DEBUG_PAINTING
    kdDebug(32500) << "KoTextDocument::drawParagWYSIWYG crect_lu=" << crect_lu << endl;
#endif

    // paintDefault will paint line 'lineChanged' at its normal Y position.
    // But the buffer-pixmap below starts at Y. We need to translate by -Y
    // so that the painting happens at the right place.
    painter->translate( 0, -offsetY );

    parag->paint( *painter, cg, drawCursor ? cursor : 0, (m_drawingFlags & DrawSelections),
                  crect_lu.x(), crect_lu.y(), crect_lu.width(), crect_lu.height() );


    if ( useDoubleBuffer ) {
	delete painter;
	painter = 0;
	p->drawPixmap( ir.topLeft(), *doubleBuffer, QRect( QPoint( 0, 0 ), ir.size() ) );
#if 0 // for debug!
        p->save();
        p->setPen( Qt::blue );
        p->drawRect( ir.x(), ir.y(), ir.width(), ir.height() );
        p->restore();
#endif
    } else {
        // undo previous translations, painter is 'p', i.e. will be used later on
        p->restore();
	//painter->translate( -ir.x(), -ir.y() );
        //painter->translate( 0, +offsetY );
        //painter->setBrushOrigin( painter->brushOrigin() - ir.topLeft() );
    }

    if ( needBrush ) {
        int docright = zoomHandler->layoutUnitToPixelX( parag->document()->x() + parag->document()->width() );
#ifdef DEBUG_PAINTING
//        kdDebug(32500) << "KoTextDocument::drawParagWYSIWYG my rect is: " << rect << endl;
#endif
        if ( rect.x() + rect.width() < docright ) {
#ifdef DEBUG_PAINTING
            kdDebug(32500) << "KoTextDocument::drawParagWYSIWYG rect doesn't go up to docright=" << docright << endl;
#endif
            p->fillRect( rect.x() + rect.width(), rect.y(),
                         docright - ( rect.x() + rect.width() ),
                         rect.height(), cg.brush( QColorGroup::Base ) );
        }
    }

    if ( resetChanged )
	parag->setChanged( FALSE );
}


KoTextDocCommand *KoTextDocument::deleteTextCommand( KoTextDocument *textdoc, int id, int index, const QMemArray<KoTextStringChar> & str, const CustomItemsMap & customItemsMap, const QValueList<KoParagLayout> & oldParagLayouts )
{
    return new KoTextDeleteCommand( textdoc, id, index, str, customItemsMap, oldParagLayouts );
}

#include "kotextdocument.moc"
