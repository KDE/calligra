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
#include "kwviewmode.h"
#include "kwcanvas.h"
#include "kwanchor.h"
#include "kwutils.h"
#include "kwcommand.h"
#include "kwdrag.h"
#include "kwstyle.h"
#include "kwformat.h"
#include "counter.h"
#include "contents.h"
#include "variable.h"
#include "variabledlgs.h"
#include "serialletter.h"
#include "autoformat.h"
#include <qclipboard.h>
#include <qdragobject.h>
#include <qtl.h>
#include <qprogressdialog.h>
#include <kapp.h>
#include <klocale.h>
#include <kconfig.h>
#include <koDataTool.h>

#include <kdebug.h>

//#define DEBUG_MARGINS
//#define DEBUG_FLOW
//#define DEBUG_FORMATS
//#define DEBUG_FORMAT_MORE
//#define DEBUG_VIEWAREA

//#define DEBUG_NTI
//#define DEBUG_ITN

KWTextFrameSet::KWTextFrameSet( KWDocument *_doc, const QString & name )
    : KWFrameSet( _doc ), undoRedoInfo( this )
{
    //kdDebug() << "KWTextFrameSet::KWTextFrameSet " << this << endl;
    if ( name.isEmpty() )
        m_name = _doc->generateFramesetName( i18n( "Text Frameset %1" ) );
    else
        m_name = name;
    m_availableHeight = -1;
    m_currentViewMode = 0L;
    m_currentDrawnFrame = 0L;
    m_origFontSizes.setAutoDelete( true );
    m_framesInPage.setAutoDelete( true );
    m_firstPage = 0;
    textdoc = new KWTextDocument( this, 0, new KWTextFormatCollection( _doc->defaultFont() ) );
    textdoc->setFlow( this );
    textdoc->setVerticalBreak( true );              // get QTextFlow methods to be called

    /* if ( QFile::exists( "bidi.txt" ) ) {
       QFile fl( "bidi.txt" );
       fl.open( IO_ReadOnly );
       QByteArray array = fl.readAll();
       QString text = QString::fromUtf8( array.data() );
       textdoc->setText( text, QString::null );
    }*/

    m_lastFormatted = textdoc->firstParag();
    // Apply 'Standard' style to paragraph
    KWStyle * style = _doc->findStyle( "Standard" );
    if ( m_lastFormatted && style )
    {
        static_cast<KWTextParag*>(m_lastFormatted)->setParagLayout( style->paragLayout() );
        m_lastFormatted->setFormat( 0, m_lastFormatted->string()->length(), & style->format() );
    }

    textdoc->addSelection( HighlightSelection );
    textdoc->setSelectionColor( HighlightSelection,
                                QApplication::palette().color( QPalette::Active, QColorGroup::Dark ) );
    textdoc->setInvertSelectionText( HighlightSelection, true );

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

KWFrame * KWTextFrameSet::normalToInternal( QPoint nPoint, QPoint &iPoint, bool mouseSelection ) const
{
#ifdef DEBUG_NTI
    kdDebug() << "KWTextFrameSet::normalToInternal nPoint:" << nPoint.x() << "," << nPoint.y() << " mouseSelection=" << mouseSelection << endl;
#endif
    // Find the frame that contains nPoint. To go fast, we look them up by page number.
    int pageNum = nPoint.y() / m_doc->paperHeight();
    QListIterator<KWFrame> frameIt( framesInPage( pageNum ) );
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame *frame = frameIt.current();
        QRect frameRect = kWordDocument()->zoomRect( *frame );
        // Due to zooming problems (and to QRect's semantics), be tolerant for one more pixel
        frameRect.rRight() += 1;
        frameRect.rBottom() += 1;
        if ( frameRect.contains( nPoint ) ) // both r and p are in "normal coordinates"
        {
            // This translates the coordinates from the normal coord system
            // into the QTextDocument's coordinate system
            // (which doesn't have frames, borders, etc.)
            iPoint.setX( nPoint.x() - frameRect.left() );
            iPoint.setY( nPoint.y() - ( frameRect.top() - frame->internalY() ) );
#ifdef DEBUG_NTI
            kdDebug() << "normalToInternal: returning " << iPoint.x() << "," << iPoint.y()
                      << " internalY=" << frame->internalY() << " because " << DEBUGRECT(frameRect)
                      << " contains nPoint:" << nPoint.x() << "," << nPoint.y() << endl;
#endif
            return frame;
        }
        else if ( mouseSelection ) // try harder if true
        {
            QRect openLeftRect( frameRect );
            openLeftRect.setX( 0 );
#ifdef DEBUG_NTI
            kdDebug() << "normalToInternal: openLeftRect=" << DEBUGRECT( openLeftRect ) << endl;
#endif
            if ( openLeftRect.contains( nPoint ) )
            {
                // We are at the left of this frame (and not in any other frame of this frameset)
                iPoint.setX( 0 );
                iPoint.setY( nPoint.y() - ( frameRect.top() - frame->internalY() ) );
#ifdef DEBUG_NTI
                kdDebug() << "normalToInternal: returning " << iPoint.x() << "," << iPoint.y()
                          << " internalY=" << frame->internalY() << " because openLeftRect=" << DEBUGRECT(openLeftRect)
                          << " contains nPoint:" << nPoint.x() << "," << nPoint.y() << endl;
#endif
                return frame;
            }
            QRect openTopRect( frameRect );
            openTopRect.setX( 0 );
            openTopRect.setY( 0 );
#ifdef DEBUG_NTI
            kdDebug() << "normalToInternal: openTopRect=" << DEBUGRECT( openTopRect ) << endl;
#endif
            if ( openTopRect.contains( nPoint ) )
            {
                // We are at the top of this frame (...)
                iPoint.setX( nPoint.x() - frameRect.left() );
                iPoint.setY( frame->internalY() );
#ifdef DEBUG_NTI
                kdDebug() << "normalToInternal: returning " << iPoint.x() << "," << iPoint.y()
                          << " internalY=" << frame->internalY() << " because openTopRect=" << DEBUGRECT(openTopRect)
                          << " contains nPoint:" << nPoint.x() << "," << nPoint.y() << endl;
#endif
                return frame;
            }
        }
#ifdef DEBUG_NTI
        //else
        //  kdDebug() << "NTI: " << DEBUGRECT(frameRect)
        //            << " doesn't contain nPoint:" << nPoint.x() << "," << nPoint.y() << endl;
#endif
    }
    // Not found. This means either:
    // if mouseSelection == false : the mouse isn't over any frame, in the page pageNum.
    // if mouseSelection == true : we are under (or at the right of), the frames in pageNum.
    if ( mouseSelection ) // in that case, go for the first frame in the next page.
    {
        if ( pageNum + 1 >= (int)m_framesInPage.size() + m_firstPage )
        {
            // Under last frame of last page!
            KWFrame *frame = frames.getLast();
            QRect frameRect = kWordDocument()->zoomRect( *frame );
            iPoint.setX( frameRect.width() );
            iPoint.setY( frame->internalY() + frameRect.height() );
#ifdef DEBUG_NTI
            kdDebug() << "normalToInternal: returning " << iPoint.x() << "," << iPoint.y()
                      << " because we are under all frames of the last page" << endl;
#endif
            return frame;
        }
        else
        {
            QListIterator<KWFrame> frameIt( framesInPage( pageNum + 1 ) );
            if ( frameIt.current() )
            {
                // There is a frame on the next page
                KWFrame *frame = frameIt.current();
                QRect frameRect = kWordDocument()->zoomRect( *frame );
                QRect openTopRect( frameRect );
                openTopRect.setY( 0 );
                if ( openTopRect.contains( nPoint ) ) // We are at the top of this frame
                    iPoint.setX( nPoint.x() - frameRect.left() );
                else
                    iPoint.setX( 0 ); // We are, hmm, on the left or right of the frames
                iPoint.setY( frame->internalY() );
#ifdef DEBUG_NTI
                kdDebug() << "normalToInternal: returning " << iPoint.x() << "," << iPoint.y()
                          << " because we are under all frames of page " << pageNum << endl;
#endif
                return frame;
            } // else there is a gap (no frames on that page, but on some other further down)
            // This case isn't handled (and should be VERY rare I think)
        }
    }

    iPoint = nPoint; // bah
    return 0;
}

KWFrame * KWTextFrameSet::internalToNormalWithHint( QPoint iPoint, QPoint & nPoint, QPoint hintNPoint ) const
{
#ifdef DEBUG_ITN
    kdDebug() << "KWTextFrameSet::internalToNormalWithHint hintNPoint: " << hintNPoint.x() << "," << hintNPoint.y() << endl;
#endif
    KWFrame *lastFrame = 0L;
    QListIterator<KWFrame> frameIt( frameIterator() );
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame *frame = frameIt.current();
        QRect frameRect = kWordDocument()->zoomRect( *frame );
        QRect r( 0, frame->internalY(), frameRect.width(), frameRect.height() );
#ifdef DEBUG_ITN
        kdDebug() << "ITN: r=" << DEBUGRECT(r) << " iPoint=" << iPoint.x() << "," << iPoint.y() << endl;
#endif
        // r is the frame in qrt coords
        if ( r.contains( iPoint ) ) // both r and p are in "qrt coordinates"
        {
            int offsetX = frameRect.left();
            int offsetY = frameRect.top() - frame->internalY();
            nPoint.setX( iPoint.x() + offsetX );
            nPoint.setY( iPoint.y() + offsetY );
#ifdef DEBUG_ITN
            kdDebug() << "copy: " << frame->isCopy() << " hintNPoint.y()=" << hintNPoint.y() << " nPoint.y()=" << nPoint.y() << endl;
#endif
            // No "hintNPoint" specified, go for the first match
            if ( hintNPoint.isNull() )
                return frame;
            // hintNPoint specified, check if we are far enough
            if ( hintNPoint.y() <= nPoint.y() )
                return frame;
            // Remember that this frame matched, in case we find no further frame that matches
            lastFrame = frame;
        }
        else if ( lastFrame )
            return lastFrame;
    }

    // This happens when the parag is on a not-yet-created page (formatMore will notice afterwards)
    // So it doesn't matter much what happens here, we'll redo it anyway.
#ifdef DEBUG_ITN
    kdDebug(32002) << "KWTextFrameSet::internalToNormalWithHint " << iPoint.x() << "," << iPoint.y()
                   << " not in any frame of " << (void*)this << endl;
#endif
    nPoint = iPoint; // bah
    return 0L;
}

QPoint KWTextFrameSet::moveToPage( int currentPgNum, short int direction ) const
{
    if ( !isVisible() || frames.isEmpty() )
        return QPoint();
    //kdDebug() << "KWTextFrameSet::moveToPage currentPgNum=" << currentPgNum << " direction=" << direction << endl;
    int num = currentPgNum + direction;
    int pages = m_doc->getPages();
    for ( ; num >= 0 && num < pages ; num += direction )
    {
        //kdDebug() << "KWTextFrameSet::moveToPage num=" << num << " pages=" << pages << endl;
        // Find the first frame on page num
        if ( num < m_firstPage || num >= (int)m_framesInPage.size() + m_firstPage )
            continue; // No frame on that page

        //kdDebug() << "KWTextFrameSet::moveToPage ok for first frame in page " << num << endl;
        QListIterator<KWFrame> frameIt( framesInPage( num ) );
        return QPoint( 0, frameIt.current()->internalY() + 2 ); // found, ok.
    }
    // Not found. Go to top of first frame or bottom of last frame, depending on direction
    if ( direction < 0 )
        return QPoint( 0, frames.getFirst()->internalY() + 2 );
    else
    {
        KWFrame * frame = frames.getLast();
        return QPoint( 0, frame->internalY() + m_doc->zoomItY( frame->height() ) );
    }
}

void KWTextFrameSet::drawContents( QPainter *p, const QRect & crect, QColorGroup &cg,
                               bool onlyChanged, bool resetChanged,
                               KWFrameSetEdit *edit, KWViewMode *viewMode, KWCanvas *canvas )
{
    m_currentViewMode = viewMode;
    KWFrameSet::drawContents( p, crect, cg, onlyChanged, resetChanged, edit, viewMode, canvas );
}

void KWTextFrameSet::drawFrame( KWFrame *frame, QPainter *painter, const QRect &r,
                                QColorGroup &cg, bool onlyChanged, bool resetChanged,
                                KWFrameSetEdit *edit )
{
    //kdDebug() << "KWTextFrameSet::drawFrame " << getName() << "(frame " << getFrameFromPtr( frame ) << ") crect(r)=" << DEBUGRECT( r ) << endl;
    m_currentDrawnFrame = frame;
    // Update variables for each frame (e.g. for page-number)
    // If more than KWPgNumVariable need this functionality, create an intermediary base class
    QListIterator<QTextCustomItem> cit( textdoc->allCustomItems() );
    for ( ; cit.current() ; ++cit )
    {
        KWPgNumVariable * var = dynamic_cast<KWPgNumVariable *>( cit.current() );
        if ( var && !var->isDeleted() && var->subtype() == KWPgNumVariable::VST_PGNUM_CURRENT )
        {
            //kdDebug() << "KWTextFrameSet::drawFrame updating pgnum variable to " << frame->pageNum()+1
            //          << " and invalidating parag " << var->paragraph() << endl;
            var->setPgNum( frame->pageNum() + 1 );
            var->resize();
            var->paragraph()->invalidate( 0 ); // size may have changed -> need reformatting !
            var->paragraph()->setChanged( true );
        }
    }

    // Do we draw a cursor ?
    bool drawCursor = edit!=0L;
    QTextCursor * cursor = edit ? static_cast<KWTextFrameSetEdit *>(edit)->getCursor() : 0;

    // This paints the 1st two lines, which QRT forgets.
    // Update: now it doesn't anymore :)
/*    if (!onlyChanged)
    {
        QRect rtop( r.intersect( QRect( 0, 0, r.right(), 2 ) ) );
        if ( !rtop.isEmpty() )
            painter->fillRect( rtop, cg.brush( QColorGroup::Base ) );
    }
    */

    QTextParag * lastFormatted = textdoc->draw( painter, r.x(), r.y(), r.width(), r.height(),
                                                cg, onlyChanged, drawCursor, cursor, resetChanged );

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

        //kdDebug(32002) << "KWTextFrameSet::drawFrame drawn. onlyChanged=" << onlyChanged << " resetChanged=" << resetChanged << " lastDrawn=" << lastDrawn->paragId() << " lastDrawn's bottom:" << lastDrawn->rect().bottom() << " r.bottom=" << r.bottom() << endl;
        if ( lastDrawn && lastDrawn->rect().bottom() > r.bottom() )
        {
            //kdDebug(32002) << "KWTextFrameSet::drawFrame setting lastDrawn " << lastDrawn->paragId() << " to changed" << endl;
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
        QRect frameRect = m_currentViewMode->normalToView( kWordDocument()->zoomRect( *frame ) );

        //QListIterator<KWFrame> frameIt( frameIterator() );
        //int totalHeight = 0;
        //for ( ; frameIt.current() && frameIt.current() != frame; ++frameIt )
        //    totalHeight += kWordDocument()->zoomItY( frameIt.current()->height() );
        int totalHeight = frames.last()->internalY()
                          + m_doc->zoomItY( frames.last()->height() );

        QRect blank( 0, docHeight, frameRect.width(), totalHeight+frameRect.height() - docHeight );
        //kdDebug(32002) << this << " Blank area: " << DEBUGRECT(blank) << endl;
        painter->fillRect( blank, cg.brush( QColorGroup::Base ) );
        // for debugging :)
        //painter->setPen( QPen(Qt::blue, 1, DashLine) );  painter->drawRect( blank );
    }
    m_currentDrawnFrame = 0L;
}

void KWTextFrameSet::drawCursor( QPainter *p, QTextCursor *cursor, bool cursorVisible, KWCanvas *canvas, KWFrame *frame )
{
    // This redraws the paragraph where the cursor is - with a small clip region around the cursor
    m_currentDrawnCanvas = canvas;
    KWViewMode *viewMode = canvas->viewMode();
    m_currentViewMode = viewMode;
    m_currentDrawnFrame = frame;

    QRect normalFrameRect( m_doc->zoomRect( *frame ) );
    QPoint topLeft = cursor->topParag()->rect().topLeft();         // in QRT coords
    int lineY;
    int h = cursor->parag()->lineHeightOfChar( cursor->index(), 0, &lineY );
    //kdDebug() << "KWTextFrameSet::drawCursor topLeft=" << topLeft.x() << "," << topLeft.y()
    //          << " x=" << cursor->x() << " y=" << lineY << endl;
    QPoint iPoint( topLeft.x() - cursor->totalOffsetX() + cursor->x(),
                   topLeft.y() - cursor->totalOffsetY() + lineY );
    //kdDebug() << "KWTextFrameSet::drawCursor iPoint=" << iPoint.x() << "," << iPoint.y() << "   h=" << h << endl;
    QPoint nPoint;
    QPoint hintNPoint = normalFrameRect.topLeft();
    if ( internalToNormalWithHint( iPoint, nPoint, hintNPoint ) )
    {
        // very small clipping around the cursor
        QRect clip( nPoint.x() - 5, nPoint.y(), 10, h );
        //kdDebug() << " clip(normal, before intersect)=" << DEBUGRECT(clip) << endl;

        clip &= normalFrameRect; // clip to frame
        // kdDebug() << "KWTextFrameSet::drawCursor normalFrameRect=" << DEBUGRECT(normalFrameRect)
        //          << " clip(normal, after intersect)=" << DEBUGRECT(clip) << endl;

        QPoint cPoint = viewMode->normalToView( clip.topLeft() );     // from normal to view contents
        clip.moveTopLeft( cPoint );

        //kdDebug(32002) << "KWTextFrameSet::drawCursor "
        //               << " cPoint=(" << cPoint.x() << "," << cPoint.y() << ")  h=" << h << endl;
        //kdDebug(32002) << " Clip for cursor (internal coords) : " << DEBUGRECT(clip) << endl;

        QRegion reg = frameClipRegion( p, frame, clip, viewMode, true );
        if ( !reg.isEmpty() )
        {
            bool wasChanged = cursor->parag()->hasChanged();
            cursor->parag()->setChanged( TRUE );      // To force the drawing to happen
            p->save();

            p->setClipRegion( reg );
            // translate to qrt coords - after setting the clip region !
            QRect viewFrameRect = viewMode->normalToView( normalFrameRect );
            // see internalToNormalWithHint
            p->translate( viewFrameRect.left(), viewFrameRect.top() - frame->internalY() );

            // The settings come from this frame
            KWFrame * settings = settingsFrame( frame );

            QPixmap *pix = 0;
            QColorGroup cg = QApplication::palette().active();
            QBrush bgBrush( settings->getBackgroundColor() );
            bgBrush.setColor( KWDocument::resolveBgColor( bgBrush.color(), p ) );
            cg.setBrush( QColorGroup::Base, bgBrush );

            textdoc->drawParag( p, cursor->parag(),
                                iPoint.x() - 5, iPoint.y(), clip.width(), clip.height(),
                                pix, cg, cursorVisible, cursor );
            p->restore();
            cursor->parag()->setChanged( wasChanged );      // Maybe we have more changes to draw!

            //XIM Position
            QPoint ximPoint = viewMode->normalToView( nPoint );
            int line;
            cursor->parag()->lineStartOfChar( cursor->index(), 0, &line );
            canvas->setXimPosition( ximPoint.x(), ximPoint.y(), 0, h - cursor->parag()->lineSpacing( line ) );
        }
    }
    m_currentDrawnCanvas = 0L;
    m_currentDrawnFrame = 0L;
}

void KWTextFrameSet::layout()
{
    invalidate();
    // Get the thing going though, repainting doesn't call formatMore
    formatMore();
}

void KWTextFrameSet::invalidate()
{
    //kdDebug() << "KWTextFrameSet::invalidate " << getName() << endl;
    m_lastFormatted = textdoc->firstParag();
    textdoc->invalidate(); // lazy layout, real update follows upon next repaint
}

int KWTextFrameSet::paragraphs()
{
    int paragraphs = 0;
    QTextParag * parag = textdoc->firstParag();
    for ( ; parag ; parag = parag->next() )
        paragraphs++;
    return paragraphs;
}

