/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Reginald Stadlbauer <reggie@kde.org>

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

#include <qrichtext_p.h>

#include "kwtextframeset.h"
#include "kwtextimage.h"
#include "kwtableframeset.h"
#include "kwdoc.h"
#include "kwview.h"
#include "kwcanvas.h"
#include "kwutils.h"
#include "kwcommand.h"
#include "kwdrag.h"
#include "kwstyle.h"
#include "kwformat.h"
#include "counter.h"
#include "variable.h"
#include "variabledlgs.h"
#include "serialletter.h"
#include <qclipboard.h>
#include <qdragobject.h>
#include <klocale.h>
#include <kconfig.h>

#include <kdebug.h>

//#define DEBUG_FLOW

KWTextFrameSet::KWTextFrameSet( KWDocument *_doc )
    : KWFrameSet( _doc ), undoRedoInfo( this )
{
    //kdDebug() << "KWTextFrameSet::KWTextFrameSet " << this << endl;
    m_availableHeight = -1;
    m_origFontSizes.setAutoDelete(true);
    textdoc = new KWTextDocument( this, 0, new KWTextFormatCollection( _doc ) );
    textdoc->setFormatter( new QTextFormatterBreakWords );
    textdoc->setFlow( this );
    textdoc->setVerticalBreak( true );              // get QTextFlow methods to be called
    textdoc->setAddMargins( true );                 // top margin and bottom are added, not max'ed

    /* if ( QFile::exists( "bidi.txt" ) ) {
       QFile fl( "bidi.txt" );
       fl.open( IO_ReadOnly );
       QByteArray array = fl.readAll();
       QString text = QString::fromUtf8( array.data() );
       textdoc->setText( text, QString::null );
    }*/

    m_lastFormatted = textdoc->firstParag();
    // Apply 'Standard' style to paragraph
    KWStyle * style = _doc->findStyle( "Standard", true );
    if ( m_lastFormatted && style )
    {
	static_cast<KWTextParag*>(m_lastFormatted)->setParagLayout( style->paragLayout() );
        m_lastFormatted->setFormat( 0, m_lastFormatted->string()->length(), & style->format() );
    }

    textdoc->addSelection( HighlightSelection );
    textdoc->setSelectionColor( HighlightSelection,
                                QApplication::palette().color( QPalette::Active, QColorGroup::Dark ) );

    interval = 0;
    changeIntervalTimer = new QTimer( this );
    connect( changeIntervalTimer, SIGNAL( timeout() ),
	     this, SLOT( doChangeInterval() ) );

    formatTimer = new QTimer( this );
    connect( formatTimer, SIGNAL( timeout() ),
	     this, SLOT( formatMore() ) );
}

KWFrameSetEdit * KWTextFrameSet::createFrameSetEdit( KWCanvas * canvas )
{
    return new KWTextFrameSetEdit( this, canvas );
}

int KWTextFrameSet::availableHeight() const
{
    if ( m_availableHeight == -1 )
        const_cast<KWTextFrameSet *>(this)->updateFrames();
    return m_availableHeight;
}

KWFrame * KWTextFrameSet::contentsToInternal( QPoint cPoint, QPoint &iPoint, bool onlyY ) const
{
    int totalHeight = 0;
    QListIterator<KWFrame> frameIt( frameIterator() );
    for ( ; frameIt.current(); ++frameIt )
    {
        QRect frameRect = kWordDocument()->zoomRect( *frameIt.current() );
        QRect r( frameRect );
        if ( onlyY )
            r.setLeft(0);      // cPoint.x will be 0 too, so only the Y coords will count
        if ( r.contains( cPoint ) ) // both r and p are in "contents coordinates"
        {
            // This translates the coordinates in the document contents
            // into the QTextDocument's coordinate system
            // (which doesn't have frames, borders, etc.)
            iPoint.setX( cPoint.x() - frameRect.left() );
            iPoint.setY( cPoint.y() - frameRect.top() + totalHeight );
            /*kdDebug() << "contentsToInternal: returning " << iPoint.x() << "," << iPoint.y()
                      << " totalHeight=" << totalHeight << " because r: " << DEBUGRECT(r)
                      << " contains cPoint:" << cPoint.x() << "," << cPoint.y() << endl;*/
            return frameIt.current();
        }
#if 0 // this was a hack, we shouldn't need it anymore
        else if ( p.y() < frameRect.top() && m_doc->processingType() == KWDocument::WP ) // ## WP-only: we've been too far, the point is between two frames
        {
            p.rx() += -frameRect.left();
            p.ry() = totalHeight;
            return p;
        }
#endif
        totalHeight += frameRect.height();
    }

#if 0
    if ( onlyY ) // we only care about Y -> easy, return "we're below the bottom"
        return QPoint( 0, totalHeight );
    kdDebug(32002) << "KWTextFrameSet::contentsToInternal " << cPoint.x() << "," << cPoint.y()
                   << " not in any frame of " << (void*)this << endl;
#endif
    return 0;
}

KWFrame * KWTextFrameSet::internalToContents( QPoint iPoint, QPoint & cPoint ) const
{
    int totalHeight = 0;
    QListIterator<KWFrame> frameIt( frameIterator() );
    for ( ; frameIt.current(); ++frameIt )
    {
        QRect frameRect = kWordDocument()->zoomRect( *frameIt.current() );
        QRect r( frameRect );
        r.moveBy( -frameRect.left(), -frameRect.top() + totalHeight );   // frame in qrt coords
        if ( r.contains( iPoint ) ) // both r and p are in "qrt coordinates"
        {
            cPoint.setX( iPoint.x() + frameRect.left() );
            cPoint.setY( iPoint.y() + frameRect.top() - totalHeight );
            return frameIt.current();
        }
        totalHeight += frameRect.height();
    }

    kdDebug(32002) << "** KWTextFrameSet::internalToContents " << iPoint.x() << "," << iPoint.y()
                   << " not in any frame of " << (void*)this << endl;
    cPoint = iPoint; // bah
    return 0L;
}

void KWTextFrameSet::drawContents( QPainter *p, const QRect & crect, QColorGroup &gb, bool onlyChanged, bool drawCursor, QTextCursor *cursor, bool resetChanged )
{
    //kdDebug(32002) << "KWTextFrameSet::drawContents drawCursor=" << drawCursor << " onlyChanged=" << onlyChanged
    //               << " resetChanged=" << resetChanged << endl;
    //if ( !cursorVisible )
    //drawCur = FALSE;
    if ( !textdoc->firstParag() )
        return;

    QListIterator<KWFrame> frameIt( frameIterator() );
    int totalHeight = 0;
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame *frame = frameIt.current();
        if ( !frame->isValid() )
        {
            kdDebug(32002) << "KWTextFrameSet::drawContents invalid frame " << frame << endl;
            continue;
        }

        QRect r(crect);
        QRect frameRect( kWordDocument()->zoomRect( *frame ) );
        //kdDebug(32002) << "KWTFS::drawContents frame=" << frame << " cr=" << DEBUGRECT(r) << endl;
        r = r.intersect( frameRect );
        //kdDebug(32002) << "                    framerect=" << DEBUGRECT(*frame) << " intersec=" << DEBUGRECT(r) << " todraw=" << !r.isEmpty() << endl;
        if ( !r.isEmpty() )
        {
            // This translates the coordinates in the document contents
            // ( frame and r are up to here in this system )
            // into the QTextDocument's coordinate system
            // (which doesn't have frames, borders, etc.)
            r.moveBy( -frameRect.left(), -frameRect.top() + totalHeight );   // portion of the frame to be drawn, in qrt coords
            QRegion reg = frameClipRegion( p, frame, crect );
            if ( !reg.isEmpty() )
            {
                p->save();
                p->setClipRegion( reg );

                p->translate( frameRect.left(), frameRect.top() - totalHeight ); // translate to qrt coords - after setting the clip region !

                gb.setBrush(QColorGroup::Base,frame->getBackgroundColor());
                QTextParag * lastFormatted = textdoc->draw( p, r.x(), r.y(), r.width(), r.height(), gb, onlyChanged, drawCursor, cursor, resetChanged );

                // The last paragraph of this frame might have a bit in the next frame too.
                // In that case, and if we're only drawing changed paragraphs, (and resetting changed),
                // we have to set changed to true again, to draw the bottom of the parag in the next frame.
                if ( onlyChanged && resetChanged )
                {
                    // Finding the "last parag of the frame" is a bit tricky.
                    // It's usually the one before lastFormatted, except if it's actually lastParag :}  [see QTextDocument::draw]
                    QTextParag * lastDrawn = lastFormatted->prev();
                    if ( lastFormatted == textdoc->lastParag() && ( !lastDrawn || lastDrawn->rect().bottom() < r.bottom() ) )
                        lastDrawn = lastFormatted;

                    //kdDebug(32002) << "KWTextFrameSet::drawContents drawn. onlyChanged=" << onlyChanged << " resetChanged=" << resetChanged << " lastDrawn=" << lastDrawn->paragId() << " lastDrawn's bottom:" << lastDrawn->rect().bottom() << " r.bottom=" << r.bottom() << endl;
                    if ( lastDrawn && lastDrawn->rect().bottom() > r.bottom() )
                    {
                        //kdDebug(32002) << "KWTextFrameSet::drawContents setting lastDrawn " << lastDrawn->paragId() << " to changed" << endl;
                        lastDrawn->setChanged( true );
                    }
                }

                // NOTE: QTextView sets m_lastFormatted to lastFormatted here
                // But when scrolling up, this causes to reformat a lot of stuff for nothing.
                // And updateViewArea takes care of formatting things before we even arrive here.

                // Blank area under the very last paragraph
                if ( lastFormatted == textdoc->lastParag() && !onlyChanged)
                {
                    int docHeight = textdoc->height();
                    QRect blank( 0, docHeight, frameRect.width(), totalHeight+frameRect.height() - docHeight );
                    //kdDebug(32002) << this << " Blank area: " << DEBUGRECT(blank) << endl;
                    p->fillRect( blank, gb.brush( QColorGroup::Base ) );
                    // for debugging :)
                    //p->setPen( QPen(Qt::blue, 1, DashLine) );  p->drawRect( blank );
                }
                p->restore();
            }
        }
        totalHeight += frameRect.height();
    }
}

void KWTextFrameSet::drawCursor( QPainter *p, QTextCursor *cursor, bool cursorVisible )
{
    // This redraws the paragraph where the cursor is

    QListIterator<KWFrame> frameIt( frameIterator() );
    int totalHeight = 0;
    bool drawn = false;
    // ### TODO save a parag -> frame pointer, to make this faster ?
    // Also needed to implement proper page breaking... but not good if breaking
    // at the line level.
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame *frame = frameIt.current();
        if ( !frame->isValid() )
            continue;

        QRect frameRect( kWordDocument()->zoomRect( *frame ) );
        // The parag is in the qtextdoc coordinates -> first translate frame to qtextdoc coords
        QRect rf( frameRect );
        rf.moveBy( -frameRect.left(), -frameRect.top() + totalHeight );
        rf = rf.intersect( cursor->topParag()->rect() );
        if ( !rf.isEmpty() )
        {
            QPoint topLeft = cursor->topParag()->rect().topLeft();                                  // in QRT coords
            int h = cursor->parag()->lineHeightOfChar( cursor->index() );                           //
            QPoint cPoint( topLeft.x() + frameRect.left(), topLeft.y() + frameRect.top() - totalHeight ); // from QRT to contents
            QRect clip = QRect( cPoint.x() + cursor->x() - 5, cPoint.y() + cursor->y(), 10, h );  // very small clipping around the cursor

            //kdDebug(32002) << "KWTextFrameSet::drawCursor topLeft=(" << topLeft.x() << "," << topLeft.y() << ")  h=" << h << endl;
            //kdDebug(32002) << this << " Clip for cursor: " << DEBUGRECT(clip) << endl;

            QRegion reg = frameClipRegion( p, frame, clip );
            if ( !reg.isEmpty() )
            {
                cursor->parag()->setChanged( TRUE );      // To force the drawing to happen
                p->save();

                p->setClipRegion( reg );
                p->translate( frameRect.left(), frameRect.top() - totalHeight ); // translate to qrt coords - after setting the clip region !

                QPixmap *pix = 0;
                QColorGroup cg = QApplication::palette().active();
                cg.setBrush(QColorGroup::Base, frame->getBackgroundColor());
                textdoc->drawParag( p, cursor->parag(), topLeft.x() - cursor->totalOffsetX() + cursor->x() - 5,
                                 topLeft.y() - cursor->totalOffsetY() + cursor->y(), 10, h,
                                 pix, cg, cursorVisible, cursor );
                p->restore();
            }
            drawn = true;
        }
        else
            if ( drawn ) // Ok, we've drawn it, and now we're after it -> exit
                break;   // Note that we might go into the above block twice, if parag is over two frames.
        totalHeight += frameRect.height();
    }
    // Well this is a no-op currently (we don't use QTextFlow::draw)
    //if ( textdoc->flow() )
    //textdoc->flow()->draw( p, r.x() - cursor->totalOffsetX(),
    //                        r.y() - cursor->totalOffsetX(), r.width(), r.height() );

}

void KWTextFrameSet::layout()
{
    m_lastFormatted = textdoc->firstParag();
    textdoc->invalidate(); // lazy layout, real update follows upon next repaint
}


void KWTextFrameSet::statistics( ulong & charsWithSpace, ulong & charsWithoutSpace, ulong & words, ulong & sentences )
{
    QTextParag * parag = textdoc->firstParag();
    for ( ; parag ; parag = parag->next() )
    {
        QString s = parag->string()->toString();
        bool wordStarted = false;
        for ( uint i = 0 ; i < s.length() - 1 /*trailing-space*/ ; ++i )
        {
            QChar ch = s[i];
            ++charsWithSpace;
            if ( !ch.isSpace() )
                ++charsWithoutSpace;
            if ( ch.isSpace() || ch.isPunct() )
            {
                if ( wordStarted )
                    ++words;
                if ( ch == '.' )
                    ++sentences;
                wordStarted = false;
            }
            else
                wordStarted = true;
        }
        if ( wordStarted )
            ++words;
    }
}

// Only interested in the body textframeset, not in header/footer
#define kdDebugBody(area) if ( getFrameInfo() == FI_BODY ) kdDebug(area)

// Helper for adjust*. Returns marginLeft/marginRight/breakEnd, each for an adjust* method.
void KWTextFrameSet::getMargins( int yp, int h, int* marginLeft, int* marginRight, int* breakEnd )
{
#ifdef DEBUG_FLOW
    kdDebugBody(32002) << "KWTextFrameSet " << this << " getMargins yp=" << yp
                       << " h=" << h << " called by "
                       << (marginLeft?"adjustLMargin":marginRight?"adjustRMargin":"adjustFlow")
                       << endl;
#endif
    QPoint p;
    KWFrame * frame = internalToContents( QPoint(0, yp), p );
    if (!frame)
        kdDebug() << "getMargins: internalToContents returned frame=0L for yp=" << yp << endl;
    // Everything from there is in contents coordinates.
    int left = frame ? kWordDocument()->zoomItX( frame->left() ) : 0;
    int from = left;
    int to = frame ? kWordDocument()->zoomItX( frame->right() ) : 0;
    int top = frame ? kWordDocument()->zoomItX( frame->top() ) : 0;
    int width = to - from;
    int bottomSkip = 0;

#ifdef DEBUG_FLOW
    kdDebug() << "KWTextFrameSet::getMargins looking for frames between " << p.y() << " and " << p.y()+h << endl;
#endif
    // For every frame on top at this height, we'll move from and to towards each other
    // The text flows between 'from' and 'to'
    QListIterator<KWFrame> fIt( m_framesOnTop );
    for ( ; fIt.current() && from < to ; ++fIt )
    {
        if ( fIt.current()->getRunAround() == RA_BOUNDINGRECT )
        {
            QRect frameRect = kWordDocument()->zoomRect( * fIt.current() );
#ifdef DEBUG_FLOW
            kdDebug() << "getMargins found frame at " << DEBUGRECT(frameRect) << endl;
#endif
            // Look for intersection between p.y() -- p.y()+h  and frameRect.top() -- frameRect.bottom()
            if ( QMAX( p.y(), frameRect.top() ) <= QMIN( p.y()+h, frameRect.bottom() ) )
            {
                int availLeft = QMAX( 0, frameRect.left() - from );
                int availRight = QMAX( 0, to - frameRect.right() );
#ifdef DEBUG_FLOW
                kdDebugBody(32002) << "getMargins availLeft=" << availLeft
                                   << " availRight=" << availRight << endl;
#endif
                if ( availLeft > availRight ) // choose the max
                    // flow text at the left of the frame
                    to = QMIN( to, from + availLeft );  // can only go left -> QMIN
                else
                    // flow text at the right of the frame
                    from = QMAX( from, to - availRight ); // can only go right -> QMAX
#ifdef DEBUG_FLOW
                kdDebugBody(32002) << "getMargins from=" << from << " to=" << to << endl;
#endif
                if ( breakEnd )
                    bottomSkip = QMAX( bottomSkip, frameRect.bottom() - top );
            }
        }
    }
    // Back to the QRT coordinates
    from -= left;
    to -= left;

    // If the available space is too small, give up on it
    if ( to - from < kWordDocument()->zoomItX( 15 ) )
        from = to;
    else
        bottomSkip = 0; // nothing to skip

    if ( marginLeft )
        *marginLeft = from;
    if ( marginRight )
        *marginRight = width - to;
    if ( breakEnd )
        *breakEnd = bottomSkip; // in internal coord already
}

