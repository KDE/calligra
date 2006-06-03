/* This file is part of the KOffice project
 * Copyright (C) 2002-2005 David Faure <faure@kde.org>
 * Copyright (C) 2005 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; version 2.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "KWFrameLayout.h"
#include "KWFrameList.h"
#include "KWPageManager.h"
#include "KWPage.h"
#include "KWTextFrameSet.h"
#include "KWDocument.h"
#include <QTimer>

// #define DEBUG_FRAMELAYOUT

#ifdef NDEBUG
#undef DEBUG_FRAMELAYOUT
#endif

KWFrameLayout::HeaderFooterFrameset::HeaderFooterFrameset( KWTextFrameSet* fs, int start, int end,
                                                           double spacing, OddEvenAll oea )
    : m_frameset(fs), m_startAtPage(start), m_endAtPage(end), m_oddEvenAll(oea),
      m_spacing(spacing), m_minY( 0 ), m_positioned( false )
{
#if 0
    if ( fs->frameCount() > 0 )
        m_height = fs->frame(0)->height();
    else
        m_height = 20; // whatever. The text layout will resize it.
    Q_ASSERT( m_height > 0 );
#endif
}


void KWFrameLayout::HeaderFooterFrameset::debug()
{
#ifdef DEBUG_FRAMELAYOUT
    HeaderFooterFrameset* hff = this;
    kDebug(32002) << " * " << hff->m_frameset->name()
                   << " pages:" << hff->m_startAtPage << "-" << (hff->m_endAtPage==-1?QString("(all)"):QString::number(hff->m_endAtPage))
                   << " page-selection:" << (hff->m_oddEvenAll==HeaderFooterFrameset::Odd ? "Odd" :
                                             hff->m_oddEvenAll==HeaderFooterFrameset::Even ? "Even" : "All")
                   << " frames:" << hff->m_frameset->frameCount()
                   << " height:" << hff->m_height
                   << " spacing:" << hff->m_spacing << endl;
#endif
}

bool KWFrameLayout::HeaderFooterFrameset::deleteFramesAfterLast( int lastPage )
{
    int lastFrame = lastFrameNumber( lastPage );
#ifdef DEBUG_FRAMELAYOUT
    //kDebug(32002) << " Final cleanup: frameset " << m_frameset->name() << ": lastFrame=" << lastFrame << endl;
#endif

    KWTextFrameSet* fs = m_frameset;

    // Special case for odd/even headers: keep at least one frame even if it doesn't appear,
    // otherwise the frame properties are lost.
    if ( fs->isHeaderOrFooter() && lastFrame == -1 ) {
        fs->setVisible( false );
        lastFrame = 0;
    }

    bool deleted = false;
    while ( (int)fs->frameCount() - 1 > lastFrame ) {
#ifdef DEBUG_FRAMELAYOUT
        kDebug(32002) << "  Final cleanup: deleting frame " << fs->frameCount() - 1 << " of " << fs->name() << endl;
#endif
        fs->deleteFrame( fs->frameCount() - 1 );
        deleted = true;
    }
    return deleted;
}

int KWFrameLayout::HeaderFooterFrameset::lastFrameNumber( int lastPage ) const {
    if ( lastPage < m_startAtPage )
        return -1; // we need none
    int pg = lastPage;
    if ( m_endAtPage > -1 )
        pg = qMin( m_endAtPage, pg );
    pg -= m_startAtPage; // always >=0
    Q_ASSERT( pg >= 0 );
    switch (m_oddEvenAll) {
        case Odd:
        case Even:
            return pg / 2; // page 0 and 1 -> 0. page 2 and 3 -> 1.
        case All:
            return pg; // page 0 -> 0 etc. ;)
        default:
            return -1;
    }
}

int KWFrameLayout::HeaderFooterFrameset::frameNumberForPage( int page ) const
{
    if ( page < m_startAtPage || ( m_endAtPage != -1 && page > m_endAtPage ) )
        return -1;
    int pg = page - m_startAtPage; // always >=0
    switch (m_oddEvenAll) {
    case Odd:
        // we test the absolute page number for odd/even, not pg!
        if ( page % 2 )
            return pg / 2; // page start+(0 or 1) -> frame 0, page start+(2 or 3) -> frame 1
        else
            return -1;
    case Even:
        if ( page % 2 == 0 )
            return pg / 2; // page start+(0 or 1) -> frame 0, page start+(2 or 3) -> frame 1
        else
            return -1;
    case All:
        return pg; // page 0[+start] -> frame 0, etc.
    default:
        return -1;
    }
}

/////

void KWFrameLayout::layout( KWFrameSet* mainTextFrameSet, int numColumns,
                            int fromPage, int toPage, uint flags )
{
#if 0
    //kDebug(32002) << "KWFrameLayout::layout " << kBacktrace() << endl;
    // Just debug stuff
#ifdef DEBUG_FRAMELAYOUT
    kDebug(32002) << "KWFrameLayout::layout " << fromPage << " to " << toPage << endl;
    Q_ASSERT( toPage >= fromPage );
    Q3PtrListIterator<HeaderFooterFrameset> itdbg( m_headersFooters );
    for ( ; itdbg.current() ; ++itdbg )
        itdbg.current()->debug();
    Q3PtrListIterator<HeaderFooterFrameset> itdbg2( m_footnotes );
    for ( ; itdbg2.current() ; ++itdbg2 )
        itdbg2.current()->debug();
    Q3PtrListIterator<HeaderFooterFrameset> itdbg3( m_endnotes );
    for ( ; itdbg3.current() ; ++itdbg3 )
        itdbg3.current()->debug();
#endif
#if 0 // old code
    // Necessary for end notes: calculate where the text goes down to
    Q_ASSERT( mainTextFrameSet->type() == FT_TEXT );
    double textBottom = 0.0;
    if ( mainTextFrameSet->hasFramesInPageArray() )
    {
        KoPoint textBottomPoint;
        KoTextParag * lastParag = static_cast<KWTextFrameSet *>(mainTextFrameSet)->textDocument()->lastParag();
        if ( lastParag->isValid() )
        {
            QRect rect = lastParag->rect();
            int bottom = rect.top() + rect.height() + 2; // cf kwtextframeset
#ifdef DEBUG_FRAMELAYOUT
            kDebug(32002) << "bottom LU=" << bottom << endl;
#endif

            if ( static_cast<KWTextFrameSet *>(mainTextFrameSet)->internalToDocument( QPoint(rect.left(), bottom), textBottomPoint ) )
                textBottom = textBottomPoint.y();
        }
    }
#ifdef DEBUG_FRAMELAYOUT
    kDebug(32002) << "textBottom = " << textBottom << "pt" << endl;
#endif
#endif
    m_framesetsToUpdate.clear();
    // Necessary for end notes: find out the last frame of the main textframeset
    KWFrame* lastMainFrame = mainTextFrameSet->frameIterator().getLast();
    double lastMainFrameBottom = lastMainFrame->bottom(); // before we change it below!

    double ptColumnWidth = m_doc->ptColumnWidth();
    int mainTextFrameResized = -1; // contains the page number of the first resized main textframe

    // The main loop is: "for each page". We lay out each page separately.
    for ( int pageNum = fromPage ; pageNum <= toPage ; ++pageNum )
    {
        KWPage *page = m_doc->pageManager()->page(pageNum);
        double top = page->offsetInDocument() + page->topMargin();
        double bottom = page->offsetInDocument() + page->height() - page->bottomMargin();
        double left = page->leftMargin();
        double right = page->width() - page->rightMargin();
        Q_ASSERT( left < right );
        KoRect oldColumnRect = firstColumnRect( mainTextFrameSet, pageNum, numColumns );
#ifdef DEBUG_FRAMELAYOUT
        kDebug(32002) << " Page " << pageNum << endl;
#endif

        // For each header/footer....
        for ( Q3PtrListIterator<HeaderFooterFrameset> it( m_headersFooters ); it.current() ; ++it )
        {
            int frameNum = it.current()->frameNumberForPage( pageNum );
            if ( frameNum != -1 )
            {
                it.current()->m_positioned = true;
                KWTextFrameSet* fs = it.current()->m_frameset;
#ifdef DEBUG_FRAMELAYOUT
                kDebug(32002) << " Page " << pageNum << ": adding frame " << frameNum << " from " << fs->name() << endl;
#endif
                KoRect rect;
                if ( fs->isAHeader() ) // add on top
                {
                    rect.setRect( left, top, right - left, it.current()->m_height );
                    top += it.current()->m_height + it.current()->m_spacing;
                } else // footer, add at bottom
                {
                    double frameHeight = it.current()->m_height;
                    double frameTop = bottom - frameHeight;
                    rect.setRect( left, frameTop, right - left, frameHeight );
                    bottom -= frameHeight + it.current()->m_spacing;
                }
                Q_ASSERT( bottom > 0 );
                Q_ASSERT( top < bottom );
#ifdef DEBUG_FRAMELAYOUT
                kDebug(32002) << "     rect:" << rect << "   - new_top:" << top << " new_bottom:" << bottom << endl;
#endif
                resizeOrCreateHeaderFooter( fs, frameNum, rect );
            }
        }

        // All headers/footers for this page have been done,
        // now resize the frame from the main textframeset (if any)
        // the first time _before_ doing the footnotes.
        resizeMainTextFrame( mainTextFrameSet, pageNum, numColumns, ptColumnWidth, m_doc->ptColumnSpacing(), left, top, bottom, NoFootNote );

        // Recalc footnote pages
        checkFootNotes();

        bool firstFootNote = true;

        //// Stay seated... We need to know if there are any footnotes on top of us, although we're going
        //// to lay them out _AFTER_. But we need their total height for the minY stuff.
        //// So we first iterate over all footnotes of the page, to get their total height.
        //// Then we'll reduce this height after every footnote being positionned, so it's always
        //// the "height on top of us".
        double totalFootNotesHeight = 0;
        for ( Q3PtrListIterator<HeaderFooterFrameset> it( m_footnotes ); it.current() ; ++it )
        {
            int frameNum = it.current()->frameNumberForPage( pageNum );
            if ( frameNum != -1 )
                totalFootNotesHeight += it.current()->m_height;
        }

        // For each footnote (caller sorted them from bottom to top)
        for ( Q3PtrListIterator<HeaderFooterFrameset> it( m_footnotes ); it.current() ; ++it )
        {
            int frameNum = it.current()->frameNumberForPage( pageNum );
            if ( frameNum != -1 )
            {
                it.current()->m_positioned = true;
                totalFootNotesHeight -= it.current()->m_height; // as discussed above
                KWTextFrameSet* fs = it.current()->m_frameset;
#ifdef DEBUG_FRAMELAYOUT
                kDebug(32002) << " Page " << pageNum << ": adding footnote frame " << frameNum << " from " << fs->name() << endl;
#endif
                KoRect rect;

                // When two footnotes are in the same page there should be 0 spacing between them
                // Yeah, write a generic frame layouter and then realize it's not flexible enough :(
                if ( fs->isFootEndNote() && !firstFootNote )
                {
                    // Undo "bottom -= spacing" (done below). This assumes equal spacing for all footnotes
                    bottom += it.current()->m_spacing;
                    bottom -= 1; // keep them one pixel apart though
                }
                double frameTop = bottom - it.current()->m_height;
                double frameHeight = it.current()->m_height;

                Q_ASSERT ( fs->isFootNote() );

                // This is where we add the "total height of the footnotes on top of this one".
                // The footnote variable can't be behind them....

                double minY = it.current()->m_minY + totalFootNotesHeight;
#ifdef DEBUG_FRAMELAYOUT
                kDebug(32002) << "   footnote: frameHeight=" << frameHeight << " frameTop (" << frameTop << ") <? minY (" << minY << ")" << endl;
#endif
                if ( frameTop < minY )
                {
                    // Ok, this is the complex case of a footnote var too far down in the page,
                    // and its footnote text is too big, so both won't fit.
                    // We do like other WPs: we create a frame on the next page
                    it.current()->m_endAtPage++; // this will do so

                    // In the current page we stop at minY
                    frameTop = minY;
                    frameHeight = bottom - frameTop;
#ifdef DEBUG_FRAMELAYOUT
                    kDebug(32002) << "   footnote: new top=" << frameTop << " new height=" << frameHeight << " remaining height=" << it.current()->m_height - frameHeight << endl;
#endif
                    Q_ASSERT( frameHeight < it.current()->m_height );
                    it.current()->m_height -= frameHeight; // calculate what remains to be done in the next frame
                    //fnFrameBehavior = KWFrame::Ignore;

                    // Make sure there'll actually be a next page
                    if ( pageNum == m_doc->pageCount()-1 ) {
#ifdef DEBUG_FRAMELAYOUT
                        kDebug(32002) << "Adding a page for the footnote overflow." << endl;
#endif
                        m_doc->appendPage();
                        m_doc->updateAllFrames();
                        toPage = m_doc->pageCount()-1;
                    }
                }

                rect.setRect( left, frameTop, right - left, frameHeight );
                bottom -= frameHeight + it.current()->m_spacing;
                Q_ASSERT( bottom > 0 );
                Q_ASSERT( top < bottom );
#ifdef DEBUG_FRAMELAYOUT
                kDebug(32002) << "     footnote rect:" << rect << "   - new_top:" << top << " new_bottom:" << bottom << endl;
#endif
                resizeOrCreateHeaderFooter( fs, frameNum, rect );
                firstFootNote = false;

                // We added a footnote, update main text frame size
#ifdef DEBUG_FRAMELAYOUT
                kDebug(32002) << "   Laid out a footnote -> call resizeMainTextFrame/checkFootNotes again" << endl;
#endif
                resizeMainTextFrame( mainTextFrameSet, pageNum, numColumns, ptColumnWidth, m_doc->ptColumnSpacing(), left, top, bottom, WithFootNotes );
                checkFootNotes();
            }
        } // for all footnotes

        // Check for endnotes, on the last page of main text
        // and on any end-notes-only page, i.e. after the last page of main text
        if ( pageNum >= m_lastMainFramePage && m_doc->hasEndNotes() ) {
            bool pageHasMainText = ( pageNum == m_lastMainFramePage );
            if ( pageHasMainText )
                lastMainFrame->setDrawFootNoteLine( true );
            double textBottom = pageHasMainText ? lastMainFrameBottom : top;
            // Leave some space on top of the endnotes, for the horizontal line
            double endNoteTop = textBottom + m_doc->ptFootnoteBodySpacing();
#ifdef DEBUG_FRAMELAYOUT
            kDebug(32002) << "  Endnotes: textBottom=" << textBottom << "pt, endNoteTop=" << endNoteTop << "pt, bottom=" << bottom << "pt" << endl;
#endif
            bool firstEndNote = true;
            for ( Q3PtrListIterator<HeaderFooterFrameset> it( m_endnotes ); it.current() ; ++it )
            {
                if ( ! it.current()->m_positioned )
                {
                    KWTextFrameSet* fs = it.current()->m_frameset;
#ifdef DEBUG_FRAMELAYOUT
                    kDebug(32002) << " Page " << pageNum << ": adding endnote frame from " << fs->name() << endl;
#endif
                    double frameHeight = it.current()->m_height;
                    if ( it.current()->m_startAtPage < 0 ) // not set yet
                        it.current()->m_startAtPage = pageNum;

                    // Check if the endnote is bigger than the available space
                    if ( endNoteTop + frameHeight > bottom )
                    {
                        // In the current page we stop at bottom
                        frameHeight = bottom - endNoteTop;

                        if ( frameHeight > 1E-10 ) // means, if frameHeight > 0
                        {
#ifdef DEBUG_FRAMELAYOUT
                            kDebug(32002) << "   endnote: new height=" << frameHeight << " remaining height=" << it.current()->m_height - frameHeight << endl;
#endif
                            Q_ASSERT( frameHeight < it.current()->m_height );
                            it.current()->m_height -= frameHeight; // calculate what remains to be done in the next frame
                        } else {
                            // No room at all on this page. Schedule for next page.
                            it.current()->m_startAtPage++;
                            break;
                        }
                        // Make sure there'll actually be a next page
                        if ( pageNum == m_doc->pageCount()-1 ) {
#ifdef DEBUG_FRAMELAYOUT
                            kDebug(32002) << "Adding a page for the endnote overflow." << endl;
#endif
                            m_doc->appendPage();
                            m_doc->updateAllFrames();
                            toPage = m_doc->pageCount()-1;
                        }
                    }
                    else // It'll all fit in this page
                    {
                        it.current()->m_positioned = true;
                    }
                    KoRect rect( left, endNoteTop, right - left, frameHeight );
                    endNoteTop += frameHeight + 1; // not + it.current()->m_spacing;
                    Q_ASSERT( bottom > 0 );
#ifdef DEBUG_FRAMELAYOUT
                    kDebug(32002) << "     rect:" << rect << "   - new_top:" << endNoteTop << " new_bottom:" << bottom << endl;
#endif
                    int frameNum = pageNum - it.current()->m_startAtPage;
                    resizeOrCreateHeaderFooter( fs, frameNum, rect );

#if 0  // Disabled. The main frame is resized by KWTextFrameSet::slotAfterFormatting already.
                    if ( pageHasMainText && firstEndNote )
                    {
                        // We positionned the first endnote, update main text frame size
#ifdef DEBUG_FRAMELAYOUT
                        kDebug(32002) << "   Laid out an endnote and the page has a maintextframe too -> call resizeMainTextFrame/checkFootNotes again top=" << top << " textBottom=" << textBottom << endl;
#endif
                        resizeMainTextFrame( mainTextFrameSet, pageNum, numColumns, ptColumnWidth, m_doc->ptColumnSpacing(), left, top, textBottom, NoChange );
                    }
#endif
                } // if not positionned yet
                firstEndNote = false; // yes, out of the if
            } // for all endnotes
        } // if page can have endnotes

        if ( mainTextFrameResized == -1 ) {
            // Test if the main text frame for this page was really resized or not.
            KoRect newColumnRect = firstColumnRect( mainTextFrameSet, pageNum, numColumns );
#ifdef DEBUG_FRAMELAYOUT
            kDebug(32002) << "  Comparing old=" << oldColumnRect << " and new=" << newColumnRect << endl;
#endif
            if ( oldColumnRect != newColumnRect ) {
                mainTextFrameResized = pageNum;
#ifdef DEBUG_FRAMELAYOUT
                kDebug(32002) << "  changed -> mainTextFrameResized=" << mainTextFrameResized << endl;
#endif
            }
        }

    } // for all pages
    m_lastMainFramePage = lastMainFrame->pageNumber();
#ifdef DEBUG_FRAMELAYOUT
    kDebug(32002) << "m_lastMainFramePage = " << m_lastMainFramePage << " lastMainFrameBottom=" << lastMainFrameBottom << endl;
#endif

    if ( ! ( flags & DontRemovePages ) )
    {
        m_doc->updateAllFrames( KWFrameSet::UpdateFramesInPage );
        // Check if the last page is now empty (e.g. this can happen when removing
        // some text above an endnote, so the endnote moves up)
        (void)m_doc->tryRemovingPages();
    }

    const int lastPage = m_doc->lastPage();
    // Final cleanup: delete all frames after lastFrameNumber in each frameset
    Q3PtrListIterator<HeaderFooterFrameset> it( m_headersFooters );
    for ( ; it.current() ; ++it )
        if ( it.current()->deleteFramesAfterLast( lastPage ) )
            m_framesetsToUpdate.insert( it.current()->m_frameset, true );
    Q3PtrListIterator<HeaderFooterFrameset> it2( m_footnotes );
    for ( ; it2.current() ; ++it2 )
        if ( it2.current()->deleteFramesAfterLast( lastPage ) )
            m_framesetsToUpdate.insert( it2.current()->m_frameset, true );
    if ( mainTextFrameSet ) {
        // For the last main text frameset, we use m_lastMainFramePage, so that
        // there's no frame on the "end notes only" page(s).
        int lastFrame = m_lastMainFramePage * numColumns + (numColumns-1);
#ifdef DEBUG_FRAMELAYOUT
        kDebug(32002) << "lastFrame: " << lastFrame << " due to " << m_lastMainFramePage << endl;
#endif
        bool deleted = false;
        while ( (int)mainTextFrameSet->frameCount() - 1 > lastFrame ) {
#ifdef DEBUG_FRAMELAYOUT
            kDebug(32002) << "  Final cleanup: deleting frame " << mainTextFrameSet->frameCount() - 1 << " of main textframeset (lastFrame=" << lastFrame << ")" << endl;
#endif
            mainTextFrameSet->deleteFrame( mainTextFrameSet->frameCount() - 1, true, false /*do not updateFrames!*/ );
            deleted = true;
        }
        if ( deleted )
            m_framesetsToUpdate.insert( mainTextFrameSet, true );
        // The last frame before the first endnote, is in auto-extend mode
        if ( m_doc->hasEndNotes() ) {
            KWFrame* lastMainFrame = mainTextFrameSet->frameIterator().getLast();
            if ( lastMainFrame->frameBehavior() != KWFrame::AutoExtendFrame )
            {
                lastMainFrame->setFrameBehavior( KWFrame::AutoExtendFrame );
                // make sure it gets resized
                if ( mainTextFrameResized == -1 )
                    mainTextFrameResized = lastMainFrame->pageNumber();
            }
        }
    }

    QMap<KWFrameSet*, bool>::iterator fsit = m_framesetsToUpdate.begin();
    for ( ; fsit != m_framesetsToUpdate.end() ; ++fsit )
        fsit.key()->updateFrames();

    // ## TODO: only if something changed? (resizing, new frames, or deleted frames...)
    KWFrameList::recalcFrames(m_doc, fromPage, toPage);

    if ( mainTextFrameResized != -1 && mainTextFrameSet->type() == FT_TEXT ) {
#ifdef DEBUG_FRAMELAYOUT
        kDebug(32002) << " Done. First maintextframe resized: " << mainTextFrameResized << endl;
#endif
        KWTextFrameSet* fs = static_cast<KWTextFrameSet *>(mainTextFrameSet);

        // Not right now, this could be called during formatting...
        //m_doc->invalidate();
        // ### This means the layout will be done during painting. Not good.
        // What about mainTextFrameSet->invalidate() or even layout()?
        //QTimer::singleShot( 0, m_doc, SLOT( invalidate() ) );

        // Invalidate main textframeset only, and from top of page only.
        // Otherwise loading a long document (with headers/footers) takes ages,
        // if we redo it all from the beginning at each new page!
        int topLU, bottomLU;
        if ( fs->minMaxInternalOnPage( mainTextFrameResized, topLU, bottomLU ) )
        {
            // Find parag at topLU
            KoTextParag* parag = fs->paragAtLUPos( topLU );
            if ( parag ) {
#ifdef DEBUG_FRAMELAYOUT
                kDebug(32002) << " Invalidating from parag " << parag->paragId() << endl;
#endif
                fs->textObject()->setLastFormattedParag( parag );
                fs->textObject()->formatMore( 2 );
            }
        }
    }