bool KWTextFrameSet::statistics( QProgressDialog *progress, ulong & charsWithSpace, ulong & charsWithoutSpace, ulong & words,
    ulong & sentences, ulong & syllables )
{
    // parts of words for better counting of syllables:
    QStringList subs_syl;
    subs_syl << "cial" << "tia" << "cius" << "cious" << "giu" << "ion" << "iou" << "sia$" << "ely$";
    QStringList add_syl;
    add_syl << "ia" << "riet" << "dien" << "iu" << "io" << "ii" << "[aeiouym]bl$" << "[aeiou]{3}"
            << "^mc" << "ism$" << "[^l]lien" << "^coa[dglx]." << "[^gq]ua[^auieo]" << "dnt$";

    QTextParag * parag = textdoc->firstParag();
    for ( ; parag ; parag = parag->next() )
    {
        progress->setProgress(progress->progress()+1);
        kapp->processEvents();
        if ( progress->wasCancelled() )
            return false;

        if ( parag->at(0)->isCustom() )     // start of a table
            continue;

        QString s = parag->string()->toString();

        // Character count
        for ( uint i = 0 ; i < s.length() - 1 /*trailing-space*/ ; ++i )
        {
            QChar ch = s[i];
            ++charsWithSpace;
            if ( !ch.isSpace() )
                ++charsWithoutSpace;
        }

        // Syllable and Word count
        // Algorithm mostly taken from Greg Fast's Lingua::EN::Syllable module for Perl.
        // This guesses correct for 70-90% of English words, but the overall value
        // is quite good, as some words get a number that's too high and others get
        // one that's too low.
        QRegExp re("\\s+");
        QStringList wordlist = QStringList::split(re, s);
        words += wordlist.count();
        re.setCaseSensitive(false);
        for ( QStringList::Iterator it = wordlist.begin(); it != wordlist.end(); ++it ) {
            QString word = *it;
            if ( word.length() <= 3 ) {  // extension to the original algorithm
                syllables++;
                continue;
            }
            re.setPattern("e$");
            word.replace(re, "");
            re.setPattern("[^aeiouy]+");
            QStringList syls = QStringList::split(re, word);
            int word_syllables = 0;
            for ( QStringList::Iterator it = subs_syl.begin(); it != subs_syl.end(); ++it ) {
                re.setPattern(*it);
                if( word.contains(re) )
                    word_syllables--;
            }
            for ( QStringList::Iterator it = add_syl.begin(); it != add_syl.end(); ++it ) {
                re.setPattern(*it);
                if( word.contains(re) )
                    word_syllables++;
            }
            word_syllables += syls.count();
            if ( word_syllables == 0 )
                word_syllables = 1;
            syllables += word_syllables;
        }
        re.setCaseSensitive(true);

        // Sentence count
        // Clean up for better result, destroys the original text but we only want to count
        s = s.stripWhiteSpace();
        QChar lastchar = s.at(s.length());
        if( ! s.isEmpty() && ! KWAutoFormat::isMark( lastchar ) ) {  // e.g. for headlines
            s = s + ".";
        }
        re.setPattern("[.?!]+");         // count "..." as only one "."
        s.replace(re, ".");
        re.setPattern("\\d\\.\\d");      // don't count floating point numbers as sentences
        s.replace(re, "0,0");
        re.setPattern("[A-Z]\\.+");      // don't count "U.S.A." as three sentences
        s.replace(re, "*");
        for ( uint i = 0 ; i < s.length() ; ++i )
        {
            QChar ch = s[i];
            if ( KWAutoFormat::isMark( ch ) )
                ++sentences;
        }
    }
    return true;
}

// Only interested in the body textframeset, not in header/footer
#define kdDebugBody(area) if ( frameSetInfo() == FI_BODY ) kdDebug(area)

// Helper for adjust*. There are 3 ways to use this method.
// marginLeft set -> determination of left margin for adjustLMargin
// marginRight set -> determination of right margin for adjustRMargin
// breakBegin, breakEnd, and paragLeftMargin set -> check whether we should jump over some frames
//                                                  [when there is not enough space besides them]
void KWTextFrameSet::getMargins( int yp, int h, int* marginLeft, int* marginRight,
                                 int* breakBegin, int* breakEnd, int paragLeftMargin )
{
#ifdef DEBUG_MARGINS
    kdDebugBody(32002) << "  KWTextFrameSet " << this << "(" << getName() << ") getMargins yp=" << yp
                       << " h=" << h << " called by "
                       << (marginLeft?"adjustLMargin":marginRight?"adjustRMargin":"adjustFlow")
                       << " paragLeftMargin=" << paragLeftMargin
                       << endl;
    // Both or none...
    if (breakBegin) assert(breakEnd);
    if (breakEnd) assert(breakBegin);
#endif

    QPoint p;
    KWFrame * frame = internalToNormal( QPoint(0, yp), p );
    if (!frame)
    {
#ifdef DEBUG_MARGINS
        kdDebug(32002) << "  getMargins: internalToNormal returned frame=0L for yp=" << yp << " ->aborting with 0 margins" << endl;
#endif
        // frame == 0 happens when the parag is on a not-yet-created page (formatMore will notice afterwards)
        // Abort then, no need to return precise values
        if ( marginLeft )
            *marginLeft = 0;
        if ( marginRight )
            *marginRight = 0;
        return;
    }

    // Note: it is very important that this method works in internal coordinates.
    // Otherwise, parags broken at the line-level (e.g. between two columns) are seen
    // as still in one piece, and we miss the frames in the 2nd column.
    int left = 0;
    int from = left;
    int to = kWordDocument()->zoomItX( frame->width() );
    bool init = false;

#ifdef DEBUG_MARGINS
    kdDebugBody(32002) << "  getMargins: looking for frames between " << yp << " and " << yp+h << " (internal coords)" << endl;
#endif
    // Principle: for every frame on top at this height, we'll move from and to
    // towards each other. The text flows between 'from' and 'to'
    QValueListIterator<FrameOnTop> fIt = m_framesOnTop.begin();
    for ( ; fIt != m_framesOnTop.end() && from < to ; ++fIt )
    {
        if ( (*fIt).frame->runAround() == KWFrame::RA_BOUNDINGRECT )
        {
            QRect rectOnTop = m_doc->zoomRect( (*fIt).intersection );
#ifdef DEBUG_MARGINS
            kdDebugBody(32002) << "   getMargins found rect-on-top at (normal coords) " << DEBUGRECT(rectOnTop) << endl;
#endif
            QPoint iTop, iBottom; // top and bottom of intersection in internal coordinates
            if ( normalToInternal( rectOnTop.topLeft(), iTop ) &&
                 iTop.y() <= yp + h && // optimization
                 normalToInternal( rectOnTop.bottomRight(), iBottom ) )
            {
#ifdef DEBUG_MARGINS
                kdDebugBody(32002) << "      in internal coords: " << DEBUGRECT(QRect(iTop,iBottom)) << endl;
#endif
                // Look for intersection between p.y() -- p.y()+h  and iTop -- iBottom
                if ( QMAX( yp, iTop.y() ) <= QMIN( yp+h, iBottom.y() ) )
                {
#ifdef DEBUG_MARGINS
                    kdDebugBody(32002) << "   getMargins iTop=" << iTop.x() << "," << iTop.y()
                                       << " iBottom=" << iBottom.x() << "," << iBottom.y() << endl;
#endif
                    int availLeft = QMAX( 0, iTop.x() - from );
                    int availRight = QMAX( 0, to - iBottom.x() );
#ifdef DEBUG_MARGINS
                    kdDebugBody(32002) << "   getMargins availLeft=" << availLeft
                                       << " availRight=" << availRight << endl;
#endif
                    if ( availLeft > availRight ) // choose the max [TODO: make it configurable]
                        // flow text at the left of the frame
                        to = QMIN( to, from + availLeft - 1 );  // can only go left -> QMIN
                    else
                        // flow text at the right of the frame
                        from = QMAX( from, to - availRight + 1 ); // can only go right -> QMAX
#ifdef DEBUG_MARGINS
                    kdDebugBody(32002) << "   getMargins from=" << from << " to=" << to << endl;
#endif
                    // If the available space is too small, give up on it
                    if ( to - from < kWordDocument()->zoomItX( 15 ) + paragLeftMargin )
                        from = to;

                    if ( breakEnd && from == to ) // no-space case
                    {
                        if ( !init ) // first time
                        {
                            init = true;
                            *breakBegin = iTop.y();
                            *breakEnd = iBottom.y();
                        }
                        else
                        {
                            *breakBegin = QMIN( *breakBegin, iTop.y() );
                            *breakEnd = QMAX( *breakEnd, iBottom.y() );
                        }
#ifdef DEBUG_MARGINS
                        kdDebugBody(32002) << "   getMargins iBottom.y=" << iBottom.y()
                                           << " breakBegin=" << *breakBegin
                                           << " breakEnd=" << *breakEnd << endl;
#endif
                    }
                } // else no intersection
            }// else we got a 0L, or the iTop.y()<=yp+h test didn't work - wrong debug output
            // kdDebugBody(32002) << "   gerMargins: normalToInternal returned 0L" << endl;
        }
    }
#ifdef DEBUG_MARGINS
    kdDebugBody(32002) << "   getMargins done. from=" << from << " to=" << to << endl;
#endif
    if ( from == to ) { // no-space case. Drop the margins we found - we'll reformat again.
        from = 0;
        to = kWordDocument()->zoomItX( frame->width() );
    }

    if ( marginLeft )
        *marginLeft = from;
    if ( marginRight )
    {
#ifdef DEBUG_MARGINS
        kdDebug(32002) << "    getMargins " << getName()
                       << " textdoc's width=" << textdoc->width()
                       << " to=" << to << endl;
#endif
        *marginRight = textdoc->width() - to;
    }
}

int KWTextFrameSet::adjustLMargin( int yp, int h, int margin, int space )
{
    int marginLeft;
    getMargins( yp, h, &marginLeft, 0L, 0L, 0L );
#ifdef DEBUG_MARGINS
    kdDebugBody(32002) << "KWTextFrameSet::adjustLMargin marginLeft=" << marginLeft << endl;
#endif
    return QTextFlow::adjustLMargin( yp, h, margin + marginLeft, space );
}

int KWTextFrameSet::adjustRMargin( int yp, int h, int margin, int space )
{
    int marginRight;
    getMargins( yp, h, 0L, &marginRight, 0L, 0L );
#ifdef DEBUG_MARGINS
    kdDebugBody(32002) << "KWTextFrameSet::adjustRMargin marginRight=" << marginRight << endl;
#endif
    return QTextFlow::adjustRMargin( yp, h, margin + marginRight, space );
}

// helper for adjustFlow
bool KWTextFrameSet::checkVerticalBreak( int & yp, int & h, QTextParag * parag, bool linesTogether, int breakBegin, int breakEnd )
{
    // We need the "+1" here because when skipping a frame on top, we want to be _under_
    // its bottom. Without the +1, we hit the frame again on the next adjustLMargin call.

    // Check for intersection between the parag (yp -- yp+h) and the break area (breakBegin -- breakEnd)
    if ( QMAX( yp, breakBegin ) <= QMIN( yp+h, breakEnd ) )
    {
        if ( !parag || linesTogether ) // Paragraph-level breaking
        {
#ifdef DEBUG_FLOW
            kdDebug(32002) << "checkVerticalBreak ADJUSTING yp=" << yp << " h=" << h
                           << " breakEnd+2 [new value for yp]=" << breakEnd+2 << endl;
#endif
            yp = breakEnd + 1;
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
                            yp = breakEnd + 1;
                            return true;
                        }
                        dy = breakEnd + 1 - y;
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
            parag->setMovedDown( true );
            parag->setHeight( h + dy );
#ifdef DEBUG_FLOW
            kdDebug(32002) << "Paragraph height set to " << h+dy << endl;
#endif
            h += dy;
            return true;
        }
    }
    return false;
}

void KWTextFrameSet::adjustFlow( int &yp, int w, int h, QTextParag * _parag, bool /*pages*/ )
{
    // This is called since the 'vertical break' QRT flag is true.
    // End of frames/pages lead to those "vertical breaks".
    // What we do, is adjust the Y accordingly,
    // to implement page-break at the paragraph level and at the line level.
    // It's cumulative (the space of one break will be included in the further
    // paragraph's y position), which makes it easy to implement.
    // But don't forget that adjustFlow is called twice for every parag, since the formatting
    // is re-done after moving down.

    int breaked = false;
    KWTextParag *parag = static_cast<KWTextParag *>( _parag );
    bool linesTogether = parag ? parag->linesTogether() : false;
    bool hardFrameBreak = parag ? parag->hardFrameBreakBefore() : false;
    if ( !hardFrameBreak && parag && parag->prev() )
        hardFrameBreak = static_cast<KWTextParag *>(parag->prev())->hardFrameBreakAfter();

#ifdef DEBUG_FLOW
    kdDebugBody(32002) << "KWTextFrameSet::adjustFlow parag=" << parag
                       << " linesTogether=" << linesTogether << " hardFrameBreak=" << hardFrameBreak
                       << " yp=" << yp
                       << " h=" << h << endl;
#endif

    int totalHeight = 0;
    QListIterator<KWFrame> frameIt( frameIterator() );
    for ( ; frameIt.current(); ++frameIt )
    {
        int frameHeight = kWordDocument()->zoomItY( frameIt.current()->height() );
        int bottom = totalHeight + frameHeight;
        // Only skip bottom of frame if there is a next one or if there'll be another one created.
        // ( Not for header/footer, for instance. )
        bool check = frameIt.atLast() && frameIt.current()->getFrameBehaviour() == KWFrame::AutoCreateNewFrame;
        if ( !check )
        {
            // ## TODO optimize this [maybe we should simply start from the end in the main loop?]
            // Or cache the attribute ( e.g. "frame->hasCopy()" ).
            QListIterator<KWFrame> nextFrame( frameIt );
            while ( !check && !nextFrame.atLast() )
            {
                ++nextFrame;
                if ( !nextFrame.current()->isCopy() )
                    check = true; // Found a frame after us that isn't a copy => we have somewhere for our overflow
            }
        }

        if ( check )
        {
            if ( hardFrameBreak && yp > totalHeight && yp < bottom && !parag->isMovedDown() )
            {
                // The paragraph wants a frame break before it, and is in the current frame
                // The last check is for whether we did the frame break already
                // (adjustFlow is called twice for each paragraph, if a break was done)
                yp = bottom /*+ 2*/;
#ifdef DEBUG_FLOW
                kdDebug(32002) << "KWTextFrameSet::adjustFlow -> HARD FRAME BREAK" << endl;
                kdDebug(32002) << "KWTextFrameSet::adjustFlow yp now " << yp << endl;
#endif
                break;
            }

#ifdef DEBUG_FLOW
            kdDebug(32002) << "KWTextFrameSet::adjustFlow frameHeight=" << frameHeight << " bottom=" << bottom << endl;
#endif
            // don't move down parags that have only one line and are bigger than the page (e.g. floating tables)
            if ( h < frameHeight || ( parag && parag->lineStartList().count() > 1 ) )
            {

                // breakBegin==breakEnd==bottom, since the next frame's top is the same as bottom, in QRT coords.
                breaked = ( checkVerticalBreak( yp, h, parag, linesTogether, bottom, bottom ) );
            }
            // Some people write a single paragraph over 3 frames! So we have to keep looking...
            //if ( breaked )
            //    break;

        }
        if ( yp+h < bottom )
            break; // we've been past the parag, so stop here
        totalHeight = bottom;
    }

    // Another case for a vertical break is frames with the RA_SKIP flag
    QValueListIterator<FrameOnTop> fIt = m_framesOnTop.begin();
    for ( ; fIt != m_framesOnTop.end() ; ++fIt )
    {
        if ( (*fIt).frame->runAround() == KWFrame::RA_SKIP )
        {
            QRect rectOnTop = m_doc->zoomRect( (*fIt).intersection );
            QPoint iTop, iBottom; // top and bottom in internal coordinates
            if ( normalToInternal( rectOnTop.topLeft(), iTop ) &&
                 iTop.y() <= yp + h &&
                 normalToInternal( rectOnTop.bottomLeft(), iBottom ) &&
                 checkVerticalBreak( yp, h, parag, linesTogether,
                                     iTop.y(), iBottom.y() ) )
            {
                kdDebug(32002) << "KWTextFrameSet::adjustFlow breaking around RA_SKIP frame yp="<<yp<<" h=" << h << endl;
                // We don't "break;" here because there could be another such frame below the first one
                // We assume that the frames on top are in order ( top to bottom ), btw.
                // They should be, since updateFrames reorders before updating frames-on-top
                breaked = true;
            }
        }
    }

    // And the last case for a vertical break is RA_BOUNDINGRECT frames that
    // leave no space by their side for any text (e.g. most tables)
    int breakBegin = 0;
    int breakEnd = 0;
    getMargins( yp, h, 0L, 0L, &breakBegin, &breakEnd, parag ? QMAX( parag->firstLineMargin(), parag->leftMargin() ) : 0 );
    if ( breakEnd )
    {
        kdDebug(32002) << "KWTextFrameSet::adjustFlow no-space case. breakBegin=" << breakBegin
                       << " breakEnd=" << breakEnd << " h=" << h << endl;
        ASSERT( breakBegin <= breakEnd );
        if ( checkVerticalBreak( yp, h, parag, linesTogether, breakBegin, breakEnd ) )
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

    // We also use adjustFlow as a hook into the formatting algo, to fix the parag rect if necessary.
    if ( parag && parag->hasBorder() )
    {
        parag->setWidth( textdoc->width() - 1 );
    }
    // Fixing the parag rect for the formatting chars (CR and frame break).
    else if ( parag && m_doc->viewFormattingChars() )
    {
        if ( parag->hardFrameBreakAfter() )
        {
            QTextFormat * lastFormat = parag->at( parag->length() - 1 )->format();
            // keep in sync with KWTextFrameSet::adjustFlow
            QString str = i18n( "--- Frame Break ---" );
            int width = 0;
            for ( int i = 0 ; i < (int)str.length() ; ++i )
                width += lastFormat->width( str, i );
            parag->setWidth( parag->rect().width() + width );
        }
        else if ( parag->lineStartList().count() == 1 ) // don't use lines() here, parag not formatted yet
        {
            QTextFormat * lastFormat = parag->at( parag->length() - 1 )->format();
            parag->setWidth( parag->rect().width() + lastFormat->width('x') );
        }
    }
}

// ################## Not called anymore, to be removed
void KWTextFrameSet::eraseAfter( QTextParag *, QPainter *, const QColorGroup & )
{
}

KWTextFrameSet::~KWTextFrameSet()
{
    textdoc->forgetFlow();
    delete textdoc;
    //kdDebug(32001) << "KWTextFrameSet::~KWTextFrameSet" << endl;
    m_doc = 0L;
}

// This struct is used for sorting frames.
// Since pages are one below the other, simply sorting on (y, x) does what we want.
struct FrameStruct
{
    KWFrame * frame;
    bool operator < ( const FrameStruct & t ) const {
        return frame->y() < t.frame->y() ||
            ( frame->y() == t.frame->y() && frame->x() < t.frame->x() );
    }
    bool operator <= ( const FrameStruct & t ) const {
        return frame->y() < t.frame->y() ||
            ( frame->y() == t.frame->y() && frame->x() <= t.frame->x() );
    }
    bool operator > ( const FrameStruct & t ) const {
        return frame->y() > t.frame->y() ||
            ( frame->y() == t.frame->y() && frame->x() > t.frame->x() );
    }
};

void KWTextFrameSet::updateFrames()
{
    // Not visible ? Don't bother then.
    if ( !isVisible() )
        return;

    if ( frames.isEmpty() )
        return; // No frames. This happens when the frameset is deleted (still exists for undo/redo)

    //kdDebug(32002) << "KWTextFrameSet::updateFrames " << getName() << " frame-count=" << frames.count() << endl;

    // Sort frames of this frameset on (page, y coord, x coord)

    QValueList<FrameStruct> sortedFrames;

    int width = 0;
    QListIterator<KWFrame> frameIt( frameIterator() );
    for ( ; frameIt.current(); ++frameIt )
    {
        // Calculate max width while we're at it
        width = QMAX( width, kWordDocument()->zoomItX( frameIt.current()->width() ) );

        FrameStruct str;
        str.frame = frameIt.current();
        sortedFrames.append( str );
    }
    if ( width != textdoc->width() )
    {
        //kdDebug(32002) << "KWTextFrameSet::updateFrames setWidth " << width << endl;
        textdoc->setMinimumWidth( -1, 0 );
        textdoc->setWidth( width );
    } //else kdDebug(32002) << "KWTextFrameSet::updateFrames width already " << width << endl;

    qHeapSort( sortedFrames );

    // Prepare the m_framesInPage structure
    m_firstPage = sortedFrames.first().frame->pageNum();
    int lastPage = sortedFrames.last().frame->pageNum();
    int oldSize = m_framesInPage.size();
    m_framesInPage.resize( lastPage - m_firstPage + 1 );
    // Clear the old elements
    int oldElements = QMIN( oldSize, (int)m_framesInPage.size() );
    for ( int i = 0 ; i < oldElements ; ++i )
        m_framesInPage[i]->clear();
    // Initialize the new elements.
    for ( int i = oldElements ; i < (int)m_framesInPage.size() ; ++i )
        m_framesInPage.insert( i, new QList<KWFrame>() );

    // Re-fill the frames list with the frames in the right order,
    // and re-fill m_framesInPage at the same time.
    frames.setAutoDelete( false );
    frames.clear();
    m_availableHeight = 0;
    int internalY = 0;
    int lastRealFrameHeight = 0;
    QValueList<FrameStruct>::Iterator it = sortedFrames.begin();
    for ( ; it != sortedFrames.end() ; ++it )
    {
        KWFrame * frame = (*it).frame;
        frames.append( frame );

        ASSERT( frame->pageNum() <= lastPage );
        m_framesInPage[frame->pageNum() - m_firstPage]->append( frame );

        if ( !frame->isCopy() )
            internalY += lastRealFrameHeight;

        frame->setInternalY( internalY );

        // Update m_availableHeight with the internal height of this frame - unless it's a copy
        if ( ! ( frame->isCopy() && it != sortedFrames.begin() ) )
        {
            lastRealFrameHeight = m_doc->zoomItY( frame->height() );
            m_availableHeight += lastRealFrameHeight;
        }
    }

    //kdDebugBody(32002) << this << " KWTextFrameSet::updateFrames m_availableHeight=" << m_availableHeight << endl;
    frames.setAutoDelete( true );

    KWFrameSet::updateFrames();
}

const QList<KWFrame> & KWTextFrameSet::framesInPage( int pageNum ) const
{
    if ( pageNum < m_firstPage || pageNum >= (int)m_framesInPage.size() + m_firstPage )
    {
#ifdef DEBUG_NTI
        kdWarning() << getName() << " framesInPage called for pageNum=" << pageNum << ". "
                    << " Min value: " << m_firstPage
                    << " Max value: " << m_framesInPage.size() + m_firstPage - 1 << endl;
#endif
        return m_emptyList; // QList<KWFrame>() doesn't work, it's a temporary
    }
    return * m_framesInPage[pageNum - m_firstPage];
}

KWFrame * KWTextFrameSet::internalToNormal( QPoint iPoint, QPoint & nPoint ) const
{
#ifdef DEBUG_ITN
    kdDebug() << getName() << " ITN called for iPoint=" << iPoint.x() << "," << iPoint.y() << endl;
#endif
    // This does a binary search in the m_framesInPage array, with internalY as criteria
    // We only look at the first frame of each page. Refining is done later on.
    ASSERT( !m_framesInPage.isEmpty() );
    int len = m_framesInPage.count();
    int n1 = 0;
    int n2 = len - 1;
    int internalY = 0;
    int mid = 0;
    bool found = FALSE;
    while ( n1 <= n2 ) {
        int res;
        mid = (n1 + n2)/2;
#ifdef DEBUG_ITN
        kdDebug() << "ITN: begin. mid=" << mid << endl;
#endif
        ASSERT( m_framesInPage[mid] ); // We have no null items
        if ( m_framesInPage[mid]->isEmpty() )
            res = -1;
        else
        {
            KWFrame * frame = m_framesInPage[mid]->first();
            internalY = frame->internalY();
#ifdef DEBUG_ITN
            kdDebug() << "ITN: iPoint.y=" << iPoint.y() << " internalY=" << internalY << endl;
#endif
            res = iPoint.y() - internalY;
#ifdef DEBUG_ITN
            kdDebug() << "ITN: res=" << res << endl;
#endif
            // Anything between this internalY (top) and internalY+height (bottom) is fine
            // (Using the next page's first frame's internalY only works if there is a frame on the next page)
            if ( res >= 0 )
            {
                int height = m_doc->zoomItY( frame->height() );
#ifdef DEBUG_ITN
                kdDebug() << "ITN: height=" << height << endl;
#endif
                if ( iPoint.y() < internalY + height )
                {
#ifdef DEBUG_ITN
                    kdDebug() << "ITN: found a match " << mid << endl;
#endif
                    found = true;
                    break;
                }
            }
        }
        ASSERT( res != 0 ); // this should have been already handled !
        if ( res < 0 )
            n2 = mid - 1;
        else // if ( res > 0 )
            n1 = mid + 1;
#ifdef DEBUG_ITN
        kdDebug() << "ITN: End of loop. n1=" << n1 << " n2=" << n2 << endl;
#endif
    }
    if ( !found )
    {
        // Not found (n2 < n1)
        // We might have missed the frame because n2 has many frames
        // (and we only looked at the first one).
        mid = n2;
#ifdef DEBUG_ITN
        kdDebug() << "ITN: Setting mid to n2=" << mid << endl;
#endif
        if ( mid < 0 )
        {
//#ifdef DEBUG_ITN
            kdDebug(32002) << "KWTextFrameSet::internalToNormal " << iPoint.x() << "," << iPoint.y()
                           << " before any frame of " << (void*)this << endl;
//#endif
            nPoint = iPoint; // "bah", I said above :)
            return 0L;
        }
    }
    // search to first of equal items
    // This happens with copied frames, which have the same internalY
    int result = mid;
    while ( mid - 1 >= 0 )
    {
        mid--;
        if ( !m_framesInPage[mid]->isEmpty() )
        {
            KWFrame * frame = m_framesInPage[mid]->first();
#ifdef DEBUG_ITN
            kdDebug() << "KWTextFrameSet::internalToNormal going back to page " << mid << " - frame: " << frame->internalY() << endl;
#endif
            if ( frame->internalY() == internalY ) // same internalY as the frame we found before
                result = mid;
            else
                break;
        }
    }

    // Now iterate over the frames in page 'result' and find the right one
    QListIterator<KWFrame> frameIt( *m_framesInPage[result] );
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame *frame = frameIt.current();
        QRect frameRect = kWordDocument()->zoomRect( *frame );
        QRect r( 0, frame->internalY(), frameRect.width(), frameRect.height() );
        // r is the frame in qrt coords
        if ( r.contains( iPoint ) ) // both r and p are in "qrt coordinates"
        {
            nPoint.setX( iPoint.x() + frameRect.left() );
            nPoint.setY( iPoint.y() + frameRect.top() - frame->internalY() );
            return frame;
        }
    }
#ifdef DEBUG_ITN
    kdDebug(32002) << "KWTextFrameSet::internalToNormal " << iPoint.x() << "," << iPoint.y()
                   << " not in any frame of " << (void*)this << " (looked on page " << result << ")" << endl;
#endif
    nPoint = iPoint; // bah again
    return 0L;
}