int KWTextFrameSet::adjustLMargin( int yp, int h, int margin, int space )
{
    int marginLeft;
    getMargins( yp, h, &marginLeft, 0L, 0L );
#ifdef DEBUG_FLOW
    kdDebugBody(32002) << "KWTextFrameSet::adjustLMargin " << marginLeft << endl;
#endif
    return QTextFlow::adjustLMargin( yp, h, margin + marginLeft, space );
}

int KWTextFrameSet::adjustRMargin( int yp, int h, int margin, int space )
{
    int marginRight;
    getMargins( yp, h, 0L, &marginRight, 0L );
#ifdef DEBUG_FLOW
    kdDebugBody(32002) << "KWTextFrameSet::adjustRMargin " << marginRight << endl;
#endif
    return QTextFlow::adjustRMargin( yp, h, margin + marginRight, space );
}

// helper for adjustFlow
bool KWTextFrameSet::checkVerticalBreak( int & yp, int h, QTextParag * parag, bool linesTogether, int breakBegin, int breakEnd )
{
    // Check for intersection between the parag (yp -- yp+h) and the break area (breakBegin -- breakEnd)
    if ( QMAX( yp, breakBegin ) <= QMIN( yp+h, breakEnd ) )
    {
        if ( !parag || linesTogether ) // Paragraph-level breaking
        {
#ifdef DEBUG_FLOW
            kdDebug(32002) << "checkVerticalBreak ADJUSTING yp=" << yp << " h=" << h
                           << " breakEnd+2 [new value for yp]=" << breakEnd+2 << endl;
#endif
            yp = breakEnd + 2;
            return true;
        }
        else // Line-level breaking
        {
            QMap<int, QTextParagLineStart*>& lineStarts = parag->lineStartList();
#ifdef DEBUG_FLOW
            kdDebug(32002) << "checkVerticalBreak parag " << parag->paragId()
                           << ". lineStarts has " << lineStarts.count()
                           << " items" << endl;
#endif
            int dy = 0;
            int line = 0;
            QMap<int, QTextParagLineStart*>::Iterator it = lineStarts.begin();
            for ( ; it != lineStarts.end() ; ++it, ++line )
            {
                QTextParagLineStart * ls = it.data();
                ASSERT( ls );
                int y = parag->rect().y() + ls->y;
#ifdef DEBUG_FLOW
                kdDebug(32002) << "checkVerticalBreak parag " << parag->paragId()
                               << " line " << line << " ls->y=" << ls->y
                               << " ls->h=" << ls->h << " y=" << y
                               << " breakBegin=" << breakBegin
                               << " breakEnd=" << breakEnd << endl;
#endif
                if ( !dy )
                {
                    if ( QMAX( y, breakBegin ) <= QMIN( y + ls->h, breakEnd ) )
                    {
                        if ( line == 0 ) // First line ? It's like a paragraph breaking then
                        {
#ifdef DEBUG_FLOW
                            kdDebug(32002) << "checkVerticalBreak parag " << parag->paragId()
                                           << " BREAKING first line -> parag break" << endl;
#endif
                            yp = breakEnd + 2;
                            return true;
                        }
                        dy = breakEnd + 2 - y;
#ifdef DEBUG_FLOW
                        kdDebug(32002) << "checkVerticalBreak parag " << parag->paragId()
                                       << " BREAKING at line " << line << " dy=" << dy << endl;
#endif
                        ls->y = breakEnd - parag->rect().y();
                    }
                }
                else
                    ls->y += dy;
            }
            parag->setHeight( h + dy );
#ifdef DEBUG_FLOW
            kdDebug(32002) << "Paragraph height set to " << h+dy << endl;
#endif
            return true;
        }
    }
    return false;
}

void KWTextFrameSet::adjustFlow( int &yp, int w, int h, QTextParag * parag, bool /*pages*/ )
{
    // This is called since the 'vertical break' QRT flag is true.
    // End of frames/pages lead to those "vertical breaks".
    // What we do, is adjust the Y accordingly,
    // to implement page-break at the paragraph level and at the line level.
    // It's cumulative (the space of one break will be included in the further
    // paragraph's y position), which makes it easy to implement.

    int breaked = false;
    bool linesTogether = parag ? static_cast<KWTextParag *>(parag)->linesTogether() : false;
    //kdDebugBody(32002) << "KWTextFrameSet::adjustFlow parag=" << parag
    //                   << " linesTogether=" << linesTogether << " yp=" << yp
    //                   << " h=" << h << endl;

    int totalHeight = 0;
    QListIterator<KWFrame> frameIt( frameIterator() );
    for ( ; frameIt.current(); ++frameIt )
    {
        int frameHeight = kWordDocument()->zoomItY( frameIt.current()->height() );
        int bottom = totalHeight + frameHeight;
        // Only skip bottom of frame if there'll be another one. Not for header/footer, for instance.
        if ( frameIt.current()->getFrameBehaviour() == AutoCreateNewFrame )
        {
            //kdDebug(32002) << "KWTextFrameSet::adjustFlow frameHeight=" << frameHeight << " bottom=" << bottom << endl;

            // breakBegin==breakEnd==bottom, since the next frame's top is the same as bottom, in QRT coords.
            breaked = ( checkVerticalBreak( yp, h, parag, linesTogether, bottom, bottom ) );
            if ( breaked )
                break;
        }
        if ( yp+h < bottom )
            break; // we've been past the parag, so stop here
        totalHeight = bottom;
    }

    // Another case for a vertical break is frames with the RA_SKIP flag
    QList<KWFrame> frames( m_framesOnTop );
    for ( KWFrame *f = frames.first(); f ; f = frames.next() )
    {
        if ( f->getRunAround() == RA_SKIP )
        {
            int top = kWordDocument()->zoomItY( f->top() );
            int bottom = kWordDocument()->zoomItY( f->bottom() );
            QPoint iTop, iBottom; // top and bottom in internal coordinates
            if ( contentsToInternal( QPoint( 0, top ), iTop, true ) &&  // we know the frame is on top, so using onlyY is ok
                 contentsToInternal( QPoint( 0, bottom ), iBottom, true ) &&
                 checkVerticalBreak( yp, h, parag, linesTogether,
                                     iTop.y(), iBottom.y() ) )
            {
                kdDebug(32002) << "KWTextFrameSet::adjustFlow breaking around RA_SKIP frame yp="<<yp<<" h=" << h << endl;
                // We don't "break;" here because there could be another such frame below the first one
                // In fact if they are in the wrong order we could fail to skip both...
                // Back to the beginning then
                frames.remove();
                f = frames.first();
                breaked = true;
            }
        }
    }

    // And the last case for a vertical break is RA_BOUNDINGRECT frames that
    // leave no space by their side for any text (e.g. most tables)
    int breakEnd = 0;
    getMargins( yp, h, 0L, 0L, &breakEnd );
    if ( breakEnd )
    {
        kdDebug(32002) << "KWTextFrameSet::adjustFlow no-space case. breakEnd=" << breakEnd
                       << " yp=" << yp << " h=" << h << endl;
        if ( checkVerticalBreak( yp, h, parag, linesTogether, yp, breakEnd ) )
        {
            breaked = true;
            //kdDebug(32002) << "checkVerticalBreak ok." << endl;
        }
        else // shouldn't happen
            kdWarning(32002) << "checkVerticalBreak didn't find it" << endl;
    }

    // ## TODO loop around those three methods until we don't move anymore ?

    int yp_ro = yp;
    QTextFlow::adjustFlow( yp_ro, w, h, parag, FALSE );
}

void KWTextFrameSet::draw( QPainter *, int /*cx*/, int /*cy*/, int /*cw*/, int /*ch*/ )
{
}

void KWTextFrameSet::eraseAfter( QTextParag * parag, QPainter * p, const QColorGroup & cg )
{
    // This is called when adjustFlow above moved a paragraph downwards to move
    // it to the next frame. Then we have to erase the space under the paragraph,
    // up to the bottom of the frame. This is what should be done here.

    QPoint cPoint;
    QRect r = parag->rect();
    QPoint iPoint = r.bottomLeft();
    iPoint.ry()++; // go under the paragraph
    //kdDebug(32002) << "KWTextFrameSet::eraseAfter parag=" << parag->paragId() << endl;
    KWFrame * frame = internalToContents( iPoint, cPoint );
    int frameBottom = kWordDocument()->zoomItY( frame->bottom() );
    ASSERT( cPoint.y() <= frameBottom );
    //kdDebug(32002) << " parag bottom=" << cPoint.y()
    //               << " frameBottom=" << frameBottom
    //               << " height of fillRect: " << frameBottom - cPoint.y() << endl;

    p->fillRect( iPoint.x(), iPoint.y(),
                 kWordDocument()->zoomItX( frame->width() ) /*r.width()*/, // erase the whole width of the frame
                 frameBottom - cPoint.y(),
                 /*Qt::blue*/ cg.brush( QColorGroup::Base ) );

}

/*================================================================*/
KWTextFrameSet::~KWTextFrameSet()
{
    //kdDebug(32001) << "KWTextFrameSet::~KWTextFrameSet" << endl;
    // don't let us delete ourselves

    if ( m_doc )
        m_doc->delFrameSet(this, false);
    m_doc = 0L;
}

/*================================================================*/
void KWTextFrameSet::updateFrames()
{
    if ( !frames.isEmpty() )
    {
        //kdDebug(32002) << "KWTextFrameSet::updateFrames " << this
        //               << " setWidth=" << frames.first()->width() << endl;
        int width = kWordDocument()->zoomItX( frames.first()->width() );
        // ## we need support for variable width.... (could be done in adjustRMargin in fact)
        if ( width != textdoc->width() )
            textdoc->setWidth( width );
    } else
    {
        //kdDebug(32002) << "KWTextFrameSet::update no frames" << endl;
        return; // No frames. This happens when the frameset is deleted (still exists for undo/redo)
    }

    typedef QList<KWFrame> FrameList;
    QList<FrameList> frameList;
    frameList.setAutoDelete( true );
    frames.setAutoDelete( false );

    // sort frames of this frameset into l2 on (page, y coord, x coord)
    KoRect pageRect;
    for ( unsigned int i = 0; i < static_cast<unsigned int>( m_doc->getPages() + 1 ); i++ ) {
        //kdDebug(32002) << "Page " << i << endl;
        pageRect = KoRect( 0, i * m_doc->ptPaperHeight(), m_doc->ptPaperWidth(), m_doc->ptPaperHeight() );
        FrameList *l = new FrameList();
        l->setAutoDelete( false );
        for ( unsigned int j = 0; j < frames.count(); j++ ) {
            if ( frames.at( j )->intersects( pageRect ) ) {
                frames.at( j )->setPageNum( i );
                //kdDebug(32002) << "KWTextFrameSet::update appending frame " << frames.at(j) << endl;
                l->append( frames.at( j ) );
                //kdDebug(32002) << "KWTextFrameSet::update removing frame " << j << endl;
                frames.remove(j--);
            }
        }


        if ( !l->isEmpty() ) {
            FrameList *ll = new FrameList();
            ll->setAutoDelete( false );
            ll->append( l->first() );
            unsigned int k = 0, m = 0;
            for ( k = 1; k < l->count(); k++ ) {
                bool inserted = false;
                for ( m = 0; m < ll->count(); m++ ) {
                    if ( l->at( k )->y() < ll->at( m )->y() ) {
                        inserted = true;
                        ll->insert( m, l->at( k ) );
                        break;
                    }
                }
                if ( !inserted ) ll->append( l->at( k ) );
            }
            FrameList *l2 = new FrameList();
            l2->setAutoDelete( false );
            l2->append( ll->first() );
            for ( k = 1; k < ll->count(); k++ ) {
                bool inserted = false;
                for ( m = 0; m < l2->count(); m++ ) {
                    if ( ll->at( k )->x() < l2->at( m )->x() ) {
                        inserted = true;
                        l2->insert( m, ll->at( k ) );
                        break;
                    }
                }
                if ( !inserted ) l2->append( ll->at( k ) );
            }

            delete ll;
            delete l;
            l = l2;
        }

        if(! l->isEmpty())
            frameList.append( l );
        else
            delete l;
    }

    // Rebuild the <frames> list and set the "most right" flag for each frame

    ASSERT( frames.isEmpty() ); // let's see
    m_availableHeight = 0;
    //int rm = 0;
    QListIterator<FrameList> frameListIt = QListIterator<FrameList>(frameList);
    for ( ; frameListIt.current(); ++frameListIt )
    {
        QListIterator<KWFrame> frameIt = QListIterator<KWFrame>(*frameListIt.current());
        for ( ; frameIt.current(); ++frameIt )
        {
            KWFrame * frame = frameIt.current();
            //kdDebugBody(32002) << "KWTextFrameSet::updateFrames adding frame " << frame
            //        << " height:" << frame->height()
            //        << " zoomed height:" << kWordDocument()->zoomItY( frame->height() ) << endl;
            ASSERT( !frames.contains(frame) );
            frames.append( frame );
            m_availableHeight += kWordDocument()->zoomItY( frame->height() );
            frames.at( frames.count() - 1 )->setMostRight( false );
            if ( frames.count() > 1 ) {
                if ( frames.at( frames.count() - 2 )->right() > frames.at( frames.count() - 1 )->right() ) {
                    frames.at( frames.count() - 2 )->setMostRight( true );
                    //rm++;
                }
            }
        }
    }
    kdDebugBody(32002) << this << " KWTextFrameSet::updateFrames m_availableHeight=" << m_availableHeight << endl;
    frames.setAutoDelete( true );

    KWFrameSet::updateFrames();
}

/*================================================================*/
void KWTextFrameSet::save( QDomElement &parentElem )
{
    if ( frames.isEmpty() ) // Deleted frameset -> don't save
        return;
    unzoom();

    QDomElement framesetElem = parentElem.ownerDocument().createElement( "FRAMESET" );
    parentElem.appendChild( framesetElem );

    if ( grpMgr ) {
        framesetElem.setAttribute( "grpMgr", correctQString( grpMgr->getName() ) );

        KWTableFrameSet::Cell *cell = (KWTableFrameSet::Cell *)this;
        framesetElem.setAttribute( "row", cell->m_row );
        framesetElem.setAttribute( "col", cell->m_col );
        framesetElem.setAttribute( "rows", cell->m_rows );
        framesetElem.setAttribute( "cols", cell->m_cols );
    }

    framesetElem.setAttribute( "frameType", static_cast<int>( getFrameType() ) );
    framesetElem.setAttribute( "frameInfo", static_cast<int>( frameInfo ) );
    framesetElem.setAttribute( "removable", static_cast<int>( removeableHeader ) );
    framesetElem.setAttribute( "visible", static_cast<int>( visible ) );
    framesetElem.setAttribute( "name", correctQString( m_name ) );

    KWFrameSet::save( framesetElem ); // Save all frames

    // Save paragraphs
    KWTextParag *start = static_cast<KWTextParag *>( textDocument()->firstParag() );
    while ( start ) {
        start->save( framesetElem );
        start = static_cast<KWTextParag *>( start->next() );
    }

    zoom();
}