#endif
}

void KWFrameLayout::resizeOrCreateHeaderFooter( KWTextFrameSet* headerFooter, uint frameNumber, const KoRect& rect )
{
#if 0
    if ( frameNumber < headerFooter->frameCount() ) {
        KWFrame* frame = headerFooter->frame( frameNumber );
        if ( *frame == rect )
            return;
        frame->setRect( rect );
#ifdef DEBUG_FRAMELAYOUT
        kDebug(32002) << "KWFrameLayout::resizeOrCreateHeaderFooter frame " << headerFooter->name() << " " << frame << " resized to " << rect << " pagenum=" << frame->pageNumber() << endl;
#endif
    }
    else
    {
#ifdef DEBUG_FRAMELAYOUT
        kDebug(32002) << "KWFrameLayout::resizeOrCreateHeaderFooter creating frame for " << headerFooter->name() << endl;
#endif
        KWFrame* frame = new KWFrame( headerFooter, rect.x(), rect.y(), rect.width(), rect.height() );
        frame->setFrameBehavior( KWFrame::AutoExtendFrame );
        if ( headerFooter->isHeaderOrFooter() ) // not for footnotes!
        {
            frame->setNewFrameBehavior( KWFrame::Copy );
            frame->setCopy( true );
        }
        else
            frame->setNewFrameBehavior( KWFrame::NoFollowup );
        headerFooter->addFrame( frame, false /*no recalc*/ );
    }
    // This updates e.g. availableHeight. Very important in the case
    // of the footnote frameset with 2 frames.
    headerFooter->updateFrames( 0 /*fast one*/ );
    m_framesetsToUpdate.insert( headerFooter, true );
#endif
}