#ifndef NDEBUG
void KWTextFrameSet::printDebug()
{
    KWFrameSet::printDebug();
    if ( !isDeleted() )
    {
        kdDebug() << "QTextDocument width = " << textdoc->width() << endl;
        kdDebug() << " -- Frames in page array -- " << endl;
        for ( uint i = 0 ; i < m_framesInPage.size() ; ++i )
        {
            QListIterator<KWFrame> it( *m_framesInPage[i] );
            for ( ; it.current() ; ++it )
                kdDebug() << i + m_firstPage << ": " << it.current() << "   " << DEBUGRECT( *it.current() )
                          << " internalY=" << it.current()->internalY() << endl;
        }
    }
}
#endif

QDomElement KWTextFrameSet::saveInternal( QDomElement &parentElem, bool saveFrames, bool saveAnchorsFramesets )
{
    if ( frames.isEmpty() ) // Deleted frameset -> don't save
        return QDomElement();
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
        framesetElem.setAttribute( "removable", static_cast<int>( m_removeableHeader ) );
    }

    KWFrameSet::saveCommon( framesetElem, saveFrames );

    // Save paragraphs
    KWTextParag *start = static_cast<KWTextParag *>( textDocument()->firstParag() );
    while ( start ) {
        start->save( framesetElem, saveAnchorsFramesets );
        start = static_cast<KWTextParag *>( start->next() );
    }

    zoom( false );
    return framesetElem;
}

void KWTextFrameSet::load( QDomElement &attributes, bool loadFrames )
{
    KWFrameSet::load( attributes, loadFrames );

    textdoc->clear(false); // Get rid of dummy paragraph (and more if any)
    KWTextParag *lastParagraph = 0L;

    // <PARAGRAPH>
    QDomElement paragraph = attributes.firstChild().toElement();
    for ( ; !paragraph.isNull() ; paragraph = paragraph.nextSibling().toElement() )
    {
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
        static_cast<KWTextParag *>( textdoc->firstParag() )->setStyle( m_doc->findStyle( "Standard" ) );
    }
    else
        textdoc->setLastParag( lastParagraph );

    m_lastFormatted = textdoc->firstParag();
    //kdDebug(32001) << "KWTextFrameSet::load done" << endl;
}

void KWTextFrameSet::zoom( bool forPrint )
{
    if ( !m_origFontSizes.isEmpty() )
        unzoom();
    QTextFormatCollection * coll = textdoc->formatCollection();
    // This is because we are setting pt sizes (so Qt applies x11AppDpiY already)
    // If you change this, fix zoomedFontSize too.
    double factor = kWordDocument()->zoomedResolutionY() *
                    ( forPrint ? 1.0 : 72.0 / QPaintDevice::x11AppDpiY() );
    //kdDebugBody(32002) << "KWTextFrameSet::zoom factor=" << factor << endl;

#ifdef DEBUG_FORMATS
    kdDebug(32002) << this << " KWTextFrameSet::zoom " << factor << " coll=" << coll << " " << coll->dict().count() << " items " << endl;
    kdDebug(32002) << this << " firstparag:" << textdoc->firstParag()
                   << " format:" << textdoc->firstParag()->paragFormat()
                   << " first-char's format:" << textdoc->firstParag()->at(0)->format()
                   << endl;
#endif
    QDict<QTextFormat> dictCopy( coll->dict() );
    coll->dict().setAutoDelete( false );
    coll->dict().clear();
    coll->dict().setAutoDelete( true );
    QDictIterator<QTextFormat> it( dictCopy );
    for ( ; it.current() ; ++it ) {
        KWTextFormat * format = dynamic_cast<KWTextFormat *>(it.current());
        ASSERT( format );
        m_origFontSizes.insert( format, new int( format->font().pointSize() ) );
#ifdef DEBUG_FORMATS
        kdDebugBody(32002) << this << " KWTextFrameSet::zooming format " << format
                           << " key=" << format->key()
                           << " from " << format->font().pointSizeFloat()
                           << " to " << format->font().pointSizeFloat() * factor << endl;
#endif
        format->setPointSizeFloat( format->font().pointSizeFloat() * factor );
        coll->dict().insert( format->key(), format );
    }
    // Do the same to the default format !
    KWTextFormat * format = static_cast<KWTextFormat *>(coll->defaultFormat());
#ifdef DEBUG_FORMATS
    kdDebug() << "KWTextFrameSet::zoom default format " << format << " " << format->key() << endl;
#endif
    m_origFontSizes.insert( format, new int( format->font().pointSize() ) );
    format->setPointSizeFloat( format->font().pointSizeFloat() * factor );

    // Update Tab Width
    textDocument()->setTabStops( textDocument()->formatCollection()->defaultFormat()->width( 'x' ) * 8 );

    // Zoom all custom items
    QListIterator<QTextCustomItem> cit( textdoc->allCustomItems() );
    for ( ; cit.current() ; ++cit )
        static_cast<KWTextCustomItem *>( cit.current() )->resize();

    // Mark all paragraphs as changed !
    for ( KWTextParag * s = static_cast<KWTextParag *>( textdoc->firstParag() ) ; s ; s = static_cast<KWTextParag *>( s->next() ) )
    {
        s->setTabList( s->tabList() ); // to recalculate with the new zoom
        s->setChanged( TRUE );
        s->invalidate( 0 );
        if ( s->counter() )
            s->counter()->invalidate();
    }
    m_lastFormatted = textdoc->firstParag();
    m_availableHeight = -1; // to be recalculated
    //emit ensureCursorVisible(); // not here. We don't want this when saving.
    KWFrameSet::zoom( forPrint );
}

void KWTextFrameSet::unzoom()
{
    QTextFormatCollection * coll = textdoc->formatCollection();

    QDict<QTextFormat> dictCopy( coll->dict() );
    coll->dict().setAutoDelete( false );
    coll->dict().clear();
    coll->dict().setAutoDelete( true );
    QDictIterator<QTextFormat> it( dictCopy );
    for ( ; it.current() ; ++it ) {
        KWTextFormat * format = static_cast<KWTextFormat *>(it.current());
        int * oldSize = m_origFontSizes.find( format );
        if ( !oldSize )
            kdWarning() << "Can't unzoom: format=" << format << " " << it.current()->key() << endl;
        else
        {
#ifdef DEBUG_FORMATS
            kdDebugBody(32002) << "KWTextFrameSet::unzoom format=" << format
                               << " key=" << format->key()
                               << " oldSize=" << *oldSize << endl;
#endif
            format->setPointSizeFloat( *oldSize );
        }
        coll->dict().insert( format->key(), format );
    }
    KWTextFormat * format = static_cast<KWTextFormat *>(coll->defaultFormat());
    int * oldSize = m_origFontSizes.find( format );
    if ( oldSize )
        format->setPointSizeFloat( *oldSize );

    m_origFontSizes.clear();
}

void KWTextFrameSet::preparePrinting( QPainter *painter, QProgressDialog *progress, int &processedParags )
{
    //textdoc->doLayout( painter, textdoc->width() );
    textdoc->setWithoutDoubleBuffer( painter != 0 );

    textdoc->formatCollection()->setPainter( painter );
    QTextParag *parag = textdoc->firstParag();
    while ( parag ) {
        parag->invalidate( 0 );
        parag->setPainter( painter );
        if ( painter )
            parag->format();
        parag = parag->next();
        if ( progress )
            progress->setProgress( ++processedParags );
    }
}

int KWTextFrameSet::docFontSize( QTextFormat * format ) const
{
    ASSERT( format );
    int * oldSize = m_origFontSizes.find( format );
    if ( !oldSize )
    {
        kdDebug() << "Can't find format in m_origFontSizes: " << format << "( " << format->key() << ")" << endl;
        return 0;
    }
    else
    {
        //kdDebug() << "KWTextFrameSet::docFontSize : format=" << format << " doc font size=" << *oldSize << endl;
        return *oldSize;
    }
}

/*int KWTextFrameSet::docFontSize( float zoomedFontSize ) const
{
    return zoomedFontSize / kWordDocument()->zoomedResolutionY();
}*/

float KWTextFrameSet::zoomedFontSize( int docFontSize ) const
{
#ifdef DEBUG_FORMATS
    kdDebug() << "   KWTextFrameSet::zoomedFontSize docFontSize=" << docFontSize
              << " zoomedResolutionY=" << kWordDocument()->zoomedResolutionY()
              << " -> " << static_cast<float>( docFontSize ) * kWordDocument()->zoomedResolutionY()
              << endl;
#endif
    // Has to be the same calculation as in zoom(), except that here we are never printing
    double factor = kWordDocument()->zoomedResolutionY() * 72.0 / QPaintDevice::x11AppDpiY();
    return static_cast<float>( docFontSize ) * factor;
}

KWTextFormat * KWTextFrameSet::zoomFormatFont( const KWTextFormat * f )
{
    KWTextFormat format = *f;
    int origFontSize = format.font().pointSize();
    format.setPointSizeFloat( zoomedFontSize( origFontSize ) );                // zoom it
    QTextFormat * fcf = textDocument()->formatCollection()->format( &format );   // find it in the collection
#ifdef DEBUG_FORMATS
    kdDebug() << "KWTextFrameSet::zoomFormatFont new format is " << fcf << " " << fcf->key() << endl;
#endif
    if ( !m_origFontSizes.find( fcf ) )
    {
#ifdef DEBUG_FORMATS
        kdDebug() << "KWTextFrameSet::zoomFormatFont inserting " << fcf << " " << fcf->key() << " into m_origFontSizes" << endl;
#endif
        m_origFontSizes.insert( fcf, new int( origFontSize ) );           // insert in dict if necessary
    }
    return static_cast<KWTextFormat *>(fcf);
}

void KWTextFrameSet::applyStyleChange( KWStyle * changedStyle, int paragLayoutChanged, int formatChanged )
{
    // TODO: if we are concerned about performance when updating many styles,
    // We could make a single call to this method, with a QMap<KWStyle *, struct holding flags>
    // in order to iterate over all paragraphs only once.

    /*kdDebug(32001) << "KWTextFrameSet::applyStyleChange " << changedStyle->name()
                     << " paragLayoutChanged=" << paragLayoutChanged
                     << " formatChanged=" << formatChanged
                     << endl;*/

    // setFormat(of,nf,flags) might create new formats, so we need to unzoom first
    // and zoom all formats again at the end.
    unzoom();
    QTextDocument * textdoc = textDocument();
    KWTextParag *p = static_cast<KWTextParag *>(textdoc->firstParag());
    while ( p ) {
        if ( p->style() == changedStyle )
        {
            if ( paragLayoutChanged == -1 || formatChanged == -1 ) // Style has been deleted
            {
                p->setStyle( m_doc->findStyle( "Standard" ) ); // keeps current formatting
                // TODO, make this undoable somehow
            }
            else
            {
                // Apply this style again, to get the changes
                QTextCursor cursor( textdoc );
                cursor.setParag( p );
                cursor.setIndex( 0 );
                kdDebug() << "KWTextFrameSet::applyStyleChange applying to paragraph " << p << " " << p->paragId() << endl;
#if 0
                KWStyle styleApplied=*style;
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
                applyStyle( &cursor, changedStyle,
                            -1, // A selection we can't possibly have
                            paragLayoutChanged, formatChanged,
                            false, false, false ); // don't zoom formats, don't create undo/redo, not interactive
            }
        }
        p = static_cast<KWTextParag *>(p->next());
    }
    zoom( false );
    //setLastFormattedParag( textdoc->firstParag() ); // done by zoom
    formatMore();
    emit repaintChanged( this );
    emit updateUI( true );
}

void KWTextFrameSet::addTextFramesets( QList<KWTextFrameSet> & lst )
{
    lst.append(this);
}