/*================================================================*/
void KWTextFrameSet::load( QDomElement &attributes )
{
    KWFrameSet::load( attributes );

    textdoc->clear(false); // Get rid of dummy paragraph (and more if any)
    KWTextParag *lastParagraph = 0L;

    // <PARAGRAPH>
    //QDomNodeList listParagraphs = attributes.elementsByTagName ( "PARAGRAPH" );
    //unsigned int count = listParagraphs.count();
    //for (unsigned int item = 0; item < count; item++)
    QDomElement paragraph = attributes.firstChild().toElement();
    for ( ; !paragraph.isNull() ; paragraph = paragraph.nextSibling().toElement() )
    {
        //QDomElement paragraph = listParagraphs.item( item ).toElement();
        if ( paragraph.tagName() == "PARAGRAPH" )
        {
            KWTextParag *parag = new KWTextParag( textdoc, lastParagraph );
            parag->load( paragraph );
            if ( !lastParagraph )        // First parag
                textdoc->setFirstParag( parag );
            lastParagraph = parag;
            m_doc->progressItemLoaded();
        }
    }

    if ( !lastParagraph )                // We created no paragraph
    {
        // Create an empty one, then. See KWTextDocument ctor.
        textdoc->clear( true );
        static_cast<KWTextParag *>( textdoc->firstParag() )->setStyleName( "Standard" );
    }
    else
        textdoc->setLastParag( lastParagraph );

    m_lastFormatted = textdoc->firstParag();
    //kdDebug(32001) << "KWTextFrameSet::load done" << endl;
}

void KWTextFrameSet::zoom()
{
    if ( !m_origFontSizes.isEmpty() )
        unzoom();
    QTextFormatCollection * coll = textdoc->formatCollection();
    double factor = kWordDocument()->zoomedResolutionY() * 72.0 / QPaintDevice::x11AppDpiY();
    kdDebug(32002) << this << " KWTextFrameSet::zoom " << factor << " coll=" << coll << " " << coll->dict().count() << " items " << endl;
    /*kdDebug(32002) << this << " firstparag:" << textdoc->firstParag()
                   << " format:" << textdoc->firstParag()->paragFormat()
                   << " first-char's format:" << textdoc->firstParag()->at(0)->format()
                   << endl;*/
    QDictIterator<QTextFormat> it( coll->dict() );
    for ( ; it.current() ; ++it ) {
        KWTextFormat * format = dynamic_cast<KWTextFormat *>(it.current());
        ASSERT( format );
        m_origFontSizes.insert( format, new int( format->font().pointSize() ) );
        /*kdDebugBody(32002) << this << " KWTextFrameSet::zooming format " << format
                           << " key=" << format->key()
                           << " from " << format->font().pointSizeFloat()
                           << " to " << format->font().pointSizeFloat() * factor << endl;*/
        format->setPointSizeFloat( format->font().pointSizeFloat() * factor );
    }
    // Do the same to the default format !
    KWTextFormat * format = static_cast<KWTextFormat *>(coll->defaultFormat());
    //kdDebug() << "KWTextFrameSet::zoom default format " << format << " " << format->key() << endl;
    m_origFontSizes.insert( format, new int( format->font().pointSize() ) );
    format->setPointSizeFloat( format->font().pointSizeFloat() * factor );

    // Zoom all custom items
    QListIterator<QTextCustomItem> cit( textdoc->allCustomItems() );
    for ( ; cit.current() ; ++cit )
    {
        // ## This is only valid as long as all our custom items update
        // the size in adjustToPainter and doesn't use the painter.
        // If other classes do it differently, make a KWTextCustomItem base class with zoom().
        cit.current()->adjustToPainter( 0L );
    }

    // Mark all paragraphs as changed !
    for ( KWTextParag * s = static_cast<KWTextParag *>( textdoc->firstParag() ) ; s ; s = static_cast<KWTextParag *>( s->next() ) )
    {
        s->setChanged( TRUE );
        s->invalidate( 0 );
        if ( s->counter() )
            s->counter()->invalidate();
    }
    m_lastFormatted = textdoc->firstParag();
    m_availableHeight = -1; // to be recalculated
    emit ensureCursorVisible();
    KWFrameSet::zoom();
}

void KWTextFrameSet::unzoom()
{
    QTextFormatCollection * coll = textdoc->formatCollection();

    QDictIterator<QTextFormat> it( coll->dict() );
    for ( ; it.current() ; ++it ) {
        KWTextFormat * format = static_cast<KWTextFormat *>(it.current());
        int * oldSize = m_origFontSizes.find( format );
        if ( !oldSize )
            kdWarning() << "Can't unzoom: format=" << format << " " << it.current()->key() << endl;
        else
        {
            /*kdDebugBody(32002) << "KWTextFrameSet::unzoom format=" << format
                               << " key=" << format->key()
                               << " oldSize=" << *oldSize << endl;*/
            format->setPointSizeFloat( *oldSize );
        }
    }
    KWTextFormat * format = static_cast<KWTextFormat *>(coll->defaultFormat());
    int * oldSize = m_origFontSizes.find( format );
    if ( oldSize )
        format->setPointSizeFloat( *oldSize );

    m_origFontSizes.clear();
}

int KWTextFrameSet::docFontSize( QTextFormat * format ) const
{
    assert( format );
    int * oldSize = m_origFontSizes.find( format );
    if ( !oldSize )
    {
        kdDebug() << "Can't find format in m_origFontSizes: " << format->key() << endl;
        return 0;
    }
    else
        return *oldSize;
}

/*int KWTextFrameSet::docFontSize( float zoomedFontSize ) const
{
    return zoomedFontSize / kWordDocument()->zoomedResolutionY();
}*/

float KWTextFrameSet::zoomedFontSize( int docFontSize ) const
{
    return static_cast<float>( docFontSize ) * kWordDocument()->zoomedResolutionY();
}

QTextFormat * KWTextFrameSet::zoomFormatFont( const QTextFormat * f )
{
    KWTextFormat format = *static_cast<const KWTextFormat *>( f );
    int origFontSize = format.font().pointSize();
    format.setPointSizeFloat( zoomedFontSize( origFontSize ) );                // zoom it
    QTextFormat * fcf = textDocument()->formatCollection()->format( &format );   // find it in the collection
    if ( !m_origFontSizes.find( fcf ) )
        m_origFontSizes.insert( fcf, new int( origFontSize ) );           // insert in dict if necessary
    return fcf;
}

void KWTextFrameSet::applyStyleChange( const QString & changedStyle )
{
    //kdDebug(32001) << "KWTextFrameSet::applyStyleChange " << changedStyle << endl;
    QTextDocument * textdoc = textDocument();
    emit hideCursor();
    KWStyle * style = m_doc->findStyle( changedStyle, true );
    KWTextParag *p = static_cast<KWTextParag *>(textdoc->firstParag());
    while ( p ) {
        if ( p->styleName() == changedStyle )
        {
            if ( style == 0L ) // style has been deleted
            {
                p->setStyleName( QString::null ); // so get rid of its name, keep current formatting
                // TODO, make this undoable !
            }
            else
            {
                // Apply this style again, to get the changes
                // To get undo/redo and to reuse code, we call applyStyle with a temp selection
                // Using Temp+1 to avoid conflicting with QRT's internals (just in case)
#if 0
                KWStyle styleApplied=*style;
#endif
                QTextCursor start( textdoc );
                QTextCursor end( textdoc );
                start.setParag( p );
                start.setIndex( 0 );
                textdoc->setSelectionStart( QTextDocument::Temp+1, &start );
                end.setParag( p );
                end.setIndex( p->string()->length()-1 );
                textdoc->setSelectionEnd( QTextDocument::Temp+1, &end );
#if 0
                if ( (m_doc->applyStyleChangeMask() & KWDocument::U_BORDER) == 0)
                {
                    styleApplied.paragLayout().leftBorder=p->leftBorder();
                    styleApplied.paragLayout().rightBorder=p->rightBorder();
                    styleApplied.paragLayout().topBorder=p->topBorder();
                    styleApplied.paragLayout().bottomBorder=p->bottomBorder();
                }
                if ( (m_doc->applyStyleChangeMask() & KWDocument::U_ALIGN )==0)
                {
                    styleApplied.setAlign(p->alignment());
                }
                if ( (m_doc->applyStyleChangeMask() & KWDocument::U_NUMBERING)==0 )
                {
                    styleApplied.paragLayout().counter=*(p->counter());
                }
                if ( (m_doc->applyStyleChangeMask() & KWDocument::U_COLOR)==0 )
                {
                    styleApplied.format().setColor(p->paragFormat()->color());
                }
                if ( (m_doc->applyStyleChangeMask() & KWDocument::U_TABS)==0 )
                {
                    styleApplied.paragLayout().setTabList(p->tabList());
                }
                if ( (m_doc->applyStyleChangeMask() & KWDocument::U_INDENT)==0 )
                {
                    styleApplied.paragLayout().lineSpacing=p->kwLineSpacing();
                    styleApplied.paragLayout().margins[QStyleSheetItem::MarginLeft]=p->margin(QStyleSheetItem::MarginLeft);
                    styleApplied.paragLayout().margins[QStyleSheetItem::MarginRight]=p->margin(QStyleSheetItem::MarginRight);
                    styleApplied.paragLayout().margins[QStyleSheetItem::MarginFirstLine]=p->margin(QStyleSheetItem::MarginFirstLine);
                    styleApplied.paragLayout().margins[QStyleSheetItem::MarginBottom]=p->margin(QStyleSheetItem::MarginBottom);
                    styleApplied.paragLayout().margins[QStyleSheetItem::MarginTop]=p->margin(QStyleSheetItem::MarginTop);
                    }
#endif
                applyStyle( 0L, style, QTextDocument::Temp+1 );
                textdoc->removeSelection( QTextDocument::Temp+1 );
            }
        }
        p = static_cast<KWTextParag *>(p->next());
    }
    setLastFormattedParag( textdoc->firstParag() );
    emit repaintChanged( this );
    emit updateUI();
    formatMore();
}

/*================================================================*/
KWTextFrameSet *KWTextFrameSet::getCopy() {
    /* returns a deep copy of self */
    KWTextFrameSet *newFS = new KWTextFrameSet(m_doc);
    newFS->setFrameInfo(getFrameInfo());
    newFS->setVisible(visible);
    newFS->setName(getName());
    newFS->setIsRemoveableHeader(isRemoveableHeader());
    QListIterator<KWFrame> frameIt = frameIterator();
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame *thisFrame=frameIt.current()->getCopy();
        newFS->addFrame(thisFrame);
    }
    //if(newFS->getNumFrames() >0)
        //newFS->assign(this);
    return newFS;
}

void KWTextFrameSet::doKeyboardAction( QTextCursor * cursor, KeyboardActionPrivate action )
{
    KWTextParag * parag = static_cast<KWTextParag *>(cursor->parag());
    setLastFormattedParag( parag );
    emit hideCursor();

    switch ( action ) {
    case ActionDelete: {
	checkUndoRedoInfo( cursor, UndoRedoInfo::Delete );
	if ( !undoRedoInfo.valid() ) {
	    undoRedoInfo.id = parag->paragId();
	    undoRedoInfo.index = cursor->index();
	    undoRedoInfo.text = QString::null;
            undoRedoInfo.name = i18n("Delete text");
	}
        QTextStringChar * ch = parag->at( cursor->index() );
	undoRedoInfo.text += ch->c;
        copyCharFormatting( ch, undoRedoInfo.text.length()-1, true );
	if ( cursor->remove() )
	    undoRedoInfo.text += "\n";
    } break;
    case ActionBackspace: {
        // Remove counter
	if ( parag->counter() && parag->counter()->style() != Counter::STYLE_NONE && cursor->index() == 0 ) {
	    // parag->decDepth(); // We don't have support for nested lists at the moment
                                  // (only in titles, but you don't want Backspace to move it up)
            Counter c;
            setCounter( cursor, c );
	    emit repaintChanged( this );
	    emit showCursor();
	    return;
	}
	checkUndoRedoInfo( cursor, UndoRedoInfo::Delete );
	if ( !undoRedoInfo.valid() ) {
	    undoRedoInfo.id = cursor->parag()->paragId();
	    undoRedoInfo.index = cursor->index();
	    undoRedoInfo.text = QString::null;
            undoRedoInfo.name = i18n("Delete text");
	}
	cursor->gotoLeft();
        QTextStringChar * ch = cursor->parag()->at( cursor->index() );
	undoRedoInfo.text.prepend( QString( ch->c ) );
        copyCharFormatting( ch, 0, true );
	undoRedoInfo.index = cursor->index();
	if ( cursor->remove() ) {
	    undoRedoInfo.text.remove( 0, 1 );
	    undoRedoInfo.text.prepend( "\n" );
	    undoRedoInfo.index = cursor->index();
	    undoRedoInfo.id = cursor->parag()->paragId();
	}
        setLastFormattedParag( cursor->parag() );
    } break;
    case ActionReturn:
	checkUndoRedoInfo( cursor, UndoRedoInfo::Return );
	if ( !undoRedoInfo.valid() ) {
	    undoRedoInfo.id = cursor->parag()->paragId();
	    undoRedoInfo.index = cursor->index();
	    undoRedoInfo.text = QString::null;
            undoRedoInfo.name = i18n("Insert text");
	}
	undoRedoInfo.text += "\n";
	cursor->splitAndInsertEmptyParag();
	if ( cursor->parag()->prev() )
            setLastFormattedParag( cursor->parag()->prev() );
	break;
    case ActionKill:
	checkUndoRedoInfo( cursor, UndoRedoInfo::Delete );
	if ( !undoRedoInfo.valid() ) {
	    undoRedoInfo.id = cursor->parag()->paragId();
	    undoRedoInfo.index = cursor->index();
	    undoRedoInfo.text = QString::null;
            undoRedoInfo.name = i18n("Delete text");
	}
	if ( cursor->atParagEnd() ) {
            QTextStringChar * ch = cursor->parag()->at( cursor->index() );
	    undoRedoInfo.text += ch->c;
            copyCharFormatting( ch, undoRedoInfo.text.length()-1, true );
            if ( cursor->remove() )
		undoRedoInfo.text += "\n";
	} else {
            int oldLen = undoRedoInfo.text.length();
	    undoRedoInfo.text += cursor->parag()->string()->toString().mid( cursor->index() );
            for ( int i = cursor->index(); i < cursor->parag()->length(); ++i )
                copyCharFormatting( cursor->parag()->at( i ), oldLen + i - cursor->index(), true );
	    cursor->killLine();
	}
	break;
    }

    formatMore();
    emit repaintChanged( this );
    emit ensureCursorVisible();
    emit showCursor();
    emit updateUI();
}

