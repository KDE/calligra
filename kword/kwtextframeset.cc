/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Reginald Stadlbauer <reggie@kde.org>
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

#include <qrichtext_p.h>

#include "kwtextframeset.h"
#include "kwtextdocument.h"
#include "kwtableframeset.h"
#include "kwdoc.h"
#include "kwview.h"
#include "kwviewmode.h"
#include "kwcanvas.h"
#include "kwanchor.h"
#include "kwcommand.h"
#include "kwdrag.h"
#include "kwstyle.h"
#include <kotextformat.h>
#include <koparagcounter.h>
#include "contents.h"
#include "variable.h"
#include "variabledlgs.h"
#include "serialletter.h"
#include "autoformat.h"
#include <qclipboard.h>
#include <qdragobject.h>
#include <qtl.h>
#include <qprogressdialog.h>
#include <qpopupmenu.h>
#include <kapp.h>
#include <klocale.h>
#include <kconfig.h>
#include <koDataTool.h>
#include <kotextobject.h>
#include <kocommand.h>

#include <kdebug.h>
#include <assert.h>

//#define DEBUG_MARGINS
//#define DEBUG_FLOW
//#define DEBUG_FORMATS
//#define DEBUG_FORMAT_MORE
//#define DEBUG_VIEWAREA

//#define DEBUG_DTI
//#define DEBUG_ITD

KWTextFrameSet::KWTextFrameSet( KWDocument *_doc, const QString & name )
    : KWFrameSet( _doc )
{
    //kdDebug() << "KWTextFrameSet::KWTextFrameSet " << this << endl;
    if ( name.isEmpty() )
        m_name = _doc->generateFramesetName( i18n( "Text Frameset %1" ) );
    else
        m_name = name;
    setName( m_name.utf8() ); // store name in the QObject, for DCOP users
    m_currentViewMode = 0L;
    m_currentDrawnFrame = 0L;
    m_framesInPage.setAutoDelete( true );
    m_firstPage = 0;
    // Create the text document to set in the text object
    KWTextDocument* textdoc = new KWTextDocument(
        this, 0, new KoTextFormatCollection( _doc->defaultFont() ) );
    textdoc->setFlow( this );
    textdoc->setVerticalBreak( true );              // get QTextFlow methods to be called

    m_textobj = new KoTextObject( textdoc, m_doc->findStyle( "Standard" ),
                                  this, (m_name+"-textobj").utf8() );

    connect( m_textobj, SIGNAL( availableHeightNeeded() ),
             SLOT( slotAvailableHeightNeeded() ) );
    connect( m_textobj, SIGNAL( afterFormatting( int, QTextParag* ) ),
             SLOT( slotAfterFormatting( int, QTextParag* ) ) );
    connect( m_textobj, SIGNAL( newCommand( KCommand * ) ),
             SLOT( slotNewCommand( KCommand * ) ) );
    connect( m_textobj, SIGNAL( repaintChanged( KoTextObject* ) ),
             SLOT( slotRepaintChanged() ) );

    /* if ( QFile::exists( "bidi.txt" ) ) {
       QFile fl( "bidi.txt" );
       fl.open( IO_ReadOnly );
       QByteArray array = fl.readAll();
       QString text = QString::fromUtf8( array.data() );
       textdoc->setText( text, QString::null );
    }*/
}

KWFrameSetEdit * KWTextFrameSet::createFrameSetEdit( KWCanvas * canvas )
{
    return new KWTextFrameSetEdit( this, canvas );
}

KoTextDocument * KWTextFrameSet::textDocument() const
{
    return m_textobj->textDocument();
}

KWTextDocument * KWTextFrameSet::kwTextDocument() const
{
     return static_cast<KWTextDocument *>(m_textobj->textDocument());
}

void KWTextFrameSet::slotAvailableHeightNeeded()
{
    kdDebug() << "KWTextFrameSet::slotAvailableHeightNeeded " << getName() << endl;
    updateFrames();
}

KWFrame * KWTextFrameSet::documentToInternal( const KoPoint &dPoint, QPoint &iPoint, bool mouseSelection ) const
{
#ifdef DEBUG_DTI
    kdDebug() << "KWTextFrameSet::documentToInternal dPoint:" << dPoint.x() << "," << dPoint.y() << " mouseSelection=" << mouseSelection << endl;
#endif
    // Find the frame that contains dPoint. To go fast, we look them up by page number.
    int pageNum = static_cast<int>( dPoint.y() / m_doc->ptPaperHeight() );
    QListIterator<KWFrame> frameIt( framesInPage( pageNum ) );
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame *frame = frameIt.current();
        // Due to zooming problems (and to QRect's semantics), be tolerant for one more pixel
        //frameRect.rRight() += 1;
        //frameRect.rBottom() += 1;
        if ( frame->contains( dPoint ) )
        {
            iPoint.setX( m_doc->ptToLayoutUnit( dPoint.x() - frame->left() ) );
            iPoint.setY( m_doc->ptToLayoutUnit( dPoint.y() - frame->top() ) + frame->internalY() );
#ifdef DEBUG_DTI
            kdDebug() << "documentToInternal: returning " << iPoint.x() << "," << iPoint.y()
                      << " internalY=" << frame->internalY() << " because frame=" << frame
                      << " contains dPoint:" << dPoint.x() << "," << dPoint.y() << endl;
#endif
            return frame;
        }
        else if ( mouseSelection ) // try harder if true
        {
            KoRect openLeftRect( *frame );
            openLeftRect.setLeft( 0 );
#ifdef DEBUG_DTI
            kdDebug() << "documentToInternal: openLeftRect=" << DEBUGRECT( openLeftRect ) << endl;
#endif
            if ( openLeftRect.contains( dPoint ) )
            {
                // We are at the left of this frame (and not in any other frame of this frameset)
                iPoint.setX( 0 );
                iPoint.setY( m_doc->ptToLayoutUnit( dPoint.y() - frame->top() ) + frame->internalY() );
#ifdef DEBUG_DTI
                kdDebug() << "documentToInternal: returning " << iPoint.x() << "," << iPoint.y()
                          << " internalY=" << frame->internalY() << " because openLeftRect=" << DEBUGRECT(openLeftRect)
                          << " contains dPoint:" << dPoint.x() << "," << dPoint.y() << endl;
#endif
                return frame;
            }
            KoRect openTopRect( KoPoint( 0, 0 ), frame->bottomRight() );
#ifdef DEBUG_DTI
            kdDebug() << "documentToInternal: openTopRect=" << DEBUGRECT( openTopRect ) << endl;
#endif
            if ( openTopRect.contains( dPoint ) )
            {
                // We are at the top of this frame (...)
                iPoint.setX( m_doc->ptToLayoutUnit( dPoint.x() - frame->left() ) );
                iPoint.setY( frame->internalY() );
#ifdef DEBUG_DTI
                kdDebug() << "documentToInternal: returning " << iPoint.x() << "," << iPoint.y()
                          << " internalY=" << frame->internalY() << " because openTopRect=" << DEBUGRECT(openTopRect)
                          << " contains dPoint:" << dPoint.x() << "," << dPoint.y() << endl;
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
            iPoint.setX( m_doc->ptToLayoutUnit( frame->width() ) );
            iPoint.setY( frame->internalY() + m_doc->ptToLayoutUnit( frame->height() ) );
#ifdef DEBUG_DTI
            kdDebug() << "documentToInternal: returning " << iPoint.x() << "," << iPoint.y()
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
                KoRect openTopRect( *frame );
                openTopRect.setTop( 0 );
                if ( openTopRect.contains( dPoint ) ) // We are at the top of this frame
                    iPoint.setX( m_doc->ptToLayoutUnit( dPoint.x() - frame->left() ) );
                else
                    iPoint.setX( 0 ); // We are, hmm, on the left or right of the frames
                iPoint.setY( frame->internalY() );
#ifdef DEBUG_DTI
                kdDebug() << "documentToInternal: returning " << iPoint.x() << "," << iPoint.y()
                          << " because we are under all frames of page " << pageNum << endl;
#endif
                return frame;
            } // else there is a gap (no frames on that page, but on some other further down)
            // This case isn't handled (and should be VERY rare I think)
        }
    }

    iPoint = m_doc->ptToLayoutUnit( dPoint ); // bah
    return 0;
}