#if 0
KWTextFrameSet *KWTextFrameSet::getCopy() {
    /* returns a deep copy of self */
    KWTextFrameSet *newFS = new KWTextFrameSet(m_doc, getName());
    newFS->setFrameSetInfo(frameSetInfo());
    newFS->setVisible(m_visible);
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
#endif

void KWTextFrameSet::doKeyboardAction( QTextCursor * cursor, KWTextFormat * & /*currentFormat*/, KeyboardAction action )
{
    KWTextParag * parag = static_cast<KWTextParag *>(cursor->parag());
    setLastFormattedParag( parag );
    emit hideCursor();
    bool doUpdateCurrentFormat = true;
    switch ( action ) {
    case ActionDelete: {
        checkUndoRedoInfo( cursor, UndoRedoInfo::Delete );
        if ( !undoRedoInfo.valid() ) {
            newPlaceHolderCommand( i18n("Delete text") );
            undoRedoInfo.id = parag->paragId();
            undoRedoInfo.index = cursor->index();
            undoRedoInfo.text = QString::null;
            undoRedoInfo.oldParagLayouts << parag->paragLayout();
        }
        QTextStringChar * ch = parag->at( cursor->index() );
        undoRedoInfo.text += ch->c;
        copyCharFormatting( parag, cursor->index(), undoRedoInfo.text.length()-1, true );
        KoParagLayout paragLayout;
        if ( parag->next() )
            paragLayout = static_cast<KWTextParag *>( parag->next() )->paragLayout();

        QTextParag *old = cursor->parag();
        if ( cursor->remove() ) {
            if ( old != cursor->parag() && m_lastFormatted == old ) // 'old' has been deleted
                m_lastFormatted = cursor->parag() ? cursor->parag()->prev() : 0;
            undoRedoInfo.text += "\n";
            undoRedoInfo.oldParagLayouts << paragLayout;
        }
    } break;
    case ActionBackspace: {
        // Remove counter
        if ( parag->counter() && parag->counter()->style() != KoParagCounter::STYLE_NONE && cursor->index() == 0 ) {
            // parag->decDepth(); // We don't have support for nested lists at the moment
                                  // (only in titles, but you don't want Backspace to move it up)
            KoParagCounter c;
            KCommand *cmd=setCounterCommand( cursor, c );
            if(cmd)
                m_doc->addCommand(cmd);
        }
        else
        {
            checkUndoRedoInfo( cursor, UndoRedoInfo::Delete );
            if ( !undoRedoInfo.valid() ) {
                newPlaceHolderCommand( i18n("Delete text") );
                undoRedoInfo.id = parag->paragId();
                undoRedoInfo.index = cursor->index();
                undoRedoInfo.text = QString::null;
                undoRedoInfo.oldParagLayouts << parag->paragLayout();
            }
            cursor->gotoLeft();
            QTextStringChar * ch = cursor->parag()->at( cursor->index() );
            undoRedoInfo.text.prepend( QString( ch->c ) );
            copyCharFormatting( cursor->parag(), cursor->index(), 0, true );
            undoRedoInfo.index = cursor->index();
            KoParagLayout paragLayout = static_cast<KWTextParag *>( cursor->parag() )->paragLayout();
            if ( cursor->remove() ) {
                undoRedoInfo.text.remove( 0, 1 );
                undoRedoInfo.text.prepend( "\n" );
                undoRedoInfo.index = cursor->index();
                undoRedoInfo.id = cursor->parag()->paragId();
                undoRedoInfo.oldParagLayouts.prepend( paragLayout );
            }
            m_lastFormatted = cursor->parag();
        }
    } break;
    case ActionReturn: {
        checkUndoRedoInfo( cursor, UndoRedoInfo::Return );
        if ( !undoRedoInfo.valid() ) {
            newPlaceHolderCommand( i18n("Insert text") );
            undoRedoInfo.id = cursor->parag()->paragId();
            undoRedoInfo.index = cursor->index();
            undoRedoInfo.text = QString::null;
        }
        undoRedoInfo.text += "\n";
        cursor->splitAndInsertEmptyParag();
        ASSERT( cursor->parag()->prev() );
        if ( cursor->parag()->prev() )
        {
            setLastFormattedParag( cursor->parag()->prev() );
            KWAutoFormat * autoFormat = kWordDocument()->getAutoFormat();
            if ( autoFormat )
                autoFormat->doAutoFormat( cursor, static_cast<KWTextParag*>(cursor->parag()->prev()),
                                          cursor->parag()->prev()->length() - 1, '\n' );
        }

        doUpdateCurrentFormat = false;
        KWStyle * style = static_cast<KWTextParag *>( cursor->parag()->prev() )->style();
        if ( style )
        {
            KWStyle * newStyle = style->followingStyle();
            if ( newStyle && style != newStyle ) // different "following style" applied
            {
                doUpdateCurrentFormat = true;
                //currentFormat = static_cast<KWTextFormat *>( textDocument()->formatCollection()->format( cursor->parag()->paragFormat() ) );
                //kdDebug() << "KWTextFrameSet::doKeyboardAction currentFormat=" << currentFormat << " " << currentFormat->key() << endl;
            }
        }

    } break;
    case ActionKill:
        checkUndoRedoInfo( cursor, UndoRedoInfo::Delete );
        if ( !undoRedoInfo.valid() ) {
            newPlaceHolderCommand( i18n("Delete text") );
            undoRedoInfo.id = cursor->parag()->paragId();
            undoRedoInfo.index = cursor->index();
            undoRedoInfo.text = QString::null;
            undoRedoInfo.oldParagLayouts << parag->paragLayout();
        }
        if ( cursor->atParagEnd() ) {
            QTextStringChar * ch = cursor->parag()->at( cursor->index() );
            undoRedoInfo.text += ch->c;
            copyCharFormatting( parag, cursor->index(), undoRedoInfo.text.length()-1, true );
            KoParagLayout paragLayout;
            if ( parag->next() )
                paragLayout = static_cast<KWTextParag *>( parag->next() )->paragLayout();
            if ( cursor->remove() )
            {
                undoRedoInfo.text += "\n";
                undoRedoInfo.oldParagLayouts << paragLayout;
            }
        } else {
            int oldLen = undoRedoInfo.text.length();
            undoRedoInfo.text += cursor->parag()->string()->toString().mid( cursor->index() );
            for ( int i = cursor->index(); i < cursor->parag()->length(); ++i )
                copyCharFormatting( cursor->parag(), i, oldLen + i - cursor->index(), true );
            cursor->killLine();
        }
        break;
    }

    if ( !undoRedoInfo.customItemsMap.isEmpty() )
        clearUndoRedoInfo();

    formatMore();
    emit repaintChanged( this );
    emit ensureCursorVisible();
    emit showCursor();
    emit updateUI( doUpdateCurrentFormat );
}

void KWTextFrameSet::ensureFormatted( QTextParag * parag )
{
    while ( !parag->isValid() )
    {
        if ( !m_lastFormatted || !isVisible() || m_availableHeight == -1 )
            return; // formatMore will do nothing -> give up
        formatMore();
    }
}

void KWTextFrameSet::formatMore()
{
    if ( !m_lastFormatted || !isVisible() || m_availableHeight == -1 )
        return;

    int bottom = 0;
    int to = !sender() ? 2 : 20; // 20 when it comes from the formatTimer

    int viewsBottom = 0;
    QMapIterator<QWidget *, int> mapIt = m_mapViewAreas.begin();
    for ( ; mapIt != m_mapViewAreas.end() ; ++mapIt )
        viewsBottom = QMAX( viewsBottom, mapIt.data() );

#ifdef DEBUG_FORMAT_MORE
    kdDebug(32002) << "formatMore " << getName()
                   << " lastFormatted id=" << m_lastFormatted->paragId()
                   << " lastFormatted's top=" << m_lastFormatted->rect().top()
                   << " lastFormatted's height=" << m_lastFormatted->rect().height()
                   << " to=" << to << " viewsBottom=" << viewsBottom
                   << " availableHeight=" << m_availableHeight << endl;
#endif
#ifdef TIMING_FORMAT
    if ( m_lastFormatted->prev() == 0 && frameSetInfo() == FI_BODY )
    {
        kdDebug(32002) << "formatMore " << getName() << ". First parag -> starting timer" << endl;
        m_time.start();
    }
#endif

    // Stop if we have formatted everything or if we need more space
    // Otherwise, stop formatting after "to" paragraphs,
    // but make sure we format everything the views need
    // ###### m_lastFormatted->rect().height() is dangerous. It's 0 initially, and it can
    // be too big after zooming out !!!
    int i;
    for ( i = 0;
          m_lastFormatted && bottom + m_lastFormatted->rect().height() <= m_availableHeight &&
 ( i < to || bottom <= viewsBottom ) ; ++i )
    {
#ifdef DEBUG_FORMAT_MORE
        kdDebug(32002) << "formatMore formatting id=" << m_lastFormatted->paragId() << endl;
#endif
        m_lastFormatted->format();
        bottom = m_lastFormatted->rect().top() + m_lastFormatted->rect().height();
#ifdef DEBUG_FORMAT_MORE
        kdDebug() << "formatMore(inside) top=" << m_lastFormatted->rect().top()
                  << " height=" << m_lastFormatted->rect().height()
                  << " bottom=" << bottom << " m_lastFormatted(next parag) = " << m_lastFormatted->next() << endl;
#endif
        if (!m_lastFormatted->isValid())
            kdWarning() << "PARAGRAPH " << m_lastFormatted->paragId() << " STILL INVALID AFTER FORMATTING" << endl;
        m_lastFormatted = m_lastFormatted->next();
    }
#ifdef DEBUG_FORMAT_MORE
    kdDebug(32002) << "formatMore finished formatting. "
                   << " bottom=" << bottom
                   << " m_lastFormatted=" << m_lastFormatted
                   << endl;
#endif

    if ( ( bottom > m_availableHeight ) ||   // this parag is already off page
         ( m_lastFormatted && bottom + m_lastFormatted->rect().height() > m_availableHeight ) ) // or next parag will be off page
    {
#ifdef DEBUG_FORMAT_MORE
        if(m_lastFormatted)
            kdDebug(32002) << "formatMore We need more space in " << getName()
                           << " bottom=" << bottom + m_lastFormatted->rect().height()
                           << " m_availableHeight=" << m_availableHeight << endl;
        else
            kdDebug(32002) << "formatMore We need more space in " << getName()
                           << " bottom2=" << bottom << " m_availableHeight=" << m_availableHeight << endl;
#endif

        if ( frames.isEmpty() )
        {
            kdWarning(32002) << "formatMore no more space, but no frame !" << endl;
            return;
        }

        double wantedPosition = 0;
        switch ( frames.last()->getFrameBehaviour() ) {
        case KWFrame::AutoExtendFrame:
        {
            int difference = ( bottom + 2 ) - m_availableHeight;
#ifdef DEBUG_FORMAT_MORE
            kdDebug(32002) << "AutoExtendFrame bottom=" << bottom << " m_availableHeight=" << m_availableHeight
                           << " => difference = " << difference << endl;
#endif
            if( m_lastFormatted && bottom + m_lastFormatted->rect().height() > m_availableHeight ) {
#ifdef DEBUG_FORMAT_MORE
                kdDebug(32002) << " next will be off -> adding " << m_lastFormatted->rect().height() << endl;
#endif
                difference += m_lastFormatted->rect().height();
            }

            if(difference > 0) {
                // There's no point in resizing a copy, so go back to the last non-copy frame
                KWFrame *theFrame = settingsFrame( frames.last() );

                if ( theFrame->getFrameSet()->isAFooter() )
                {
                    double maxFooterSize=footerHeaderSizeMax(  theFrame );
                    wantedPosition = theFrame->top() - m_doc->unzoomItY( difference );
                    if ( wantedPosition != theFrame->top() &&  QMAX(theFrame->bottom()-maxFooterSize,wantedPosition)==wantedPosition )
                    {
                        theFrame->setTop( wantedPosition);
                        frameResized( theFrame );
                    }
                    break;
                }

                wantedPosition = m_doc->unzoomItY( difference ) + theFrame->bottom();
                double pageBottom = (double) (theFrame->pageNum()+1) * m_doc->ptPaperHeight();
                pageBottom -= m_doc->ptBottomBorder();
                double newPosition = QMIN( wantedPosition, pageBottom );

                if ( theFrame->getFrameSet()->isAHeader() )
                {
                    double maxHeaderSize=footerHeaderSizeMax(  theFrame );
                    newPosition = QMIN( newPosition, maxHeaderSize+theFrame->top() );
                }

                newPosition = QMAX( newPosition, theFrame->top() ); // avoid negative heights
                bool resized = theFrame->bottom() != newPosition;

                if ( resized )
                {
#ifdef DEBUG_FORMAT_MORE
                    kdDebug(32002) << "formatMore setting bottom to " << newPosition << endl;
#endif
                    theFrame->setBottom(newPosition);
                }

                if(newPosition < wantedPosition && (theFrame->getNewFrameBehaviour() == KWFrame::NoFollowup)) {
                    if ( resized )
                        frameResized( theFrame );
                    m_lastFormatted = 0;
                    break;
                }
                if(newPosition < wantedPosition && theFrame->getNewFrameBehaviour() == KWFrame::Reconnect) {
                    wantedPosition = wantedPosition - newPosition + theFrame->top() + m_doc->ptPaperHeight();
                    // fall through to AutoCreateNewFrame
                } else {
                    if ( resized )
                        frameResized( theFrame );
                    break;
                }
            }
        }
        case KWFrame::AutoCreateNewFrame:
        {
            // We need a new frame in this frameset.
#ifdef DEBUG_FORMAT_MORE
            kdDebug(32002) << "formatMore creating new frame in frameset " << getName() << endl;
#endif
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
                //frm->setPageNum( frame->pageNum()+1 );
                addFrame( frm );
            }

            if (wantedPosition > 0)
                frames.last()->setBottom( wantedPosition );

            m_doc->updateAllFrames();
            /// We don't want to start from the beginning every time !
            ////m_doc->invalidate();

            if ( m_lastFormatted )
            {
                // Reformat the last paragraph. If it's over the two pages, it will need
                // the new page (e.g. for inline frames that need itn to work)
                if ( m_lastFormatted->prev() )
                {
                    m_lastFormatted = m_lastFormatted->prev();
                    m_lastFormatted->invalidate( 0 );
                }

                //interval = 0;
                // not good enough, we need to keep formatting right now
                formatMore(); // that, or a goto ?
                return;
            }
            QTimer::singleShot( 0, m_doc, SLOT( slotRepaintAllViews() ) );
        } break;
        case KWFrame::Ignore:
#ifdef DEBUG_FORMAT_MORE
            kdDebug(32002) << "formatMore frame behaviour is Ignore" << endl;
#endif
            m_lastFormatted = 0;
            break;
        }
    }
    // Handle the case where the last frame is empty, so we may want to
    // remove the last page.
    else if ( frames.count() > 1 && !m_lastFormatted && !isAHeader() && !isAFooter()
              && bottom < m_availableHeight - kWordDocument()->zoomItY( frames.last()->height() ) )
    {
#ifdef DEBUG_FORMAT_MORE
        kdDebug(32002) << "formatMore too much space (" << bottom << ", " << m_availableHeight << ") , trying to remove last frame" << endl;
#endif
        int lastPage = m_doc->getPages() - 1;
        // Last frame is empty -> try removing last page, and more if necessary
        while ( lastPage > 0 && m_doc->canRemovePage( lastPage ) )
        {
            m_doc->removePage( lastPage );
            if ( lastPage <= m_doc->getPages() - 1 )
            {
                kdWarning() << "Didn't manage to remove page " << lastPage << " (still having " << m_doc->getPages() << " pages ). Aborting" << endl;
                break;
            }
            lastPage = m_doc->getPages()-1;
        }
    }
    // Handle the case where the last frame is in AutoExtendFrame mode
    // and there is less text than space
    else if ( !m_lastFormatted && bottom + 2 < m_availableHeight &&
              frames.last()->getFrameBehaviour() == KWFrame::AutoExtendFrame )
    {
        // The + 2 here leaves 2 pixels below the last line. Without it we hit
        // the "break at end of frame" case in adjustFlow (!!).
        int difference = m_availableHeight - ( bottom + 2 );
        kdDebug(32002) << "formatMore less text than space (AutoExtendFrame) difference=" << difference << endl;
        // There's no point in resizing a copy, so go back to the last non-copy frame
        KWFrame *theFrame = settingsFrame( frames.last() );
        if ( theFrame->getFrameSet()->isAFooter() )
        {
            double wantedPosition = theFrame->top() + m_doc->unzoomItY( difference );
            if ( wantedPosition != theFrame->top() )
            {
                kdDebug() << "top= " << theFrame->top() << " setTop " << wantedPosition << endl;
                theFrame->setTop( wantedPosition );
                frameResized( theFrame );
            }
        }
        else
        {
            double wantedPosition = theFrame->bottom() - m_doc->unzoomItY( difference );
            kdDebug() << "formatMore wantedPosition=" << wantedPosition << " top+minheight=" << theFrame->top() + minFrameHeight << endl;
            wantedPosition = QMAX( wantedPosition, theFrame->top() + minFrameHeight );
            wantedPosition = QMAX( wantedPosition, theFrame->minFrameHeight()+theFrame->top() );
            if ( wantedPosition != theFrame->bottom()) {
                kdDebug() << "setBottom " << wantedPosition << endl;
                theFrame->setBottom( wantedPosition );
                frameResized( theFrame );
            }
        }
    }

    // Now let's see when we'll need to get back here.
    if ( m_lastFormatted )
    {
        formatTimer->start( interval, TRUE );
#ifdef DEBUG_FORMAT_MORE
        kdDebug(32002) << "formatMore: will have to format more. formatTimer->start with interval=" << interval << endl;
#endif
    }
    else
    {
        interval = QMAX( 0, interval );
#ifdef DEBUG_FORMAT_MORE
        kdDebug(32002) << "formatMore: all formatted interval=" << interval << endl;
#endif
#ifdef TIMING_FORMAT
        if ( frameSetInfo() == FI_BODY )
        {
            kdDebug(32002) << "formatMore: " << getName() << " all formatted. Took "
                           << (double)(m_time.elapsed()) / 1000 << " seconds." << endl;
        }
#endif
    }
}

double KWTextFrameSet::footerHeaderSizeMax( KWFrame *theFrame )
{
    double tmp =m_doc->ptPaperHeight()-m_doc->ptBottomBorder()-m_doc->ptTopBorder()-40;//default min 40 for page size
    int page = theFrame->pageNum();
    bool header=theFrame->getFrameSet()->isAHeader();
    if( header ? m_doc->isHeaderVisible():m_doc->isFooterVisible() )
    {
        QListIterator<KWFrameSet> fit = m_doc->framesetsIterator();
        for ( ; fit.current() ; ++fit )
        {
            bool state = header ? fit.current()->isAFooter():fit.current()->isAHeader();
            if(fit.current()->isVisible() && state)
            {
                KWFrame * frm=fit.current()->getFrame( 0 );
                if(frm->pageNum()==page )
                    return (tmp-frm->height());
            }

        }
    }
    return tmp;
}

void KWTextFrameSet::frameResized( KWFrame *theFrame )
{
    kdDebug() << "KWTextFrameSet::frameResized " << theFrame << endl;
    if ( theFrame->getFrameSet()->frameSetInfo() != KWFrameSet::FI_BODY )
        m_doc->recalcFrames();

    KWTableFrameSet *table = theFrame->getFrameSet()->getGroupManager();
    if ( table )
    {
        KWTableFrameSet::Cell *cell = (KWTableFrameSet::Cell *)this;
        table->recalcCols(cell->m_col,cell->m_row);
        table->recalcRows(cell->m_col,cell->m_row);
        table->updateTempHeaders();
    }

    // m_doc->frameChanged( theFrame );
    // Warning, can't call layout() (frameChanged calls it)
    // from here, since it calls formatMore() !
    m_doc->updateAllFrames();
    m_doc->invalidate();
    theFrame->updateRulerHandles();

    // Can't call this directly, we might be in a paint event already
    //m_doc->repaintAllViews();
    QTimer::singleShot( 0, m_doc, SLOT( slotRepaintAllViews() ) );
}

bool KWTextFrameSet::isFrameEmpty( KWFrame * frame )
{
    QTextParag * lastParag = textdoc->lastParag();
    ensureFormatted( lastParag );
    int bottom = lastParag->rect().top() + lastParag->rect().height();

    if ( frame->getFrameSet() == this ) // safety check
        return bottom < frame->internalY();

    kdWarning() << "KWTextFrameSet::isFrameEmpty called for frame " << frame << " which isn't a child of ours!" << endl;
    if ( frame->getFrameSet() )
        kdDebug() << "(this is " << getName() << " and the frame belongs to " << frame->getFrameSet()->getName() << ")" << endl;
    return false;
}

bool KWTextFrameSet::canRemovePage( int num )
{
    //kdDebug() << "KWTextFrameSet(" << getName() << ")::canRemovePage " << num << endl;

    // No frame on that page ? ok for us then
    if ( num < m_firstPage || num >= (int)m_framesInPage.size() + m_firstPage )
        return true;

    QListIterator<KWFrame> frameIt( framesInPage( num ) );
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame * frame = frameIt.current();
        ASSERT( frame->pageNum() == num );
        ASSERT( frame->getFrameSet() == this );
        bool isEmpty = isFrameEmpty( frame );
#ifdef DEBUG_FORMAT_MORE
        kdDebug() << "KWTextFrameSet(" << getName() << ")::canRemovePage"
                  << " found a frame on page " << num << " empty:" << isEmpty << endl;
#endif
        // Ok, so we have a frame on that page -> we can't remove it unless it's a copied frame OR it's empty
        bool isCopy = frame->isCopy() && frameIt.current() != frames.first();
        if ( !isCopy && !isEmpty )
            return false;
    }
    return true;
}

void KWTextFrameSet::delFrame( KWFrame *frm, bool remove )
{
    emit frameDeleted( frm );
    KWFrameSet::delFrame( frm, remove );
}

void KWTextFrameSet::doChangeInterval()
{
#ifdef DEBUG_FORMAT_MORE
    kdDebug() << "KWTextFrameSet::doChangeInterval back to interval=0" << endl;
#endif
    interval = 0;
}

void KWTextFrameSet::updateViewArea( QWidget * w, const QPoint & nPointBottom )
{
    (void) availableHeight(); // make sure that it's not -1
#ifdef DEBUG_VIEWAREA
    kdDebug(32002) << "KWTextFrameSet::updateViewArea " << (void*)w << " " << w->name()
                     << " nPointBottom=" << nPointBottom.x() << "," << nPointBottom.y()
                     << " m_availableHeight=" << m_availableHeight << " textdoc->height()=" << textdoc->height() << endl;
#endif

    // Find last page that is visible
    int maxPage = ( nPointBottom.y() + m_doc->paperHeight() /*equiv. to ceil()*/ ) / m_doc->paperHeight();
    int maxY = 0;
    if ( maxPage < m_firstPage || maxPage >= (int)m_framesInPage.size() + m_firstPage )
        maxY = m_availableHeight;
    else
    {
        // Find frames on that page, and keep the max bottom, in internal coordinates
        QListIterator<KWFrame> frameIt( framesInPage( maxPage ) );
        for ( ; frameIt.current(); ++frameIt )
        {
            maxY = QMAX( maxY, frameIt.current()->internalY()
                         + m_doc->zoomItY( frameIt.current()->height() ) );
        }
    }
#ifdef DEBUG_VIEWAREA
    kdDebug(32002) << "KWTextFrameSet (" << getName() << ")::updateViewArea maxY now " << maxY << endl;
#endif
    // Update map
    m_mapViewAreas.replace( w, maxY );

    formatMore();
}

KWTextFrameSet::UndoRedoInfo::UndoRedoInfo( KWTextFrameSet *fs )
    : type( Invalid ), textfs(fs), cursor( 0 )
{
    text = QString::null;
    id = -1;
    index = -1;
    placeHolderCmd = 0L;
}

bool KWTextFrameSet::UndoRedoInfo::valid() const
{
    return text.length() > 0 && id >= 0 && index >= 0;
}

void KWTextFrameSet::UndoRedoInfo::clear()
{
    if ( valid() ) {
        QTextDocument * textdoc = textfs->textDocument();
        switch (type) {
            case Insert:
            case Return:
            {
                QTextCommand * cmd = new KWTextInsertCommand( textdoc, id, index, text.rawData(), customItemsMap, oldParagLayouts );
                textdoc->addCommand( cmd );
                ASSERT( placeHolderCmd );
                // Inserting any custom items -> macro command, to let custom items add their command
                if ( !customItemsMap.isEmpty() )
                {
                    CustomItemsMap::Iterator it = customItemsMap.begin();
                    for ( ; it != customItemsMap.end(); ++it )
                    {
                        KWTextCustomItem * item = it.data();
                        KCommand * itemCmd = item->createCommand();
                        if ( itemCmd )
                            placeHolderCmd->addCommand( itemCmd );
                    }
                    placeHolderCmd->addCommand( new KWTextCommand( textfs, /*cmd, */QString::null ) );
                }
                else
                {
                    placeHolderCmd->addCommand( new KWTextCommand( textfs, /*cmd, */QString::null ) );
                }
            } break;
            case Delete:
            case RemoveSelected:
            {
                QTextCommand * cmd = new KWTextDeleteCommand( textdoc, id, index, text.rawData(), customItemsMap, oldParagLayouts );
                textdoc->addCommand( cmd );
                ASSERT( placeHolderCmd );
                placeHolderCmd->addCommand( new KWTextCommand( textfs, /*cmd, */QString::null ) );
                // Deleting any custom items -> let them add their command
                if ( !customItemsMap.isEmpty() )
                {
                    customItemsMap.deleteAll( placeHolderCmd );
                }
           } break;
            case Invalid:
                break;
        }
    }
    type = Invalid;
    text = QString::null; // calls QTextString::clear(), which calls resize(0) on the array, which _detaches_. Tricky.
    id = -1;
    index = -1;
    oldParagLayouts.clear();
    customItemsMap.clear();
    placeHolderCmd = 0L;
}

// Copies a formatted char, <parag, position>, into undoRedoInfo.text, at position <index>.
void KWTextFrameSet::copyCharFormatting( QTextParag *parag, int position, int index /*in text*/, bool moveCustomItems )
{
    QTextStringChar * ch = parag->at( position );
    if ( ch->format() ) {
        ch->format()->addRef();
        undoRedoInfo.text.at( index ).setFormat( ch->format() );
    }
    if ( ch->isCustom() )
    {
        kdDebug(32001) << "KWTextFrameSet::copyCharFormatting moving custom item " << ch->customItem() << " to text's " << index << " char"  << endl;
        undoRedoInfo.customItemsMap.insert( index, static_cast<KWTextCustomItem *>( ch->customItem() ) );
        // We copy the custom item to customItemsMap in all cases (see setFormat)
        // We only remove from 'ch' if moveCustomItems was specified
        if ( moveCustomItems )
            static_cast<KWTextParag*>(parag)->removeCustomItem(position);
        //ch->loseCustomItem();
    }
}