void KWTextFrameSet::formatMore()
{
    if ( !m_lastFormatted || !isVisible() || m_availableHeight == -1 )
	return;

    int bottom = -1;
    int to = !sender() ? 2 : 20; // 20 when it comes from the formatTimer

    int viewsBottom = 0;
    QMapIterator<QWidget *, int> mapIt = m_mapViewAreas.begin();
    for ( ; mapIt != m_mapViewAreas.end() ; ++mapIt )
        viewsBottom = QMAX( viewsBottom, mapIt.data() );

    QTextParag *lastFormatted = m_lastFormatted;
    //kdDebug(32002) << "KWTextFrameSet::formatMore lastFormatted id=" << lastFormatted->paragId()
    //               << " to=" << to << " viewsBottom=" << viewsBottom
    //               << " availableHeight=" << m_availableHeight << endl;

    // Stop if we have formatted everything or if we need more space
    // Otherwise, stop formatting after "to" paragraphs,
    // but make sure we format everything the views need
    // ###### lastFormatted->rect().height() is dangerous. It's 0 initially, and it can
    // be too big after zooming out !!!
    for ( int i = 0;
          lastFormatted && bottom + lastFormatted->rect().height() <= m_availableHeight &&
          ( i < to || bottom <= viewsBottom ) ; ++i )
    {
        //kdDebug(32002) << "KWTextFrameSet::formatMore formatting id=" << lastFormatted->paragId() << endl;
	lastFormatted->format();
	bottom = lastFormatted->rect().top() + lastFormatted->rect().height();
	lastFormatted = lastFormatted->next();
    }
    //kdDebug(32002) << "KWTextFrameSet::formatMore finished formatting. Setting m_lastFormatted to " << lastFormatted << endl;
    m_lastFormatted = lastFormatted;

    if ( bottom != -1 && ( ( bottom > m_availableHeight ) ||   // this parag is already off page
                           ( lastFormatted && bottom + lastFormatted->rect().height() > m_availableHeight ) ) ) // or next parag will be off page
    {
    if(lastFormatted)
        kdDebug(32002) << "KWTextFrameSet::formatMore We need more space. bottom=" << bottom + lastFormatted->rect().height() << " m_availableHeight=" << m_availableHeight << endl;
    else
        kdDebug(32002) << "KWTextFrameSet::formatMore We need more space. bottom2=" << bottom << " m_availableHeight=" << m_availableHeight << endl;

        if ( !frames.isEmpty() )
        {
            double wantedPosition = 0;
            switch ( frames.last()->getFrameBehaviour() )
            {
                case AutoExtendFrame:
                {
                    double difference = bottom - m_availableHeight;
                    if(lastFormatted) difference += lastFormatted->rect().height();

                    if(difference > 0) {
                        KWFrame *theFrame = frames.last();

                        if ( theFrame->getFrameSet()->isAFooter() )
                        {
                            theFrame->setTop( theFrame->top() - difference );

                            m_doc->recalcFrames();
                            m_doc->frameChanged( theFrame );

                            updateFrames();
                            break;
                        }

                        wantedPosition = difference + theFrame->bottom();
                        double pageBottom = (double) (theFrame->pageNum()+1) * m_doc->ptPaperHeight();
                        pageBottom -= m_doc->ptBottomBorder();
                        double newPosition = QMIN(wantedPosition, pageBottom );
                        theFrame->setBottom(newPosition);
                        if(theFrame->getFrameSet()->getFrameInfo() != FI_BODY)
                        {
                            m_doc->recalcFrames();
                        }

                        KWTableFrameSet *table = theFrame->getFrameSet()->getGroupManager();
                        if (table)
                        {
                            KWTableFrameSet::Cell *cell = (KWTableFrameSet::Cell *)this;
                            table->recalcCols(cell->m_col,cell->m_row);
                            table->recalcRows(cell->m_col,cell->m_row);
                            table->updateTempHeaders();
                        }
                        if(newPosition < wantedPosition && theFrame->getNewFrameBehaviour() == Reconnect) {
                            wantedPosition = wantedPosition - newPosition + theFrame->top() + m_doc->ptPaperHeight();
                            // fall through to AutoCreateNewFrame
                        } else {
                            m_doc->frameChanged( theFrame );

                            updateFrames();
                            break;
                        }
                    }
                }
                case AutoCreateNewFrame:
                {
                    // We need a new frame in this frameset.

                    uint oldCount = frames.count();
                    // First create a new page for it if necessary
                    if ( frames.last()->pageNum() == m_doc->getPages() - 1 )
                        m_doc->appendPage();

                    // Maybe this created the frame, then we're done
                    if ( frames.count() == oldCount )
                    {
                        // Otherwise, create a new frame on next page
                        KWFrame *frame = frames.last();
                        KWFrame *frm = frame->getCopy();
                        frm->moveBy( 0, m_doc->ptPaperHeight() );
                        frm->setPageNum( frame->pageNum()+1 );
                        addFrame( frm );
                    }

                    if ( lastFormatted )
                        interval = 0;
                    if(wantedPosition > 0)
                        frames.last()->setBottom( wantedPosition );

                   updateFrames();
                }
                break;
                case Ignore:
                    break;
            }

        }
        else
        {
            kdWarning(32002) << "KWTextFrameSet::formatMore no more space, but no frame !" << endl;
        }
    }
    else
        if ( frames.count() > 1 && !lastFormatted && !isAHeader() && !isAFooter()
             && bottom < m_availableHeight - kWordDocument()->zoomItY( frames.last()->height() ) )
        {
            kdDebug(32002) << "KWTextFrameSet::formatMore too much space (" << bottom << ", " << m_availableHeight << ") , trying to remove last frame" << endl;
            int lastPage = m_doc->getPages() - 1;
            // Last frame is empty -> try removing last page, and more if necessary
            while ( lastPage > 0 && frames.count() > 1 && bottom < m_availableHeight - kWordDocument()->zoomItY( frames.last()->height() ) )
            {
                delFrame(frames.last());
                m_doc->removePage( lastPage );
                if ( m_doc->getPages() - 1 >= lastPage ) // removing didn't work
                    break;
                lastPage = m_doc->getPages() - 1;
            }
        }

    if ( lastFormatted )
	formatTimer->start( interval, TRUE );
    else
    {
	interval = QMAX( 0, interval );
        //kdDebug(32002) << "KWTextFrameSet::formatMore all formatted" << endl;
    }
}

void KWTextFrameSet::doChangeInterval()
{
    interval = 0;
}

void KWTextFrameSet::updateViewArea( QWidget * w, int maxY )
{
    (void) availableHeight(); // make sure that it's not -1
    //kdDebug(32002) << "KWTextFrameSet::updateViewArea " << (void*)w << " " << w->name() << " maxY=" << maxY << " m_availableHeight=" << m_availableHeight << " textdoc->height()=" << textdoc->height() << endl;
    if ( maxY >= m_availableHeight ) // Speedup
        maxY = m_availableHeight;
    else
    {
        // Convert to internal qtextdoc coordinates
        QPoint iPoint;
        if ( contentsToInternal( QPoint(0, maxY), iPoint, true /* only care for Y */ ) )
            maxY = iPoint.y();
        else // not found, assume worse
            maxY = m_availableHeight;
    }
    //kdDebug(32002) << "KWTextFrameSet::updateViewArea maxY now " << maxY << endl;
    // Update map
    m_mapViewAreas.replace( w, maxY );

    formatMore();
}

void KWTextFrameSet::UndoRedoInfo::clear()
{
    if ( valid() ) {
        QTextDocument * textdoc = textfs->textDocument();
        QTextCommand * cmd = 0;
        switch (type) {
            case Insert:
            case Return:
                cmd = new KWTextInsertCommand( textdoc, id, index, text.rawData(), customItemsMap, oldParagLayouts );
                break;
            case Delete:
            case RemoveSelected:
                cmd = new KWTextDeleteCommand( textdoc, id, index, text.rawData(), customItemsMap, oldParagLayouts );
                // Deleting any custom items -> macro command, to let custom items add their command
                if ( !customItemsMap.isEmpty() )
                {
                    textdoc->addCommand( cmd );
                    KMacroCommand * macroCmd = new KMacroCommand( name );
                    macroCmd->addCommand( new KWTextCommand( textfs, /*cmd, */name ) );
                    CustomItemsMap::Iterator it = customItemsMap.begin();
                    for ( ; it != customItemsMap.end(); ++it )
                    {
                        KWTextCustomItem * item = static_cast<KWTextCustomItem *>( it.data() );
                        item->addDeleteCommand( macroCmd );
                        item->setDeleted( true );
                    }
                    textfs->kWordDocument()->addCommand( macroCmd );
                    cmd = 0L;
                }
                break;
            case Format:
                cmd = new QTextFormatCommand( textdoc, id, index, eid, eindex, text.rawData(), format, flags );
                break;
            case Alignment:
                cmd = new KWTextParagCommand( textdoc, id, eid, oldParagLayouts, newParagLayout, KWTextParagCommand::Alignment );
                break;
            case Counter:
                //kdDebug() << "KWTextFrameSet::UndoRedoInfo::clear Counter undo/redo info, countertype=" << canvas->currentParagLayout().counter.counterType << " oldParagLayouts.count()=" << oldParagLayouts.count() << endl;
                cmd = new KWTextParagCommand( textdoc, id, eid, oldParagLayouts, newParagLayout, KWTextParagCommand::Counter );
                break;
            case Margin:
                cmd = new KWTextParagCommand( textdoc, id, eid, oldParagLayouts, newParagLayout, KWTextParagCommand::Margin, margin );
                break;
	    case LineSpacing:
	        cmd = new KWTextParagCommand( textdoc, id, eid, oldParagLayouts, newParagLayout, KWTextParagCommand::LineSpacing );
	        break;
	    case Borders:
                cmd = new KWTextParagCommand( textdoc, id, eid, oldParagLayouts, newParagLayout, KWTextParagCommand::Borders );
	        break;
            case Tabulator:
                cmd = new KWTextParagCommand( textdoc, id, eid, oldParagLayouts, newParagLayout, KWTextParagCommand::Tabulator );
	        break;
            case PageBreaking:
                cmd = new KWTextParagCommand( textdoc, id, eid, oldParagLayouts, newParagLayout, KWTextParagCommand::PageBreaking );
                break;
            case Invalid:
                break;
            default:
                kdDebug(32001) << "KWTextFrameSet::UndoRedoInfo::clear unknown type " << type << endl;
        }

        if ( cmd )
        {
            textdoc->addCommand( cmd );
            textfs->kWordDocument()->addCommand( new KWTextCommand( textfs, /*cmd, */name ) );
            //kdDebug(32001) << "KWTextFrameSet::UndoRedoInfo::clear New KWTextCommand : " << name << endl;
        }
    }
    text = QString::null; // calls QTextString::clear(), which calls resize(0) on the array, which _detaches_. Tricky.
    id = -1;
    index = -1;
    oldParagLayouts.clear();
    customItemsMap.clear();
}

KWTextFrameSet::UndoRedoInfo::UndoRedoInfo( KWTextFrameSet *fs )
    : type( Invalid ), textfs(fs), cursor( 0 )
{
    text = QString::null;
    id = -1;
    index = -1;
}

KWTextFrameSet::UndoRedoInfo::~UndoRedoInfo()
{
}

bool KWTextFrameSet::UndoRedoInfo::valid() const
{
    return text.length() > 0  && id >= 0 && index >= 0;
}

// Copies a formatted char, <ch>, into undoRedoInfo.text, at position <index>.
void KWTextFrameSet::copyCharFormatting( QTextStringChar * ch, int index /*in text*/, bool moveCustomItems )
{
    if ( ch->format() ) {
        ch->format()->addRef();
        undoRedoInfo.text.at( index ).setFormat( ch->format() );
    }
    if ( moveCustomItems && ch->isCustom() )
    {
        kdDebug(32001) << "KWTextFrameSet::copyCharFormatting moving custom item " << ch->customItem() << " to text's " << index << " char"  << endl;
        undoRedoInfo.customItemsMap.insert( index, ch->customItem() );
        ch->loseCustomItem();
    }
}

// Based on QTextView::readFormats - with all code duplication moved to copyCharFormatting
void KWTextFrameSet::readFormats( QTextCursor &c1, QTextCursor &c2, int oldLen, bool copyParagLayouts, bool moveCustomItems )
{
    c2.restoreState();
    c1.restoreState();
    if ( c1.parag() == c2.parag() ) {
        for ( int i = c1.index(); i < c2.index(); ++i )
            copyCharFormatting( c1.parag()->at( i ), oldLen + i - c1.index(), moveCustomItems );
    } else {
        int lastIndex = oldLen;
        int i;
        for ( i = c1.index(); i < c1.parag()->length(); ++i, ++lastIndex )
            copyCharFormatting( c1.parag()->at( i ), lastIndex, moveCustomItems );
        lastIndex++; // '\n'
        QTextParag *p = c1.parag()->next();
        while ( p && p != c2.parag() ) {
            for ( i = 0; i < p->length(); ++i )
                copyCharFormatting( p->at( i ), i + lastIndex, moveCustomItems );
            lastIndex += p->length() + 1;
            p = p->next();
        }
        for ( i = 0; i < c2.index(); ++i )
            copyCharFormatting( c2.parag()->at( i ), i + lastIndex, moveCustomItems );
    }

    if ( copyParagLayouts ) {
        QTextParag *p = c1.parag();
        while ( p ) {
            undoRedoInfo.oldParagLayouts << static_cast<KWTextParag*>(p)->paragLayout();
            if ( p == c2.parag() )
                break;
            p = p->next();
        }
    }
}

void KWTextFrameSet::applyStyle( QTextCursor * cursor, const KWStyle * newStyle, int selectionId )
{
    QTextDocument * textdoc = textDocument();
    emit hideCursor();

    /// Applying a style is three distinct operations :
    /// 1 - Changing the paragraph settings (setParagLayout)
    /// 2 - Changing the character formatting for each char in the paragraph (setFormat(indices))
    /// 3 - Changing the character formatting for the whole paragraph (setFormat()), for the counters
    /// -> We need a macro command to hold both changes
    KMacroCommand * macroCmd = new KMacroCommand( i18n("Apply style %1").arg(newStyle->name()) );

    // 1
    kdDebug(32001) << "KWTextFrameSet::applyStyle setParagLayout" << endl;
    storeParagUndoRedoInfo( cursor, selectionId );
    undoRedoInfo.type = UndoRedoInfo::Invalid; // tricky, we don't want clear() to create a command
    if ( !textdoc->hasSelection( selectionId ) ) {
	static_cast<KWTextParag*>(cursor->parag())->setParagLayout( newStyle->paragLayout() );
    } else {
	QTextParag *start = textdoc->selectionStart( selectionId );
	QTextParag *end = textdoc->selectionEnd( selectionId );
        for ( ; start && start != end->next() ; start = start->next() )
            static_cast<KWTextParag*>(start)->setParagLayout( newStyle->paragLayout() );
    }

    if ( cursor )
    {
        //kdDebug(32001) << "KWTextFrameSet::applyStyle KWTextParagCommand" << endl;
        QTextCommand * cmd = new KWTextParagCommand( textdoc, undoRedoInfo.id, undoRedoInfo.eid, undoRedoInfo.oldParagLayouts,
                                                     newStyle->paragLayout(), KWTextParagCommand::All );
        textdoc->addCommand( cmd );
        macroCmd->addCommand( new KWTextCommand( this, /*cmd, */QString::null ) );
    }

    // 2
    //kdDebug(32001) << "KWTextFrameSet::applyStyle gathering text and formatting" << endl;
    QValueList<QTextFormat *> lstFormats;
    QTextParag * firstParag;
    QTextParag * lastParag;
    if ( !textdoc->hasSelection( selectionId ) ) {
        // No selection -> apply style formatting to the whole paragraph
        firstParag = cursor->parag();
        lastParag = cursor->parag();
    }
    else
    {
        firstParag = textdoc->selectionStart( selectionId );
        lastParag = textdoc->selectionEnd( selectionId );
    }

    QTextFormat * newFormat = zoomFormatFont( & newStyle->format() );

    if ( cursor )
    {
        QString str;
        for ( QTextParag * parag = firstParag ; parag && parag != lastParag->next() ; parag = parag->next() )
        {
            str += parag->string()->toString(); // ## do we need to add a '\n' here ?
            lstFormats.append( parag->paragFormat() );
        }

        QTextCursor c1( textdoc );
        c1.setParag( firstParag );
        c1.setIndex( 0 );
        QTextCursor c2( textdoc );
        c2.setParag( lastParag );
        c2.setIndex( lastParag->string()->length()-1 );
        undoRedoInfo.clear();
        undoRedoInfo.type = UndoRedoInfo::Invalid; // same trick
        undoRedoInfo.text = str;
        readFormats( c1, c2, 0 ); // gather char-format info but not paraglayouts nor customitems

        QTextCommand * cmd = new QTextFormatCommand( textdoc, firstParag->paragId(), 0,
                                                     lastParag->paragId(), c2.index(),
                                                     undoRedoInfo.text.rawData(), newFormat,
                                                     QTextFormat::Format );
        textdoc->addCommand( cmd );
        macroCmd->addCommand( new KWTextCommand( this, /*cmd, */QString::null ) );

        // sub-command for '3' (paragFormat)
        cmd = new KWParagFormatCommand( textdoc, firstParag->paragId(), lastParag->paragId(),
                                        lstFormats, newFormat );
        textdoc->addCommand( cmd );
        macroCmd->addCommand( new KWTextCommand( this, /*cmd, */QString::null ) );
    }

    // apply '2' and '3' (format)
    for ( QTextParag * parag = firstParag ; parag && parag != lastParag->next() ; parag = parag->next() )
    {
        //kdDebug(32001) << "KWTextFrameSet::applyStyle " << parag->string()->length() << endl;
        parag->setFormat( 0, parag->string()->length(), newFormat, true );
        parag->setFormat( newFormat ); // set default format (for counter)
    }


    if ( cursor )
    {
        setLastFormattedParag( firstParag );
        emit repaintChanged( this );
        formatMore();
        emit updateUI();
        m_doc->addCommand( macroCmd );
        //m_doc->setModified(true);
    }

    undoRedoInfo.clear();
    emit showCursor();
}

