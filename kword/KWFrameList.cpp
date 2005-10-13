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
#include "KWFrameList.h"
#include "kwframe.h"
#include "kwdoc.h"
#include "kwviewmode.h"

//#define DEBUG_SPEED

KWFrameList::KWFrameList(KWDocument *doc, KWFrame *theFrame) {
    m_doc = doc;
    m_frame = theFrame;

    update();
}

QValueList<KWFrame *> KWFrameList::framesBelow() {
    QValueList<KWFrame *> frames;
//kdDebug() << "framesBelow " << endl;
    int index = m_frames.findRef(m_frame);
    if (index == -1)
        return frames;

    int count = 0;
    KWFrame *frame = m_frames.first();
    while( frame ) {
        if (count++ < index)
            frames.append( frame );
        frame = m_frames.next();
    }

    return  frames;
}

QValueList<KWFrame *> KWFrameList::framesOnTop() {
//kdDebug() << "framesOnTop " << endl;
    QValueList<KWFrame *> frames;
    int index = m_frames.findRef(m_frame);
    if (index == -1)
        return frames;

    KWFrame *frame = m_frames.next();
    while( frame ) {
        frames.append( frame );
        frame = m_frames.next();
    }

    return frames;
}

void KWFrameList::setFrames(const QPtrList<KWFrame> &frames) {
    m_frames.clear();
    if ( m_doc->viewMode() && !m_doc->viewMode()->hasFrames() )
        return;

    QPtrList<KWFrameSet> parentFramesets;
    KWFrameSet *fs = m_frame->frameSet();
    while(fs) {
        parentFramesets.append(fs);
        fs = (KWFrameSet*) fs->anchorFrameset();
    }

    // We now look at all other frames (in the same page)
    // to check for intersections. This is o(n^2), but with n small.
    QPtrListIterator<KWFrame> it( frames );
    for ( ; it.current() ; ++it )
    {
        KWFrame* daFrame = it.current();
        // kdDebug(32001) << "frame: " << daFrame->frameSet()->getName() << endl;
        if ( m_frame == daFrame ) {
            m_frames.append( daFrame );
            continue;
        }
        // Skip 'daFrame' if it belongs to a table.
        // We trust that KWTableFrameSet will not make cells overlap ;)
        if ( m_frame->frameSet()->getGroupManager() || daFrame->frameSet()->getGroupManager() )
            continue;
        // Skip all frames from the parent frameset, if 'm_frame' is floating
        // ## might need a for loop for the case of inline-inside-inline,
        // or maybe calling isPaintedBy instead [depending on what should happen for tables]
        if ( daFrame->frameSet()->isFloating() && parentFramesets.contains(daFrame->frameSet()) )
            continue;
        // Floating frames are not "on top", they are "inside".
        // They are not "below" anything either - the parent frameset is.
        if ( m_frame->frameSet()->isFloating() )
            continue;
        KoRect intersect = m_frame->intersect( daFrame->outerKoRect() );
        if ( !intersect.isEmpty() )
            m_frames.append( daFrame );
    }
    m_frames.sort();
}

void KWFrameList::updateAfterMove(int oldPageNum) {
    int pageNumber = m_doc->getPageOfRect(*m_frame);
    updateZOrderFor(m_doc->framesInPage( pageNumber, false ));

    if (pageNumber != oldPageNum)
        updateZOrderFor(m_doc->framesInPage( oldPageNum, false ));
}

void KWFrameList::update() {
    int pageNumber = m_doc->getPageOfRect(*m_frame);
    updateZOrderFor(m_doc->framesInPage( pageNumber, false ));
}

void KWFrameList::updateZOrderFor(const QPtrList<KWFrame> &frames) {
#ifdef DEBUG_SPEED
    kdDebug(32001) << "KWFrameList::updateZOrderFor " << frames.count() << " frames"<< endl;
    QTime dt;
    dt.start();
    int numberAdded = 0;
#endif

    QPtrListIterator<KWFrame> iter(frames);
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
    kdDebug(32001) << "  updateZOrderFor took " << (float)(dt.elapsed()) / 1000 << " seconds, added " << numberAdded << " frames" << endl;
#endif
}

void KWFrameList::recalcAllFrames() {
    recalcFrames(0, m_doc->numPages());
}

void KWFrameList::recalcFrames(int pageFrom, int pageTo) {
    for(int i=pageFrom; i >= pageTo; i--) {
        QPtrList<KWFrame> framesOnPage = m_doc->framesInPage( i, false );
        KWFrame *f = framesOnPage.first();
        while(f) {
            Q_ASSERT(f->frameStack());
            f->frameStack()->setFrames(framesOnPage);
            f = framesOnPage.next();
        }
    }
}

// ****** statics ******
KWFrameList *KWFrameList::getFirstFrameList(KWDocument *doc) {
    for (QPtrListIterator<KWFrameSet> fsit = doc->framesetsIterator(); fsit.current() ; ++fsit ) {
        KWFrame *frame = fsit.current()->frame(0);
        if (frame && frame->frameStack())
            return frame->frameStack();
    }
    return 0;
}

void KWFrameList::recalcFrames(KWDocument *doc, int pageFrom, int pageTo) {
    KWFrameList *fl = getFirstFrameList(doc);
    if (fl)
        fl->recalcFrames(pageFrom, pageTo);
    else kdDebug(32001)<<"WARN  recalcFrames ("<< pageFrom<< ","<<pageTo<< ") No frame stack found"<<endl;
}

void KWFrameList::recalcAllFrames(KWDocument *doc) {
    KWFrameList *fl = getFirstFrameList(doc);
    if (fl)
        fl->recalcAllFrames();
    else kdDebug(32001)<<"WARN  recalcAllFrames () No frame stack found"<<endl;
}

void KWFrameList::createFrameList(KWFrame *f, KWDocument *doc) {
    if(f->frameStack())
        return;
    f->setFrameStack(new KWFrameList(doc, f));
}

void KWFrameList::createFrameList(KWFrameSet *fs, KWDocument *doc, bool forceUpdate) {
    QPtrListIterator<KWFrame> iter( fs->frameIterator() );
    KWFrame *f = iter.current();
    while(f) {
        createFrameList(f, doc);
        if(forceUpdate)
            f->frameStack()->update();
        ++iter;
        f = iter.current();
    }
}