// Based on QTextView::readFormats - with all code duplication moved to copyCharFormatting
void KWTextFrameSet::readFormats( QTextCursor &c1, QTextCursor &c2, bool copyParagLayouts, bool moveCustomItems )
{
    //kdDebug() << "KWTextFrameSet::readFormats moveCustomItems=" << moveCustomItems << endl;
    c2.restoreState();
    c1.restoreState();
    int oldLen = undoRedoInfo.text.length();
    if ( c1.parag() == c2.parag() ) {
        undoRedoInfo.text += c1.parag()->string()->toString().mid( c1.index(), c2.index() - c1.index() );
        for ( int i = c1.index(); i < c2.index(); ++i )
            copyCharFormatting( c1.parag(), i, oldLen + i - c1.index(), moveCustomItems );
    } else {
        int lastIndex = oldLen;
        int i;
        //kdDebug() << "KWTextFrameSet::readFormats copying from " << c1.index() << " to " << c1.parag()->length()-1 << " into lastIndex=" << lastIndex << endl;
        // Replace the trailing spaces with '\n'. That char carries the formatting for the trailing space.
        undoRedoInfo.text += c1.parag()->string()->toString().mid( c1.index(), c1.parag()->length() - 1 - c1.index() ) + '\n';
        for ( i = c1.index(); i < c1.parag()->length(); ++i, ++lastIndex )
            copyCharFormatting( c1.parag(), i, lastIndex, moveCustomItems );
        //++lastIndex; // skip the '\n'.
        QTextParag *p = c1.parag()->next();
        while ( p && p != c2.parag() ) {
            undoRedoInfo.text += p->string()->toString().left( p->length() - 1 ) + '\n';
            //kdDebug() << "KWTextFrameSet::readFormats (mid) copying from 0 to "  << p->length()-1 << " into i+" << lastIndex << endl;
            for ( i = 0; i < p->length(); ++i )
                copyCharFormatting( p, i, i + lastIndex, moveCustomItems );
            lastIndex += p->length(); // + 1; // skip the '\n'
            //kdDebug() << "KWTextFrameSet::readFormats lastIndex now " << lastIndex << endl;
            p = p->next();
        }
        //kdDebug() << "KWTextFrameSet::readFormats copying [last] from 0 to " << c2.index() << " into i+" << lastIndex << endl;
        undoRedoInfo.text += c2.parag()->string()->toString().left( c2.index() );
        for ( i = 0; i < c2.index(); ++i )
            copyCharFormatting( c2.parag(), i, i + lastIndex, moveCustomItems );
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

void KWTextFrameSet::applyStyle( QTextCursor * cursor, const KWStyle * newStyle,
                                 int selectionId,
                                 int paragLayoutFlags, int formatFlags,
                                 bool zoomFormats, bool createUndoRedo, bool interactive )
{
    QTextDocument * textdoc = textDocument();
    if ( interactive )
        emit hideCursor();

    /// Applying a style is three distinct operations :
    /// 1 - Changing the paragraph settings (setParagLayout)
    /// 2 - Changing the character formatting for each char in the paragraph (setFormat(indices))
    /// 3 - Changing the character formatting for the whole paragraph (setFormat()) [just in case]
    /// -> We need a macro command to hold the 3 commands
    KMacroCommand * macroCmd = createUndoRedo ? new KMacroCommand( i18n("Apply style %1").
                                                                   arg(newStyle->translatedName() ) ) : 0;

    // 1
    //kdDebug(32001) << "KWTextFrameSet::applyStyle setParagLayout" << endl;
    storeParagUndoRedoInfo( cursor, selectionId );
    undoRedoInfo.type = UndoRedoInfo::Invalid; // tricky, we don't want clear() to create a command
    if ( paragLayoutFlags != 0 )
    {
        if ( !textdoc->hasSelection( selectionId ) ) {
            static_cast<KWTextParag*>(cursor->parag())->setParagLayout( newStyle->paragLayout(), paragLayoutFlags );
        } else {
            QTextParag *start = textdoc->selectionStart( selectionId );
            QTextParag *end = textdoc->selectionEnd( selectionId );
            for ( ; start && start != end->next() ; start = start->next() )
                static_cast<KWTextParag*>(start)->setParagLayout( newStyle->paragLayout(), paragLayoutFlags );
        }

        if ( createUndoRedo )
        {
            //kdDebug(32001) << "KWTextFrameSet::applyStyle KWTextParagCommand" << endl;
            QTextCommand * cmd = new KWTextParagCommand( textdoc, undoRedoInfo.id, undoRedoInfo.eid,
                                                         undoRedoInfo.oldParagLayouts,
                                                         newStyle->paragLayout(), paragLayoutFlags );
            textdoc->addCommand( cmd );
            macroCmd->addCommand( new KWTextCommand( this, /*cmd, */QString::null ) );
        }
    }

    // 2
    //kdDebug(32001) << "KWTextFrameSet::applyStyle gathering text and formatting" << endl;
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

    if ( formatFlags != 0 )
    {
        KWTextFormat * newFormat;
        if ( zoomFormats )
        {
            newFormat = zoomFormatFont( & newStyle->format() );
#ifdef DEBUG_FORMATS
            kdDebug() << "KWTextFrameSet::applyStyle zoomed format " << newStyle->format().key()
                      << " (pointsize " << newStyle->format().pointSizeFloat() << ")"
                      << " to newFormat=" << newFormat << " " << newFormat->key()
                      << " (pointsize " << newFormat->pointSizeFloat() << ")" << endl;
#endif
        } else {
            newFormat = static_cast<KWTextFormat *>( textdoc->formatCollection()->format( &newStyle->format() ) );
        }

        if ( createUndoRedo )
        {
            QValueList<QTextFormat *> lstFormats;
            //QString str;
            for ( QTextParag * parag = firstParag ; parag && parag != lastParag->next() ; parag = parag->next() )
            {
                //str += parag->string()->toString() + '\n';
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
            readFormats( c1, c2 ); // gather char-format info but not paraglayouts nor customitems

            QTextCommand * cmd = new KWTextFormatCommand( textdoc, firstParag->paragId(), 0,
                                                         lastParag->paragId(), c2.index(),
                                                         undoRedoInfo.text.rawData(), newFormat,
                                                         formatFlags );
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
            kdDebug(32001) << "KWTextFrameSet::applyStyle parag:" << parag->paragId()
                           << ", from 0 to " << parag->string()->length() << ", format=" << newFormat << endl;
            parag->setFormat( 0, parag->string()->length(), newFormat, true, formatFlags );
            parag->setFormat( newFormat );
        }
        //currentFormat = static_cast<KWTextFormat *>( textDocument()->formatCollection()->format( newFormat ) );
        //kdDebug() << "KWTextFrameSet::applyStyle currentFormat=" << currentFormat << " " << currentFormat->key() << endl;
    }

    //resize all variables after applied style
    QListIterator<QTextCustomItem> cit( textdoc->allCustomItems() );
    for ( ; cit.current() ; ++cit )
        static_cast<KWTextCustomItem *>( cit.current() )->resize();


    if ( interactive )
    {
        setLastFormattedParag( firstParag );
        formatMore();
        emit repaintChanged( this );
        emit updateUI( true );
        if ( createUndoRedo )
            m_doc->addCommand( macroCmd );
        emit showCursor();
    }

    undoRedoInfo.clear();
}

void KWTextFrameSet::newPlaceHolderCommand( const QString & name )
{
    ASSERT( !undoRedoInfo.placeHolderCmd );
    undoRedoInfo.placeHolderCmd = new KMacroCommand( name );
    m_doc->addCommand( undoRedoInfo.placeHolderCmd );
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

KCommand *KWTextFrameSet::setCounterCommand( QTextCursor * cursor, const KoParagCounter & counter )
{
    QTextDocument * textdoc = textDocument();
    const KoParagCounter * curCounter = static_cast<KWTextParag*>(cursor->parag())->counter();
    if ( !textdoc->hasSelection( QTextDocument::Standard ) &&
         curCounter && counter == *curCounter )
        return 0L;
    emit hideCursor();
    storeParagUndoRedoInfo( cursor );
    if ( !textdoc->hasSelection( QTextDocument::Standard ) ) {
        static_cast<KWTextParag*>(cursor->parag())->setCounter( counter );
        setLastFormattedParag( cursor->parag() );
    } else {
        QTextParag *start = textdoc->selectionStart( QTextDocument::Standard );
        QTextParag *end = textdoc->selectionEnd( QTextDocument::Standard );
        // Special hack for BR25742, don't apply bullet to last empty parag of the selection
        if ( start != end && end->length() <= 1 )
        {
            end = end->prev();
            undoRedoInfo.eid = end->paragId();
        }
        setLastFormattedParag( start );
        for ( ; start && start != end->next() ; start = start->next() )
            static_cast<KWTextParag*>(start)->setCounter( counter );
    }
    formatMore();
    emit repaintChanged( this );
    if ( !undoRedoInfo.newParagLayout.counter )
        undoRedoInfo.newParagLayout.counter = new KoParagCounter;
    *undoRedoInfo.newParagLayout.counter = counter;
    KWTextParagCommand *cmd = new KWTextParagCommand(
        textdoc, undoRedoInfo.id, undoRedoInfo.eid,
        undoRedoInfo.oldParagLayouts, undoRedoInfo.newParagLayout,
        KoParagLayout::BulletNumber );
    textdoc->addCommand( cmd );
    //m_doc->addCommand( new KWTextCommand( this, /*cmd, */i18n("Change list type") ) );

    undoRedoInfo.clear(); // type is still Invalid -> no command created
    emit showCursor();
    emit updateUI( true );
    return new KWTextCommand( this, /*cmd, */i18n("Change list type") );
}

KCommand * KWTextFrameSet::setAlignCommand( QTextCursor * cursor, int align )
{
    QTextDocument * textdoc = textDocument();
    if ( !textdoc->hasSelection( QTextDocument::Standard ) &&
         cursor->parag()->alignment() == align )
        return 0L; // No change needed.

    emit hideCursor();
    storeParagUndoRedoInfo( cursor );
    if ( !textdoc->hasSelection( QTextDocument::Standard ) ) {
        static_cast<KWTextParag *>(cursor->parag())->setAlign(align);
        setLastFormattedParag( cursor->parag() );
    }
    else
    {
        QTextParag *start = textDocument()->selectionStart( QTextDocument::Standard );
        QTextParag *end = textDocument()->selectionEnd( QTextDocument::Standard );
        setLastFormattedParag( start );
        for ( ; start && start != end->next() ; start = start->next() )
            static_cast<KWTextParag *>(start)->setAlign(align);
    }
    formatMore();
    emit repaintChanged( this );
    undoRedoInfo.newParagLayout.alignment = align;
    KWTextParagCommand *cmd = new KWTextParagCommand(
        textdoc, undoRedoInfo.id, undoRedoInfo.eid,
        undoRedoInfo.oldParagLayouts, undoRedoInfo.newParagLayout,
        KoParagLayout::Alignment );
    textdoc->addCommand( cmd );
    //m_doc->addCommand( new KWTextCommand( this, /*cmd, */i18n("Change Alignment") ) );
    undoRedoInfo.clear(); // type is still Invalid -> no command created
    emit showCursor();
    emit updateUI( true );
    return new KWTextCommand( this, /*cmd, */i18n("Change Alignment") );
}

KCommand * KWTextFrameSet::setMarginCommand( QTextCursor * cursor, QStyleSheetItem::Margin m, double margin ) {
    QTextDocument * textdoc = textDocument();
    //kdDebug(32001) << "KWTextFrameSet::setMargin " << m << " to value " << margin << endl;
    //kdDebug(32001) << "Current margin is " << static_cast<KWTextParag *>(cursor->parag())->margin(m) << endl;
    if ( !textdoc->hasSelection( QTextDocument::Standard ) &&
         static_cast<KWTextParag *>(cursor->parag())->margin(m) == margin )
        return 0L; // No change needed.

    emit hideCursor();
    storeParagUndoRedoInfo( cursor );
    if ( !textdoc->hasSelection( QTextDocument::Standard ) ) {
        static_cast<KWTextParag *>(cursor->parag())->setMargin(m, margin);
        setLastFormattedParag( cursor->parag() );
    }
    else
    {
        QTextParag *start = textDocument()->selectionStart( QTextDocument::Standard );
        QTextParag *end = textDocument()->selectionEnd( QTextDocument::Standard );
        setLastFormattedParag( start );
        for ( ; start && start != end->next() ; start = start->next() )
            static_cast<KWTextParag *>(start)->setMargin(m, margin);
    }
    formatMore();
    emit repaintChanged( this );
    undoRedoInfo.newParagLayout.margins[m] = margin;
    KWTextParagCommand *cmd = new KWTextParagCommand(
        textdoc, undoRedoInfo.id, undoRedoInfo.eid,
        undoRedoInfo.oldParagLayouts, undoRedoInfo.newParagLayout,
        KoParagLayout::Margins, m );
    textdoc->addCommand( cmd );
    QString name;
    if ( m == QStyleSheetItem::MarginFirstLine )
        name = i18n("Change First Line Indent");
    else if ( m == QStyleSheetItem::MarginLeft || m == QStyleSheetItem::MarginRight )
        name = i18n("Change Indent");
    else
        name = i18n("Change Paragraph Spacing");
    //m_doc->addCommand( new KWTextCommand( this, /*cmd, */name ) );
    undoRedoInfo.clear();
    emit showCursor();
    emit updateUI( true );
    return  new KWTextCommand( this, /*cmd, */name );
}

KCommand * KWTextFrameSet::setLineSpacingCommand( QTextCursor * cursor, double spacing )
{
    QTextDocument * textdoc = textDocument();
    //kdDebug(32001) << "KWTextFrameSet::setLineSpacing to value " << spacing << endl;
    //kdDebug(32001) << "Current spacing is " << static_cast<KWTextParag *>(cursor->parag())->kwLineSpacing() << endl;
    //kdDebug(32001) << "Comparison says " << ( static_cast<KWTextParag *>(cursor->parag())->kwLineSpacing() == spacing ) << endl;
    //kdDebug(32001) << "hasSelection " << textdoc->hasSelection( QTextDocument::Standard ) << endl;
    if ( !textdoc->hasSelection( QTextDocument::Standard ) &&
         static_cast<KWTextParag *>(cursor->parag())->kwLineSpacing() == spacing )
        return 0L; // No change needed.

    emit hideCursor();
    storeParagUndoRedoInfo( cursor );
    if ( !textdoc->hasSelection( QTextDocument::Standard ) ) {
        static_cast<KWTextParag *>(cursor->parag())->setLineSpacing(spacing);
        setLastFormattedParag( cursor->parag() );
    }
    else
    {
        QTextParag *start = textDocument()->selectionStart( QTextDocument::Standard );
        QTextParag *end = textDocument()->selectionEnd( QTextDocument::Standard );
        setLastFormattedParag( start );
        for ( ; start && start != end->next() ; start = start->next() )
            static_cast<KWTextParag *>(start)->setLineSpacing(spacing);
    }
    formatMore();
    emit repaintChanged( this );
    undoRedoInfo.newParagLayout.lineSpacing = spacing;
    KWTextParagCommand *cmd = new KWTextParagCommand(
        textdoc, undoRedoInfo.id, undoRedoInfo.eid,
        undoRedoInfo.oldParagLayouts, undoRedoInfo.newParagLayout,
        KoParagLayout::LineSpacing );
    textdoc->addCommand( cmd );
    //m_doc->addCommand( new KWTextCommand( this, /*cmd, */i18n("Change Line Spacing") ) );

    undoRedoInfo.clear();
    emit showCursor();
    return new KWTextCommand( this, /*cmd, */i18n("Change Line Spacing") );
}


KCommand * KWTextFrameSet::setBordersCommand( QTextCursor * cursor, Border leftBorder, Border rightBorder, Border topBorder, Border bottomBorder )
{
  QTextDocument * textdoc = textDocument();
  if ( !textdoc->hasSelection( QTextDocument::Standard ) &&
       static_cast<KWTextParag *>(cursor->parag())->leftBorder() ==leftBorder &&
       static_cast<KWTextParag *>(cursor->parag())->rightBorder() ==rightBorder &&
       static_cast<KWTextParag *>(cursor->parag())->topBorder() ==topBorder &&
       static_cast<KWTextParag *>(cursor->parag())->bottomBorder() ==bottomBorder )
        return 0L; // No change needed.

    emit hideCursor();
    storeParagUndoRedoInfo( cursor );
    if ( !textdoc->hasSelection( QTextDocument::Standard ) ) {
      static_cast<KWTextParag *>(cursor->parag())->setLeftBorder(leftBorder);
      static_cast<KWTextParag *>(cursor->parag())->setRightBorder(rightBorder);
      static_cast<KWTextParag *>(cursor->parag())->setBottomBorder(bottomBorder);
      static_cast<KWTextParag *>(cursor->parag())->setTopBorder(topBorder);
      setLastFormattedParag( cursor->parag() );
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

    KWTextParagCommand *cmd = new KWTextParagCommand(
        textdoc, undoRedoInfo.id, undoRedoInfo.eid,
        undoRedoInfo.oldParagLayouts, undoRedoInfo.newParagLayout,
        KoParagLayout::Borders);
    textdoc->addCommand( cmd );
    //m_doc->addCommand( new KWTextCommand( this, /*cmd, */i18n("Change Borders") ) );

    undoRedoInfo.clear();
    emit showCursor();
    emit updateUI( true );
    return new KWTextCommand( this, /*cmd, */i18n("Change Borders") );
}


KCommand * KWTextFrameSet::setTabListCommand( QTextCursor * cursor, const KoTabulatorList &tabList )
{
    QTextDocument * textdoc = textDocument();
    if ( !textdoc->hasSelection( QTextDocument::Standard ) && static_cast<KWTextParag *>(cursor->parag())->tabList() == tabList )
        return 0L; // No change needed.

    emit hideCursor();

    storeParagUndoRedoInfo( cursor );

    if ( !textdoc->hasSelection( QTextDocument::Standard ) ) {
        static_cast<KWTextParag *>(cursor->parag())->setTabList( tabList );
        setLastFormattedParag( cursor->parag() );
    }
    else
    {
        QTextParag *start = textDocument()->selectionStart( QTextDocument::Standard );
        QTextParag *end = textDocument()->selectionEnd( QTextDocument::Standard );
        setLastFormattedParag( start );
        for ( ; start && start != end->next() ; start = start->next() )
            static_cast<KWTextParag *>(start)->setTabList( tabList );
    }

    formatMore();
    emit repaintChanged( this );
    undoRedoInfo.newParagLayout.setTabList( tabList );
    KWTextParagCommand *cmd = new KWTextParagCommand(
        textdoc, undoRedoInfo.id, undoRedoInfo.eid,
        undoRedoInfo.oldParagLayouts, undoRedoInfo.newParagLayout,
        KoParagLayout::Tabulator);
    textdoc->addCommand( cmd );
    //m_doc->addCommand( new KWTextCommand( this, /*cmd, */i18n("Change Tabulator") ) );
    undoRedoInfo.clear();
    emit showCursor();
    emit updateUI( true );
    return new KWTextCommand( this, /*cmd, */i18n("Change Tabulator") );
}

KCommand * KWTextFrameSet::setPageBreakingCommand( QTextCursor * cursor, int pageBreaking )
{
    QTextDocument * textdoc = textDocument();
    if ( !textdoc->hasSelection( QTextDocument::Standard ) &&
         static_cast<KWTextParag *>(cursor->parag())->pageBreaking() == pageBreaking )
        return 0L; // No change needed.

    emit hideCursor();

    storeParagUndoRedoInfo( cursor );

    if ( !textdoc->hasSelection( QTextDocument::Standard ) ) {
        static_cast<KWTextParag *>(cursor->parag())->setPageBreaking( pageBreaking );
        setLastFormattedParag( cursor->parag() );
    }
    else
    {
        QTextParag *start = textDocument()->selectionStart( QTextDocument::Standard );
        QTextParag *end = textDocument()->selectionEnd( QTextDocument::Standard );
        setLastFormattedParag( start );
        for ( ; start && start != end->next() ; start = start->next() )
            static_cast<KWTextParag *>(start)->setPageBreaking( pageBreaking );
    }

    formatMore();
    emit repaintChanged( this );
    undoRedoInfo.newParagLayout.pageBreaking = pageBreaking;
    KWTextParagCommand *cmd = new KWTextParagCommand(
        textdoc, undoRedoInfo.id, undoRedoInfo.eid,
        undoRedoInfo.oldParagLayouts, undoRedoInfo.newParagLayout,
        KoParagLayout::PageBreaking );
    textdoc->addCommand( cmd );
    undoRedoInfo.clear();
    emit showCursor();
    emit updateUI( true );
    emit ensureCursorVisible();
    // ## find a better name for the command
    return new KWTextCommand( this, /*cmd, */i18n("Change Paragraph Attribute") );
}


void KWTextFrameSet::removeSelectedText( QTextCursor * cursor, int selectionId, const QString & cmdName )
{
    QTextDocument * textdoc = textDocument();
    emit hideCursor();
    checkUndoRedoInfo( cursor, UndoRedoInfo::RemoveSelected );
    if ( !undoRedoInfo.valid() ) {
        textdoc->selectionStart( selectionId, undoRedoInfo.id, undoRedoInfo.index );
        undoRedoInfo.text = QString::null;
        newPlaceHolderCommand( cmdName.isNull() ? i18n("Remove Selected Text") : cmdName );
    }
    QTextCursor c1 = textdoc->selectionStartCursor( selectionId );
    QTextCursor c2 = textdoc->selectionEndCursor( selectionId );
    readFormats( c1, c2, true, true );
    //kdDebug() << "KWTextFrameSet::removeSelectedText text=" << undoRedoInfo.text.toString() << endl;

    textdoc->removeSelectedText( selectionId, cursor );

    setLastFormattedParag( cursor->parag() );
    formatMore();
    emit repaintChanged( this );
    emit ensureCursorVisible();
    emit updateUI( true );
    emit showCursor();
    if(selectionId==QTextDocument::Standard)
        selectionChangedNotify();
    undoRedoInfo.clear();
}

KCommand * KWTextFrameSet::removeSelectedTextCommand( QTextCursor * cursor, int selectionId )
{
    undoRedoInfo.clear();
    textdoc->selectionStart( selectionId, undoRedoInfo.id, undoRedoInfo.index );

    QTextCursor c1 = textdoc->selectionStartCursor( selectionId );
    QTextCursor c2 = textdoc->selectionEndCursor( selectionId );
    readFormats( c1, c2, true, true );

    textdoc->removeSelectedText( selectionId, cursor );

    KMacroCommand *macroCmd = new KMacroCommand( i18n("Remove Selected Text") );

    QTextCommand *cmd = new KWTextDeleteCommand( textdoc, undoRedoInfo.id, undoRedoInfo.index,
                                                 undoRedoInfo.text.rawData(),
                                                 undoRedoInfo.customItemsMap,
                                                 undoRedoInfo.oldParagLayouts );
    textdoc->addCommand(cmd);
    macroCmd->addCommand(new KWTextCommand( this, /*cmd, */QString::null ));

    if(!undoRedoInfo.customItemsMap.isEmpty())
        undoRedoInfo.customItemsMap.deleteAll( macroCmd );

    undoRedoInfo.type = UndoRedoInfo::Invalid; // we don't want clear() to create a command
    undoRedoInfo.clear();

    return macroCmd;
}

KCommand* KWTextFrameSet::replaceSelectionCommand( QTextCursor * cursor, const QString & replacement,
                                                   int selectionId, const QString & cmdName)
{
    emit hideCursor();
    KMacroCommand * macroCmd = new KMacroCommand( cmdName );

    // Remember formatting
    QTextCursor c1 = textdoc->selectionStartCursor( selectionId );
    QTextFormat * format = c1.parag()->at( c1.index() )->format();
    format->addRef();

    // Remove selected text
    macroCmd->addCommand( removeSelectedTextCommand( cursor, selectionId ) );

    // Insert replacement
    insert( cursor, static_cast<KWTextFormat *>(format),
            replacement, true, false, QString::null /* no place holder command */ );

    QTextCommand * cmd = new KWTextInsertCommand( textdoc, undoRedoInfo.id, undoRedoInfo.index,
                                                  undoRedoInfo.text.rawData(),
                                                  CustomItemsMap(), undoRedoInfo.oldParagLayouts );
    textdoc->addCommand( cmd );
    macroCmd->addCommand( new KWTextCommand( this, /*cmd, */QString::null ) );

    undoRedoInfo.type = UndoRedoInfo::Invalid; // we don't want clear() to create a command
    undoRedoInfo.clear();

    format->removeRef();

    //kWordDocument()->addCommand( macroCmd );

    setLastFormattedParag( c1.parag() );
    formatMore();
    emit repaintChanged( this );
    emit ensureCursorVisible();
    emit updateUI( true );
    emit showCursor();
    if(selectionId==QTextDocument::Standard)
        selectionChangedNotify();
    return macroCmd;
}

QString KWTextFrameSet::textChangedCase(const QString _text,TypeOfCase _type)
{
    QString text(_text);
    switch(_type)
    {
        case UpperCase:
            text=text.upper();
            break;
        case LowerCase:
            text=text.lower();
            break;
        case TitleCase:
            for(uint i=0;i<text.length();i++)
            {
                if((i==0 &&text.at(i)!=' ') ||(text.at(i)!=' '&& text.at(QMAX(i-1,0))==' ')||(text.at(i)!=' '&& text.at(QMAX(i-1,0))=='\n'))
                    text=text.replace(i, 1, text.at(i).upper() );
            }
            break;
        case ToggleCase:
            for(uint i=0;i<text.length();i++)
            {
                QString repl=QString(text.at(i));
                if(text.at(i)!=text.at(i).upper())
                    repl=repl.upper();
                else if(text.at(i).lower()!=text.at(i))
                    repl=repl.lower();
                text=text.replace(i, 1, repl );
            }
            break;
        default:
            kdDebug()<<"Error in changeCaseOfText !\n";
            break;

    }
    return text;
}

void KWTextFrameSet::changeCaseOfText(QTextCursor *cursor,TypeOfCase _type)
{
    KMacroCommand * macroCmd = new KMacroCommand( i18n("Change case") );

    QTextCursor start = textdoc->selectionStartCursor( QTextDocument::Standard );
    QTextCursor end = textdoc->selectionEndCursor( QTextDocument::Standard );

    int posStart=start.index();
    int posEnd=start.index();
    QTextCursor c1( textdoc );
    QTextCursor c2( textdoc );
    QString repl;
    QString text;
    if ( start.parag() == end.parag() )
    {
        text = start.parag()->string()->toString().mid( start.index(), end.index() - start.index() );
        for ( int i = start.index(); i < end.index(); ++i )
        {
            if( start.parag()->at(i)->isCustom())
            {
                posEnd=i;
                c1.setParag(start.parag()  );
                c1.setIndex( posStart );
                c2.setParag( start.parag() );
                c2.setIndex( posEnd );

                repl=text.mid(posStart-start.index(),posEnd-posStart);
                textdoc->setSelectionStart( QTextDocument::Temp, &c1 );
                textdoc->setSelectionEnd( QTextDocument::Temp, &c2 );
                macroCmd->addCommand(replaceSelectionCommand( cursor,textChangedCase(repl,_type),
                                                              QTextDocument::Temp, "" ));
                do
                {
                    ++i;
                }
                while( start.parag()->at(i)->isCustom() && i != end.index());
                posStart=i;
                posEnd=i;
            }
        }
        //change last word
        c1.setParag(start.parag()  );
        c1.setIndex( posStart );
        c2.setParag( start.parag() );
        c2.setIndex( end.index() );

        textdoc->setSelectionStart( QTextDocument::Temp, &c1 );
        textdoc->setSelectionEnd( QTextDocument::Temp, &c2 );
        repl=text.mid(posStart-start.index(),end.index()-posStart);
        macroCmd->addCommand(replaceSelectionCommand( cursor,textChangedCase(repl,_type) ,
                                                      QTextDocument::Temp, "" ));
    }
    else
    {
        int i;
        text = start.parag()->string()->toString().mid( start.index(), start.parag()->length() - 1 - start.index() );
        for ( i = start.index(); i < start.parag()->length(); ++i )
        {
            if( start.parag()->at(i)->isCustom())
            {
                posEnd=i;

                c1.setParag( start.parag() );
                c1.setIndex( posStart );

                c2.setParag(start.parag() );
                c2.setIndex( posEnd );

                textdoc->setSelectionStart( QTextDocument::Temp, &c1 );
                textdoc->setSelectionEnd( QTextDocument::Temp, &c2 );
                repl=text.mid(posStart-start.index(),posEnd-posStart);
                macroCmd->addCommand(replaceSelectionCommand( cursor,textChangedCase(repl,_type) ,
                                                              QTextDocument::Temp, "" ));
                do
                {
                    ++i;
                }
                while( start.parag()->at(i)->isCustom() && i != end.index());
                posStart=i;
                posEnd=i;
            }
        }
        //change last word
        c1.setParag(start.parag()  );
        c1.setIndex( posStart );
        c2.setParag( start.parag() );
        c2.setIndex( text.length()+start.index() );

        textdoc->setSelectionStart( QTextDocument::Temp, &c1 );
        textdoc->setSelectionEnd( QTextDocument::Temp, &c2 );
        repl=text.mid(posStart-start.index(),end.index()-posStart);
        macroCmd->addCommand(replaceSelectionCommand( cursor,textChangedCase(repl,_type) ,
                                                      QTextDocument::Temp, "" ));

        QTextParag *p = start.parag()->next();
        while ( p && p != end.parag() )
        {
            posStart=0;
            posEnd=0;
            text = p->string()->toString().left( p->length() - 1 );
            for ( i = 0; i < p->length(); ++i )
            {
                if( p->at(i)->isCustom())
                {
                    posEnd=i;

                    c1.setParag( p );
                    c1.setIndex( posStart );
                    c2.setParag(p );
                    c2.setIndex( posEnd );

                    textdoc->setSelectionStart( QTextDocument::Temp, &c1 );
                    textdoc->setSelectionEnd( QTextDocument::Temp, &c2 );
                    repl=text.mid(posStart,posEnd-posStart);
                    macroCmd->addCommand(replaceSelectionCommand( cursor,textChangedCase(repl,_type),
                                                                  QTextDocument::Temp, "" ));
                    do
                    {
                        ++i;
                    }
                    while( p->at(i)->isCustom() && i != end.index());
                    posStart=i;
                    posEnd=i;

                }
            }
            //change last word
            c1.setParag(p  );
            c1.setIndex( posStart );
            c2.setParag( p );
            c2.setIndex( text.length() );

            textdoc->setSelectionStart( QTextDocument::Temp, &c1 );
            textdoc->setSelectionEnd( QTextDocument::Temp, &c2 );
            repl=text.mid(posStart,text.length()-posStart);
            macroCmd->addCommand(replaceSelectionCommand( cursor,textChangedCase(repl,_type) ,
                                                          QTextDocument::Temp, "" ));

            p = p->next();
        }

        text = end.parag()->string()->toString().left( end.index() );
        posStart=0;
        posEnd=0;
        for ( i = 0; i < end.index(); ++i )
        {
            if( end.parag()->at(i)->isCustom())
            {
                posEnd=i;

                c1.setParag( end.parag() );
                c1.setIndex( posStart );
                c2.setParag(end.parag() );
                c2.setIndex( posEnd );

                textdoc->setSelectionStart( QTextDocument::Temp, &c1 );
                textdoc->setSelectionEnd( QTextDocument::Temp, &c2 );
                repl=text.mid(posStart,posEnd-posStart);
                macroCmd->addCommand(replaceSelectionCommand( cursor,textChangedCase(repl,_type) ,
                                                              QTextDocument::Temp, "" ));
                do
                {
                    ++i;
                }
                while( p->at(i)->isCustom() && i != end.index());
                posStart=i;
                posEnd=i;
            }
        }
        //change last word
        c1.setParag(end.parag()  );
        c1.setIndex( posStart );
        c2.setParag( end.parag() );
        c2.setIndex( end.index() );

        textdoc->setSelectionStart( QTextDocument::Temp, &c1 );
        textdoc->setSelectionEnd( QTextDocument::Temp, &c2 );
        repl=text.mid(posStart,end.index()-posStart);
        macroCmd->addCommand(replaceSelectionCommand( cursor,textChangedCase(repl,_type) ,
                                                      QTextDocument::Temp, "" ));

    }
    m_doc->addCommand( macroCmd);
}


void KWTextFrameSet::insert( QTextCursor * cursor, KWTextFormat * currentFormat,
                             const QString &txt, bool checkNewLine,
                             bool removeSelected, const QString & commandName,
                             CustomItemsMap customItemsMap )
{
    //kdDebug(32001) << "KWTextFrameSet::insert" << endl;
    QTextDocument *textdoc = textDocument();
    emit hideCursor();
    if ( textdoc->hasSelection( QTextDocument::Standard ) && removeSelected ) {
        removeSelectedText( cursor );
    }
    QTextCursor c2 = *cursor;
    if ( !customItemsMap.isEmpty() )
        clearUndoRedoInfo();
    checkUndoRedoInfo( cursor, UndoRedoInfo::Insert );
    if ( !undoRedoInfo.valid() ) {
        if ( !commandName.isNull() ) // see replace-selection
            newPlaceHolderCommand( commandName );
        undoRedoInfo.id = cursor->parag()->paragId();
        undoRedoInfo.index = cursor->index();
        undoRedoInfo.text = QString::null;
    }
    int oldLen = undoRedoInfo.text.length();
    setLastFormattedParag( checkNewLine && cursor->parag()->prev() ?
                           cursor->parag()->prev() : cursor->parag() );
    QTextCursor oldCursor = *cursor;
    cursor->insert( txt, checkNewLine );  // insert the text

    if ( !customItemsMap.isEmpty() ) {
        customItemsMap.insertItems( oldCursor, txt.length() );
        undoRedoInfo.customItemsMap = customItemsMap;
    }

    textdoc->setSelectionStart( QTextDocument::Temp, &oldCursor );
    textdoc->setSelectionEnd( QTextDocument::Temp, cursor );
    //kdDebug() << "KWTextFrameSet::insert setting format " << currentFormat << endl;
    textdoc->setFormat( QTextDocument::Temp, currentFormat, QTextFormat::Format );
    textdoc->removeSelection( QTextDocument::Temp );

    // Speed optimization: if we only type a char, and it doesn't
    // invalidate the next parag, only format the current one
    QTextParag *parag = cursor->parag();
    if ( !checkNewLine && m_lastFormatted == parag && parag->next() && parag->next()->isValid() )
    {
        parag->format();
        m_lastFormatted = m_lastFormatted->next();
    } else
    {
        formatMore();
    }
    emit repaintChanged( this );
    emit ensureCursorVisible();
    emit showCursor();
    undoRedoInfo.text += txt;
    for ( int i = 0; i < (int)txt.length(); ++i ) {
        if ( txt[ oldLen + i ] != '\n' )
            copyCharFormatting( c2.parag(), c2.index(), oldLen + i, false );
        c2.gotoRight();
    }

    m_doc->setModified(true);
    if ( !removeSelected ) {
        // ## not sure why we do this. I'd prefer leaving the selection unchanged...
        // but then it'd need adjustements in the offsets etc.
        if ( textdoc->removeSelection( QTextDocument::Standard ) )
        {
            selectionChangedNotify();
            emit repaintChanged( this );
        }
    }
    if ( !customItemsMap.isEmpty() )
        clearUndoRedoInfo();
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
    QTimer::singleShot( 0, this, SLOT( slotAfterUndoRedo() ) );
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
    QTimer::singleShot( 0, this, SLOT( slotAfterUndoRedo() ) );
}

// This is done in a singleShot timer because of macro-commands.
// We need to do this _after_ terminating the macro command (for instance
// in the case of undoing a floating-frame insertion, we need to delete
// the frame first)
void KWTextFrameSet::slotAfterUndoRedo()
{
    formatMore();
    emit repaintChanged( this );
    emit updateUI( true );
    emit showCursor();
    emit ensureCursorVisible();
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

KCommand * KWTextFrameSet::pasteKWord( QTextCursor * cursor, const QCString & data, bool removeSelected )
{
    // Having data as a QCString instead of a QByteArray seems to fix the trailing 0 problem
    // I tried using QDomDocument::setContent( QByteArray ) but that leads to parse error at the end

    //kdDebug(32001) << "KWTextFrameSet::pasteKWord" << endl;
    KMacroCommand * macroCmd = new KMacroCommand( i18n("Paste Text") );
    QTextDocument *textdoc = textDocument();
    if ( removeSelected && textdoc->hasSelection( QTextDocument::Standard ) )
        macroCmd->addCommand( removeSelectedTextCommand( cursor, QTextDocument::Standard ) );
    emit hideCursor();
    // correct but useless due to unzoom/zoom
    // (which invalidates everything and sets lastformatted to firstparag)
    //setLastFormattedParag( cursor->parag()->prev() ?
    //                       cursor->parag()->prev() : cursor->parag() );

    // We have our own command for this.
    // Using insert() wouldn't help storing the parag stuff for redo
    KWPasteTextCommand * cmd = new KWPasteTextCommand( textDocument(), cursor->parag()->paragId(), cursor->index(), data );
    textDocument()->addCommand( cmd );

    macroCmd->addCommand( new KWTextCommand( this, /*cmd, */QString::null ) );

    *cursor = *( cmd->execute( cursor ) );

    (void) availableHeight(); // calculate it again (set to -1 due to unzoom/zoom)

    formatMore();
    emit repaintChanged( this );
    emit ensureCursorVisible();
    emit updateUI( true );
    selectionChangedNotify();
    emit showCursor();
    return macroCmd;
}

void KWTextFrameSet::insertTOC( QTextCursor * cursor )
{
    emit hideCursor();
    KMacroCommand * macroCmd = new KMacroCommand( i18n("Insert Table Of Contents") );

    // Remove old TOC

    QTextCursor *cur= KWInsertTOCCommand::removeTOC( this, cursor, macroCmd );

    // Insert new TOC

    QTextCommand * cmd = new KWInsertTOCCommand( this,cur ? cur->parag(): cursor->parag() );
    textdoc->addCommand( cmd );
    macroCmd->addCommand( new KWTextCommand( this, QString::null ) );
    *cursor = *( cmd->execute( cursor ) );

    setLastFormattedParag( textdoc->firstParag() );
    formatMore();
    emit repaintChanged( this );
    emit ensureCursorVisible();
    emit updateUI( true );
    emit showCursor();

    m_doc->addCommand( macroCmd );
}

void KWTextFrameSet::setLastFormattedParag( QTextParag *parag )
{
    if ( !m_lastFormatted || m_lastFormatted->paragId() > parag->paragId() )
        m_lastFormatted = parag;
}

KCommand * KWTextFrameSet::insertParagraphCommand( QTextCursor *cursor )
{
    return replaceSelectionCommand( cursor, "\n", QTextDocument::Standard, QString::null );
}

void KWTextFrameSet::insertFrameBreak( QTextCursor *cursor )
{
    clearUndoRedoInfo();
    KMacroCommand* macroCmd = new KMacroCommand( i18n( "Insert Break After Paragraph" ) );
    // Ensure "Frame break" is at beginning of paragraph -> create new parag if necessary
    if ( cursor->index() > 0 )
        macroCmd->addCommand( insertParagraphCommand( cursor ) );

    KWTextParag *parag = static_cast<KWTextParag *>( cursor->parag() );
    macroCmd->addCommand( setPageBreakingCommand( cursor, parag->pageBreaking() | KoParagLayout::HardFrameBreakAfter ) );

    if ( parag->next() == 0 )
        macroCmd->addCommand( insertParagraphCommand( cursor ) );

    m_doc->addCommand( macroCmd );

    setLastFormattedParag( parag );
    formatMore();
    emit repaintChanged( this );
    emit ensureCursorVisible();
    emit updateUI( true );
    emit showCursor();
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
            format->setPointSizeFloat( zoomedFontSize( origFontSize ) );
            //kdDebug(32001) << "KWTextFrameSet::setFormat format " << format << " " << format->key() << " zoomed from " << origFontSize << " to " << format->font().pointSizeFloat() << endl;
        }
        else // The format has a zoomed font already (we're only changing bold/italic, or the family, etc.)
        {
            //kdDebug(32001) << "KWTextFrameSet::setFormat font already zoomed,  finding orig size" << endl;
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
        //kdDebug() << "KWTextFrameSet::setFormat format=" << format << " found in coll: currentFormat=" << currentFormat << " " << currentFormat->key() << endl;
        format = currentFormat;

        if ( origFontSize > 0 && ! m_origFontSizes.find( currentFormat ) )
        {
            //kdDebug() << "KWTextFrameSet::setFormat inserting entry for " << currentFormat->key() << "  origFontSize=" << origFontSize << endl;
            m_origFontSizes.insert( currentFormat, new int( origFontSize ) );
        }
    }

    if ( textdoc->hasSelection( QTextDocument::Standard ) ) {
        emit hideCursor();
        QTextCursor c1 = textdoc->selectionStartCursor( QTextDocument::Standard );
        QTextCursor c2 = textdoc->selectionEndCursor( QTextDocument::Standard );
        undoRedoInfo.clear();
        int id = c1.parag()->paragId();
        int index = c1.index();
        int eid = c2.parag()->paragId();
        int eindex = c2.index();
        readFormats( c1, c2 ); // read previous formatting info
        //kdDebug(32001) << "KWTextFrameSet::setFormat calling textdoc->setFormat " << format->key() << " flags=" << flags << endl;
        textdoc->setFormat( QTextDocument::Standard, format, flags );
        if ( !undoRedoInfo.customItemsMap.isEmpty() )
        {
            // Some custom items (e.g. variables) depend on the format
            CustomItemsMap::Iterator it = undoRedoInfo.customItemsMap.begin();
            for ( ; it != undoRedoInfo.customItemsMap.end(); ++it )
                it.data()->resize();
        }
        KWTextFormatCommand *cmd = new KWTextFormatCommand(
            textdoc, id, index, eid, eindex, undoRedoInfo.text.rawData(),
            format, flags );
        textdoc->addCommand( cmd );
        m_doc->addCommand( new KWTextCommand( this, /*cmd, */i18n("Format text") ) );
        undoRedoInfo.clear();
        setLastFormattedParag( c1.parag() );
        formatMore();
        emit repaintChanged( this );
        emit showCursor();
    }
    //kdDebug(32001) << "KWTextFrameSet::setFormat currentFormat:" << currentFormat->key() << " new format:" << format->key() << endl;
    if ( newFormat ) {
        emit showCurrentFormat();
        //kdDebug(32001) << "KWTextFrameSet::setFormat index=" << cursor->index() << " length-1=" << cursor->parag()->length() - 1 << endl;
        if ( cursor->index() == cursor->parag()->length() - 1 ) {
            currentFormat->addRef();
            cursor->parag()->string()->setFormat( cursor->index(), currentFormat, TRUE );
            if ( cursor->parag()->length() == 1 ) {
                currentFormat->addRef();
                cursor->parag()->setFormat( currentFormat );
                cursor->parag()->invalidate(0);
                cursor->parag()->format();
                emit repaintChanged( this );
            }
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
    // ## Warning. Imagine a paragraph cut in two pieces (at the line-level),
    // between two columns. A single rect in internal coords, but two rects in
    // normal coords. QRect( topLeft, bottomRight ) is just plain wrong.
    // Currently this method is only used for "ensure visible" so that's fine, but
    // we shouldn't use it for more precise stuff.
    QPoint topLeft;
    (void)internalToNormal( parag->rect().topLeft(), topLeft );
    QPoint bottomRight;
    (void)internalToNormal( parag->rect().bottomRight(), bottomRight );
    return QRect( topLeft, bottomRight );
}

void KWTextFrameSet::findPosition( const QPoint &nPoint, QTextParag * & parag, int & index )
{
    QTextCursor cursor( textdoc );

    QPoint iPoint;
    if ( normalToInternal( nPoint, iPoint ) )
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

KCommand * KWTextFrameSet::deleteAnchoredFrame( KWAnchor * anchor )
{
    kdDebug() << "KWTextFrameSet::deleteAnchoredFrame anchor->index=" << anchor->index() << endl;
    ASSERT( anchor );
    QTextCursor c( textdoc );
    c.setParag( anchor->paragraph() );
    c.setIndex( anchor->index() );

    textdoc->setSelectionStart( HighlightSelection, &c );
    c.setIndex( anchor->index() + 1 );
    textdoc->setSelectionEnd( HighlightSelection, &c );
    KCommand *cmd = removeSelectedTextCommand( &c, HighlightSelection );

    m_doc->repaintAllViews();
    return cmd;
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

void KWTextFrameSet::typingStarted()
{
#ifdef DEBUG_FORMAT_MORE
    kdDebug() << "KWTextFrameSet::typingStarted" << endl;
#endif
    changeIntervalTimer->stop();
    interval = 10;
}

void KWTextFrameSet::typingDone()
{
    changeIntervalTimer->start( 100, TRUE );
}

#ifndef NDEBUG
void KWTextFrameSet::printRTDebug( int info )
{
    for (QTextParag * parag = textDocument()->firstParag(); parag ; parag = parag->next())
    {
        KWTextParag * p = static_cast<KWTextParag *>(parag);
        p->printRTDebug( info );
    }
    if ( info == 1 )
        textDocument()->formatCollection()->debug();
}
#endif

///////////////////////////////////////////////////////////////////////////////

KWTextFrameSetEdit::KWTextFrameSetEdit( KWTextFrameSet * fs, KWCanvas * canvas )
    : KWFrameSetEdit( fs, canvas )
{
    //kdDebug(32001) << "KWTextFrameSetEdit::KWTextFrameSetEdit " << fs->getName() << endl;
    connect( fs, SIGNAL( hideCursor() ), this, SLOT( hideCursor() ) );
    connect( fs, SIGNAL( showCursor() ), this, SLOT( showCursor() ) );
    connect( fs, SIGNAL( setCursor( QTextCursor * ) ), this, SLOT( setCursor( QTextCursor * ) ) );
    connect( fs, SIGNAL( updateUI(bool, bool) ), this, SLOT( updateUI(bool, bool) ) );
    connect( fs, SIGNAL( showCurrentFormat() ), this, SLOT( showCurrentFormat() ) );
    connect( fs, SIGNAL( ensureCursorVisible() ), this, SLOT( ensureCursorVisible() ) );
    connect( fs, SIGNAL( selectionChanged(bool) ), canvas, SIGNAL( selectionChanged(bool) ) );
    connect( fs, SIGNAL( frameDeleted(KWFrame *) ), this, SLOT( slotFrameDeleted(KWFrame *) ) );

    cursor = new QTextCursor( textDocument() );

    cursorVisible = false;
    showCursor();
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

    m_currentFormat = 0;
    updateUI( true, true );
}

KWTextFrameSetEdit::~KWTextFrameSetEdit()
{
    //kdDebug(32001) << "KWTextFrameSetEdit::~KWTextFrameSetEdit" << endl;
    delete cursor;
}

void KWTextFrameSetEdit::terminate()
{
    textFrameSet()->clearUndoRedoInfo();
    if ( textDocument()->removeSelection( QTextDocument::Standard ) )
        textFrameSet()->selectionChangedNotify();
    hideCursor();
    disconnect( frameSet(), SIGNAL( selectionChanged(bool) ), m_canvas, SIGNAL( selectionChanged(bool) ) );
}

void KWTextFrameSetEdit::slotFrameDeleted( KWFrame *frm )
{
    if ( m_currentFrame == frm )
        m_currentFrame = 0L;
}

void KWTextFrameSetEdit::keyPressEvent( QKeyEvent * e )
{
    textFrameSet()->typingStarted();

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
        moveCursor( e->state() & ControlButton ? MoveWordBackward : MoveBackward, e->state() & ShiftButton );
        break;
    case Key_Right:
        moveCursor( e->state() & ControlButton ? MoveWordForward : MoveForward, e->state() & ShiftButton );
        break;
    case Key_Up:
        moveCursor( e->state() & ControlButton ? MoveParagUp : MoveUp, e->state() & ShiftButton );
        break;
    case Key_Down:
        moveCursor( e->state() & ControlButton ? MoveParagDown : MoveDown, e->state() & ShiftButton );
        break;
    case Key_Home:
        moveCursor( e->state() & ControlButton ? MoveHome : MoveLineStart, e->state() & ShiftButton );
        break;
    case Key_End:
        moveCursor( e->state() & ControlButton ? MoveEnd : MoveLineEnd, e->state() & ShiftButton );
        break;
    case Key_Prior:
        moveCursor( e->state() & ControlButton ? MovePgUp : MoveViewportUp, e->state() & ShiftButton );
        break;
    case Key_Next:
        moveCursor( e->state() & ControlButton ? MovePgDown : MoveViewportDown, e->state() & ShiftButton );
        break;
    case Key_Return: case Key_Enter:
        if ( textDocument()->hasSelection( QTextDocument::Standard ) )
            textFrameSet()->removeSelectedText( cursor );
        clearUndoRedoInfo = FALSE;
        textFrameSet()->doKeyboardAction( cursor, m_currentFormat, KWTextFrameSet::ActionReturn );
        break;
    case Key_Delete:
        if ( textDocument()->hasSelection( QTextDocument::Standard ) ) {
            textFrameSet()->removeSelectedText( cursor );
            break;
        }

        if ( e->state() & ControlButton )
        {
            // Delete a word
            textDocument()->setSelectionStart( QTextDocument::Standard, cursor );

            do {
                cursor->gotoRight();
            } while ( !cursor->atParagEnd()
                      && !cursor->parag()->at( cursor->index() )->c.isSpace() );
            textDocument()->setSelectionEnd( QTextDocument::Standard, cursor );
            textFrameSet()->removeSelectedText( cursor, QTextDocument::Standard, i18n("Remove word") );
        }
        else
            textFrameSet()->doKeyboardAction( cursor, m_currentFormat, KWTextFrameSet::ActionDelete );

        clearUndoRedoInfo = FALSE;
        break;
    case Key_Backspace:
        if ( textDocument()->hasSelection( QTextDocument::Standard ) ) {
            textFrameSet()->removeSelectedText( cursor );
            break;
        }
        if ( !cursor->parag()->prev() &&
             cursor->atParagStart() )
        {
            KWTextParag * parag = static_cast<KWTextParag *>(cursor->parag());
            if ( parag->counter() && parag->counter()->style() != KoParagCounter::STYLE_NONE)
                textFrameSet()->doKeyboardAction( cursor, m_currentFormat, KWTextFrameSet::ActionBackspace );
            break;
        }
        if ( e->state() & ControlButton )
        {
            // Delete a word
            textDocument()->setSelectionStart( QTextDocument::Standard, cursor );

            do {
                cursor->gotoLeft();
            } while ( !cursor->atParagStart()
                      && !cursor->parag()->at( cursor->index()-1 )->c.isSpace() );
            textDocument()->setSelectionEnd( QTextDocument::Standard, cursor );
            textFrameSet()->removeSelectedText( cursor, QTextDocument::Standard, i18n("Remove word") );
        }
        else
            textFrameSet()->doKeyboardAction( cursor, m_currentFormat, KWTextFrameSet::ActionBackspace );

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
                // Port to setCounter if we really want that - and make configurable
                /*if ( cursor->parag()->style() &&
                     cursor->parag()->style()->displayMode() == QStyleSheetItem::DisplayBlock &&
                     cursor->index() == 0 && ( e->text() == "-" || e->text() == "*" ) ) {
                    setParagType( QStyleSheetItem::DisplayListItem, QStyleSheetItem::ListDisc );
                    break;
                }*/
                QString text = e->text();
                if(textFrameSet()->hasSelection() )
                    frameSet()->kWordDocument()->addCommand(textFrameSet()->replaceSelectionCommand(  cursor, text, QTextDocument::Standard , i18n("Insert Text")));
                else
                    textFrameSet()->insert( cursor, m_currentFormat, text, false, true, i18n("Insert Text") );

                KWAutoFormat * autoFormat = textFrameSet()->kWordDocument()->getAutoFormat();
                if ( autoFormat )
                    autoFormat->doAutoFormat( cursor, static_cast<KWTextParag*>(cursor->parag()),
                                              cursor->index() - 1,
                                              text[ text.length() - 1 ] );

                break;
            }
            // We should use KAccel instead, to make this configurable !
            if ( e->state() & ControlButton ) {
                switch ( e->key() ) {
                case Key_F16: // Copy key on Sun keyboards
                    copy();
                    break;
                case Key_A:
                    moveCursor( MoveLineStart, e->state() & ShiftButton );
                    break;
                case Key_E:
                    moveCursor( MoveLineEnd, e->state() & ShiftButton );
                    break;
                case Key_K:
                    textFrameSet()->doKeyboardAction( cursor, m_currentFormat, KWTextFrameSet::ActionKill );
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
    textFrameSet()->typingDone();
}

void KWTextFrameSetEdit::moveCursor( CursorAction action, bool select )
{
    hideCursor();
    if ( select ) {
        if ( !textDocument()->hasSelection( QTextDocument::Standard ) )
            textDocument()->setSelectionStart( QTextDocument::Standard, cursor );
        moveCursor( action );
        if ( textDocument()->setSelectionEnd( QTextDocument::Standard, cursor ) ) {
            //      cursor->parag()->document()->nextDoubleBuffered = TRUE; ##### we need that only if we have nested items/documents
            textFrameSet()->selectionChangedNotify();
        } else {
            showCursor();
        }
    } else {
        bool redraw = textDocument()->removeSelection( QTextDocument::Standard );
        moveCursor( action );
        if ( redraw ) {
            //cursor->parag()->document()->nextDoubleBuffered = TRUE; // we need that only if we have nested items/documents
            textFrameSet()->selectionChangedNotify();
        }
    }

    ensureCursorVisible();
    showCursor();
    updateUI( true );
}

void KWTextFrameSetEdit::moveCursor( CursorAction action )
{
    switch ( action ) {
        case MoveBackward:
            cursor->gotoLeft();
            break;
        case MoveWordBackward:
            cursor->gotoWordLeft();
            break;
        case MoveForward:
            cursor->gotoRight();
            break;
        case MoveWordForward:
            cursor->gotoWordRight();
            break;
        case MoveUp:
            cursor->gotoUp();
            break;
        case MoveDown:
            cursor->gotoDown();
            break;
        case MoveViewportUp:
        {
            QRect crect( m_canvas->contentsX(), m_canvas->contentsY(),
                         m_canvas->visibleWidth(), m_canvas->visibleHeight() );
            crect = m_canvas->viewMode()->viewToNormal( crect );

#if 0
            // One idea would be: move up until first-visible-paragraph wouldn't be visible anymore
            // First find the first-visible paragraph...
            QTextParag *firstVis = cursor->parag();
            while ( firstVis && crect.intersects( s->rect() ) )
                firstVis = firstVis->prev();
            if ( !firstVis )
                firstVis = textFrameSet()->textDocument()->firstParag();
            else if ( firstVis->next() )
                firstVis = firstVis->next();
#endif
            // Go up of 90% of crect.height()
            int h = static_cast<int>( (double)crect.height() * 0.9 );
            QTextParag *s = cursor->parag();
            int y = s->rect().y();
            while ( s ) {
                if ( y - s->rect().y() >= h )
                    break;
                s = s->prev();
            }

            if ( !s )
                s = textFrameSet()->textDocument()->firstParag();

            cursor->setParag( s );
            cursor->setIndex( 0 );

            //cursor->gotoPageUp( m_canvas->visibleHeight() );
        }
        break;
        case MoveViewportDown:
        {
            QRect crect( m_canvas->contentsX(), m_canvas->contentsY(),
                         m_canvas->visibleWidth(), m_canvas->visibleHeight() );
            crect = m_canvas->viewMode()->viewToNormal( crect );
            // Go down of 90% of crect.height()
            int h = static_cast<int>( (double)crect.height() * 0.9 );

            QTextParag *s = cursor->parag();
            int y = s->rect().y();
            while ( s ) {
                if ( s->rect().y() - y >= h )
                    break;
                s = s->next();
            }

            if ( !s ) {
                s = textFrameSet()->textDocument()->lastParag();
                cursor->setParag( s );
                cursor->setIndex( s->length() - 1 );
            } else {
                cursor->setParag( s );
                cursor->setIndex( 0 );
            }
            //cursor->gotoPageDown( m_canvas->visibleHeight() );
        } break;
        case MovePgUp:
            if ( m_currentFrame )
            {
                QPoint iPoint = textFrameSet()->moveToPage( m_currentFrame->pageNum(), -1 );
                if ( !iPoint.isNull() )
                    placeCursor( iPoint );
            }
            break;
        case MovePgDown:
            if ( m_currentFrame )
            {
                QPoint iPoint = textFrameSet()->moveToPage( m_currentFrame->pageNum(), +1 );
                if ( !iPoint.isNull() )
                    placeCursor( iPoint );
            }
            break;
        case MoveLineStart:
            cursor->gotoLineStart();
            break;
        case MoveHome:
            cursor->gotoHome();
            break;
        case MoveLineEnd:
            cursor->gotoLineEnd();
            break;
        case MoveEnd:
            textFrameSet()->ensureFormatted( textFrameSet()->textDocument()->lastParag() );
            cursor->gotoEnd();
            break;
        case MoveParagUp: {
            QTextParag * parag = cursor->parag()->prev();
            if ( parag )
            {
                cursor->setParag( parag );
                cursor->setIndex( 0 );
            }
        } break;
        case MoveParagDown: {
            QTextParag * parag = cursor->parag()->next();
            if ( parag )
            {
                cursor->setParag( parag );
                cursor->setIndex( 0 );
            }
        } break;
    }

    updateUI( true );
}

void KWTextFrameSetEdit::paste()
{
    QMimeSource *data = QApplication::clipboard()->data();
    if ( data->provides( KWTextDrag::selectionMimeType() ) )
    {
        QByteArray arr = data->encodedData( KWTextDrag::selectionMimeType() );
        if ( arr.size() )
            frameSet()->kWordDocument()->addCommand(textFrameSet()->pasteKWord( cursor, QCString( arr ), true ));
    }
    else
    {
        // Note: QClipboard::text() seems to do a better job than encodedData( "text/plain" )
        // In particular it handles charsets (in the mimetype).
        QString text = QApplication::clipboard()->text();
        if ( !text.isEmpty() )
            textFrameSet()->pasteText( cursor, text, m_currentFormat, true );
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
        KWTextDrag *kd = newDrag( 0L );
        QApplication::clipboard()->setData( kd );
    }
}

void KWTextFrameSetEdit::startDrag()
{
    mightStartDrag = FALSE;
    inDoubleClick = FALSE;
    m_canvas->dragStarted();
    KWTextDrag *drag = newDrag( m_canvas->viewport() );
    if ( !frameSet()->kWordDocument()->isReadWrite() )
        drag->dragCopy();
    else {
        if ( drag->drag() && QDragObject::target() != m_canvas && QDragObject::target() != m_canvas->viewport() ) {
            //This is when dropping text _out_ of KWord. Since we have Move and Copy
            //options (Copy being accessed by pressing CTRL), both are possible.
            //But is that intuitive enough ? Doesn't the user expect a Copy in all cases ?
            //Losing the selection when dropping out of kword seems quite unexpected to me.
            //Undecided about this........
            textFrameSet()->removeSelectedText( cursor );
        }
    }
}

KWTextDrag * KWTextFrameSetEdit::newDrag( QWidget * parent ) const
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
        static_cast<KWTextParag *>(c1.parag())->save( elem, c1.index(), c2.index()-1, true );
    }
    else
    {
        text += c1.parag()->string()->toString().mid( c1.index() ) + "\n";
        static_cast<KWTextParag *>(c1.parag())->save( elem, c1.index(), c1.parag()->length()-2, true );
        QTextParag *p = c1.parag()->next();
        while ( p && p != c2.parag() ) {
            text += p->string()->toString() + "\n";
            static_cast<KWTextParag *>(p)->save( elem, 0, p->length()-2, true );
            p = p->next();
        }
        text += c2.parag()->string()->toString().left( c2.index() );
        static_cast<KWTextParag *>(c2.parag())->save( elem, 0, c2.index()-1, true );
    }
    textFrameSet()->zoom( false );

    KWTextDrag *kd = new KWTextDrag( parent );
    kd->setPlain( text );
    kd->setKWord( domDoc.toCString() );
    kdDebug(32001) << "KWTextFrameSetEdit::newDrag " << domDoc.toCString() << endl;
    return kd;
}

void KWTextFrameSetEdit::ensureCursorVisible()
{
    //kdDebug() << "KWTextFrameSetEdit::ensureCursorVisible paragId=" << cursor->parag()->paragId() << endl;
    QTextParag * parag = cursor->parag();
    textFrameSet()->ensureFormatted( parag );
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
    QPoint hintNPoint;
    if ( m_currentFrame )
        hintNPoint = frameSet()->kWordDocument()->zoomPoint( m_currentFrame->topLeft() );
    KWFrame * frame = textFrameSet()->internalToNormalWithHint( QPoint(x, y), p, hintNPoint );
    //kdDebug() << "KWTextFrameSetEdit::ensureCursorVisible frame=" << frame << " m_currentFrame=" << m_currentFrame << endl;
    if ( frame && m_currentFrame != frame )
    {
        m_currentFrame = frame;
        m_canvas->gui()->getView()->updatePageInfo();
    }
    p = m_canvas->viewMode()->normalToView( p );
    m_canvas->ensureVisible( p.x(), p.y() + h / 2, w, h / 2 + 2 );
}

void KWTextFrameSetEdit::mousePressEvent( QMouseEvent *e, const QPoint & nPoint, const KoPoint & )
{
    textFrameSet()->clearUndoRedoInfo();
    if ( m_currentFrame )
        hideCursor(); // Need to do that with the old m_currentFrame

    mightStartDrag = FALSE;

    QPoint iPoint;
    KWFrame * frame = textFrameSet()->normalToInternal( nPoint, iPoint, true );
    if ( frame && m_currentFrame != frame )
    {
        m_currentFrame = frame;
        m_canvas->gui()->getView()->updatePageInfo();
    }

    if ( m_currentFrame )
    {
        QTextCursor oldCursor = *cursor;
        placeCursor( iPoint );
        ensureCursorVisible();

        if ( e->button() != LeftButton )
        {
            showCursor();
            return;
        }

        QTextDocument * textdoc = textDocument();
        if ( textdoc->inSelection( QTextDocument::Standard, iPoint ) ) {
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
            showCursor();
        } else {
            textFrameSet()->selectionChangedNotify();
        }
    }
}

void KWTextFrameSetEdit::mouseMoveEvent( QMouseEvent * e, const QPoint & nPoint, const KoPoint & )
{
    if ( mightStartDrag ) {
        dragStartTimer->stop();
        if ( ( e->pos() - dragStartPos ).manhattanLength() > QApplication::startDragDistance() )
            startDrag();
        return;
    }
    QPoint iPoint;
    if ( nPoint.y() > 0 && textFrameSet()->normalToInternal( nPoint, iPoint, true ) )
    {
        hideCursor();
        QTextCursor oldCursor = *cursor;
        placeCursor( iPoint );

        // Double click + mouse still down + moving the mouse selects full words.
        if ( inDoubleClick ) {
            QTextCursor cl = *cursor;
            cl.gotoWordLeft();
            QTextCursor cr = *cursor;
            cr.gotoWordRight();

            int diff = QABS( oldCursor.parag()->at( oldCursor.index() )->x - iPoint.x() );
            int ldiff = QABS( cl.parag()->at( cl.index() )->x - iPoint.x() );
            int rdiff = QABS( cr.parag()->at( cr.index() )->x - iPoint.x() );

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
        //the autoscroll already takes care of the scrolling
        //ensureCursorVisible();

        bool redraw = FALSE;
        if ( textDocument()->hasSelection( QTextDocument::Standard ) )
            redraw = textDocument()->setSelectionEnd( QTextDocument::Standard, cursor ) || redraw;
        else // it may be that the initial click was out of the frame
            textDocument()->setSelectionStart( QTextDocument::Standard, cursor );

        if ( redraw )
            textFrameSet()->selectionChangedNotify( false );

        showCursor();
    }
}

void KWTextFrameSetEdit::mouseReleaseEvent( QMouseEvent *, const QPoint &, const KoPoint & )
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
        // But auto-copy in readonly mode, since there is no action available in that case.
        if ( !frameSet()->kWordDocument()->isReadWrite() )
            copy();
    }

    inDoubleClick = FALSE;
    emit showCursor();
}

void KWTextFrameSetEdit::mouseDoubleClickEvent( QMouseEvent *, const QPoint &, const KoPoint & )
{
    inDoubleClick = TRUE;
    *cursor = selectWordUnderCursor();
    textFrameSet()->selectionChangedNotify();
    // No auto-copy, will readd with Qt 3 using setSelectionMode(true/false)
    // But auto-copy in readonly mode, since there is no action available in that case.
    if ( !frameSet()->kWordDocument()->isReadWrite() )
        copy();
}

QTextCursor KWTextFrameSetEdit::selectWordUnderCursor()
{
    QTextCursor c1 = *cursor;
    QTextCursor c2 = *cursor;
    if ( cursor->index() > 0
         && !cursor->parag()->at( cursor->index()-1 )->c.isSpace()
         && !cursor->parag()->at( cursor->index()-1 )->isCustom())
        c1.gotoWordLeft();

    if ( !cursor->parag()->at( cursor->index() )->c.isSpace()
         && !cursor->atParagEnd()
         && !cursor->parag()->at( cursor->index() )->isCustom())
        c2.gotoWordRight();

    textDocument()->setSelectionStart( QTextDocument::Standard, &c1 );
    textDocument()->setSelectionEnd( QTextDocument::Standard, &c2 );
    return c2;
}

void KWTextFrameSetEdit::dragEnterEvent( QDragEnterEvent * e )
{
    if ( !frameSet()->kWordDocument()->isReadWrite() || !KWTextDrag::canDecode( e ) )
    {
        e->ignore();
        return;
    }
    e->acceptAction();
}

void KWTextFrameSetEdit::dragMoveEvent( QDragMoveEvent * e, const QPoint &nPoint, const KoPoint & )
{
    if ( !frameSet()->kWordDocument()->isReadWrite() || !KWTextDrag::canDecode( e ) )
    {
        e->ignore();
        return;
    }
    QPoint iPoint;
    if ( textFrameSet()->normalToInternal( nPoint, iPoint ) )
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

void KWTextFrameSetEdit::dropEvent( QDropEvent * e, const QPoint & nPoint, const KoPoint & )
{
    if ( frameSet()->kWordDocument()->isReadWrite() && KWTextDrag::canDecode( e ) )
    {
        KMacroCommand *macroCmd=new KMacroCommand(i18n("Paste Text"));
        e->acceptAction();
        QTextCursor dropCursor( textDocument() );
        QPoint dropPoint;
        if ( !textFrameSet()->normalToInternal( nPoint, dropPoint ) )
            return; // Don't know where to paste

        dropCursor.place( dropPoint, textDocument()->firstParag() );
        kdDebug(32001) << "KWTextFrameSetEdit::dropEvent dropCursor at parag=" << dropCursor.parag()->paragId() << " index=" << dropCursor.index() << endl;

        if ( ( e->source() == m_canvas ||
               e->source() == m_canvas->viewport() ) &&
             e->action() == QDropEvent::Move ) {

            if ( textDocument()->hasSelection( QTextDocument::Standard ) )
            {
                // Dropping into the selection itself ?
                QTextCursor startSel = textDocument()->selectionStartCursor( QTextDocument::Standard );
                QTextCursor endSel = textDocument()->selectionEndCursor( QTextDocument::Standard );
                bool inSelection = false;
                if ( startSel.parag() == endSel.parag() )
                    inSelection = ( dropCursor.parag() == startSel.parag() )
                                    && dropCursor.index() >= startSel.index()
                                    && dropCursor.index() <= endSel.index();
                else
                {
                    // Looking at first line first:
                    inSelection = dropCursor.parag() == startSel.parag() && dropCursor.index() >= startSel.index();
                    if ( !inSelection )
                    {
                        // Look at all other paragraphs except last one
                        QTextParag *p = startSel.parag()->next();
                        while ( !inSelection && p && p != endSel.parag() )
                        {
                            inSelection = ( p == dropCursor.parag() );
                            p = p->next();
                        }
                        // Look at last paragraph
                        if ( !inSelection )
                            inSelection = dropCursor.parag() == endSel.parag() && dropCursor.index() <= endSel.index();
                    }
                }
                if ( inSelection )
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
                if ( endSel.parag() == dropCursor.parag() )
                {
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

                //textFrameSet()->removeSelectedText( cursor );
                macroCmd->addCommand(textFrameSet()->removeSelectedTextCommand( cursor,QTextDocument::Standard ));
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

        if ( e->provides( KWTextDrag::selectionMimeType() ) )
        {
            QByteArray arr = e->encodedData( KWTextDrag::selectionMimeType() );
            if ( arr.size() )
                macroCmd->addCommand(textFrameSet()->pasteKWord( cursor, QCString(arr), false ));
        }
        else
        {
            QString text;
            if ( QTextDrag::decode( e, text ) )
                textFrameSet()->pasteText( cursor, text, m_currentFormat, false );
        }
        frameSet()->kWordDocument()->addCommand(macroCmd);
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

void KWTextFrameSetEdit::placeCursor( const QPoint &pos )
{
    cursor->restoreState();
    QTextParag *s = textDocument()->firstParag();
    cursor->place( pos,  s );
    updateUI( true );
}

void KWTextFrameSetEdit::blinkCursor()
{
    //kdDebug() << "KWTextFrameSetEdit::blinkCursor cursorVisible=" << cursorVisible
    //          << " blinkCursorVisible=" << blinkCursorVisible << endl;
    if ( !cursorVisible )
        return;
    bool cv = cursorVisible;
    blinkCursorVisible = !blinkCursorVisible;
    drawCursor( blinkCursorVisible );
    cursorVisible = cv;
}

void KWTextFrameSetEdit::drawCursor( bool visible )
{
    //kdDebug() << "KWTextFrameSetEdit::drawCursor " << visible << endl;
    cursorVisible = visible;
    if ( !cursor->parag() )
        return;

    if ( !cursor->parag()->isValid() )
        textFrameSet()->ensureFormatted( cursor->parag() );

    if ( !frameSet()->kWordDocument()->isReadWrite() )
        return;
    if ( !m_currentFrame )
        return;

    QPainter p( m_canvas->viewport() );
    p.translate( -m_canvas->contentsX(), -m_canvas->contentsY() );
    p.setBrushOrigin( -m_canvas->contentsX(), -m_canvas->contentsY() );

    //QRect crect( m_canvas->contentsX(), m_canvas->contentsY(), m_canvas->visibleWidth(), m_canvas->visibleHeight() );
    textFrameSet()->drawCursor( &p, cursor, visible, m_canvas, m_currentFrame );
}


void KWTextFrameSetEdit::setDefaultFormat() {
    QTextFormatCollection * coll = textDocument()->formatCollection();
    KWTextFormat * format = static_cast<KWTextFormat *>(coll->defaultFormat());
    textFrameSet()->setFormat( cursor, m_currentFormat, format, QTextFormat::Format );
}


void KWTextFrameSetEdit::setBold( bool on ) {
    KWTextFormat format( *m_currentFormat );
    format.setBold( on );
    textFrameSet()->setFormat( cursor, m_currentFormat, &format, QTextFormat::Bold );
   //kdDebug(32003) << "KWTextFrameSetEdit::setBold new m_currentFormat " << m_currentFormat << " " << m_currentFormat->key() << endl;
}

void KWTextFrameSetEdit::setItalic( bool on ) {
    KWTextFormat format( *m_currentFormat );
    format.setItalic( on );
    textFrameSet()->setFormat( cursor, m_currentFormat, &format, QTextFormat::Italic );
}

void KWTextFrameSetEdit::setUnderline( bool on ) {
    KWTextFormat format( *m_currentFormat );
    format.setUnderline( on );
    textFrameSet()->setFormat( cursor, m_currentFormat, &format, QTextFormat::Underline );
}

void KWTextFrameSetEdit::setStrikeOut( bool on ) {
    KWTextFormat format( *m_currentFormat );
    format.setStrikeOut( on);
    textFrameSet()->setFormat( cursor, m_currentFormat, &format, KWTextFormat::StrikeOut );
}

QColor KWTextFrameSetEdit::textColor() const {
    return m_currentFormat->color();
}

QFont KWTextFrameSetEdit::textFont() const {
    QFont fn( m_currentFormat->font() );
    fn.setPointSize( textFrameSet()->docFontSize( m_currentFormat ) ); // "unzoom" the font size
    return fn;
}

/*int KWTextFrameSetEdit::textFontSize()const{
    return textFrameSet()->docFontSize( m_currentFormat );
}*/

QString KWTextFrameSetEdit::textFontFamily()const {
    return m_currentFormat->font().family();
}

void KWTextFrameSetEdit::setPointSize( int s ){
    KWTextFormat format( *m_currentFormat );
    format.setPointSize( s );
    textFrameSet()->setFormat( cursor, m_currentFormat, &format, QTextFormat::Size, true /* zoom the font size */ );
}

void KWTextFrameSetEdit::setFamily(const QString &font){
    KWTextFormat format( *m_currentFormat );
    format.setFamily( font );
    textFrameSet()->setFormat( cursor, m_currentFormat, &format, QTextFormat::Family );
}

void KWTextFrameSetEdit::setFont( const QFont &font, bool _subscript, bool _superscript, const QColor &col, int flags )
{
    KWTextFormat format( *m_currentFormat );
    format.setFont( font );
    format.setColor( col );

    if(!_subscript)
    {
        if(!_superscript)
            format.setVAlign(QTextFormat::AlignNormal);
        else
            format.setVAlign(QTextFormat::AlignSuperScript);
    }
    else
        format.setVAlign(QTextFormat::AlignSubScript);

    textFrameSet()->setFormat( cursor, m_currentFormat, &format, flags, true /* zoom the font size */);
}

void KWTextFrameSetEdit::setTextColor(const QColor &color) {
    KWTextFormat format( *m_currentFormat );
    format.setColor( color );
    textFrameSet()->setFormat( cursor, m_currentFormat, &format, QTextFormat::Color );
}

void KWTextFrameSetEdit::setTextSubScript(bool on)
{
    KWTextFormat format( *m_currentFormat );
    if(!on)
        format.setVAlign(QTextFormat::AlignNormal);
    else
        format.setVAlign(QTextFormat::AlignSubScript);
    textFrameSet()->setFormat( cursor, m_currentFormat, &format, QTextFormat::VAlign );
}

void KWTextFrameSetEdit::setTextSuperScript(bool on)
{
    KWTextFormat format( *m_currentFormat );
    if(!on)
        format.setVAlign(QTextFormat::AlignNormal);
    else
        format.setVAlign(QTextFormat::AlignSuperScript);
    textFrameSet()->setFormat( cursor, m_currentFormat, &format, QTextFormat::VAlign );
}

void KWTextFrameSetEdit::insertSpecialChar(QChar _c)
{
    if(textFrameSet()->hasSelection() )
        frameSet()->kWordDocument()->addCommand(textFrameSet()->replaceSelectionCommand(  cursor, _c, QTextDocument::Standard, i18n("Insert Special Char")));
    else
        textFrameSet()->insert( cursor, m_currentFormat, _c, false /* no newline */, true, i18n("Insert Special Char") );
}

void  KWTextFrameSetEdit::insertExpression(const QString &_c)
{
    if(textFrameSet()->hasSelection() )
        frameSet()->kWordDocument()->addCommand(textFrameSet()->replaceSelectionCommand(  cursor, _c, QTextDocument::Standard , i18n("Insert Expression")));
    else
       textFrameSet()->insert( cursor, m_currentFormat, _c, false /* no newline */, true, i18n("Insert Expression") );
}

void KWTextFrameSetEdit::insertFloatingFrameSet( KWFrameSet * fs, const QString & commandName )
{
    textFrameSet()->clearUndoRedoInfo();
    CustomItemsMap customItemsMap;
    QString placeHolders;
    // TODO support for multiple floating items (like multiple-page tables)
    int frameNumber = 0;
    int index = 0;
    bool ownline = false;
    { // the loop will start here :)
        KWAnchor * anchor = fs->createAnchor( textFrameSet()->textDocument(), frameNumber );
        if ( frameNumber == 0 && anchor->ownLine() && cursor->index() > 0 ) // enforce start of line - currently unused
        {
            kdDebug() << "ownline -> prepending \\n" << endl;
            placeHolders += QChar('\n');
            index++;
            ownline = true;
        }
        placeHolders += KWTextFrameSet::customItemChar();
        customItemsMap.insert( index, anchor );
    }
    fs->setAnchored( textFrameSet() );
    textFrameSet()->insert( cursor, m_currentFormat, placeHolders,
                            ownline, false, commandName,
                            customItemsMap );
}


void KWTextFrameSetEdit::insertCustomVariable( const QString &name)
{
     KWVariable * var = 0L;
     KWDocument * doc = frameSet()->kWordDocument();
     var = new KWCustomVariable( textFrameSet(),name, doc->variableFormat( VT_CUSTOM ) );
     insertVariable( var);
}

void KWTextFrameSetEdit::insertVariable( int type, int subtype )
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
        var = KWVariable::createVariable( type, subtype, textFrameSet() );

    insertVariable( var);

}

void KWTextFrameSetEdit::insertVariable( KWVariable *var)
{
    if ( var )
    {
        CustomItemsMap customItemsMap;
        customItemsMap.insert( 0, var );
        kdDebug() << "KWTextFrameSetEdit::insertVariable inserting into paragraph" << endl;
#ifdef DEBUG_FORMATS
        kdDebug() << "KWTextFrameSetEdit::insertVariable m_currentFormat=" << m_currentFormat << endl;
#endif
        textFrameSet()->insert( cursor, m_currentFormat, KWTextFrameSet::customItemChar(),
                                false, false, i18n("Insert Variable"),
                                customItemsMap );
        var->recalc();
        cursor->parag()->invalidate(0);
        cursor->parag()->setChanged( true );
        frameSet()->kWordDocument()->slotRepaintChanged( frameSet() );
        frameSet()->kWordDocument()->refreshMenuCustomVariable();
    }
}

// Update the GUI toolbar button etc. to reflect the current cursor position.
void KWTextFrameSetEdit::updateUI( bool updateFormat, bool force )
{
    // Update UI - only for those items which have changed

    if ( updateFormat )
    {
        int i = cursor->index();
        if ( i > 0 )
            --i;
#ifdef DEBUG_FORMATS
        if ( m_currentFormat )
            kdDebug(32003) << "KWTextFrameSet::updateUI old m_currentFormat=" << m_currentFormat
                           << " " << m_currentFormat->key()
                           << " parag format=" << cursor->parag()->at( i )->format()->key() << endl;
        else
            kdDebug(32003) << "KWTextFrameSetEdit::updateUI old m_currentFormat=0" << endl;
#endif
        if ( !m_currentFormat || m_currentFormat->key() != cursor->parag()->at( i )->format()->key() )
        {
            if ( m_currentFormat )
                m_currentFormat->removeRef();
#ifdef DEBUG_FORMATS
            kdDebug() << "Setting m_currentFormat from format " << cursor->parag()->at( i )->format()
                      << " ( character " << i << " in paragraph " << cursor->parag()->paragId() << " )" << endl;
#endif
            m_currentFormat = static_cast<KWTextFormat *>( textDocument()->formatCollection()->format( cursor->parag()->at( i )->format() ) );
            if ( m_currentFormat->isMisspelled() ) {
                m_currentFormat->removeRef();
                m_currentFormat = static_cast<KWTextFormat *>( textDocument()->formatCollection()->format( m_currentFormat->font(), m_currentFormat->color() ) );
            }
            showCurrentFormat();
        }
    }

    // Paragraph settings
    KWTextParag * parag = static_cast<KWTextParag *>(cursor->parag());

    if ( m_paragLayout.alignment != parag->alignment() || force ) {
        m_paragLayout.alignment = parag->alignment();
        m_canvas->gui()->getView()->showAlign( m_paragLayout.alignment );
    }

    // Counter
    if ( !m_paragLayout.counter )
        m_paragLayout.counter = new KoParagCounter; // we can afford to always have one here
    KoParagCounter::Style cstyle = m_paragLayout.counter->style();
    if ( parag->counter() )
        *m_paragLayout.counter = *parag->counter();
    else
    {
        m_paragLayout.counter->setNumbering( KoParagCounter::NUM_NONE );
        m_paragLayout.counter->setStyle( KoParagCounter::STYLE_NONE );
    }
    if ( m_paragLayout.counter->style() != cstyle || force )
        m_canvas->gui()->getView()->showCounter( * m_paragLayout.counter );

    if(m_paragLayout.leftBorder!=parag->leftBorder() ||
       m_paragLayout.rightBorder!=parag->rightBorder() ||
       m_paragLayout.topBorder!=parag->topBorder() ||
       m_paragLayout.bottomBorder!=parag->bottomBorder() || force )
    {
        m_paragLayout.leftBorder = parag->leftBorder();
        m_paragLayout.rightBorder = parag->rightBorder();
        m_paragLayout.topBorder = parag->topBorder();
        m_paragLayout.bottomBorder = parag->bottomBorder();
        m_canvas->gui()->getView()->showParagBorders( m_paragLayout.leftBorder, m_paragLayout.rightBorder, m_paragLayout.topBorder, m_paragLayout.bottomBorder );
    }

    if ( !parag->style() )
        kdWarning() << "Paragraph " << parag->paragId() << " has no style" << endl;
    else if ( m_paragLayout.style != parag->style() || force )
    {
        m_paragLayout.style = parag->style();
        m_canvas->gui()->getView()->showStyle( m_paragLayout.style->name() );
    }

    if( m_paragLayout.margins[QStyleSheetItem::MarginLeft] != parag->margin(QStyleSheetItem::MarginLeft)
        || m_paragLayout.margins[QStyleSheetItem::MarginFirstLine] != parag->margin(QStyleSheetItem::MarginFirstLine) || force )
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

void KWTextFrameSetEdit::applyStyle( const KWStyle * style )
{
    textFrameSet()->applyStyle( cursor, style );
    kdDebug() << "KWTextFrameSetEdit::applyStyle m_currentFormat=" << m_currentFormat << " " << m_currentFormat->key() << endl;
    showCurrentFormat();
}

void KWTextFrameSetEdit::showCurrentFormat()
{
    //kdDebug() << "KWTextFrameSetEdit::showCurrentFormat m_currentFormat=" << m_currentFormat << endl;
    KWTextFormat format = *m_currentFormat;
    format.setPointSize( textFrameSet()->docFontSize( m_currentFormat ) ); // "unzoom" the font size
    m_canvas->gui()->getView()->showFormat( format );
}

QList<KAction> KWTextFrameSetEdit::dataToolActionList()
{
    m_singleWord = false;
    m_wordUnderCursor = QString::null;
    KWDocument * doc = frameSet()->kWordDocument();
    QString text;
    if ( textFrameSet()->hasSelection() )
    {
        text = textFrameSet()->selectedText();
        if ( text.find(' ') == -1 && text.find('\t') == -1 && text.find(KWTextFrameSet::customItemChar()) == -1 )
            m_singleWord = true;
    }
    else // No selection -> get word under cursor
    {
        selectWordUnderCursor();
        text = textFrameSet()->selectedText();
        if(text.find(KWTextFrameSet::customItemChar()) == -1)
        {
            textDocument()->removeSelection( QTextDocument::Standard );
            m_singleWord = true;
            m_wordUnderCursor = text;
        }
        else
        {
            text = "";
        }
    }

#ifdef KSPELL_HAS_IGNORE_UPPER_WORD
    if(!text.isEmpty() && doc->dontCheckTitleCase() && text==text.upper())
    {
        text="";
        m_singleWord = false;
    }
    else if(!text.isEmpty() && doc->dontCheckUpperWord() && text[0]==text[0].upper())
    {
        QString tmp=text[0]+text.right(text.length()-1).lower();
        if(text==tmp)
        {
            text="";
            m_singleWord = false;
        }
    }
#endif

    if ( text.isEmpty() ) // Nothing to apply a tool to
        return QList<KAction>();

    // Any tool that works on plain text is relevant
    QValueList<KoDataToolInfo> tools = KoDataToolInfo::query( "QString", "text/plain", doc );

    // Add tools that work on a single word if that is the case
    if ( m_singleWord )
        tools += KoDataToolInfo::query( "QString", "application/x-singleword", doc );

    // Maybe one day we'll have tools that link to kwordpart (later: qt3), to act on formatted text
    tools += KoDataToolInfo::query( "QTextString", "application/x-qrichtext", doc );

    return KoDataToolAction::dataToolActionList( tools, this, SLOT( slotToolActivated( const KoDataToolInfo &, const QString & ) ) );
}

void KWTextFrameSetEdit::slotToolActivated( const KoDataToolInfo & info, const QString & command )
{
    KoDataTool* tool = info.createTool( frameSet()->kWordDocument() );
    if ( !tool )
    {
        kdWarning() << "Could not create Tool !" << endl;
        return;
    }

    kdDebug() << "KWTextFrameSetEdit::slotToolActivated command=" << command
              << " dataType=" << info.dataType() << endl;

    QString text;
    if ( textFrameSet()->hasSelection() )
        text = textFrameSet()->selectedText();
    else
        text = m_wordUnderCursor;

    // Preferred type is richtext
    QString mimetype = "application/x-qrichtext";
    QString datatype = "QTextString";
    // If unsupported, try text/plain
    if ( !info.mimeTypes().contains( mimetype ) )
    {
        mimetype = "text/plain";
        datatype = "QString";
    }
    // If unsupported (and if we have a single word indeed), try application/x-singleword
    if ( !info.mimeTypes().contains( mimetype ) && m_singleWord )
        mimetype = "application/x-singleword";

    kdDebug() << "Running tool with datatype=" << datatype << " mimetype=" << mimetype << endl;

    QString origText = text;
    if ( tool->run( command, &text, datatype, mimetype) )
    {
        kdDebug() << "Tool ran. Text is now " << text << endl;
        if ( origText != text )
        {
            if ( !textFrameSet()->hasSelection() )
                selectWordUnderCursor();
            // replace selection with 'text'
            textFrameSet()->kWordDocument()->addCommand(textFrameSet()->replaceSelectionCommand( cursor, text, QTextDocument::Standard, i18n("Replace word") ));
        }
    }

    delete tool;
}

void KWTextFrameSetEdit::changeCaseOfText(TypeOfCase _type)
{
    QString text;
    if ( textFrameSet()->hasSelection() )
        text = textFrameSet()->selectedText();
    if(!text.isEmpty()&& text.find(KWTextFrameSet::customItemChar())==-1)
    {
        textFrameSet()->kWordDocument()->addCommand(textFrameSet()->replaceSelectionCommand( cursor, textFrameSet()->textChangedCase(text,_type), QTextDocument::Standard, i18n("Change case") ));
    }
    else if(!text.isEmpty())
        textFrameSet()->changeCaseOfText(cursor,_type);
}

#include "kwtextframeset.moc"