// This prepares undoRedoInfo for a paragraph formatting change
// If this does too much, we could pass an enum flag to it.
// But the main point is to avoid too much duplicated code
void KWTextFrameSet::storeParagUndoRedoInfo( QTextCursor * cursor, int selectionId )
{
    undoRedoInfo.clear();
    QTextDocument * textdoc = textDocument();
    undoRedoInfo.oldParagLayouts.clear();
    undoRedoInfo.text = " ";
    undoRedoInfo.index = 1;
    if ( !textdoc->hasSelection( selectionId ) ) {
        QTextParag * p = cursor->parag();
        undoRedoInfo.id = p->paragId();
        undoRedoInfo.eid = p->paragId();
        undoRedoInfo.oldParagLayouts << static_cast<KWTextParag*>(p)->paragLayout();
    }
    else{
	QTextParag *start = textdoc->selectionStart( selectionId );
	QTextParag *end = textdoc->selectionEnd( selectionId );
        undoRedoInfo.id = start->paragId();
        undoRedoInfo.eid = end->paragId();
        for ( ; start && start != end->next() ; start = start->next() )
        {
            undoRedoInfo.oldParagLayouts << static_cast<KWTextParag*>(start)->paragLayout();
            //kdDebug(32001) << "KWTextFrameSet::storeParagUndoRedoInfo storing counter " << static_cast<KWTextParag*>(start)->paragLayout().counter.counterType << endl;
        }
    }
}

void KWTextFrameSet::setCounter( QTextCursor * cursor, const Counter & counter )
{
    QTextDocument * textdoc = textDocument();
    const Counter * curCounter = static_cast<KWTextParag*>(cursor->parag())->counter();
    if ( !textdoc->hasSelection( QTextDocument::Standard ) &&
	 curCounter && counter == *curCounter )
        return;
    emit hideCursor();
    storeParagUndoRedoInfo( cursor );
    undoRedoInfo.type = UndoRedoInfo::Counter;
    undoRedoInfo.name = i18n("Change list type"); // Is that name correct ?
    if ( !textdoc->hasSelection( QTextDocument::Standard ) ) {
	static_cast<KWTextParag*>(cursor->parag())->setCounter( counter );
	emit repaintChanged( this );
    } else {
	QTextParag *start = textdoc->selectionStart( QTextDocument::Standard );
	QTextParag *end = textdoc->selectionEnd( QTextDocument::Standard );
        setLastFormattedParag( start );
        for ( ; start && start != end->next() ; start = start->next() )
            static_cast<KWTextParag*>(start)->setCounter( counter );
	emit repaintChanged( this );
	formatMore();
    }
    if ( !undoRedoInfo.newParagLayout.counter )
        undoRedoInfo.newParagLayout.counter = new Counter;
    *undoRedoInfo.newParagLayout.counter = counter;
    undoRedoInfo.clear();
    emit showCursor();
    emit updateUI();
}

void KWTextFrameSet::setAlign( QTextCursor * cursor, int align )
{
    QTextDocument * textdoc = textDocument();
    if ( !textdoc->hasSelection( QTextDocument::Standard ) &&
         cursor->parag()->alignment() == align )
        return; // No change needed.

    emit hideCursor();
    storeParagUndoRedoInfo( cursor );
    undoRedoInfo.type = UndoRedoInfo::Alignment;
    undoRedoInfo.name = i18n("Change Alignment");
    if ( !textdoc->hasSelection( QTextDocument::Standard ) ) {
        static_cast<KWTextParag *>(cursor->parag())->setAlign(align);
        emit repaintChanged( this );
    }
    else
    {
	QTextParag *start = textDocument()->selectionStart( QTextDocument::Standard );
	QTextParag *end = textDocument()->selectionEnd( QTextDocument::Standard );
        setLastFormattedParag( start );
        for ( ; start && start != end->next() ; start = start->next() )
            static_cast<KWTextParag *>(start)->setAlign(align);
	emit repaintChanged( this );
	formatMore();
    }
    undoRedoInfo.newParagLayout.alignment = align;
    undoRedoInfo.clear();
    emit showCursor();
    emit updateUI();
}

void KWTextFrameSet::setMargin( QTextCursor * cursor, QStyleSheetItem::Margin m, double margin ) {
    QTextDocument * textdoc = textDocument();
    //kdDebug(32001) << "KWTextFrameSet::setMargin " << m << " to value " << margin << endl;
    //kdDebug(32001) << "Current margin is " << static_cast<KWTextParag *>(cursor->parag())->margin(m) << endl;
    if ( !textdoc->hasSelection( QTextDocument::Standard ) &&
         static_cast<KWTextParag *>(cursor->parag())->margin(m) == margin )
        return; // No change needed.

    emit hideCursor();
    storeParagUndoRedoInfo( cursor );
    undoRedoInfo.type = UndoRedoInfo::Margin;
    undoRedoInfo.margin = m;
    if ( m == QStyleSheetItem::MarginFirstLine )
        undoRedoInfo.name = i18n("Change First Line Indent");
    else if ( m == QStyleSheetItem::MarginLeft || m == QStyleSheetItem::MarginRight )
        undoRedoInfo.name = i18n("Change Indent");
    else
        undoRedoInfo.name = i18n("Change Paragraph Spacing");
    if ( !textdoc->hasSelection( QTextDocument::Standard ) ) {
        static_cast<KWTextParag *>(cursor->parag())->setMargin(m, margin);
    }
    else
    {
	QTextParag *start = textDocument()->selectionStart( QTextDocument::Standard );
	QTextParag *end = textDocument()->selectionEnd( QTextDocument::Standard );
        setLastFormattedParag( start );
        for ( ; start && start != end->next() ; start = start->next() )
            static_cast<KWTextParag *>(start)->setMargin(m, margin);
    }
    emit repaintChanged( this );
    formatMore();
    undoRedoInfo.newParagLayout.margins[m] = margin;
    undoRedoInfo.clear();
    emit showCursor();
    emit updateUI();
}

void KWTextFrameSet::setLineSpacing( QTextCursor * cursor, double spacing )
{
    QTextDocument * textdoc = textDocument();
    //kdDebug(32001) << "KWTextFrameSet::setLineSpacing to value " << spacing << endl;
    //kdDebug(32001) << "Current spacing is " << static_cast<KWTextParag *>(cursor->parag())->kwLineSpacing() << endl;
    //kdDebug(32001) << "Comparison says " << ( static_cast<KWTextParag *>(cursor->parag())->kwLineSpacing() == spacing ) << endl;
    //kdDebug(32001) << "hasSelection " << textdoc->hasSelection( QTextDocument::Standard ) << endl;
    if ( !textdoc->hasSelection( QTextDocument::Standard ) &&
         static_cast<KWTextParag *>(cursor->parag())->kwLineSpacing() == spacing )
        return; // No change needed.

    emit hideCursor();
    storeParagUndoRedoInfo( cursor );
    undoRedoInfo.type = UndoRedoInfo::LineSpacing;
    undoRedoInfo.name = i18n("Change Line Spacing");
    if ( !textdoc->hasSelection( QTextDocument::Standard ) ) {
        static_cast<KWTextParag *>(cursor->parag())->setLineSpacing(spacing);
        emit repaintChanged( this );
    }
    else
    {
	QTextParag *start = textDocument()->selectionStart( QTextDocument::Standard );
	QTextParag *end = textDocument()->selectionEnd( QTextDocument::Standard );
        setLastFormattedParag( start );
        for ( ; start && start != end->next() ; start = start->next() )
            static_cast<KWTextParag *>(start)->setLineSpacing(spacing);
	emit repaintChanged( this );
	formatMore();
    }
    undoRedoInfo.newParagLayout.lineSpacing = spacing;
    undoRedoInfo.clear();
    emit showCursor();
}


void KWTextFrameSet::setBorders( QTextCursor * cursor, Border leftBorder, Border rightBorder, Border topBorder, Border bottomBorder )
{
  QTextDocument * textdoc = textDocument();
  if ( !textdoc->hasSelection( QTextDocument::Standard ) &&
       static_cast<KWTextParag *>(cursor->parag())->leftBorder() ==leftBorder &&
       static_cast<KWTextParag *>(cursor->parag())->rightBorder() ==rightBorder &&
       static_cast<KWTextParag *>(cursor->parag())->topBorder() ==topBorder &&
       static_cast<KWTextParag *>(cursor->parag())->bottomBorder() ==bottomBorder )
        return; // No change needed.

    emit hideCursor();
    storeParagUndoRedoInfo( cursor );
    undoRedoInfo.type = UndoRedoInfo::Borders;
    undoRedoInfo.name = i18n("Change Borders");
    if ( !textdoc->hasSelection( QTextDocument::Standard ) ) {
      static_cast<KWTextParag *>(cursor->parag())->setLeftBorder(leftBorder);
      static_cast<KWTextParag *>(cursor->parag())->setRightBorder(rightBorder);
      static_cast<KWTextParag *>(cursor->parag())->setBottomBorder(bottomBorder);
      static_cast<KWTextParag *>(cursor->parag())->setTopBorder(topBorder);
    }
    else
    {
	QTextParag *start = textDocument()->selectionStart( QTextDocument::Standard );
	QTextParag *end = textDocument()->selectionEnd( QTextDocument::Standard );
        setLastFormattedParag( start );
	Border tmpBorder;
	tmpBorder.ptWidth=0;
        for ( ; start && start != end->next() ; start = start->next() )
	  {
	    static_cast<KWTextParag *>(start)->setLeftBorder(leftBorder);
	    static_cast<KWTextParag *>(start)->setRightBorder(rightBorder);
	    //remove border
	    static_cast<KWTextParag *>(start)->setTopBorder(tmpBorder);
	    static_cast<KWTextParag *>(start)->setBottomBorder(tmpBorder);
	  }
	static_cast<KWTextParag *>(end)->setBottomBorder(bottomBorder);
	static_cast<KWTextParag *>(textDocument()->selectionStart( QTextDocument::Standard ))->setTopBorder(topBorder);
    }
    formatMore();
    emit repaintChanged( this );
    undoRedoInfo.newParagLayout.leftBorder=leftBorder;
    undoRedoInfo.newParagLayout.rightBorder=rightBorder;
    undoRedoInfo.newParagLayout.topBorder=topBorder;
    undoRedoInfo.newParagLayout.bottomBorder=bottomBorder;
    undoRedoInfo.clear();
    emit showCursor();
    emit updateUI();
}


void KWTextFrameSet::setTabList( QTextCursor * cursor, const KoTabulatorList &tabList )
{
    QTextDocument * textdoc = textDocument();
    if ( !textdoc->hasSelection( QTextDocument::Standard ) && static_cast<KWTextParag *>(cursor->parag())->tabList() == tabList )
        return; // No change needed.

    emit hideCursor();

    storeParagUndoRedoInfo( cursor );
    undoRedoInfo.type = UndoRedoInfo::Tabulator;
    undoRedoInfo.name = i18n("Change Tabulator");

    if ( !textdoc->hasSelection( QTextDocument::Standard ) ) {
        static_cast<KWTextParag *>(cursor->parag())->setTabList( tabList );
        emit repaintChanged( this );
    }
    else
    {
	QTextParag *start = textDocument()->selectionStart( QTextDocument::Standard );
	QTextParag *end = textDocument()->selectionEnd( QTextDocument::Standard );
        setLastFormattedParag( start );
        for ( ; start && start != end->next() ; start = start->next() )
            static_cast<KWTextParag *>(start)->setTabList( tabList );
	emit repaintChanged( this );
	formatMore();
    }

    undoRedoInfo.newParagLayout.setTabList( tabList );
    undoRedoInfo.clear();
    emit showCursor();
    emit updateUI();
}

void KWTextFrameSet::setPageBreaking( QTextCursor * cursor, bool linesTogether )
{
    QTextDocument * textdoc = textDocument();
    if ( !textdoc->hasSelection( QTextDocument::Standard ) &&
         static_cast<KWTextParag *>(cursor->parag())->linesTogether() == linesTogether )
        return; // No change needed.

    emit hideCursor();

    storeParagUndoRedoInfo( cursor );
    undoRedoInfo.type = UndoRedoInfo::PageBreaking;
    undoRedoInfo.name = i18n("Change paragraph attribute"); // bleh

    if ( !textdoc->hasSelection( QTextDocument::Standard ) ) {
        static_cast<KWTextParag *>(cursor->parag())->setLinesTogether( linesTogether );
        emit repaintChanged( this );
    }
    else
    {
	QTextParag *start = textDocument()->selectionStart( QTextDocument::Standard );
	QTextParag *end = textDocument()->selectionEnd( QTextDocument::Standard );
        setLastFormattedParag( start );
        for ( ; start && start != end->next() ; start = start->next() )
            static_cast<KWTextParag *>(start)->setLinesTogether( linesTogether );
	emit repaintChanged( this );
    }

    formatMore();
    undoRedoInfo.newParagLayout.linesTogether = linesTogether;
    undoRedoInfo.clear();
    emit showCursor();
    emit updateUI();
}


void KWTextFrameSet::removeSelectedText( QTextCursor * cursor, int selectionId )
{
    QTextDocument * textdoc = textDocument();
    //for ( int i = 1; i < (int)QTextDocument::Temp; ++i )
    //   textdoc->removeSelection( i );
    emit hideCursor();
    checkUndoRedoInfo( cursor, UndoRedoInfo::RemoveSelected );
    if ( !undoRedoInfo.valid() ) {
	textdoc->selectionStart( selectionId, undoRedoInfo.id, undoRedoInfo.index );
	undoRedoInfo.text = QString::null;
        undoRedoInfo.name = i18n("Remove Selected Text");
    }
    int oldLen = undoRedoInfo.text.length();
    undoRedoInfo.text = textdoc->selectedText( selectionId );
    QTextCursor c1 = textdoc->selectionStartCursor( selectionId );
    QTextCursor c2 = textdoc->selectionEndCursor( selectionId );
    readFormats( c1, c2, oldLen, true, true );

    textdoc->removeSelectedText( selectionId, cursor );

    emit ensureCursorVisible();
    setLastFormattedParag( cursor->parag() );
    formatMore();
    emit repaintChanged( this );
    emit ensureCursorVisible();
    emit showCursor();
    undoRedoInfo.clear();
}