// Called at beginning and end of the layout for a given page,
// to determine if the main-text-frame layout really changed or not.
// Testing in resizeMainTextFrame doesn't work, we call it several times,
// once for each footnote, so it can't detect the "no change" case.
KoRect KWFrameLayout::firstColumnRect( KWFrameSet* mainTextFrameSet, int pageNum, int numColumns ) const
{
#if 0
    uint frameNum = pageNum * numColumns /*+ col  0 here*/;
    if ( mainTextFrameSet && frameNum < mainTextFrameSet->frameCount() )
        return * mainTextFrameSet->frame( frameNum );
    else
        return KoRect();
#endif
}

bool KWFrameLayout::resizeMainTextFrame( KWFrameSet* mainTextFrameSet, int pageNum, int numColumns, double ptColumnWidth, double ptColumnSpacing, double left, double top, double bottom, HasFootNotes hasFootNotes )
{
#if 0
    if ( !mainTextFrameSet )
        return false;
    bool mainTextFrameResized = false;
    for ( int col = 0; col < numColumns; col++ ) {
        Q_ASSERT( bottom > top );
        // Calculate wanted rect for this frame
        KoRect rect( left + col * ( ptColumnWidth + ptColumnSpacing ),
                     top, ptColumnWidth, bottom - top );
        uint frameNum = (pageNum - m_doc->startPage()) * numColumns + col;
        KWFrame* frame;
        if ( frameNum < mainTextFrameSet->frameCount() ) {
            // Resize existing frame
            frame = mainTextFrameSet->frame( frameNum );
            // Special case for last-frame-before-endnotes: don't resize its bottom
            if ( m_doc->hasEndNotes() && pageNum >= m_lastMainFramePage )
                rect.setBottom( frame->bottom() );
            bool resized = (rect != *frame);
            if ( resized ) {
#ifdef DEBUG_FRAMELAYOUT
                kDebug(32002) << " Page " << pageNum << ": resizing main text frame " << frameNum << "(" << frame << ") to " << rect << endl;
#endif
                frame->setRect( rect );
                frame->updateRulerHandles();
                mainTextFrameResized = true;
                mainTextFrameSet->updateFrames( 0xff - KWFrameSet::SortFrames ); // Don't sort frames yet!
            }
        } else {
            // Create new frame
            frame = new KWFrame( mainTextFrameSet, rect.x(), rect.y(), rect.width(), rect.height() );
#ifdef DEBUG_FRAMELAYOUT
            kDebug(32002) << " Page " << pageNum << ": creating new main text frame " << frameNum << "(" << frame << ") to " << rect << endl;
#endif
            mainTextFrameSet->addFrame( frame );
            Q_ASSERT( frameNum == mainTextFrameSet->frameCount()-1 );
            mainTextFrameResized = true;
            mainTextFrameSet->updateFrames( 0xff - KWFrameSet::SortFrames ); // Don't sort frames yet!
        }
        if ( hasFootNotes == NoFootNote )
            frame->setDrawFootNoteLine( false );
        else if ( hasFootNotes == WithFootNotes )
            frame->setDrawFootNoteLine( true );
        // unchanged in the other cases
        // By default, all main-text frames are in "auto-create new frames" mode
        frame->setFrameBehavior( KWFrame::AutoCreateNewFrame );
    }
    return mainTextFrameResized;
#endif
}