KWFrame * KWTextFrameSet::internalToDocumentWithHint( const QPoint &iPoint, KoPoint &dPoint, const KoPoint &hintDPoint ) const
{
#ifdef DEBUG_ITD
    kdDebug() << "KWTextFrameSet::internalToDocumentWithHint hintDPoint: " << hintDPoint.x() << "," << hintDPoint.y() << endl;
#endif
    KWFrame *lastFrame = 0L;
    QListIterator<KWFrame> frameIt( frameIterator() );
    for ( ; frameIt.current(); ++frameIt )
    {
        KWFrame *frame = frameIt.current();
        QRect r( 0, frame->internalY(), m_doc->ptToLayoutUnit( frame->width() ), m_doc->ptToLayoutUnit( frame->height() ) );
#ifdef DEBUG_ITD
        kdDebug() << "ITN: r=" << DEBUGRECT(r) << " iPoint=" << iPoint.x() << "," << iPoint.y() << endl;
#endif
        // r is the frame in qrt coords
        if ( r.contains( iPoint ) ) // both r and p are in layout units (aka internal)
        {
            double offsetX = frame->x();
            double offsetY = frame->y() - m_doc->layoutUnitToPt( frame->internalY() );
            dPoint.setX( m_doc->layoutUnitToPt( iPoint.x() ) + offsetX );
            dPoint.setY( m_doc->layoutUnitToPt( iPoint.y() ) + offsetY );
#ifdef DEBUG_ITD
            kdDebug() << "copy: " << frame->isCopy() << " hintDPoint.y()=" << hintDPoint.y() << " dPoint.y()=" << dPoint.y() << endl;
#endif
            // First test: No "hintDPoint" specified, go for the first match
            // Second test: hintDPoint specified, check if we are far enough
            if ( hintDPoint.isNull() || hintDPoint.y() <= dPoint.y() )
                return frame;
            // Remember that this frame matched, in case we find no further frame that matches
            lastFrame = frame;
        }
        else if ( lastFrame )
        {
            return lastFrame;
        }
    }

    // This happens when the parag is on a not-yet-created page (formatMore will notice afterwards)
    // So it doesn't matter much what happens here, we'll redo it anyway.
#ifdef DEBUG_ITD
    kdDebug(32002) << "KWTextFrameSet::internalToDocumentWithHint " << iPoint.x() << "," << iPoint.y()
                   << " not in any frame of " << (void*)this << endl;
#endif
    dPoint = m_doc->layoutUnitToPt( iPoint ); // bah
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
        return QPoint( 0, frame->internalY() + m_doc->ptToLayoutUnit( frame->height() ) );
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
    QListIterator<QTextCustomItem> cit( textDocument()->allCustomItems() );
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
    QTextCursor * cursor = edit ? static_cast<KWTextFrameSetEdit *>(edit)->cursor() : 0;

    QTextParag * lastFormatted = textDocument()->drawWYSIWYG(
        painter, r.x(), r.y(), r.width(), r.height(),
        cg, onlyChanged, drawCursor, cursor, resetChanged );

    // The last paragraph of this frame might have a bit in the next frame too.
    // In that case, and if we're only drawing changed paragraphs, (and resetting changed),
    // we have to set changed to true again, to draw the bottom of the parag in the next frame.
    if ( onlyChanged && resetChanged )
    {
        // Finding the "last parag of the frame" is a bit tricky.
        // It's usually the one before lastFormatted, except if it's actually lastParag :}  [see QTextDocument::draw]
        QTextParag * lastDrawn = lastFormatted->prev();
        if ( lastFormatted == textDocument()->lastParag() && ( !lastDrawn || m_doc->layoutUnitToPixelY( lastDrawn->rect().bottom() ) < r.bottom() ) )
            lastDrawn = lastFormatted;

        //kdDebug(32002) << "KWTextFrameSet::drawFrame drawn. onlyChanged=" << onlyChanged << " resetChanged=" << resetChanged << " lastDrawn=" << lastDrawn->paragId() << " lastDrawn's bottom:" << lastDrawn->rect().bottom() << " r.bottom=" << r.bottom() << endl;
        if ( lastDrawn && m_doc->layoutUnitToPixelY( lastDrawn->rect().bottom() ) > r.bottom() )
        {
            //kdDebug(32002) << "KWTextFrameSet::drawFrame setting lastDrawn " << lastDrawn->paragId() << " to changed" << endl;
            lastDrawn->setChanged( true );
        }
    }

    // NOTE: QTextView sets m_lastFormatted to lastFormatted here
    // But when scrolling up, this causes to reformat a lot of stuff for nothing.
    // And updateViewArea takes care of formatting things before we even arrive here.

    // Blank area under the very last paragraph - QRT draws it up to textdoc->height,
    // we have to draw it from there up to the bottom of the last frame.
    if ( lastFormatted == textDocument()->lastParag() && !onlyChanged )
    {
        // This is drawing code, so we convert everything to pixels
        int docHeight = m_doc->layoutUnitToPixelY( textDocument()->height() );
        QRect frameRect = m_doc->zoomRect( *frame );

        int totalHeight = m_doc->layoutUnitToPixelY( frames.last()->internalY() )
                          +  m_doc->zoomItY( frames.last()->height() );

        QRect blank( 0, docHeight, frameRect.width(), totalHeight + frameRect.height() - docHeight );
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
    // Cursor height, in pixels
    int cursorHeight = m_doc->layoutUnitToPixelY( topLeft.y(), cursor->parag()->lineHeightOfChar( cursor->index(), 0, &lineY ) );
    //kdDebug() << "KWTextFrameSet::drawCursor topLeft=" << topLeft.x() << "," << topLeft.y()
    //          << " x=" << cursor->x() << " y=" << lineY << endl;
    QPoint iPoint( topLeft.x() - cursor->totalOffsetX() + cursor->x(),
                   topLeft.y() - cursor->totalOffsetY() + lineY );
    //kdDebug() << "KWTextFrameSet::drawCursor iPoint=" << iPoint.x() << "," << iPoint.y()
    //          << "   cursorHeight=" << cursorHeight << endl;
    KoPoint dPoint;
    KoPoint hintDPoint = frame->topLeft();
    if ( internalToDocumentWithHint( iPoint, dPoint, hintDPoint ) )
    {
        QPoint vPoint = viewMode->normalToView( m_doc->zoomPoint( dPoint ) ); // from doc to view contents
        // from now on, iPoint will be in pixels
        iPoint = m_doc->layoutUnitToPixel( iPoint );
        //kdDebug() << "     iPoint in pixels : " << iPoint.x() << "," << iPoint.y() << endl;
        int xadj = cursor->parag()->at( cursor->index() )->pixelxadj;
        iPoint.rx() += xadj;
        vPoint.rx() += xadj;
        // very small clipping around the cursor
        QRect clip( vPoint.x() - 5, vPoint.y(), 10, cursorHeight );

        //kdDebug() << " clip(normal, before intersect)=" << DEBUGRECT(clip) << endl;

        clip &= normalFrameRect; // clip to frame
        // kdDebug() << "KWTextFrameSet::drawCursor normalFrameRect=" << DEBUGRECT(normalFrameRect)
        //          << " clip(normal, after intersect)=" << DEBUGRECT(clip) << endl;

        QPoint cPoint = viewMode->normalToView( clip.topLeft() );     // from normal to view contents
        clip.moveTopLeft( cPoint );

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

            textDocument()->drawParagWYSIWYG( p, cursor->parag(),
                                iPoint.x() - 5, iPoint.y(), clip.width(), clip.height(),
                                pix, cg, cursorVisible, cursor );
            p->restore();
            cursor->parag()->setChanged( wasChanged );      // Maybe we have more changes to draw!

            //XIM Position
            QPoint ximPoint = vPoint;
            int line;
            cursor->parag()->lineStartOfChar( cursor->index(), 0, &line );
            canvas->setXimPosition( ximPoint.x(), ximPoint.y(),
                                    0, cursorHeight - cursor->parag()->lineSpacing( line ) );
        }
    }
    m_currentDrawnCanvas = 0L;
    m_currentDrawnFrame = 0L;
}

void KWTextFrameSet::layout()
{
    invalidate();
    // Get the thing going though, repainting doesn't call formatMore
    m_textobj->formatMore();
}

void KWTextFrameSet::invalidate()
{
    //kdDebug() << "KWTextFrameSet::invalidate " << getName() << endl;
    m_textobj->setLastFormattedParag( textDocument()->firstParag() );
    textDocument()->invalidate(); // lazy layout, real update follows upon next repaint
}

void KWTextFrameSet::slotRepaintChanged()
{
    emit repaintChanged( this );
}