void KWTextFrameSet::insert( QTextCursor * cursor, KWTextFormat * currentFormat,
                             const QString &txt, bool checkNewLine,
                             bool removeSelected, const QString & commandName )
{
    //kdDebug(32001) << "KWTextFrameSet::insert" << endl;
    QTextDocument *textdoc = textDocument();
    emit hideCursor();
    if ( textdoc->hasSelection( QTextDocument::Standard ) && removeSelected ) {
        removeSelectedText( cursor );
    }
    QTextCursor c2 = *cursor;
    checkUndoRedoInfo( cursor, UndoRedoInfo::Insert );
    if ( !undoRedoInfo.valid() ) {
	undoRedoInfo.id = cursor->parag()->paragId();
	undoRedoInfo.index = cursor->index();
	undoRedoInfo.text = QString::null;
        undoRedoInfo.name = commandName;
    }
    int oldLen = undoRedoInfo.text.length();
    setLastFormattedParag( checkNewLine && cursor->parag()->prev() ?
                           cursor->parag()->prev() : cursor->parag() );
    QTextCursor oldCursor = *cursor;
    cursor->insert( txt, checkNewLine );  // insert the text

    if ( textdoc->useFormatCollection() ) { // (always true)   apply the formatting
        textdoc->setSelectionStart( QTextDocument::Temp, &oldCursor );
        textdoc->setSelectionEnd( QTextDocument::Temp, cursor );
        textdoc->setFormat( QTextDocument::Temp, currentFormat, QTextFormat::Format );
        textdoc->removeSelection( QTextDocument::Temp );
    }

    formatMore();
    emit repaintChanged( this );
    emit ensureCursorVisible();
    emit showCursor();
    undoRedoInfo.text += txt;

    for ( int i = 0; i < (int)txt.length(); ++i ) {
        if ( txt[ oldLen + i ] != '\n' )
            copyCharFormatting( c2.parag()->at( c2.index() ), oldLen + i, true );
        c2.gotoRight();
    }

    m_doc->setModified(true);
    if ( !removeSelected ) {
        // ## not sure why we do this. I'd prefer leaving the selection unchanged...
        // but then it'd need adjustements in the offsets etc.
        textdoc->removeSelection( QTextDocument::Standard );
        emit repaintChanged( this );
    }
}

void KWTextFrameSet::undo()
{
    undoRedoInfo.clear();
    emit hideCursor();
    QTextCursor *cursor = new QTextCursor( textDocument() ); // Kindof a dummy cursor
    QTextCursor *c = textDocument()->undo( cursor );
    if ( !c ) {
	emit showCursor();
	return;
    }
    // We have to set this new cursor position in all views :(
    // It sucks a bit for useability, but otherwise one view might still have
    // a cursor inside a deleted paragraph -> crash.
    emit setCursor( c );
    setLastFormattedParag( textdoc->firstParag() );
    emit ensureCursorVisible();
    emit repaintChanged( this );
    emit updateUI();
    emit showCursor();
    formatMore();
}

void KWTextFrameSet::redo()
{
    undoRedoInfo.clear();
    emit hideCursor();
    QTextCursor *cursor = new QTextCursor( textDocument() ); // Kindof a dummy cursor
    QTextCursor *c = textDocument()->redo( cursor );
    if ( !c ) {
	emit showCursor();
	return;
    }
    emit setCursor( c ); // see undo
    setLastFormattedParag( textdoc->firstParag() );
    emit repaintChanged( this );
    emit ensureCursorVisible();
    emit updateUI();
    emit showCursor();
    formatMore();
}

void KWTextFrameSet::clearUndoRedoInfo()
{
    undoRedoInfo.clear();
}

void KWTextFrameSet::pasteText( QTextCursor * cursor, const QString & text, KWTextFormat * currentFormat, bool removeSelected )
{
    kdDebug(32001) << "KWTextFrameSet::pasteText" << endl;
    QString t = text;
    // Need to convert CRLF to NL
    QRegExp crlf( QString::fromLatin1("\r\n") );
    t.replace( crlf, QChar('\n') );
    // Convert non-printable chars
    for ( int i=0; (uint) i<t.length(); i++ ) {
        if ( t[ i ] < ' ' && t[ i ] != '\n' && t[ i ] != '\t' )
            t[ i ] = ' ';
    }
    if ( !t.isEmpty() )
        insert( cursor, currentFormat, t, true /*checkNewLine*/, removeSelected, i18n("Paste Text") );
}

void KWTextFrameSet::pasteKWord( QTextCursor * cursor, const QCString & data, bool removeSelected )
{
    QTextDocument *textdoc = textDocument();
    if ( removeSelected && textdoc->hasSelection( QTextDocument::Standard ) )
        removeSelectedText( cursor );
    emit hideCursor();
    setLastFormattedParag( cursor->parag()->prev() ?
                           cursor->parag()->prev() : cursor->parag() );

    // We have our own command for this.
    // Using insert() wouldn't help storing the parag stuff for redo
    KWPasteCommand * cmd = new KWPasteCommand( textDocument(), cursor->parag()->paragId(), cursor->index(), data );
    textDocument()->addCommand( cmd );
    m_doc->addCommand( new KWTextCommand( this, /*cmd, */i18n("Paste Text") ) ); // the wrapper KCommand
    //m_doc->setModified(true);
    *cursor = *( cmd->execute( cursor ) );

    formatMore();
    emit repaintChanged( this );
    emit ensureCursorVisible();
    emit showCursor();
}

void KWTextFrameSet::setLastFormattedParag( QTextParag *parag )
{
    if ( !m_lastFormatted || m_lastFormatted->paragId() > parag->paragId() )
        m_lastFormatted = parag;
}

void KWTextFrameSet::checkUndoRedoInfo( QTextCursor * cursor, UndoRedoInfo::Type t )
{
    if ( undoRedoInfo.valid() && ( t != undoRedoInfo.type || cursor != undoRedoInfo.cursor ) ) {
	undoRedoInfo.clear();
    }
    undoRedoInfo.type = t;
    undoRedoInfo.cursor = cursor;
}

void KWTextFrameSet::setFormat( QTextCursor * cursor, KWTextFormat * & currentFormat, KWTextFormat *format, int flags, bool zoomFont )
{
    QTextDocument * textdoc = textDocument();
    ASSERT( currentFormat );
    bool newFormat = ( currentFormat && currentFormat->key() != format->key() );
    if ( newFormat )
    {
        int origFontSize = 0;
        if ( zoomFont ) // The format has an unzoomed font (e.g. setting a style, or a new font size)
        {
            origFontSize = format->font().pointSize();
            static_cast<KWTextFormat *>(format)->setPointSizeFloat( zoomedFontSize( origFontSize ) );
            kdDebug(32001) << "KWTextFrameSet::setFormat format " << format->key() << " zoomed from " << origFontSize << " to " << format->font().pointSizeFloat() << endl;
        }
        else // The format has a zoomed font already (we're only changing bold/italic, or the family, etc.)
        {
            kdDebug(32001) << "KWTextFrameSet::setFormat font already zoomed,  finding orig size" << endl;
            if ( currentFormat->font().pointSizeFloat() == format->font().pointSizeFloat() )
            {
                // read orig size from previous format, to reuse it.
                int * pDocFontSize = m_origFontSizes.find( currentFormat );
                if ( !pDocFontSize )
                {
                    ASSERT( currentFormat->parent() );
                    kdWarning() << currentFormat << " " << currentFormat->key() << " not in m_origFontSizes " << endl;
                    QPtrDictIterator<int> it( m_origFontSizes );
                    for ( ; it.current() ; ++it ) {
                        kdDebug() << " m_origFontSizes : format=" << it.currentKey()
                                  << " key=" << static_cast<QTextFormat*>(it.currentKey())->key()
                                  << " size=" << it.current() << endl;
                    }
                }
                else
                    origFontSize = *pDocFontSize;
            }
            else // shouldn't happen ?
                kdWarning() << "Zoomed font, no clue about doc-font-size for it " << currentFormat->key() << " old: " << format->key() << endl;
        }

        // Find format in collection
        currentFormat->removeRef();
        currentFormat = static_cast<KWTextFormat *>( textdoc->formatCollection()->format( format ) );
        if ( currentFormat->isMisspelled() ) {
            currentFormat->removeRef();
            currentFormat = static_cast<KWTextFormat *>( textdoc->formatCollection()->format( currentFormat->font(), currentFormat->color() ) );
        }

        if ( origFontSize > 0 && ! m_origFontSizes.find( currentFormat ) )
        {
            kdDebug() << "KWTextFrameSet::setFormat inserting entry for " << currentFormat->key() << "  origFontSize=" << origFontSize << endl;
            m_origFontSizes.insert( currentFormat, new int( origFontSize ) );
        }
    }

    if ( textdoc->hasSelection( QTextDocument::Standard ) ) {
        emit hideCursor();
        QString str = textdoc->selectedText( QTextDocument::Standard );
        QTextCursor c1 = textdoc->selectionStartCursor( QTextDocument::Standard );
        QTextCursor c2 = textdoc->selectionEndCursor( QTextDocument::Standard );
        undoRedoInfo.clear();
        undoRedoInfo.type = UndoRedoInfo::Format;
        undoRedoInfo.name = i18n("Format text");
        undoRedoInfo.id = c1.parag()->paragId();
        undoRedoInfo.index = c1.index();
        undoRedoInfo.eid = c2.parag()->paragId();
        undoRedoInfo.eindex = c2.index();
        undoRedoInfo.text = str;
        readFormats( c1, c2, 0 ); // read previous formatting info
        undoRedoInfo.format = format;
        undoRedoInfo.flags = flags;
        undoRedoInfo.clear();
        //kdDebug(32001) << "KWTextFrameSet::setFormat undoredo info done" << endl;
        textdoc->setFormat( QTextDocument::Standard, format, flags );
        emit repaintChanged( this );
        formatMore();
        emit showCursor();
    }
    //kdDebug(32001) << "KWTextFrameSet::setFormat currentFormat:" << currentFormat->key() << " new format:" << format->key() << endl;
    if ( newFormat ) {
        emit showCurrentFormat();
        //kdDebug(32001) << "KWTextFrameSet::setFormat index=" << cursor->index() << " length-1=" << cursor->parag()->length() - 1 << endl;
        if ( cursor->index() == cursor->parag()->length() - 1 ) {
            currentFormat->addRef();
            cursor->parag()->string()->setFormat( cursor->index(), currentFormat, TRUE );
        }
    }
}

void KWTextFrameSet::selectAll( bool select )
{
    if ( !select )
	textDocument()->removeSelection( QTextDocument::Standard );
    else
	textDocument()->selectAll( QTextDocument::Standard );
    selectionChangedNotify();
}

void KWTextFrameSet::selectionChangedNotify( bool enableActions /* = true */)
{
    emit repaintChanged( this );
    if ( enableActions )
        emit selectionChanged( hasSelection() );
}

QRect KWTextFrameSet::paragRect( QTextParag * parag ) const
{
    QPoint topLeft;
    (void)internalToContents( parag->rect().topLeft(), topLeft );
    QPoint bottomRight;
    (void)internalToContents( parag->rect().bottomRight(), bottomRight );
    return QRect( topLeft, bottomRight );
}

void KWTextFrameSet::findPosition( const QPoint &cPoint, QTextParag * & parag, int & index )
{
    QTextCursor cursor( textdoc );

    QPoint iPoint;
    if ( contentsToInternal( cPoint, iPoint ) )
    {
        cursor.place( iPoint, textdoc->firstParag() );
        parag = cursor.parag();
        index = cursor.index();
    }
    else
    {
        // Not found, maybe under everything ?
        parag = textdoc->lastParag();
        if ( parag )
            index = parag->length() - 1;
    }
}

void KWTextFrameSet::highlightPortion( QTextParag * parag, int index, int length, KWCanvas * canvas )
{
    removeHighlight(); // remove previous highlighted selection
    QTextCursor cursor( textdoc );
    cursor.setParag( parag );
    cursor.setIndex( index );
    textdoc->setSelectionStart( HighlightSelection, &cursor );
    cursor.setIndex( index + length );
    textdoc->setSelectionEnd( HighlightSelection, &cursor );
    parag->setChanged( true );
    emit repaintChanged( this );
    QRect expose = paragRect( parag );
    canvas->ensureVisible( (expose.left()+expose.right()) / 2,  // point = center of the rect
                           (expose.top()+expose.bottom()) / 2,
                           (expose.right()-expose.left()) / 2,  // margin = half-width of the rect
                           (expose.bottom()-expose.top()) / 2);
}

void KWTextFrameSet::removeHighlight()
{
    if ( textdoc->hasSelection( HighlightSelection ) )
    {
        QTextParag * oldParag = textdoc->selectionStart( HighlightSelection );
        oldParag->setChanged( true );
        textdoc->removeSelection( HighlightSelection );
    }
    emit repaintChanged( this );
}

#ifndef NDEBUG
void KWTextFrameSet::printRTDebug( int info )
{
    for (QTextParag * parag = textDocument()->firstParag(); parag ; parag = parag->next())
    {
        KWTextParag * p = static_cast<KWTextParag *>(parag);
        p->printRTDebug( info );
    }
}
#endif

///////////////////////////////////////////////////////////////////////////////

KWTextFrameSetEdit::KWTextFrameSetEdit( KWTextFrameSet * fs, KWCanvas * canvas )
    : KWFrameSetEdit( fs, canvas )
{
    connect( fs, SIGNAL( hideCursor() ), this, SLOT( hideCursor() ) );
    connect( fs, SIGNAL( showCursor() ), this, SLOT( showCursor() ) );
    connect( fs, SIGNAL( setCursor( QTextCursor * ) ), this, SLOT( setCursor( QTextCursor * ) ) );
    connect( fs, SIGNAL( updateUI() ), this, SLOT( updateUI() ) );
    connect( fs, SIGNAL( showCurrentFormat() ), this, SLOT( showCurrentFormat() ) );
    connect( fs, SIGNAL( ensureCursorVisible() ), this, SLOT( ensureCursorVisible() ) );
    connect( fs, SIGNAL( selectionChanged(bool) ), canvas, SIGNAL( selectionChanged(bool) ) );

    cursor = new QTextCursor( textDocument() );

    cursorVisible = TRUE;
    blinkTimer = new QTimer( this );
    connect( blinkTimer, SIGNAL( timeout() ),
	     this, SLOT( blinkCursor() ) );
    blinkTimer->start( QApplication::cursorFlashTime() / 2 );

    dragStartTimer = new QTimer( this );
    connect( dragStartTimer, SIGNAL( timeout() ),
	     this, SLOT( startDrag() ) );

    textFrameSet()->formatMore();

    blinkCursorVisible = FALSE;
    inDoubleClick = FALSE;
    mightStartDrag = FALSE;

    currentFormat = 0;
    updateUI();
}

KWTextFrameSetEdit::~KWTextFrameSetEdit()
{
    //kdDebug(32001) << "KWTextFrameSetEdit::~KWTextFrameSetEdit" << endl;
    delete cursor;
}

void KWTextFrameSetEdit::terminate()
{
    textFrameSet()->clearUndoRedoInfo();
    textDocument()->removeSelection( QTextDocument::Standard );
    textFrameSet()->selectionChangedNotify();
    hideCursor();
    disconnect( frameSet(), SIGNAL( selectionChanged(bool) ), m_canvas, SIGNAL( selectionChanged(bool) ) );
}