void KWFrameLayout::checkFootNotes()
{
#if 0
    // We recalculate all footnotes pages, but we return true
    // if those on pageNum have changed.
    Q3PtrListIterator<HeaderFooterFrameset> it( m_footnotes );
    for ( ; it.current() ; ++it )
    {
        HeaderFooterFrameset* hff = it.current();
        if ( ! hff->m_positioned )
        {
            Q_ASSERT ( hff->m_frameset->isFootEndNote() );
            KWFootNoteFrameSet* fnfs = static_cast<KWFootNoteFrameSet *>( hff->m_frameset );
            KWFootNoteVariable* fnvar = fnfs->footNoteVariable();
            //necessary to test paragraph because when we delete mutli
            //footnote, first footnote who delete call setDelete(true)
            //and force recalc, but with multi footnote deleted
            //paragraph is null before we apply attribute to
            //kotextcustom.
            if ( !fnvar || !fnvar->paragraph() )
                continue;
            double varY = fnvar->varY();
            if ( varY == 0 ) // not able to calculate it yet
                continue;
            hff->m_minY = varY + /*2 * */ hff->m_spacing + 2 /* some spacing */;
            int pageNum = m_doc->pageManager()->pageNumber(varY);
            if ( pageNum != hff->m_startAtPage ) {
#ifdef DEBUG_FRAMELAYOUT
                kDebug(32002) << " checkFootNotes: found minY=" << hff->m_minY << " start/end=" << pageNum << " for footnote " << fnvar->text() << endl;
#endif
                hff->m_startAtPage = pageNum;
                hff->m_endAtPage = pageNum;
            }
        }
    }
#endif
}