int KWTextFrameSet::paragraphs()
{
    int paragraphs = 0;
    QTextParag * parag = textDocument()->firstParag();
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

    QTextParag * parag = textDocument()->firstParag();
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
    KoPoint pt;
    // The +h in here is a little hack, for the case where this line is going to
    // be moved down by adjustFlow. We anticipate, and look at the bottom of the
    // line rather than the top of it, in order to find the bottom frame (the one
    // in which we'll end up). See TODO file for a real solution.
    KWFrame * frame = internalToDocument( QPoint(0, yp+h), pt );
    if (!frame)
    {
#ifdef DEBUG_MARGINS
        kdDebug(32002) << "  getMargins: internalToDocument returned frame=0L for yp+h=" << yp+h << " ->aborting with 0 margins" << endl;
#endif
        // frame == 0 happens when the parag is on a not-yet-created page (formatMore will notice afterwards)
        // Abort then, no need to return precise values
        if ( marginLeft )
            *marginLeft = 0;
        if ( marginRight )
            *marginRight = 0;
        return;
    }
#ifdef DEBUG_MARGINS
    else
        kdDebugBody(32002) << "  getMargins: internalToDocument returned frame=" << DEBUGRECT( *frame )
                           << " and pt=" << pt.x() << "," << pt.y() << endl;
#endif
    // Everything from there is in layout units
    // Note: it is very important that this method works in internal coordinates.
    // Otherwise, parags broken at the line-level (e.g. between two columns) are seen
    // as still in one piece, and we miss the frames in the 2nd column.
    int left = 0;
    int from = left;
    int to = m_doc->ptToLayoutUnit( frame->width() );
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
            KoRect rectOnTop = (*fIt).intersection;
#ifdef DEBUG_MARGINS
            kdDebugBody(32002) << "   getMargins found rect-on-top at (normal coords) " << DEBUGRECT(rectOnTop) << endl;
#endif
            QPoint iTop, iBottom; // top and bottom of intersection in internal coordinates
            if ( documentToInternal( rectOnTop.topLeft(), iTop ) &&
                 iTop.y() <= yp + h && // optimization
                 documentToInternal( rectOnTop.bottomRight(), iBottom ) )
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
                    if ( to - from < m_doc->ptToLayoutUnit( 15 ) + paragLeftMargin )
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
    if ( from == to ) {
        from = 0;
        to = m_doc->ptToLayoutUnit( frame->width() );
    }

    if ( marginLeft )
        *marginLeft = from;
    if ( marginRight )
    {
#ifdef DEBUG_MARGINS
        kdDebug(32002) << "    getMargins " << getName()
                       << " textdoc's width=" << textDocument()->width()
                       << " to=" << to << endl;
#endif
        *marginRight = textDocument()->width() - to;
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
                        ls->y = breakEnd - parag->rect().y();
#ifdef DEBUG_FLOW
                        kdDebug(32002) << "checkVerticalBreak parag " << parag->paragId()
                                       << " BREAKING at line " << line << " dy=" << dy << "  Setting ls->y to " << ls->y << ", y=" << breakEnd << endl;
#endif
                    }
                }
                else
		{
                    ls->y += dy;
#ifdef DEBUG_FLOW
		    if ( dy )
                        kdDebug(32002) << "                   moving down to position ls->y=" << ls->y << endl;
#endif
		}
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
        int frameHeight = kWordDocument()->ptToLayoutUnit( frameIt.current()->height() );
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
            KoRect rectOnTop = (*fIt).intersection;
            QPoint iTop, iBottom; // top and bottom in internal coordinates
            if ( documentToInternal( rectOnTop.topLeft(), iTop ) &&
                 iTop.y() <= yp + h &&
                 documentToInternal( rectOnTop.bottomLeft(), iBottom ) &&
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
        parag->setWidth( textDocument()->width() - 1 );
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

KWTextFrameSet::~KWTextFrameSet()
{
    textDocument()->takeFlow();
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
        width = QMAX( width, m_doc->ptToLayoutUnit( frameIt.current()->width() ) );

        FrameStruct str;
        str.frame = frameIt.current();
        sortedFrames.append( str );
    }
    if ( width != textDocument()->width() )
    {
        //kdDebug(32002) << "KWTextFrameSet::updateFrames setWidth " << width << endl;
        textDocument()->setMinimumWidth( -1, 0 );
        textDocument()->setWidth( width );
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
    int availHeight = 0;
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

        // Update availHeight with the internal height of this frame - unless it's a copy
        if ( ! ( frame->isCopy() && it != sortedFrames.begin() ) )
        {
            lastRealFrameHeight = m_doc->ptToLayoutUnit( frame->height() );
            availHeight += lastRealFrameHeight;
        }
    }

    m_textobj->setAvailableHeight( availHeight );
    //kdDebugBody(32002) << this << " KWTextFrameSet::updateFrames availHeight=" << availHeight << endl;
    frames.setAutoDelete( true );

    KWFrameSet::updateFrames();
}

int KWTextFrameSet::availableHeight() const
{
    return m_textobj->availableHeight();
}

const QList<KWFrame> & KWTextFrameSet::framesInPage( int pageNum ) const
{
    if ( pageNum < m_firstPage || pageNum >= (int)m_framesInPage.size() + m_firstPage )
    {
#ifdef DEBUG_DTI
        kdWarning() << getName() << " framesInPage called for pageNum=" << pageNum << ". "
                    << " Min value: " << m_firstPage
                    << " Max value: " << m_framesInPage.size() + m_firstPage - 1 << endl;
#endif
        return m_emptyList; // QList<KWFrame>() doesn't work, it's a temporary
    }
    return * m_framesInPage[pageNum - m_firstPage];
}