void KWTextFrameSetEdit::keyPressEvent( QKeyEvent * e )
{
#if 0
    // TODO Move to KWTextFrameSet
    changeIntervalTimer->stop();
    interval = 10;
#endif

    /* bool selChanged = FALSE;
    for ( int i = 1; i < textDocument()->numSelections(); ++i )
	selChanged = textDocument()->removeSelection( i ) || selChanged;

    if ( selChanged ) {
	// cursor->parag()->document()->nextDoubleBuffered = TRUE; ######## we need that only if we have nested items/documents
        textFrameSet()->selectionChangedNotify();
    }*/

    bool clearUndoRedoInfo = TRUE;

    switch ( e->key() ) {
    case Key_Left:
	moveCursor( MoveLeft, e->state() & ShiftButton, e->state() & ControlButton );
	break;
    case Key_Right:
	moveCursor( MoveRight, e->state() & ShiftButton, e->state() & ControlButton );
	break;
    case Key_Up:
	moveCursor( MoveUp, e->state() & ShiftButton, e->state() & ControlButton );
	break;
    case Key_Down:
	moveCursor( MoveDown, e->state() & ShiftButton, e->state() & ControlButton );
	break;
    case Key_Home:
	moveCursor( MoveHome, e->state() & ShiftButton, e->state() & ControlButton );
	break;
    case Key_End:
	moveCursor( MoveEnd, e->state() & ShiftButton, e->state() & ControlButton );
	break;
    case Key_Prior:
	moveCursor( MovePgUp, e->state() & ShiftButton, e->state() & ControlButton );
	break;
    case Key_Next:
	moveCursor( MovePgDown, e->state() & ShiftButton, e->state() & ControlButton );
	break;
    case Key_Return: case Key_Enter:
	if ( textDocument()->hasSelection( QTextDocument::Standard ) )
            textFrameSet()->removeSelectedText( cursor );
	clearUndoRedoInfo = FALSE;
	textFrameSet()->doKeyboardAction( cursor, KWTextFrameSet::ActionReturn );
	break;
    case Key_Delete:
	if ( textDocument()->hasSelection( QTextDocument::Standard ) ) {
            textFrameSet()->removeSelectedText( cursor );
	    break;
	}

        if ( e->state() & ControlButton )
        {
            // Delete a word
            do {
                textFrameSet()->doKeyboardAction( cursor, KWTextFrameSet::ActionDelete );
            } while ( !cursor->atParagEnd()
                      && !cursor->parag()->at( cursor->index() )->c.isSpace() );
        }
        else
            textFrameSet()->doKeyboardAction( cursor, KWTextFrameSet::ActionDelete );

       clearUndoRedoInfo = FALSE;
	break;
    case Key_Backspace:
	if ( textDocument()->hasSelection( QTextDocument::Standard ) ) {
	    textFrameSet()->removeSelectedText( cursor );
	    break;
	}

	if ( !cursor->parag()->prev() &&
	     cursor->atParagStart() )
	    break;

        if ( e->state() & ControlButton )
        {
            // Delete a word
            do {
                textFrameSet()->doKeyboardAction( cursor, KWTextFrameSet::ActionBackspace );
                //kdDebug() << "KWTextFrameSetEdit::keyPressEvent one char removed, current char="
                //          << cursor->parag()->at( cursor->index() )->c.latin1() << endl;
            } while ( !cursor->atParagStart()
                      && !cursor->parag()->at( cursor->index()-1 )->c.isSpace() );
        }
        else
            textFrameSet()->doKeyboardAction( cursor, KWTextFrameSet::ActionBackspace );

       clearUndoRedoInfo = FALSE;
	break;
    case Key_F16: // Copy key on Sun keyboards
	copy();
	break;
    case Key_F18:  // Paste key on Sun keyboards
	paste();
	break;
    case Key_F20:  // Cut key on Sun keyboards
	cut();
	break;
    default: {
            //kdDebug() << "KWTextFrameSetEdit::keyPressEvent ascii=" << e->ascii() << " text=" << e->text()[0].unicode() << endl;
	    if ( e->text().length() &&
//               !( e->state() & AltButton ) &&
		 ( !e->ascii() || e->ascii() >= 32 ) ||
		 ( e->text() == "\t" && !( e->state() & ControlButton ) ) ) {
		clearUndoRedoInfo = FALSE;
		if ( e->key() == Key_Tab ) {
                    // We don't have support for nested counters at the moment.
		    /*if ( cursor->index() == 0 && cursor->parag()->style() &&
			 cursor->parag()->style()->displayMode() == QStyleSheetItem::DisplayListItem ) {
			static_cast<KWTextParag * >(cursor->parag())->incDepth();
			emit hideCursor();
			emit repaintChanged();
			emit showCursor();
			break;
		    }*/
		}
                // Port to setCounter if we really want that
		/*if ( cursor->parag()->style() &&
		     cursor->parag()->style()->displayMode() == QStyleSheetItem::DisplayBlock &&
		     cursor->index() == 0 && ( e->text() == "-" || e->text() == "*" ) ) {
		    setParagType( QStyleSheetItem::DisplayListItem, QStyleSheetItem::ListDisc );
		} else {*/
		    textFrameSet()->insert( cursor, currentFormat, e->text(), false, true, i18n("Insert Text") );
		//}
		break;
	    }
            // We should use KAccel instead, to make this configurable !
	    if ( e->state() & ControlButton ) {
		switch ( e->key() ) {
                case Key_F16: // Copy key on Sun keyboards
		    copy();
		    break;
		case Key_A:
		    moveCursor( MoveHome, e->state() & ShiftButton, FALSE );
		    break;
		case Key_E:
		    moveCursor( MoveEnd, e->state() & ShiftButton, FALSE );
		    break;
		case Key_K:
                    textFrameSet()->doKeyboardAction( cursor, KWTextFrameSet::ActionKill );
		    break;
                case Key_Insert:
		    copy();
		    break;
		}
		break;
	    }
	}
    }

    if ( clearUndoRedoInfo ) {
        textFrameSet()->clearUndoRedoInfo();
    }
    // TODO changeIntervalTimer->start( 100, TRUE );
}

void KWTextFrameSetEdit::moveCursor( MoveDirectionPrivate direction, bool shift, bool control )
{
    hideCursor();
    if ( shift ) {
	if ( !textDocument()->hasSelection( QTextDocument::Standard ) )
	    textDocument()->setSelectionStart( QTextDocument::Standard, cursor );
	moveCursor( direction, control );
	if ( textDocument()->setSelectionEnd( QTextDocument::Standard, cursor ) ) {
	    //	    cursor->parag()->document()->nextDoubleBuffered = TRUE; ##### we need that only if we have nested items/documents
            textFrameSet()->selectionChangedNotify();
	} else {
	    showCursor();
	}
	ensureCursorVisible();
    } else {
	bool redraw = textDocument()->removeSelection( QTextDocument::Standard );
	moveCursor( direction, control );
	if ( redraw ) {
	    //cursor->parag()->document()->nextDoubleBuffered = TRUE; // we need that only if we have nested items/documents
            textFrameSet()->selectionChangedNotify();
        }
        ensureCursorVisible();
        showCursor();
    }

    showCursor();
    updateUI();
}

void KWTextFrameSetEdit::moveCursor( MoveDirectionPrivate direction, bool control )
{
    switch ( direction ) {
    case MoveLeft: {
	if ( !control )
	    cursor->gotoLeft();
	else
	    cursor->gotoWordLeft();
    } break;
    case MoveRight: {
	if ( !control )
	    cursor->gotoRight();
	else
	    cursor->gotoWordRight();
    } break;
    case MoveUp: {
	if ( !control )
	    cursor->gotoUp();
	else
        {
	    //cursor->gotoPageUp( m_canvas->visibleHeight() );
            // M. Brade suggested up one paragraph instead.
            QTextParag * parag = cursor->parag()->prev();
            if ( parag )
            {
                cursor->setParag( parag );
                cursor->setIndex( 0 );
            }
        }
    } break;
    case MoveDown: {
	if ( !control )
	    cursor->gotoDown();
	else
        {
	    //cursor->gotoPageDown( m_canvas->visibleHeight() );
            // M. Brade suggested down one paragraph instead.
            QTextParag * parag = cursor->parag()->next();
            if ( parag )
            {
                cursor->setParag( parag );
                cursor->setIndex( 0 );
            }
        }
    } break;
    case MoveHome: {
	if ( !control )
	    cursor->gotoLineStart();
	else
	    cursor->gotoHome();
    } break;
    case MoveEnd: {
	if ( !control )
	    cursor->gotoLineEnd();
	else
	    cursor->gotoEnd();
    } break;
    case MovePgUp:
	cursor->gotoPageUp( m_canvas->visibleHeight() );
	break;
    case MovePgDown:
	cursor->gotoPageDown( m_canvas->visibleHeight() );
	break;
    }

    updateUI();
}

void KWTextFrameSetEdit::paste()
{
    QMimeSource *data = QApplication::clipboard()->data();
    if ( data->provides( MIME_TYPE ) )
    {
        QByteArray arr = data->encodedData( MIME_TYPE );
        if ( arr.size() )
            textFrameSet()->pasteKWord( cursor, QCString(arr), true );
    }
    else
    {
        // Note: QClipboard::text() seems to do a better job than encodedData( "text/plain" )
        // In particular it handles charsets (in the mimetype).
        QString text = QApplication::clipboard()->text();
        if ( !text.isEmpty() )
            textFrameSet()->pasteText( cursor, text, currentFormat, true );
    }
}

void KWTextFrameSetEdit::cut()
{
    if ( textDocument()->hasSelection( QTextDocument::Standard ) ) {
	copy();
	textFrameSet()->removeSelectedText( cursor );
    }
}

void KWTextFrameSetEdit::copy()
{
    if ( textDocument()->hasSelection( QTextDocument::Standard ) ) {
        KWDrag *kd = newDrag( 0L );
        QApplication::clipboard()->setData( kd );
    }
}

void KWTextFrameSetEdit::startDrag()
{
    mightStartDrag = FALSE;
    inDoubleClick = FALSE;
    KWDrag *drag = newDrag( m_canvas->viewport() );
    if ( !frameSet()->kWordDocument()->isReadWrite() )
        drag->dragCopy();
    else {
        if ( drag->drag() && QDragObject::target() != m_canvas && QDragObject::target() != m_canvas->viewport() ) {
            //textDocument()->removeSelectedText( QTextDocument::Standard, cursor ); // Hmm, no undo ?
            textFrameSet()->removeSelectedText( cursor );
        }
    }
}

KWDrag * KWTextFrameSetEdit::newDrag( QWidget * parent ) const
{
    textFrameSet()->unzoom();
    QTextCursor c1 = textDocument()->selectionStartCursor( QTextDocument::Standard );
    QTextCursor c2 = textDocument()->selectionEndCursor( QTextDocument::Standard );

    QString text;
    QDomDocument domDoc( "PARAGRAPHS" );
    QDomElement elem = domDoc.createElement( "PARAGRAPHS" );
    domDoc.appendChild( elem );
    if ( c1.parag() == c2.parag() )
    {
        text = c1.parag()->string()->toString().mid( c1.index(), c2.index() - c1.index() );
        static_cast<KWTextParag *>(c1.parag())->save( elem, c1.index(), c2.index()-1 );
    }
    else
    {
        text += c1.parag()->string()->toString().mid( c1.index() ) + "\n";
        static_cast<KWTextParag *>(c1.parag())->save( elem, c1.index(), -1 );
        QTextParag *p = c1.parag()->next();
        while ( p && p != c2.parag() ) {
            text += p->string()->toString() + "\n";
            static_cast<KWTextParag *>(p)->save( elem );
            p = p->next();
        }
        text += c2.parag()->string()->toString().left( c2.index() );
        static_cast<KWTextParag *>(c2.parag())->save( elem, 0, c2.index()-1 );
    }
    textFrameSet()->zoom();

    KWDrag *kd = new KWDrag( parent );
    kd->setPlain( text );
    kd->setKWord( domDoc.toCString() );
    kdDebug(32001) << "KWTextFrameSetEdit::newDrag " << domDoc.toCString() << endl;
    return kd;
}

void KWTextFrameSetEdit::ensureCursorVisible()
{
    //kdDebug() << "KWTextFrameSetEdit::ensureCursorVisible paragId=" << cursor->parag()->paragId() << endl;
    QTextParag * parag = cursor->parag();
    if ( !parag->isValid() )
        parag->format();
    QTextStringChar *chr = parag->at( cursor->index() );
    int h = parag->lineHeightOfChar( cursor->index() );
    int x = parag->rect().x() + chr->x + cursor->offsetX();
    //kdDebug() << "parag->rect().x()=" << parag->rect().x() << " chr->x=" << chr->x
    //          << " cursor->offsetX()=" << cursor->offsetX() << endl;
    int y = 0; int dummy;
    parag->lineHeightOfChar( cursor->index(), &dummy, &y );
    y += parag->rect().y() + cursor->offsetY();
    int w = 1;
    QPoint p;
    KWFrame * frame = textFrameSet()->internalToContents( QPoint(x, y), p );
    if ( frame && m_currentFrame != frame )
    {
        m_currentFrame = frame;
        m_canvas->gui()->getView()->updatePageInfo();
    }
    m_canvas->ensureVisible( p.x(), p.y() + h / 2, w, h / 2 + 2 );
}

void KWTextFrameSetEdit::mousePressEvent( QMouseEvent * e )
{
    textFrameSet()->clearUndoRedoInfo();
    mightStartDrag = FALSE;

    QPoint iPoint;
    if ( textFrameSet()->contentsToInternal( e->pos(), iPoint ) )
    {
        mousePos = iPoint;
        emit hideCursor();
        QTextCursor oldCursor = *cursor;
        placeCursor( mousePos );
        ensureCursorVisible();

        QTextDocument * textdoc = textDocument();
        if ( textdoc->inSelection( QTextDocument::Standard, mousePos ) ) {
            mightStartDrag = TRUE;
            emit showCursor();
            dragStartTimer->start( QApplication::startDragTime(), TRUE );
            dragStartPos = e->pos();
            return;
        }

        bool redraw = FALSE;
        if ( textdoc->hasSelection( QTextDocument::Standard ) ) {
            if ( !( e->state() & ShiftButton ) ) {
                redraw = textdoc->removeSelection( QTextDocument::Standard );
                textdoc->setSelectionStart( QTextDocument::Standard, cursor );
            } else {
                redraw = textdoc->setSelectionEnd( QTextDocument::Standard, cursor ) || redraw;
            }
        } else {
            if ( !( e->state() & ShiftButton ) ) {
                textdoc->setSelectionStart( QTextDocument::Standard, cursor );
            } else {
                textdoc->setSelectionStart( QTextDocument::Standard, &oldCursor );
                redraw = textdoc->setSelectionEnd( QTextDocument::Standard, cursor ) || redraw;
            }
        }

        //for ( int i = 1; i < textdoc->numSelections(); ++i )
        //    redraw = textdoc->removeSelection( i ) || redraw;

        //kdDebug() << "KWTextFrameSetEdit::mousePressEvent redraw=" << redraw << endl;
        if ( !redraw ) {
            emit showCursor();
        } else {
            textFrameSet()->selectionChangedNotify();
        }
    }
}

void KWTextFrameSetEdit::mouseMoveEvent( QMouseEvent * e )
{
    if ( mightStartDrag ) {
        dragStartTimer->stop();
        if ( ( e->pos() - dragStartPos ).manhattanLength() > QApplication::startDragDistance() )
            startDrag();
        return;
    }
    QPoint iPoint;
    if ( textFrameSet()->contentsToInternal( e->pos(), iPoint ) )
        mousePos = iPoint;
}

void KWTextFrameSetEdit::mouseReleaseEvent( QMouseEvent * )
{
    if ( dragStartTimer->isActive() )
	dragStartTimer->stop();
    if ( mightStartDrag ) {
	selectAll( FALSE );
        mightStartDrag = false;
    }
    else
    {
        if ( textDocument()->selectionStartCursor( QTextDocument::Standard ) == textDocument()->selectionEndCursor( QTextDocument::Standard ) )
            textDocument()->removeSelection( QTextDocument::Standard );

        textFrameSet()->selectionChangedNotify();
        // No auto-copy, will readd with Qt 3 using setSelectionMode(true/false)
        //if ( !textDocument()->selectedText( QTextDocument::Standard ).isEmpty() )
        //    textDocument()->copySelectedText( QTextDocument::Standard );
    }

    inDoubleClick = FALSE;
    emit showCursor();
}

void KWTextFrameSetEdit::mouseDoubleClickEvent( QMouseEvent * )
{
    inDoubleClick = TRUE;
    QTextCursor c1 = *cursor;
    QTextCursor c2 = *cursor;
    c1.gotoWordLeft();
    c2.gotoWordRight();

    textDocument()->setSelectionStart( QTextDocument::Standard, &c1 );
    textDocument()->setSelectionEnd( QTextDocument::Standard, &c2 );
    textFrameSet()->selectionChangedNotify();

    *cursor = c2;
}

void KWTextFrameSetEdit::dragEnterEvent( QDragEnterEvent * e )
{
    e->acceptAction();
}

void KWTextFrameSetEdit::dragMoveEvent( QDragMoveEvent * e )
{
    QPoint iPoint;
    if ( textFrameSet()->contentsToInternal( e->pos(), iPoint ) )
    {
        emit hideCursor();
        placeCursor( iPoint );
        emit showCursor();
        e->acceptAction(); // here or out of the if ?
    }
}

void KWTextFrameSetEdit::dragLeaveEvent( QDragLeaveEvent * )
{
}

