/* This file is part of the KOffice project
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
#include <algorithm>
#include "KWFrameList.h"
#include "KWFrame.h"
#include "KWFrameSet.h"
#include "KWTextFrameSet.h"
#include "KWDocument.h"
#include "KWViewMode.h"
#include "KWPageManager.h"
//Added by qt3to4:
#include <QList>
#include <Q3PtrList>

//#define DEBUG_SPEED

KWFrameList::KWFrameList(KWDocument *doc, KWFrame *theFrame) {
    m_doc = doc;
    m_frame = theFrame;

    update();
}

QList<KWFrame *> KWFrameList::framesBelow() const {
    QList<KWFrame *> frames;
//kDebug() << "framesBelow " << endl;

    // Copy until we find m_frame
    for ( Q3ValueVector<KWFrame*>::const_iterator it = m_frames.begin(), end = m_frames.end(); it != end && *it != m_frame; ++it) {
        frames.append( *it );
    }

    return  frames;
}

QList<KWFrame *> KWFrameList::framesOnTop() const {
//kDebug() << "framesOnTop " << endl;
    QList<KWFrame *> frames;

    // Copy from m_frame to the end
    bool found = false;
    for ( Q3ValueVector<KWFrame*>::const_iterator it = m_frames.begin(), end = m_frames.end(); it != end; ++it) {
        KWFrame* frame = *it;
        if ( found ) {
            Q_ASSERT( !frame->frameSet()->isFloating() );
            frames.append( frame );
        }
        else if ( frame == m_frame )
            found = true;
    }

    return frames;
}

void KWFrameList::setFrames(const Q3PtrList<KWFrame> &frames) {
#if 0
    // kDebug(31001) << "KWFrameList::setFrames for " << m_frame->frameSet()->name() << endl;
    m_frames.clear();
    if ( m_doc->layoutViewMode() && !m_doc->layoutViewMode()->hasFrames() )
        return;

    Q3PtrList<KWFrameSet> parentFramesets;
    KWFrameSet *fs = m_frame->frameSet();
    while(fs) {
        parentFramesets.append(fs);
        fs = (KWFrameSet*) fs->anchorFrameset();
    }

    // We now look at all other frames (in the same page)
    // to check for intersections. This is o(n^2), but with n small.
    Q3PtrListIterator<KWFrame> it( frames );
    for ( ; it.current() ; ++it )
    {
        KWFrame* daFrame = it.current();
        // kDebug(32001) << "frame: " << daFrame->frameSet()->name() << endl;
        if ( m_frame == daFrame ) {
            m_frames.append( daFrame );
            continue;
        }
        // Skip 'daFrame' if it belongs to a table.
        // We trust that KWTableFrameSet will not make cells overlap ;)
        if ( m_frame->frameSet()->groupmanager() || daFrame->frameSet()->groupmanager() )
            continue;
        // Skip all frames from the parent frameset, if 'm_frame' is floating
        // ## might need a for loop for the case of inline-inside-inline,
        // or maybe calling isPaintedBy instead [depending on what should happen for tables]
        if ( daFrame->frameSet()->isFloating() &&
                (parentFramesets.contains(daFrame->frameSet()->anchorFrameset()) ||
                 daFrame->frameSet()->isPaintedBy(m_frame->frameSet())) )
            continue;
        // Floating frames are not "on top", they are "inside".
        // They are not "below" anything either - the parent frameset is.
        if ( m_frame->frameSet()->isFloating() )
            continue;
        KoRect intersect = m_frame->intersect( daFrame->outerKoRect() );
        if ( !intersect.isEmpty() )
            m_frames.append( daFrame );
    }
    std::sort( m_frames.begin(), m_frames.end(), KWFrame::compareFrameZOrder );
#endif
}

void KWFrameList::updateAfterMove(int oldPageNum) {
    int pageNumber = m_doc->pageManager()->pageNumber(m_frame);
    updateZOrderFor(m_doc->framesInPage( pageNumber, false ));

    if (pageNumber != oldPageNum)
        updateZOrderFor(m_doc->framesInPage( oldPageNum, false ));
}

void KWFrameList::update() {
    int pageNumber = m_doc->pageManager()->pageNumber(m_frame);
    if(pageNumber == -1)
        return;
    updateZOrderFor(m_doc->framesInPage( pageNumber, false ));
}

void KWFrameList::updateZOrderFor(const Q3PtrList<KWFrame> &frames) {
#ifdef DEBUG_SPEED
    kDebug(32001) << "KWFrameList::updateZOrderFor " << frames.count() << " frames"<< endl;
    QTime dt;
    dt.start();
    int numberAdded = 0;
#endif

    Q3PtrListIterator<KWFrame> iter(frames);
    while( iter.current() ) {
        KWFrame *frame = iter.current();
        Q_ASSERT( frame->frameStack() );

        frame->frameStack()->setFrames(frames);
#ifdef DEBUG_SPEED
        numberAdded += frame->frameStack()->m_frames.count();
#endif
        ++iter;
    }

#ifdef DEBUG_SPEED
    kDebug(32001) << "  updateZOrderFor took " << (float)(dt.elapsed()) / 1000 << " seconds, added " << numberAdded << " frames" << endl;
#endif
}

// ****** statics ******
KWFrameList *KWFrameList::getFirstFrameList(KWDocument *doc) {
    for (Q3PtrListIterator<KWFrameSet> fsit = doc->framesetsIterator(); fsit.current() ; ++fsit ) {
        KWFrame *frame = fsit.current()->frame(0);
        if (frame && frame->frameStack())
            return frame->frameStack();
    }
    return 0;
}

void KWFrameList::recalcFrames(KWDocument *doc, int pageFrom, int pageTo) {
    for(int i=pageTo; i >= pageFrom; i--) {
        Q3PtrList<KWFrame> framesOnPage = doc->framesInPage( i, false );
        KWFrame *f = framesOnPage.first();
        while(f) {
            Q_ASSERT(f->frameStack());
            f->frameStack()->setFrames(framesOnPage);
            f = framesOnPage.next();
        }
    }
}

void KWFrameList::recalcAllFrames(KWDocument *doc) {
    recalcFrames(doc, doc->startPage(), doc->lastPage());
}

void KWFrameList::createFrameList(KWFrame *f, KWDocument *doc) {
    Q_ASSERT(f);
    Q_ASSERT(doc);
    if(f->frameStack())
        return;
    f->setFrameStack(new KWFrameList(doc, f));
}

void KWFrameList::createFrameList(KWFrameSet *fs, KWDocument *doc, bool forceUpdate) {
    Q3PtrListIterator<KWFrame> iter( fs->frameIterator() );
    KWFrame *f = iter.current();
    while(f) {
        createFrameList(f, doc);
        if(forceUpdate)
            f->frameStack()->update();
        ++iter;
        f = iter.current();
    }
}