KWFrame * KWTextFrameSet::internalToDocument( const QPoint &iPoint, KoPoint &dPoint ) const
{
#ifdef DEBUG_ITD
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
#ifdef DEBUG_ITD
        kdDebug() << "ITN: begin. mid=" << mid << endl;
#endif
        ASSERT( m_framesInPage[mid] ); // We have no null items
        if ( m_framesInPage[mid]->isEmpty() )
            res = -1;
        else
        {
            KWFrame * frame = m_framesInPage[mid]->first();
            internalY = frame->internalY();
#ifdef DEBUG_ITD
            kdDebug() << "ITN: iPoint.y=" << iPoint.y() << " internalY=" << internalY << endl;
#endif
            res = iPoint.y() - internalY;
#ifdef DEBUG_ITD
            kdDebug() << "ITN: res=" << res << endl;
#endif
            // Anything between this internalY (top) and internalY+height (bottom) is fine
            // (Using the next page's first frame's internalY only works if there is a frame on the next page)
            if ( res >= 0 )
            {
                int height = m_doc->ptToLayoutUnit( frame->height() );
#ifdef DEBUG_ITD
                kdDebug() << "ITN: height=" << height << endl;
#endif
                if ( iPoint.y() < internalY + height )
                {
#ifdef DEBUG_ITD
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
#ifdef DEBUG_ITD
        kdDebug() << "ITN: End of loop. n1=" << n1 << " n2=" << n2 << endl;
#endif
    }
    if ( !found )
    {
        // Not found (n2 < n1)
        // We might have missed the frame because n2 has many frames
        // (and we only looked at the first one).
        mid = n2;
#ifdef DEBUG_ITD
        kdDebug() << "ITN: Setting mid to n2=" << mid << endl;
#endif
        if ( mid < 0 )
        {
//#ifdef DEBUG_ITD
            kdDebug(32002) << "KWTextFrameSet::internalToDocument " << iPoint.x() << "," << iPoint.y()
                           << " before any frame of " << (void*)this << endl;
//#endif
            dPoint = m_doc->layoutUnitToPt( iPoint ); // "bah", I said above :)
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
        QRect r( 0, frame->internalY(), m_doc->ptToLayoutUnit( frame->width() ), m_doc->ptToLayoutUnit( frame->height() ) );
        // r is the frame in qrt coords
        if ( r.contains( iPoint ) ) // both r and p are in "qrt coordinates"
        {
            dPoint.setX( m_doc->layoutUnitToPt( iPoint.x() ) + frame->left() );
            dPoint.setY( m_doc->layoutUnitToPt( iPoint.y() - frame->internalY() ) + frame->top() );
            return frame;
        }
    }
#ifdef DEBUG_ITD
    kdDebug(32002) << "KWTextFrameSet::internalToDocument " << iPoint.x() << "," << iPoint.y()
                   << " not in any frame of " << (void*)this << " (looked on page " << result << ")" << endl;
#endif
    dPoint = m_doc->layoutUnitToPt( iPoint ); // bah again
    return 0L;
}

#ifndef NDEBUG
void KWTextFrameSet::printDebug()
{
    KWFrameSet::printDebug();
    if ( !isDeleted() )
    {
        kdDebug() << "QTextDocument width = " << textDocument()->width() << endl;
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
        framesetElem.setAttribute( "grpMgr", grpMgr->getName() );

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

    textDocument()->clear(false); // Get rid of dummy paragraph (and more if any)
    KWTextParag *lastParagraph = 0L;

    // <PARAGRAPH>
    QDomElement paragraph = attributes.firstChild().toElement();
    for ( ; !paragraph.isNull() ; paragraph = paragraph.nextSibling().toElement() )
    {
        if ( paragraph.tagName() == "PARAGRAPH" )
        {
            KWTextParag *parag = new KWTextParag( textDocument(), lastParagraph );
            parag->load( paragraph );
            if ( !lastParagraph )        // First parag
                textDocument()->setFirstParag( parag );
            lastParagraph = parag;
            m_doc->progressItemLoaded();
        }
    }

    if ( !lastParagraph )                // We created no paragraph
    {
        // Create an empty one, then. See KWTextDocument ctor.
        textDocument()->clear( true );
        static_cast<KWTextParag *>( textDocument()->firstParag() )->setStyle( m_doc->findStyle( "Standard" ) );
    }
    else
        textDocument()->setLastParag( lastParagraph );

    m_textobj->setLastFormattedParag( textDocument()->firstParag() );
    //kdDebug(32001) << "KWTextFrameSet::load done" << endl;
}

void KWTextFrameSet::zoom( bool forPrint )
{
#ifdef ZOOM_FONTS // old stuff (i.e. this is all commented out)
    if ( !m_origFontSizes.isEmpty() )
        unzoom();
    QTextFormatCollection * coll = textDocument()->formatCollection();
    // This is because we are setting pt sizes (so Qt applies x11AppDpiY already)
    double factor = kWordDocument()->zoomedResolutionY() *
                    ( forPrint ? 1.0 : 72.0 / QPaintDevice::x11AppDpiY() );
    //kdDebugBody(32002) << "KWTextFrameSet::zoom factor=" << factor << endl;

#ifdef DEBUG_FORMATS
    kdDebug(32002) << this << " KWTextFrameSet::zoom " << factor << " coll=" << coll << " " << coll->dict().count() << " items " << endl;
    kdDebug(32002) << this << " firstparag:" << textDocument()->firstParag()
                   << " format:" << textDocument()->firstParag()->paragFormat()
                   << " first-char's format:" << textDocument()->firstParag()->at(0)->format()
                   << endl;
#endif
    QDictIterator<QTextFormat> it( coll->dict() );
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
    QListIterator<QTextCustomItem> cit( textDocument()->allCustomItems() );
    for ( ; cit.current() ; ++cit )
        static_cast<KoTextCustomItem *>( cit.current() )->resize();

    // Mark all paragraphs as changed !
    for ( KWTextParag * s = static_cast<KWTextParag *>( textDocument()->firstParag() ) ; s ; s = static_cast<KWTextParag *>( s->next() ) )
    {
        s->setTabList( s->tabList() ); // to recalculate with the new zoom
        s->setChanged( TRUE );
        s->invalidate( 0 );
        if ( s->counter() )
            s->counter()->invalidate();
    }
    m_lastFormatted = textDocument()->firstParag();
    m_availableHeight = -1; // to be recalculated
#endif
    KWFrameSet::zoom( forPrint );
}

void KWTextFrameSet::hideCustomItems(bool _hide)
{
    QListIterator<QTextCustomItem> cit( textDocument()->allCustomItems() );
    for ( ; cit.current() ; ++cit )
        static_cast<KWAnchor *>( cit.current() )->frameSet()->setVisible( _hide );
}

void KWTextFrameSet::unzoom()
{
#ifdef ZOOM_FONTS // not defined
    QTextFormatCollection * coll = textDocument()->formatCollection();

    QDictIterator<QTextFormat> it( coll->dict() );
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
    }
    KWTextFormat * format = static_cast<KWTextFormat *>(coll->defaultFormat());
    int * oldSize = m_origFontSizes.find( format );
    if ( oldSize )
        format->setPointSizeFloat( *oldSize );

    m_origFontSizes.clear();
#endif
}

void KWTextFrameSet::preparePrinting( QPainter *painter, QProgressDialog *progress, int &processedParags )
{
    //textDocument()->doLayout( painter, textDocument()->width() );
    textDocument()->setWithoutDoubleBuffer( painter != 0 );

    textDocument()->formatCollection()->setPainter( painter );
    QTextParag *parag = textDocument()->firstParag();
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

void KWTextFrameSet::addTextFramesets( QList<KWTextFrameSet> & lst )
{
    lst.append(this);
}

void KWTextFrameSet::slotNewCommand( KCommand *cmd )
{
    m_doc->addCommand( cmd );
}

void KWTextFrameSet::ensureFormatted( QTextParag * parag )
{
    if (!isVisible())
        return;
    m_textobj->ensureFormatted( parag );
}

void KWTextFrameSet::slotAfterFormatting( int bottom, QTextParag *lastFormatted )
{
    int availHeight = availableHeight();
    if ( ( bottom > availHeight ) ||   // this parag is already off page
         ( lastFormatted && bottom + lastFormatted->rect().height() > availHeight ) ) // or next parag will be off page
    {
#ifdef DEBUG_FORMAT_MORE
        if(lastFormatted)
            kdDebug(32002) << "formatMore We need more space in " << getName()
                           << " bottom=" << bottom + lastFormatted->rect().height()
                           << " availHeight=" << availHeight << endl;
        else
            kdDebug(32002) << "formatMore We need more space in " << getName()
                           << " bottom2=" << bottom << " availHeight=" << availHeight << endl;
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
            int difference = ( bottom + 2 ) - availHeight; // in layout units
#ifdef DEBUG_FORMAT_MORE
            kdDebug(32002) << "AutoExtendFrame bottom=" << bottom << " availHeight=" << availHeight
                           << " => difference = " << difference << endl;
#endif
            if( lastFormatted && bottom + lastFormatted->rect().height() > availHeight ) {
#ifdef DEBUG_FORMAT_MORE
                kdDebug(32002) << " next will be off -> adding " << lastFormatted->rect().height() << endl;
#endif
                difference += lastFormatted->rect().height();
            }

            if(difference > 0) {
                // There's no point in resizing a copy, so go back to the last non-copy frame
                KWFrame *theFrame = settingsFrame( frames.last() );

                if ( theFrame->getFrameSet()->isAFooter() )
                {
                    double maxFooterSize = footerHeaderSizeMax(  theFrame );
                    wantedPosition = theFrame->top() - m_doc->layoutUnitToPt( difference );
                    if ( wantedPosition != theFrame->top() && QMAX(theFrame->bottom()-maxFooterSize,wantedPosition)==wantedPosition )
                    {
                        theFrame->setTop( wantedPosition);
                        frameResized( theFrame );
                    }
                    break;
                }

                wantedPosition = m_doc->layoutUnitToPt( difference ) + theFrame->bottom();
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
                    m_textobj->setLastFormattedParag( 0 );
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

            if ( lastFormatted )
            {
                // Reformat the last paragraph. If it's over the two pages, it will need
                // the new page (e.g. for inline frames that need itn to work)
                if ( lastFormatted->prev() )
                {
                    m_textobj->setLastFormattedParag( lastFormatted->prev() );
                    lastFormatted->invalidate( 0 );
                }

                //interval = 0;
                // not good enough, we need to keep formatting right now
                m_textobj->formatMore(); // that, or a goto ?
                return;
            }
            QTimer::singleShot( 0, m_doc, SLOT( slotRepaintAllViews() ) );
        } break;
        case KWFrame::Ignore:
#ifdef DEBUG_FORMAT_MORE
            kdDebug(32002) << "formatMore frame behaviour is Ignore" << endl;
#endif
            m_textobj->setLastFormattedParag( 0 );
            break;
        }
    }
    // Handle the case where the last frame is empty, so we may want to
    // remove the last page.
    else if ( frames.count() > 1 && !lastFormatted && !isAHeader() && !isAFooter()
              && bottom < availHeight - m_doc->ptToLayoutUnit( frames.last()->height() ) )
    {
#ifdef DEBUG_FORMAT_MORE
        kdDebug(32002) << "formatMore too much space (" << bottom << ", " << availHeight << ") , trying to remove last frame" << endl;
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
    else if ( !lastFormatted && bottom + 2 < availHeight &&
              frames.last()->getFrameBehaviour() == KWFrame::AutoExtendFrame )
    {
        // The + 2 here leaves 2 pixels below the last line. Without it we hit
        // the "break at end of frame" case in adjustFlow (!!).
        int difference = availHeight - ( bottom + 2 );
        kdDebug(32002) << "formatMore less text than space (AutoExtendFrame) difference=" << difference << endl;
        // There's no point in resizing a copy, so go back to the last non-copy frame
        KWFrame *theFrame = settingsFrame( frames.last() );
        if ( theFrame->getFrameSet()->isAFooter() )
        {
            double wantedPosition = theFrame->top() + m_doc->layoutUnitToPt( difference );
            if ( wantedPosition != theFrame->top() )
            {
                kdDebug() << "top= " << theFrame->top() << " setTop " << wantedPosition << endl;
                theFrame->setTop( wantedPosition );
                frameResized( theFrame );
            }
        }
        else
        {
            double wantedPosition = theFrame->bottom() - m_doc->layoutUnitToPt( difference );
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
    QTextParag * lastParag = textDocument()->lastParag();
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

void KWTextFrameSet::updateViewArea( QWidget * w, const QPoint & nPointBottom )
{
    if (!isVisible())
        return;
    int ah = availableHeight(); // make sure that it's not -1
#ifdef DEBUG_VIEWAREA
    kdDebug(32002) << "KWTextFrameSet::updateViewArea " << (void*)w << " " << w->name()
                     << " nPointBottom=" << nPointBottom.x() << "," << nPointBottom.y()
                     << " availHeight=" << availHeight << " textDocument()->height()=" << textDocument()->height() << endl;
#endif

    // Find last page that is visible
    int maxPage = ( nPointBottom.y() + m_doc->paperHeight() /*equiv. to ceil()*/ ) / m_doc->paperHeight();
    int maxY = 0;
    if ( maxPage < m_firstPage || maxPage >= (int)m_framesInPage.size() + m_firstPage )
        maxY = ah;
    else
    {
        // Find frames on that page, and keep the max bottom, in internal coordinates
        QListIterator<KWFrame> frameIt( framesInPage( maxPage ) );
        for ( ; frameIt.current(); ++frameIt )
        {
            maxY = QMAX( maxY, frameIt.current()->internalY()
                         + m_doc->ptToLayoutUnit( frameIt.current()->height() ) );
        }
    }
#ifdef DEBUG_VIEWAREA
    kdDebug(32002) << "KWTextFrameSet (" << getName() << ")::updateViewArea maxY now " << maxY << endl;
#endif
    m_textobj->setViewArea( w, maxY );
    m_textobj->formatMore();
}

KCommand * KWTextFrameSet::setPageBreakingCommand( QTextCursor * cursor, int pageBreaking )
{
    if ( !textDocument()->hasSelection( QTextDocument::Standard ) &&
         static_cast<KWTextParag *>(cursor->parag())->pageBreaking() == pageBreaking )
        return 0L; // No change needed.

    m_textobj->emitHideCursor();

    m_textobj->storeParagUndoRedoInfo( cursor );

    if ( !textDocument()->hasSelection( QTextDocument::Standard ) ) {
        static_cast<KWTextParag *>(cursor->parag())->setPageBreaking( pageBreaking );
        m_textobj->setLastFormattedParag( cursor->parag() );
    }
    else
    {
        QTextParag *start = textDocument()->selectionStart( QTextDocument::Standard );
        QTextParag *end = textDocument()->selectionEnd( QTextDocument::Standard );
        m_textobj->setLastFormattedParag( start );
        for ( ; start && start != end->next() ; start = start->next() )
            static_cast<KWTextParag *>(start)->setPageBreaking( pageBreaking );
    }

    m_textobj->formatMore();
    emit repaintChanged( this );
    KoTextObject::UndoRedoInfo & undoRedoInfo = m_textobj->undoRedoInfoStruct();
    undoRedoInfo.newParagLayout.pageBreaking = pageBreaking;
    KoTextParagCommand *cmd = new KoTextParagCommand(
        textDocument(), undoRedoInfo.id, undoRedoInfo.eid,
        undoRedoInfo.oldParagLayouts, undoRedoInfo.newParagLayout,
        KoParagLayout::PageBreaking );
    textDocument()->addCommand( cmd );
    undoRedoInfo.clear();
    m_textobj->emitShowCursor();
    m_textobj->emitUpdateUI( true );
    m_textobj->emitEnsureCursorVisible();
    // ## find a better name for the command
    return new KoTextCommand( m_textobj, /*cmd, */i18n("Change Paragraph Attribute") );
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
                if(text.at(i)!=' ')
                {
                    if((i==0 &&text.at(i)!=' ') ||(text.at(i)!=' '&& text.at(QMAX(i-1,0))==' ')||(text.at(i)!=' '&& text.at(QMAX(i-1,0))=='\n'))
                        text=text.replace(i, 1, text.at(i).upper() );
                }
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

    QTextCursor start = textDocument()->selectionStartCursor( QTextDocument::Standard );
    QTextCursor end = textDocument()->selectionEndCursor( QTextDocument::Standard );

    int posStart=start.index();
    int posEnd=start.index();
    QTextCursor c1( textDocument() );
    QTextCursor c2( textDocument() );
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
                textDocument()->setSelectionStart( QTextDocument::Temp, &c1 );
                textDocument()->setSelectionEnd( QTextDocument::Temp, &c2 );
                macroCmd->addCommand(m_textobj->replaceSelectionCommand(
                    cursor, textChangedCase(repl,_type),
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

        textDocument()->setSelectionStart( QTextDocument::Temp, &c1 );
        textDocument()->setSelectionEnd( QTextDocument::Temp, &c2 );
        repl=text.mid(posStart-start.index(),end.index()-posStart);
        macroCmd->addCommand(m_textobj->replaceSelectionCommand(
            cursor, textChangedCase(repl,_type),
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

                textDocument()->setSelectionStart( QTextDocument::Temp, &c1 );
                textDocument()->setSelectionEnd( QTextDocument::Temp, &c2 );
                repl=text.mid(posStart-start.index(),posEnd-posStart);
                macroCmd->addCommand(m_textobj->replaceSelectionCommand(
                    cursor, textChangedCase(repl,_type) ,
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

        textDocument()->setSelectionStart( QTextDocument::Temp, &c1 );
        textDocument()->setSelectionEnd( QTextDocument::Temp, &c2 );
        repl=text.mid(posStart-start.index(),end.index()-posStart);
        macroCmd->addCommand(m_textobj->replaceSelectionCommand(
            cursor, textChangedCase(repl,_type),
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

                    textDocument()->setSelectionStart( QTextDocument::Temp, &c1 );
                    textDocument()->setSelectionEnd( QTextDocument::Temp, &c2 );
                    repl=text.mid(posStart,posEnd-posStart);
                    macroCmd->addCommand(m_textobj->replaceSelectionCommand(
                        cursor, textChangedCase(repl,_type),
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

            textDocument()->setSelectionStart( QTextDocument::Temp, &c1 );
            textDocument()->setSelectionEnd( QTextDocument::Temp, &c2 );
            repl=text.mid(posStart,text.length()-posStart);
            macroCmd->addCommand(m_textobj->replaceSelectionCommand(
                cursor, textChangedCase(repl,_type),
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

                textDocument()->setSelectionStart( QTextDocument::Temp, &c1 );
                textDocument()->setSelectionEnd( QTextDocument::Temp, &c2 );
                repl=text.mid(posStart,posEnd-posStart);
                macroCmd->addCommand(m_textobj->replaceSelectionCommand(
                    cursor, textChangedCase(repl,_type),
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

        textDocument()->setSelectionStart( QTextDocument::Temp, &c1 );
        textDocument()->setSelectionEnd( QTextDocument::Temp, &c2 );
        repl=text.mid(posStart,end.index()-posStart);
        macroCmd->addCommand(m_textobj->replaceSelectionCommand(
            cursor, textChangedCase(repl,_type) ,
            QTextDocument::Temp, "" ));

    }
    m_doc->addCommand( macroCmd);
}

KCommand * KWTextFrameSet::pasteKWord( QTextCursor * cursor, const QCString & data, bool removeSelected )
{
    // Having data as a QCString instead of a QByteArray seems to fix the trailing 0 problem
    // I tried using QDomDocument::setContent( QByteArray ) but that leads to parse error at the end

    //kdDebug(32001) << "KWTextFrameSet::pasteKWord" << endl;
    KMacroCommand * macroCmd = new KMacroCommand( i18n("Paste Text") );
    if ( removeSelected && textDocument()->hasSelection( QTextDocument::Standard ) )
        macroCmd->addCommand( m_textobj->removeSelectedTextCommand( cursor, QTextDocument::Standard ) );
    m_textobj->emitHideCursor();
    // correct but useless due to unzoom/zoom
    // (which invalidates everything and sets lastformatted to firstparag)
    //setLastFormattedParag( cursor->parag()->prev() ?
    //                       cursor->parag()->prev() : cursor->parag() );

    // We have our own command for this.
    // Using insert() wouldn't help storing the parag stuff for redo
    KWPasteTextCommand * cmd = new KWPasteTextCommand( textDocument(), cursor->parag()->paragId(), cursor->index(), data );
    textDocument()->addCommand( cmd );

    macroCmd->addCommand( new KoTextCommand( m_textobj, /*cmd, */QString::null ) );

    *cursor = *( cmd->execute( cursor ) );

    (void) availableHeight(); // calculate it again (set to -1 due to unzoom/zoom)

    m_textobj->formatMore();
    emit repaintChanged( this );
    m_textobj->emitEnsureCursorVisible();
    m_textobj->emitUpdateUI( true );
    m_textobj->emitShowCursor();
    m_textobj->selectionChangedNotify();
    return macroCmd;
}

void KWTextFrameSet::insertTOC( QTextCursor * cursor )
{
    m_textobj->emitHideCursor();
    KMacroCommand * macroCmd = new KMacroCommand( i18n("Insert Table Of Contents") );

    // Remove old TOC

    QTextCursor *cur= KWInsertTOCCommand::removeTOC( this, cursor, macroCmd );

    // Insert new TOC

    QTextCommand * cmd = new KWInsertTOCCommand( this,cur ? cur->parag(): cursor->parag() );
    textDocument()->addCommand( cmd );
    macroCmd->addCommand( new KoTextCommand( m_textobj, QString::null ) );
    *cursor = *( cmd->execute( cursor ) );

    m_textobj->setLastFormattedParag( textDocument()->firstParag() );
    m_textobj->formatMore();
    emit repaintChanged( this );
    m_textobj->emitEnsureCursorVisible();
    m_textobj->emitUpdateUI( true );
    m_textobj->emitShowCursor();

    m_doc->addCommand( macroCmd );
}

void KWTextFrameSet::insertFrameBreak( QTextCursor *cursor )
{
    clearUndoRedoInfo();
    KMacroCommand* macroCmd = new KMacroCommand( i18n( "Insert Break After Paragraph" ) );
    // Ensure "Frame break" is at beginning of paragraph -> create new parag if necessary
    if ( cursor->index() > 0 )
        macroCmd->addCommand( m_textobj->insertParagraphCommand( cursor ) );

    KWTextParag *parag = static_cast<KWTextParag *>( cursor->parag() );
    macroCmd->addCommand( setPageBreakingCommand( cursor, parag->pageBreaking() | KoParagLayout::HardFrameBreakAfter ) );

    if ( parag->next() == 0 )
        macroCmd->addCommand( m_textobj->insertParagraphCommand( cursor ) );

    m_doc->addCommand( macroCmd );

    m_textobj->setLastFormattedParag( parag );
    m_textobj->formatMore();
    emit repaintChanged( this );
    m_textobj->emitEnsureCursorVisible();
    m_textobj->emitUpdateUI( true );
    m_textobj->emitShowCursor();
}

QRect KWTextFrameSet::paragRect( QTextParag * parag ) const
{
    // ## Warning. Imagine a paragraph cut in two pieces (at the line-level),
    // between two columns. A single rect in internal coords, but two rects in
    // normal coords. QRect( topLeft, bottomRight ) is just plain wrong.
    // Currently this method is only used for "ensure visible" so that's fine, but
    // we shouldn't use it for more precise stuff.
    KoPoint p;
    (void)internalToDocument( parag->rect().topLeft(), p );
    QPoint topLeft = m_doc->zoomPoint( p );
    (void)internalToDocument( parag->rect().bottomRight(), p );
    QPoint bottomRight = m_doc->zoomPoint( p );
    return QRect( topLeft, bottomRight );
}

void KWTextFrameSet::findPosition( const KoPoint &dPoint, QTextParag * & parag, int & index )
{
    QTextCursor cursor( textDocument() );

    QPoint iPoint;
    if ( documentToInternal( dPoint, iPoint ) )
    {
        cursor.place( iPoint, textDocument()->firstParag() );
        parag = cursor.parag();
        index = cursor.index();
    }
    else
    {
        // Not found, maybe under everything ?
        parag = textDocument()->lastParag();
        if ( parag )
            index = parag->length() - 1;
    }
}

KCommand * KWTextFrameSet::deleteAnchoredFrame( KWAnchor * anchor )
{
    kdDebug() << "KWTextFrameSet::deleteAnchoredFrame anchor->index=" << anchor->index() << endl;
    ASSERT( anchor );
    QTextCursor c( textDocument() );
    c.setParag( anchor->paragraph() );
    c.setIndex( anchor->index() );

    textDocument()->setSelectionStart( QTextDocument::Temp, &c );
    c.setIndex( anchor->index() + 1 );
    textDocument()->setSelectionEnd( QTextDocument::Temp, &c );
    KCommand *cmd = m_textobj->removeSelectedTextCommand( &c, QTextDocument::Temp );

    m_doc->repaintAllViews();
    return cmd;
}

bool KWTextFrameSet::hasSelection() const
{
    return m_textobj->hasSelection();
}

QString KWTextFrameSet::selectedText() const
{
    return m_textobj->selectedText();
}

void KWTextFrameSet::highlightPortion( QTextParag * parag, int index, int length, KWCanvas * canvas )
{
    m_textobj->highlightPortion( parag, index, length );
    QRect expose = canvas->viewMode()->normalToView( paragRect( parag ) );
    canvas->ensureVisible( (expose.left()+expose.right()) / 2,  // point = center of the rect
                           (expose.top()+expose.bottom()) / 2,
                           (expose.right()-expose.left()) / 2,  // margin = half-width of the rect
                           (expose.bottom()-expose.top()) / 2);
}

void KWTextFrameSet::removeHighlight()
{
    m_textobj->removeHighlight();
}

void KWTextFrameSet::clearUndoRedoInfo()
{
    m_textobj->clearUndoRedoInfo();
}

void KWTextFrameSet::applyStyleChange( KoStyle * changedStyle, int paragLayoutChanged, int formatChanged )
{
    m_textobj->applyStyleChange( changedStyle, paragLayoutChanged, formatChanged );
}

void KWTextFrameSet::showPopup( KWFrame *frame, KWFrameSetEdit *edit, KWView *view, const QPoint &point )
{
    KWTextFrameSetEdit * textedit = dynamic_cast<KWTextFrameSetEdit *>(edit);
    ASSERT( textedit ); // is it correct that this is always set ?
    if (textedit)
        textedit->showPopup( frame, view, point );
    else
    {
        QPopupMenu * popup = view->popupMenu("text_popup");
        ASSERT(popup);
        if (popup)
            popup->popup( point );
    }
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
    : KoTextView( fs->textObject() ), KWFrameSetEdit( fs, canvas )
{
    //kdDebug(32001) << "KWTextFrameSetEdit::KWTextFrameSetEdit " << fs->getName() << endl;
    KoTextView::setReadWrite( fs->kWordDocument()->isReadWrite() );
    KoTextObject* textobj = fs->textObject();
    connect( textobj, SIGNAL( selectionChanged(bool) ), canvas, SIGNAL( selectionChanged(bool) ) );
    connect( fs, SIGNAL( frameDeleted(KWFrame *) ), this, SLOT( slotFrameDeleted(KWFrame *) ) );
    connect( textView(), SIGNAL( cut() ), SLOT( cut() ) );
    connect( textView(), SIGNAL( copy() ), SLOT( copy() ) );
    connect( textView(), SIGNAL( paste() ), SLOT( paste() ) );
    updateUI( true, true );

    m_actionList.setAutoDelete( true );
}

KWTextFrameSetEdit::~KWTextFrameSetEdit()
{
    //kdDebug(32001) << "KWTextFrameSetEdit::~KWTextFrameSetEdit" << endl;
}

void KWTextFrameSetEdit::terminate()
{
    disconnect( textView()->textObject(), SIGNAL( selectionChanged(bool) ), m_canvas, SIGNAL( selectionChanged(bool) ) );
    textView()->terminate();
}

void KWTextFrameSetEdit::slotFrameDeleted( KWFrame *frm )
{
    if ( m_currentFrame == frm )
        m_currentFrame = 0L;
}

void KWTextFrameSetEdit::paste()
{
    QMimeSource *data = QApplication::clipboard()->data();
    if ( data->provides( KWTextDrag::selectionMimeType() ) )
    {
        QByteArray arr = data->encodedData( KWTextDrag::selectionMimeType() );
        if ( arr.size() )
            frameSet()->kWordDocument()->addCommand(textFrameSet()->pasteKWord( cursor(), QCString( arr ), true ));
    }
    else
    {
        // Note: QClipboard::text() seems to do a better job than encodedData( "text/plain" )
        // In particular it handles charsets (in the mimetype).
        QString text = QApplication::clipboard()->text();
        if ( !text.isEmpty() )
            textObject()->pasteText( cursor(), text, currentFormat(), true );
    }
}

void KWTextFrameSetEdit::cut()
{
    if ( textDocument()->hasSelection( QTextDocument::Standard ) ) {
        copy();
        textObject()->removeSelectedText( cursor() );
    }
}

void KWTextFrameSetEdit::copy()
{
    if ( textDocument()->hasSelection( QTextDocument::Standard ) ) {
        KWTextDrag *kd = newDrag( 0L );
        QApplication::clipboard()->setData( kd );
    }
}

void KWTextFrameSetEdit::doAutoFormat( QTextCursor* cursor, KoTextParag *parag, int index, QChar ch )
{
    KWAutoFormat * autoFormat = textFrameSet()->kWordDocument()->getAutoFormat();
    if ( autoFormat )
        autoFormat->doAutoFormat( cursor, parag, index, ch );
}

void KWTextFrameSetEdit::startDrag()
{
    textView()->dragStarted();
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
            textObject()->removeSelectedText( cursor() );
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
    //kdDebug() << "KWTextFrameSetEdit::ensureCursorVisible paragId=" << cursor()->parag()->paragId() << endl;
    QTextParag * parag = cursor()->parag();
    textFrameSet()->ensureFormatted( parag );
    QTextStringChar *chr = parag->at( cursor()->index() );
    int h = parag->lineHeightOfChar( cursor()->index() );
    int x = parag->rect().x() + chr->x + cursor()->offsetX();
    //kdDebug() << "parag->rect().x()=" << parag->rect().x() << " chr->x=" << chr->x
    //          << " cursor()->offsetX()=" << cursor()->offsetX() << endl;
    int y = 0; int dummy;
    parag->lineHeightOfChar( cursor()->index(), &dummy, &y );
    y += parag->rect().y() + cursor()->offsetY();
    int w = 1;
    KoPoint pt;
    KoPoint hintDPoint;
    if ( m_currentFrame )
        hintDPoint = m_currentFrame->topLeft();
    KWFrame * frame = textFrameSet()->internalToDocumentWithHint( QPoint(x, y), pt, hintDPoint );
    //kdDebug() << "KWTextFrameSetEdit::ensureCursorVisible frame=" << frame << " m_currentFrame=" << m_currentFrame << endl;
    if ( frame && m_currentFrame != frame )
    {
        m_currentFrame = frame;
        m_canvas->gui()->getView()->updatePageInfo();
    }
    QPoint p = textFrameSet()->kWordDocument()->zoomPoint( pt );
    p = m_canvas->viewMode()->normalToView( p );
    w = textFrameSet()->kWordDocument()->layoutUnitToPixelX( w );
    h = textFrameSet()->kWordDocument()->layoutUnitToPixelY( h );
    m_canvas->ensureVisible( p.x(), p.y() + h / 2, w, h / 2 + 2 );
}

void KWTextFrameSetEdit::keyPressEvent( QKeyEvent* e )
{
    textView()->handleKeyPressEvent( e );
}

void KWTextFrameSetEdit::mousePressEvent( QMouseEvent *e, const QPoint &, const KoPoint & dPoint )
{
    textFrameSet()->textObject()->clearUndoRedoInfo();
    if ( m_currentFrame )
        hideCursor(); // Need to do that with the old m_currentFrame

    QPoint iPoint;
    KWFrame * frame = textFrameSet()->documentToInternal( dPoint, iPoint, true );
    if ( frame && m_currentFrame != frame )
    {
        m_currentFrame = frame;
        m_canvas->gui()->getView()->updatePageInfo();
    }

    if ( m_currentFrame )
    {
        textView()->handleMousePressEvent( e, iPoint );
    }
    // else mightStartDrag = FALSE; necessary?
}

void KWTextFrameSetEdit::mouseMoveEvent( QMouseEvent * e, const QPoint & nPoint, const KoPoint & )
{
    if ( textView()->maybeStartDrag( e ) )
        return;
    QPoint iPoint;
    KoPoint dPoint = frameSet()->kWordDocument()->unzoomPoint( nPoint );
    if ( nPoint.y() > 0 && textFrameSet()->documentToInternal( dPoint, iPoint, true ) )
    {
        textView()->handleMouseMoveEvent( e, iPoint );
    }
}

void KWTextFrameSetEdit::mouseReleaseEvent( QMouseEvent *, const QPoint &, const KoPoint & )
{
    textView()->handleMouseReleaseEvent();
}

void KWTextFrameSetEdit::mouseDoubleClickEvent( QMouseEvent *e, const QPoint &, const KoPoint & )
{
    textView()->handleMouseDoubleClickEvent( e, QPoint() /* Currently unused */ );
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
    KoPoint dPoint = frameSet()->kWordDocument()->unzoomPoint( nPoint );
    if ( textFrameSet()->documentToInternal( dPoint, iPoint ) )
    {
        textObject()->emitHideCursor();
        placeCursor( iPoint );
        textObject()->emitShowCursor();
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
        e->acceptAction();
        QTextCursor dropCursor( textDocument() );
        QPoint dropPoint;
        KoPoint dPoint = frameSet()->kWordDocument()->unzoomPoint( nPoint );
        if ( !textFrameSet()->documentToInternal( dPoint, dropPoint ) )
            return; // Don't know where to paste

        KMacroCommand *macroCmd=new KMacroCommand(i18n("Paste Text"));
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
                    delete macroCmd;
                    textDocument()->removeSelection( QTextDocument::Standard );
                    textObject()->selectionChangedNotify();
                    hideCursor();
                    *cursor() = dropCursor;
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

                macroCmd->addCommand(textObject()->removeSelectedTextCommand( cursor(), QTextDocument::Standard ));
            }
            hideCursor();
            *cursor() = dropCursor;
            showCursor();
            kdDebug(32001) << "cursor set back to drop cursor: parag=" << cursor()->parag()->paragId() << " index=" << cursor()->index() << endl;

        } else
        {   // drop coming from outside -> forget about current selection
            textDocument()->removeSelection( QTextDocument::Standard );
            textObject()->selectionChangedNotify();
        }

        if ( e->provides( KWTextDrag::selectionMimeType() ) )
        {
            QByteArray arr = e->encodedData( KWTextDrag::selectionMimeType() );
            if ( arr.size() )
                macroCmd->addCommand(textFrameSet()->pasteKWord( cursor(), QCString(arr), false ));
        }
        else
        {
            QString text;
            if ( QTextDrag::decode( e, text ) )
                textObject()->pasteText( cursor(), text, currentFormat(), false );
        }
        frameSet()->kWordDocument()->addCommand(macroCmd);
    }
}

void KWTextFrameSetEdit::focusInEvent()
{
    textView()->focusInEvent();
}

void KWTextFrameSetEdit::focusOutEvent()
{
    textView()->focusOutEvent();
}

void KWTextFrameSetEdit::selectAll()
{
    textObject()->selectAll( true );
}

void KWTextFrameSetEdit::drawCursor( bool visible )
{
    //kdDebug() << "KWTextFrameSetEdit::drawCursor " << visible << endl;
    KoTextView::drawCursor( visible );
    if ( !cursor()->parag() )
        return;

    if ( !cursor()->parag()->isValid() )
        textFrameSet()->ensureFormatted( cursor()->parag() );

    if ( !frameSet()->kWordDocument()->isReadWrite() )
        return;
    if ( !m_currentFrame )
        return;

    QPainter p( m_canvas->viewport() );
    p.translate( -m_canvas->contentsX(), -m_canvas->contentsY() );
    p.setBrushOrigin( -m_canvas->contentsX(), -m_canvas->contentsY() );

    //QRect crect( m_canvas->contentsX(), m_canvas->contentsY(), m_canvas->visibleWidth(), m_canvas->visibleHeight() );
    textFrameSet()->drawCursor( &p, cursor(), visible, m_canvas, m_currentFrame );
}

void KWTextFrameSetEdit::pgUpKeyPressed()
{
    QRect crect( m_canvas->contentsX(), m_canvas->contentsY(),
                 m_canvas->visibleWidth(), m_canvas->visibleHeight() );
    crect = m_canvas->viewMode()->viewToNormal( crect );

#if 0
    // One idea would be: move up until first-visible-paragraph wouldn't be visible anymore
    // First find the first-visible paragraph...
    QTextParag *firstVis = m_cursor->parag();
    while ( firstVis && crect.intersects( s->rect() ) )
        firstVis = firstVis->prev();
    if ( !firstVis )
        firstVis = textFrameSet()->textDocument()->firstParag();
    else if ( firstVis->next() )
        firstVis = firstVis->next();
#endif
    // Go up of 90% of crect.height()
    int h = static_cast<int>( (double)crect.height() * 0.9 );
    QTextParag *s = textView()->cursor()->parag();
    int y = s->rect().y();
    while ( s ) {
        if ( y - s->rect().y() >= h )
            break;
        s = s->prev();
    }

    if ( !s )
        s = textDocument()->firstParag();

    textView()->cursor()->setParag( s );
    textView()->cursor()->setIndex( 0 );
}

void KWTextFrameSetEdit::pgDownKeyPressed()
{
    QRect crect( m_canvas->contentsX(), m_canvas->contentsY(),
                 m_canvas->visibleWidth(), m_canvas->visibleHeight() );
    crect = m_canvas->viewMode()->viewToNormal( crect );
    // Go down of 90% of crect.height()
    int h = static_cast<int>( (double)crect.height() * 0.9 );

    QTextCursor *cursor = textView()->cursor();
    QTextParag *s = cursor->parag();
    int y = s->rect().y();
    while ( s ) {
        if ( s->rect().y() - y >= h )
            break;
        s = s->next();
    }

    if ( !s ) {
        s = textDocument()->lastParag();
        cursor->setParag( s );
        cursor->setIndex( s->length() - 1 );
    } else {
        cursor->setParag( s );
        cursor->setIndex( 0 );
    }
}

void KWTextFrameSetEdit::ctrlPgUpKeyPressed()
{
    if ( m_currentFrame )
    {
        QPoint iPoint = textFrameSet()->moveToPage( m_currentFrame->pageNum(), -1 );
        if ( !iPoint.isNull() )
            placeCursor( iPoint );
    }
}

void KWTextFrameSetEdit::ctrlPgDownKeyPressed()
{
    if ( m_currentFrame )
    {
        QPoint iPoint = textFrameSet()->moveToPage( m_currentFrame->pageNum(), +1 );
        if ( !iPoint.isNull() )
            placeCursor( iPoint );
    }
}

void KWTextFrameSetEdit::insertSpecialChar(QChar _c)
{
    if(textObject()->hasSelection() )
        frameSet()->kWordDocument()->addCommand(textObject()->replaceSelectionCommand(
            cursor(), _c, QTextDocument::Standard, i18n("Insert Special Char")));
    else
        textObject()->insert( cursor(), currentFormat(), _c, false /* no newline */, true, i18n("Insert Special Char") );
}

void  KWTextFrameSetEdit::insertExpression(const QString &_c)
{
    if(textObject()->hasSelection() )
        frameSet()->kWordDocument()->addCommand(textObject()->replaceSelectionCommand(
            cursor(), _c, QTextDocument::Standard , i18n("Insert Expression")));
    else
       textObject()->insert( cursor(), currentFormat(), _c, false /* no newline */, true, i18n("Insert Expression") );
}

void KWTextFrameSetEdit::insertFloatingFrameSet( KWFrameSet * fs, const QString & commandName )
{
    textObject()->clearUndoRedoInfo();
    CustomItemsMap customItemsMap;
    QString placeHolders;
    // TODO support for multiple floating items (like multiple-page tables)
    int frameNumber = 0;
    int index = 0;
    bool ownline = false;
    { // the loop will start here :)
        KWAnchor * anchor = fs->createAnchor( textFrameSet()->kwTextDocument(), frameNumber );
        if ( frameNumber == 0 && anchor->ownLine() && cursor()->index() > 0 ) // enforce start of line - currently unused
        {
            kdDebug() << "ownline -> prepending \\n" << endl;
            placeHolders += QChar('\n');
            index++;
            ownline = true;
        }
        placeHolders += KoTextObject::customItemChar();
        customItemsMap.insert( index, anchor );
    }
    fs->setAnchored( textFrameSet() );
    textObject()->insert( cursor(), currentFormat(), placeHolders,
                          ownline, false, commandName,
                          customItemsMap );
}


void KWTextFrameSetEdit::insertCustomVariable( const QString &name)
{
     KWVariable * var = 0L;
     KWDocument * doc = frameSet()->kWordDocument();
     var = new KWCustomVariable( textFrameSet(), name, doc->variableFormatCollection()->format( "STRING" ) );
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
            var = new KWCustomVariable( textFrameSet(), dia.getName(), doc->variableFormatCollection()->format( "STRING" ) );
    }
    else if ( type == VT_SERIALLETTER )
    {
        KWSerialLetterVariableInsertDia dia( m_canvas, doc->getSerialLetterDataBase() );
        if ( dia.exec() == QDialog::Accepted )
            var = new KWSerialLetterVariable( textFrameSet(), dia.getName(), doc->variableFormatCollection()->format( "STRING" ) );
    }
    else
        var = KWVariable::createVariable( type, subtype, textFrameSet(), 0L );

    insertVariable( var );
}

void KWTextFrameSetEdit::insertVariable( KWVariable *var )
{
    if ( var )
    {
        CustomItemsMap customItemsMap;
        customItemsMap.insert( 0, var );
        kdDebug() << "KWTextFrameSetEdit::insertVariable inserting into paragraph" << endl;
#ifdef DEBUG_FORMATS
        kdDebug() << "KWTextFrameSetEdit::insertVariable currentFormat=" << currentFormat() << endl;
#endif
        textObject()->insert( cursor(), currentFormat(), KoTextObject::customItemChar(),
                                false, false, i18n("Insert Variable"),
                                customItemsMap );
        var->recalc();
        cursor()->parag()->invalidate(0);
        cursor()->parag()->setChanged( true );
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
        int i = cursor()->index();
        if ( i > 0 )
            --i;
#ifdef DEBUG_FORMATS
        if ( currentFormat() )
            kdDebug(32003) << "KWTextFrameSet::updateUI old currentFormat=" << currentFormat()
                           << " " << currentFormat()->key()
                           << " parag format=" << cursor()->parag()->at( i )->format()->key() << endl;
        else
            kdDebug(32003) << "KWTextFrameSetEdit::updateUI old currentFormat=0" << endl;
#endif
        if ( !currentFormat() || currentFormat()->key() != cursor()->parag()->at( i )->format()->key() )
        {
            if ( currentFormat() )
                currentFormat()->removeRef();
#ifdef DEBUG_FORMATS
            kdDebug() << "Setting currentFormat from format " << cursor()->parag()->at( i )->format()
                      << " ( character " << i << " in paragraph " << cursor()->parag()->paragId() << " )" << endl;
#endif
            setCurrentFormat( static_cast<KWTextFormat *>( textDocument()->formatCollection()->format( cursor()->parag()->at( i )->format() ) ) );
            if ( currentFormat()->isMisspelled() ) {
                currentFormat()->removeRef();
                // ## this forgets the charset, etc.
                setCurrentFormat( static_cast<KWTextFormat *>( textDocument()->formatCollection()->format( currentFormat()->font(), currentFormat()->color() ) ) );
            }
            showCurrentFormat();
        }
    }

    // Paragraph settings
    KWTextParag * parag = static_cast<KWTextParag *>(cursor()->parag());

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
        || m_paragLayout.margins[QStyleSheetItem::MarginFirstLine] != parag->margin(QStyleSheetItem::MarginFirstLine)
        || m_paragLayout.margins[QStyleSheetItem::MarginRight] != parag->margin(QStyleSheetItem::MarginRight)
	|| force )
    {
        m_paragLayout.margins[QStyleSheetItem::MarginFirstLine] = parag->margin(QStyleSheetItem::MarginFirstLine);
        m_paragLayout.margins[QStyleSheetItem::MarginLeft] = parag->margin(QStyleSheetItem::MarginLeft);
        m_paragLayout.margins[QStyleSheetItem::MarginRight] = parag->margin(QStyleSheetItem::MarginRight);
        m_canvas->gui()->getView()->showRulerIndent( m_paragLayout.margins[QStyleSheetItem::MarginLeft], m_paragLayout.margins[QStyleSheetItem::MarginFirstLine], m_paragLayout.margins[QStyleSheetItem::MarginRight] );
    }

    m_paragLayout.setTabList( parag->tabList() );
    KoRuler * hr = m_canvas->gui()->getHorzRuler();
    if ( hr ) hr->setTabList( parag->tabList() );
    // There are more paragraph settings, but those that are not directly
    // visible in the UI don't need to be handled here.
    // For instance parag and line spacing stuff, borders etc.
}

void KWTextFrameSetEdit::showFormat( KoTextFormat *format )
{
    m_canvas->gui()->getView()->showFormat( *format );
}

QList<KAction> KWTextFrameSetEdit::dataToolActionList()
{
    m_singleWord = false;
    m_wordUnderCursor = QString::null;
    KWDocument * doc = frameSet()->kWordDocument();
    QString text;
    if ( textObject()->hasSelection() )
    {
        text = textObject()->selectedText();
        if ( text.find(' ') == -1 && text.find('\t') == -1 && text.find(KoTextObject::customItemChar()) == -1 )
            m_singleWord = true;
    }
    else // No selection -> get word under cursor
    {
        selectWordUnderCursor();
        text = textObject()->selectedText();
        if(text.find(KoTextObject::customItemChar()) == -1)
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

    // EEEEEK. Please, no KSpell-specific things in here. This is the generic support
    // for any datatool!
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
    if ( textObject()->hasSelection() )
        text = textObject()->selectedText();
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
            if ( !textObject()->hasSelection() )
                textView()->selectWordUnderCursor();
            // replace selection with 'text'
            textFrameSet()->kWordDocument()->addCommand(textObject()->replaceSelectionCommand(
                cursor(), text, QTextDocument::Standard, i18n("Replace word") ));
        }
    }

    delete tool;
}

void KWTextFrameSetEdit::changeCaseOfText(KWTextFrameSet::TypeOfCase _type)
{
    QString text;
    if ( textObject()->hasSelection() )
        text = textObject()->selectedText();
    if(!text.isEmpty()&& text.find(KoTextObject::customItemChar())==-1)
    {
        textFrameSet()->kWordDocument()->addCommand(textObject()->replaceSelectionCommand(
            cursor(), textFrameSet()->textChangedCase(text,_type), QTextDocument::Standard, i18n("Change case") ));
    }
    else if(!text.isEmpty())
        textFrameSet()->changeCaseOfText(cursor(), _type);
}

void KWTextFrameSetEdit::showPopup( KWFrame * /*frame*/, KWView *view, const QPoint &point )
{
    // Removed previous stuff
    view->unplugActionList( "datatools" );
    m_actionList.clear();
    m_actionList = dataToolActionList();
    kdDebug() << "KWView::openPopupMenuInsideFrame plugging actionlist with " << m_actionList.count() << " actions" << endl;
    view->plugActionList( "datatools", m_actionList );
    QPopupMenu * popup = view->popupMenu("text_popup");
    ASSERT(popup);
    if (popup)
        popup->popup( point ); // using exec() here breaks the spellcheck tool (event loop pb)
}

#include "kwtextframeset.moc"