void KWTextFrameSetEdit::dropEvent( QDropEvent * e )
{
    if ( KWDrag::canDecode( e ) )
    {
        e->acceptAction();

        QTextCursor dropCursor( textDocument() );
        QPoint dropPoint;
        if ( !textFrameSet()->contentsToInternal( e->pos(), dropPoint ) )
            return; // Don't know where to paste

        dropCursor.place( dropPoint, textDocument()->firstParag() );
        kdDebug(32001) << "KWTextFrameSetEdit::dropEvent dropCursor at parag=" << dropCursor.parag()->paragId() << " index=" << dropCursor.index() << endl;

        if ( ( e->source() == m_canvas ||
               e->source() == m_canvas->viewport() ) &&
             e->action() == QDropEvent::Move ) {

            if ( textDocument()->hasSelection( QTextDocument::Standard ) )
            {
                // Dropping into the selection itself ?
                if ( textDocument()->inSelection( QTextDocument::Standard, dropPoint ) )
                {
                    textDocument()->removeSelection( QTextDocument::Standard );
                    textFrameSet()->selectionChangedNotify();
                    hideCursor();
                    *cursor = dropCursor;
                    showCursor();
                    ensureCursorVisible();
                    return;
                }

                // Tricky. We don't want to do the placeCursor after removing the selection
                // (the user pointed at some text with the old selection in place).
                // However, something got deleted in our parag, dropCursor's index needs adjustment.
                QTextCursor endSel = textDocument()->selectionEndCursor( QTextDocument::Standard );
                if ( endSel.parag() == dropCursor.parag() )
                {
                    QTextCursor startSel = textDocument()->selectionStartCursor( QTextDocument::Standard );
                    // Does the selection starts before (other parag or same parag) ?
                    if ( startSel.parag() != dropCursor.parag() || startSel.index() < dropCursor.index() )
                    {
                        // If other -> endSel.parag() will get deleted. The final position is in startSel.parag(),
                        // where the selection started + how much after the end we are. Make a drawing :)
                        // If same -> simply move back by how many chars we've deleted. Funny thing is, it's the same formula.
                        int dropIndex = dropCursor.index();
                        dropCursor.setParag( startSel.parag() );
                        // If dropCursor - endSel < 0, selection ends after, we're dropping into selection (no-op)
                        dropCursor.setIndex( dropIndex - QMIN( endSel.index(), dropIndex ) + startSel.index() );
                    }
                    kdDebug(32001) << "dropCursor: parag=" << dropCursor.parag()->paragId() << " index=" << dropCursor.index() << endl;
                }

                textFrameSet()->removeSelectedText( cursor );
            }
            hideCursor();
            *cursor = dropCursor;
            showCursor();
            kdDebug(32001) << "cursor set back to drop cursor: parag=" << cursor->parag()->paragId() << " index=" << cursor->index() << endl;

        } else
        {   // drop coming from outside -> forget about current selection
            textDocument()->removeSelection( QTextDocument::Standard );
            textFrameSet()->selectionChangedNotify();
        }

        if ( e->provides( MIME_TYPE ) )
        {
            QByteArray arr = e->encodedData( MIME_TYPE );
            if ( arr.size() )
                textFrameSet()->pasteKWord( cursor, QCString(arr), false );
        }
        else
        {
            QString text;
            if ( QTextDrag::decode( e, text ) )
                textFrameSet()->pasteText( cursor, text, currentFormat, false );
        }
    }
}

void KWTextFrameSetEdit::focusInEvent()
{
    blinkTimer->start( QApplication::cursorFlashTime() / 2 );
    showCursor();
}

void KWTextFrameSetEdit::focusOutEvent()
{
    blinkTimer->stop();
    hideCursor();
}

void KWTextFrameSetEdit::doAutoScroll( QPoint pos )
{
    if ( mightStartDrag )
        return;
    QPoint iPoint;
    if ( !textFrameSet()->contentsToInternal( pos, iPoint ) )
        return;
    hideCursor();
    QTextCursor oldCursor = *cursor;
    placeCursor( iPoint );
    if ( inDoubleClick ) {
	QTextCursor cl = *cursor;
	cl.gotoWordLeft();
	QTextCursor cr = *cursor;
	cr.gotoWordRight();

	int diff = QABS( oldCursor.parag()->at( oldCursor.index() )->x - mousePos.x() );
	int ldiff = QABS( cl.parag()->at( cl.index() )->x - mousePos.x() );
	int rdiff = QABS( cr.parag()->at( cr.index() )->x - mousePos.x() );

	if ( cursor->parag()->lineStartOfChar( cursor->index() ) !=
	     oldCursor.parag()->lineStartOfChar( oldCursor.index() ) )
	    diff = 0xFFFFFF;

	if ( rdiff < diff && rdiff < ldiff )
	    *cursor = cr;
	else if ( ldiff < diff && ldiff < rdiff )
	    *cursor = cl;
	else
	    *cursor = oldCursor;

    }
    ensureCursorVisible();

    bool redraw = FALSE;
    if ( textDocument()->hasSelection( QTextDocument::Standard ) )
	redraw = textDocument()->setSelectionEnd( QTextDocument::Standard, cursor ) || redraw;
    else // it may be that the initial click was out of the frame
        textDocument()->setSelectionStart( QTextDocument::Standard, cursor );

    if ( redraw )
        textFrameSet()->selectionChangedNotify( false );

    showCursor();
}

void KWTextFrameSetEdit::placeCursor( const QPoint &pos )
{
    cursor->restoreState();
    QTextParag *s = textDocument()->firstParag();
    cursor->place( pos,  s );
    updateUI();
}

void KWTextFrameSetEdit::blinkCursor()
{
    if ( !cursorVisible )
	return;
    bool cv = cursorVisible;
    blinkCursorVisible = !blinkCursorVisible;
    drawCursor( blinkCursorVisible );
    cursorVisible = cv;
}

void KWTextFrameSetEdit::drawCursor( bool visible )
{
    if ( !cursor->parag() ||
	 !cursor->parag()->isValid() )
	return;

    QPainter p( m_canvas->viewport() );
    p.translate( -m_canvas->contentsX(), -m_canvas->contentsY() );
    p.setBrushOrigin( -m_canvas->contentsX(), -m_canvas->contentsY() );

    textFrameSet()->drawCursor( &p, cursor, visible );
    cursorVisible = visible;
}


void KWTextFrameSetEdit::setBold( bool on ) {
    KWTextFormat format( *currentFormat );
    format.setBold( on );
    textFrameSet()->setFormat( cursor, currentFormat, &format, QTextFormat::Bold );
   //kdDebug(32003) << "KWTextFrameSetEdit::setBold new currentFormat " << currentFormat << " " << currentFormat->key() << endl;
}

void KWTextFrameSetEdit::setItalic( bool on ) {
    KWTextFormat format( *currentFormat );
    format.setItalic( on );
    textFrameSet()->setFormat( cursor, currentFormat, &format, QTextFormat::Italic );
}

void KWTextFrameSetEdit::setUnderline( bool on ) {
    KWTextFormat format( *currentFormat );
    format.setUnderline( on );
    textFrameSet()->setFormat( cursor, currentFormat, &format, QTextFormat::Underline );
}

void KWTextFrameSetEdit::setStrikeOut( bool on ) {
    KWTextFormat format( *currentFormat );
    format.setStrikeOut( on);
    textFrameSet()->setFormat( cursor, currentFormat, &format, QTextFormat::Format );
}

QColor KWTextFrameSetEdit::textColor() const {
    return currentFormat->color();
}

QFont KWTextFrameSetEdit::textFont() const {
    return currentFormat->font();
}

int KWTextFrameSetEdit::textFontSize()const{
    return currentFormat->font().pointSize ();
}

QString KWTextFrameSetEdit::textFontFamily()const {
    return currentFormat->font().family();
}

void KWTextFrameSetEdit::setPointSize( int s ){
    KWTextFormat format( *currentFormat );
    format.setPointSize( s );
    textFrameSet()->setFormat( cursor, currentFormat, &format, QTextFormat::Size, true /* zoom the font size */ );
}

void KWTextFrameSetEdit::setFamily(const QString &font){
    KWTextFormat format( *currentFormat );
    format.setFamily( font );
    textFrameSet()->setFormat( cursor, currentFormat, &format, QTextFormat::Family );
}

void KWTextFrameSetEdit::setFont(const QFont &font,bool _subscript,bool _superscript){
    KWTextFormat format( *currentFormat );
    format.setFont( font );

    if(!_subscript)
        {
            if(!_superscript)
                format.setVAlign(QTextFormat::AlignNormal);
            else
                format.setVAlign(QTextFormat::AlignSuperScript);
        }
    else
        format.setVAlign(QTextFormat::AlignSubScript);
    textFrameSet()->setFormat( cursor, currentFormat, &format, QTextFormat::Format,true /* zoom the font size */);
}

void KWTextFrameSetEdit::setTextColor(const QColor &color) {
    KWTextFormat format( *currentFormat );
    format.setColor( color );
    textFrameSet()->setFormat( cursor, currentFormat, &format, QTextFormat::Color );
}

void KWTextFrameSetEdit::setTextSubScript(bool on)
{
    KWTextFormat format( *currentFormat );
    if(!on)
        format.setVAlign(QTextFormat::AlignNormal);
    else
        format.setVAlign(QTextFormat::AlignSubScript);
    textFrameSet()->setFormat( cursor, currentFormat, &format, QTextFormat::VAlign );
}

void KWTextFrameSetEdit::setTextSuperScript(bool on)
{
    KWTextFormat format( *currentFormat );
    if(!on)
        format.setVAlign(QTextFormat::AlignNormal);
    else
        format.setVAlign(QTextFormat::AlignSuperScript);
    textFrameSet()->setFormat( cursor, currentFormat, &format, QTextFormat::VAlign );
}

void KWTextFrameSetEdit::insertSpecialChar(QChar _c)
{
    textFrameSet()->insert( cursor, currentFormat, _c, false /* no newline */, true, i18n("Insert Special Char") );
}

void KWTextFrameSetEdit::insertPicture( const QString & file )
{
    KWTextImage * custom = new KWTextImage( textDocument(), file );
    cursor->parag()->insert( cursor->index(), QChar('@') /*whatever*/ );
    static_cast<KWTextParag *>( cursor->parag() )->setCustomItem( cursor->index(), custom, currentFormat );
    // TODO undo/redo support
}

void KWTextFrameSetEdit::insertFloatingFrameSet( KWFrameSet * fs )
{
    fs->setAnchored( textFrameSet(), static_cast<KWTextParag *>(cursor->parag()), cursor->index() );
}

void KWTextFrameSetEdit::insertVariable( int type )
{
    kdDebug() << "KWTextFrameSetEdit::insertVariable " << type << endl;
    KWDocument * doc = frameSet()->kWordDocument();

    KWVariable * var = 0L;
    if ( type == VT_CUSTOM )
    {
        // Choose an existing variable
        KWVariableNameDia dia( m_canvas, doc->getVariables() );
        if ( dia.exec() == QDialog::Accepted )
            var = new KWCustomVariable( textFrameSet(), dia.getName(), doc->variableFormat( type ) );
    }
    else if ( type == VT_SERIALLETTER )
    {
        KWSerialLetterVariableInsertDia dia( m_canvas, doc->getSerialLetterDataBase() );
        if ( dia.exec() == QDialog::Accepted )
            var = new KWSerialLetterVariable( textFrameSet(), dia.getName(), doc->variableFormat( type ) );
    }
    else
        var = KWVariable::createVariable( type, textFrameSet() );

    if ( var )
    {
        kdDebug() << "KWTextFrameSetEdit::insertVariable inserting into paragraph" << endl;
        // TODO undo/redo support
        cursor->parag()->insert( cursor->index(), QChar('&') /*whatever*/ );
        static_cast<KWTextParag *>( cursor->parag() )->setCustomItem( cursor->index(), var, currentFormat );
        var->adjustToPainter( 0L ); // just a way to compute the width from the start
        cursor->parag()->setChanged( true );
        // repaintChanged?
    }
}

// Update the GUI toolbar button etc. to reflect the current cursor position.
void KWTextFrameSetEdit::updateUI()
{
    // Update UI - only for those items which have changed

    int i = cursor->index();
    if ( i > 0 )
        --i;
    /*if ( currentFormat )
        kdDebug(32003) << "KWTextFrameSet::updateUI currentFormat=" << currentFormat
                       << " " << currentFormat->key()
                       << " parag format=" << cursor->parag()->at( i )->format()->key() << endl;*/

    if ( !currentFormat || currentFormat->key() != cursor->parag()->at( i )->format()->key() )
    {
        if ( currentFormat )
            currentFormat->removeRef();
        currentFormat = static_cast<KWTextFormat *>( textDocument()->formatCollection()->format( cursor->parag()->at( i )->format() ) );
        if ( currentFormat->isMisspelled() ) {
            currentFormat->removeRef();
            currentFormat = static_cast<KWTextFormat *>( textDocument()->formatCollection()->format( currentFormat->font(), currentFormat->color() ) );
        }
        showCurrentFormat();
        //textDocument()->formatCollection()->debug();
    }

    // Paragraph settings
    KWTextParag * parag = static_cast<KWTextParag *>(cursor->parag());

    if ( m_paragLayout.alignment != parag->alignment() ) {
	m_paragLayout.alignment = parag->alignment();
        m_canvas->gui()->getView()->showAlign( m_paragLayout.alignment );
    }

    // Counter
    if ( !m_paragLayout.counter )
        m_paragLayout.counter = new Counter; // we can afford to always have one here
    Counter::Style cstyle = m_paragLayout.counter->style();
    if ( parag->counter() )
        *m_paragLayout.counter = *parag->counter();
    else
    {
        m_paragLayout.counter->setNumbering( Counter::NUM_NONE );
        m_paragLayout.counter->setStyle( Counter::STYLE_NONE );
    }
    if ( m_paragLayout.counter->style() != cstyle )
        m_canvas->gui()->getView()->showCounter( * m_paragLayout.counter );

    if(m_paragLayout.leftBorder!=parag->leftBorder() ||
       m_paragLayout.rightBorder!=parag->rightBorder() ||
       m_paragLayout.topBorder!=parag->topBorder() ||
       m_paragLayout.bottomBorder!=parag->bottomBorder())
    {
        m_paragLayout.leftBorder = parag->leftBorder();
	m_paragLayout.rightBorder = parag->rightBorder();
	m_paragLayout.topBorder = parag->topBorder();
	m_paragLayout.bottomBorder = parag->bottomBorder();
	m_canvas->gui()->getView()->showParagBorders( m_paragLayout.leftBorder, m_paragLayout.rightBorder, m_paragLayout.topBorder, m_paragLayout.bottomBorder );
    }

    if ( m_paragLayout.styleName() != parag->styleName() )
    {
        m_paragLayout.setStyleName( parag->styleName() );
        m_canvas->gui()->getView()->showStyle( m_paragLayout.styleName() );
    }

    if( m_paragLayout.margins[QStyleSheetItem::MarginLeft] != parag->margin(QStyleSheetItem::MarginLeft)
	|| m_paragLayout.margins[QStyleSheetItem::MarginFirstLine] != parag->margin(QStyleSheetItem::MarginFirstLine) )
    {
	m_paragLayout.margins[QStyleSheetItem::MarginFirstLine] = parag->margin(QStyleSheetItem::MarginFirstLine);
	m_paragLayout.margins[QStyleSheetItem::MarginLeft] = parag->margin(QStyleSheetItem::MarginLeft);
	m_canvas->gui()->getView()->showRulerIndent( m_paragLayout.margins[QStyleSheetItem::MarginLeft], m_paragLayout.margins[QStyleSheetItem::MarginFirstLine] );
    }

    m_paragLayout.setTabList( parag->tabList() );
    KoRuler * hr = m_canvas->gui()->getHorzRuler();
    if ( hr ) hr->setTabList( parag->tabList() );
    // There are more paragraph settings, but those that are not directly
    // visible in the UI don't need to be handled here.
    // For instance parag and line spacing stuff, borders etc.
}

void KWTextFrameSetEdit::showCurrentFormat()
{
    KWTextFormat format = *currentFormat;
    format.setPointSize( textFrameSet()->docFontSize( currentFormat ) ); // "unzoom" the font size
    m_canvas->gui()->getView()->showFormat( format );
}

#include "kwtextframeset.moc"
