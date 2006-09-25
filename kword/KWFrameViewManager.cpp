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
#include "KWDocument.h"
#include "KWFrameViewManager.h"
#include <KWFrameView.h>
#include <KWFrame.h>
#include <KWFrameSet.h>
#include "KWView.h"

#include <kdebug.h>
#include <kcursor.h>
#include <QCursor>
#include <QTimer>
#include "q3popupmenu.h"
//Added by qt3to4:
#include <Q3ValueList>
#include <algorithm>

KWFrameViewManager::KWFrameViewManager() : QObject() {
    setName("KWFrameViewManager");
    m_queueRequested = false;
    m_blockEvents = false;
}

KWFrameViewManager::KWFrameViewManager(KWDocument *doc) {
    m_queueRequested = false;
    m_blockEvents = true;
    Q3PtrListIterator<KWFrameSet> frameSets = doc->framesetsIterator();
    while(frameSets.current()) {
        slotFrameSetAdded(*frameSets);
        ++frameSets;
    }
    m_blockEvents = false;
    connect(doc, SIGNAL( sigFrameSetAdded(KWFrameSet*)), SLOT( slotFrameSetAdded(KWFrameSet*)));
    connect(doc, SIGNAL( sigFrameSetRemoved(KWFrameSet*)), SLOT( slotFrameSetRemoved(KWFrameSet*)));
    recalculateFrameCache();
}

KWFrameViewManager::~KWFrameViewManager() {
    Q3ValueListIterator<FrameEvent *> events = m_frameEvents.begin();
    while(events != m_frameEvents.end()) {
        delete (*events);
        ++events;
    }
}

void KWFrameViewManager::addKWFramesListener(KWFramesListener *listener) {
    m_framesListener.append(listener);
}
void KWFrameViewManager::removeKWFramesListener(KWFramesListener *listener) {
    m_framesListener.remove(listener);
}

void KWFrameViewManager::slotFrameSetAdded(KWFrameSet *fs) {
    if(! m_blockEvents)
        m_frameEvents.append(new FrameEvent(FrameEvent::FrameSetAdded, fs));
    connect(fs, SIGNAL( sigFrameAdded(KWFrame*)), SLOT( slotFrameAdded(KWFrame *)));
    connect(fs, SIGNAL( sigFrameRemoved(KWFrame*)), SLOT( slotFrameRemoved(KWFrame *)));
    connect(fs, SIGNAL( sigNameChanged(KWFrameSet*)), SLOT( slotFrameSetRenamed(KWFrameSet *)));
    Q3PtrListIterator<KWFrame> frames = fs->frameIterator();
    while(frames.current()) {
        KWFrame *f = frames.current();
        slotFrameAdded(f);
        ++frames;
    }
    requestFireEvents();
}
void KWFrameViewManager::slotFrameSetRemoved(KWFrameSet *fs) {
    if(! m_blockEvents)
        m_frameEvents.append(new FrameEvent(FrameEvent::FrameSetRemoved, fs));
    disconnect(fs, SIGNAL( sigFrameAdded(KWFrame*)), this, SLOT( slotFrameAdded(KWFrame *)));
    disconnect(fs, SIGNAL( sigFrameRemoved(KWFrame*)), this, SLOT( slotFrameRemoved(KWFrame *)));
    disconnect(fs, SIGNAL( sigNameChanged(KWFrameSet*)), this, SLOT( slotFrameSetRenamed(KWFrameSet *)));
    Q3PtrListIterator<KWFrame> frames = fs->frameIterator();
    while(frames.current()) {
        KWFrame *f = frames.current();
        slotFrameRemoved(f);
        ++frames;
    }
    requestFireEvents();
}
void KWFrameViewManager::slotFrameAdded(KWFrame *f) {
    if(! m_blockEvents)
        m_frameEvents.append(new FrameEvent(FrameEvent::FrameAdded, f));
    m_frames.append(new KWFrameView(this, f));
    requestFireEvents();
}
void KWFrameViewManager::slotFrameRemoved(KWFrame *f) {
    if(! m_blockEvents)
        m_frameEvents.append(new FrameEvent(FrameEvent::FrameRemoved, f));
    Q3ValueListIterator<KWFrameView *> frames = m_frames.begin();
    while(frames != m_frames.end()) {
        KWFrameView *fv = *frames;
        if(fv->frame() == f) {
            if(fv->selected())
                m_frameEvents.append(new FrameEvent(FrameEvent::FrameSelectionChanged));
            m_frames.remove(frames);
            delete fv;
            break;
        }
        ++frames;
    }
    requestFireEvents();
}
void KWFrameViewManager::slotFrameMoved(KWFrame *f, double previousYPosition) {
    Q_UNUSED(previousYPosition); // to be used for the page caches to mark them dirty
    if(! m_blockEvents)
        m_frameEvents.append(new FrameEvent(FrameEvent::FrameMoved, f));
    // TODO; KWFrameList update??
    // update our caches..
    requestFireEvents();
}
void KWFrameViewManager::slotFrameResized(KWFrame *f) {
    if(! m_blockEvents)
        m_frameEvents.append(new FrameEvent(FrameEvent::FrameResized, f));
    // update our caches..
    requestFireEvents();
}
void KWFrameViewManager::slotFrameSelectionChanged() {
    if(! m_blockEvents)
        m_frameEvents.append(new FrameEvent(FrameEvent::FrameSelectionChanged));
    // update our caches..
    requestFireEvents();
}

void KWFrameViewManager::slotFrameSetRenamed(KWFrameSet *fs) {
    if(! m_blockEvents)
        m_frameEvents.append(new FrameEvent(FrameEvent::FrameSetRenamed, fs));
    requestFireEvents();
}

void KWFrameViewManager::requestFireEvents() {
    if(m_queueRequested && !m_blockEvents)
        return;
    m_queueRequested = true;
    QTimer::singleShot ( 0, this, SLOT(fireEvents()) );
}

void KWFrameViewManager::fireEvents() {
    m_queueRequested = false;
    if(m_frameEvents.isEmpty())
        return;
    recalculateFrameCache();

    Q3ValueList<FrameEvent *> copy(m_frameEvents);
    m_frameEvents.clear();

    Q3ValueList<KWFrame*> resizedFrames;
    Q3ValueList<KWFrame*> movedFrames;
    Q3ValueList<KWFramesListener *> listenersCopy(m_framesListener);
    bool selectionChangedFired=false;

    Q3ValueListIterator<FrameEvent *> events = copy.begin();
    while(events != copy.end()) {
        FrameEvent *event = *events;

        // emit based.
        if(!selectionChangedFired && event->m_action == FrameEvent::FrameSelectionChanged) {
            emit sigFrameSelectionChanged();
            selectionChangedFired = true;  // only fire ones.
        } else if(event->m_action == FrameEvent::FrameSetRenamed) {
            Q3PtrListIterator<KWFrame> frames = event->m_frameSet->frameIterator();
            for(;frames.current();++frames) {
                if(view(frames.current())->selected()) {
                    emit sigFrameSetRenamed();
                    break;
                }
            }
        } else if(event->m_action == FrameEvent::FrameResized) {
            resizedFrames.append(event->m_frame);
        } else if(event->m_action == FrameEvent::FrameMoved) {
            movedFrames.append(event->m_frame);
        }

        // listener based
        Q3ValueListIterator<KWFramesListener *> listeners = listenersCopy.begin();
        while(listeners != listenersCopy.end()) {
            if(event->m_action == FrameEvent::FrameRemoved)
                (*listeners)->frameRemoved(event->m_frame);
            else if(event->m_action == FrameEvent::FrameAdded)
                (*listeners)->frameAdded(event->m_frame);
            else if(event->m_action == FrameEvent::FrameSetRemoved)
                (*listeners)->frameSetRemoved(event->m_frameSet);
            else if(event->m_action == FrameEvent::FrameSetAdded)
                (*listeners)->frameSetAdded(event->m_frameSet);
            ++listeners;
        }

        delete event;
        events = copy.remove(events);
    }
    if(resizedFrames.count() > 0)
        emit sigFrameResized(resizedFrames);
    if(movedFrames.count() > 0)
        emit sigFrameMoved(movedFrames);
}

void KWFrameViewManager::recalculateFrameCache() {
    // TODO :) design and implement a cache...
    // list of frames sorted on y-coord, with an additional list containing a jump-index
    kDebug(31001) << "recalculateFrameCache " << m_frames.count() << " frames are currently registered" << endl;
}

KWFrameView *KWFrameViewManager::view(const KoPoint &point, SelectionType selected, bool borderOnly) const {
    Q3ValueVector<KWFrameView*> framesThatAreHit = framesAt(point, borderOnly);
    bool foundCycleFrame = false;
    Q3ValueVector<KWFrameView*>::iterator sortedFrames = framesThatAreHit.begin();
    while(sortedFrames != framesThatAreHit.end()) {
        if(selected == nextUnselected) {
            if((*sortedFrames)->selected() )
                foundCycleFrame = true;
            else if(foundCycleFrame && !(*sortedFrames)->selected() )
                return *sortedFrames;
        }
        else if(selected == frameOnTop)
            return *sortedFrames;
        else { // not cycle, so simply a selected check.
            if((*sortedFrames)->selected() == (selected == KWFrameViewManager::selected ))
                return *sortedFrames;
        }
        ++sortedFrames;
    }
    if(selected == nextUnselected && framesThatAreHit.count() > 0)
        return framesThatAreHit[0];
    return 0;
}

Q3ValueVector<KWFrameView*> KWFrameViewManager::framesAt(const KoPoint &point, bool borderOnly) const {
    Q3ValueVector<KWFrameView*> framesThatAreHit;
    // This is probably the slowest and worst way to do it, mark this for optimalisation!
    for(Q3ValueListConstIterator<KWFrameView*> frames = m_frames.begin();
            frames != m_frames.end(); ++frames) {
        if(! (*frames)->frame()->frameSet()->isVisible())
            continue;
        if(borderOnly && (*frames)->isBorderHit(point) ||
                !borderOnly && (*frames)->contains(point))
            framesThatAreHit.append(*frames);
    }
    // sort on z-ordering; top on first
    std::sort(framesThatAreHit.begin(), framesThatAreHit.end(), compareFrameViewZOrder);
    return framesThatAreHit;
}

KWFrameView *KWFrameViewManager::view(const KWFrame *frame) const {
    // This is probably the slowest and worst way to do it, mark this for optimalisation!
    Q3ValueListConstIterator<KWFrameView*> frames = m_frames.begin();
    while(frames != m_frames.end()) {
        if((*frames)->frame() == frame)
            return *frames;
        ++frames;
    }
    return 0;
}

bool KWFrameViewManager::compareFrameViewZOrder(KWFrameView *f1, KWFrameView *f2) {
    return f1->frame()->zOrder() >= f2->frame()->zOrder();
}

QCursor KWFrameViewManager::mouseCursor( const KoPoint &point, int keyState ) const {
    KWFrameView *view = 0;
    Q3ValueVector<KWFrameView*> framesThatAreHit = framesAt(point);
    Q3ValueVector<KWFrameView*>::iterator sortedFrames = framesThatAreHit.begin();
    MouseMeaning meaning;
    while(sortedFrames != framesThatAreHit.end()) {
        meaning = (*sortedFrames)->mouseMeaning(point, keyState);
        if(meaning != MEANING_NONE) {
            view = (*sortedFrames);
            break;
        }
        ++sortedFrames;
    }

    if(view == 0)
        return QCursor(); // default arrow shape

    KWFrameSet*frameSet = view->frame()->frameSet();
    switch ( meaning ) {
        case MEANING_NONE:
            return Qt::IBeamCursor; // default cursor in margins
        case MEANING_MOUSE_INSIDE:
            return QCursor(); // default arrow shape
        case MEANING_MOUSE_INSIDE_TEXT:
            return Qt::IBeamCursor;
        case MEANING_MOUSE_OVER_LINK:
            return Qt::PointingHandCursor;
        case MEANING_MOUSE_OVER_FOOTNOTE:
            return Qt::PointingHandCursor;
        case MEANING_MOUSE_MOVE:
            return Qt::SizeAllCursor;
        case MEANING_MOUSE_SELECT:
            return KCursor::handCursor();
        case MEANING_ACTIVATE_PART:
            return KCursor::handCursor();
        case MEANING_TOPLEFT:
        case MEANING_BOTTOMRIGHT:
            return Qt::SizeFDiagCursor;
        case MEANING_LEFT:
        case MEANING_RIGHT:
            return Qt::SizeHorCursor;
        case MEANING_BOTTOMLEFT:
        case MEANING_TOPRIGHT:
            return Qt::SizeBDiagCursor;
        case MEANING_TOP:
        case MEANING_BOTTOM:
            if ( frameSet->isProtectSize() || frameSet->isMainFrameset())
                return Qt::ForbiddenCursor;
            return Qt::SizeHorCursor;
        case MEANING_RESIZE_COLUMN:
            // Bug in Qt up to Qt-3.1.1 : Qt::SplitVCursor and Qt::SplitHCursor are swapped!
#if QT_VERSION <= 0x030101
            return Qt::SplitVCursor;
#else
            return Qt::SplitHCursor;
#endif
        case MEANING_RESIZE_ROW:
#if QT_VERSION <= 0x030101
            return Qt::SplitHCursor;
#else
            return Qt::SplitVCursor;
#endif
        case MEANING_SELECT_RANGE:
        case MEANING_SELECT_COLUMN:
        case MEANING_SELECT_ROW:
        case MEANING_FORBIDDEN:
            return KCursor::handCursor();
    }
    return QCursor(); // default arrow shape
}

MouseMeaning KWFrameViewManager::mouseMeaning( const KoPoint &point, int keyState) const {
    Q3ValueVector<KWFrameView*> framesThatAreHit = framesAt(point);
    Q3ValueVector<KWFrameView*>::iterator sortedFrames = framesThatAreHit.begin();
    while(sortedFrames != framesThatAreHit.end()) {
        MouseMeaning answer = (*sortedFrames)->mouseMeaning(point, keyState);
        if(answer != MEANING_NONE) {
            //kDebug() << "mouseMeaning at " << point << " is " << answer << endl;
            return answer;
        }
        ++sortedFrames;
    }
    return MEANING_NONE;
}

Q3ValueList<KWFrameView*> KWFrameViewManager::selectedFrames() const {
    Q3ValueList<KWFrameView*> selectedFrames;

    Q3ValueList<KWFrameView*>::const_iterator frames = m_frames.begin();
    for(; frames != m_frames.end(); ++frames )
        if( (*frames)->selected() )
            selectedFrames.append( *frames );
    return selectedFrames;
}

KWFrameView* KWFrameViewManager::selectedFrame() const {
    Q3ValueList<KWFrameView*>::const_iterator frames = m_frames.begin();
    for(; frames != m_frames.end(); ++frames )
        if( (*frames)->selected() )
            return *frames;
    return 0;
}

void KWFrameViewManager::showPopup( const KoPoint &point, KWView *view, int keyState, const QPoint &popupPoint) const {
    Q3ValueVector<KWFrameView*> framesThatAreHit = framesAt(point);
    if(framesThatAreHit.count() == 0) {
        view->popupMenu("action_popup")->popup(popupPoint);
        return;
    }
    if(keyState == Qt::ControlModifier) {
        // show the border popup of the top most frame.
// TODO reenable
        //framesThatAreHit[0]->showPopup(framesThatAreHit[0]->frame()->position(), view, popupPoint);
        return;
    }
    Q3ValueVector<KWFrameView*>::iterator iter = framesThatAreHit.begin();
    while(iter != framesThatAreHit.end()) {
        if( (*iter)->selected() && keyState == Qt::ControlModifier ) {
            (*iter)->showPopup(point, view, popupPoint);
            return;
        }
        ++iter;
    }
    framesThatAreHit[0]->showPopup(point, view, popupPoint);
}

void KWFrameViewManager::selectFrames(const KoPoint &point, int keyState, bool leftClick) {
    MouseMeaning mm = mouseMeaning(point, keyState);
    bool multiSelect = mm == MEANING_MOUSE_SELECT || ( keyState & Qt::ControlModifier );
    SelectionType se = frameOnTop;
    if(leftClick && multiSelect)
        se = nextUnselected;
    KWFrameView *toBeSelected = view(point, se, !multiSelect);
    //kDebug() << "KWFrameViewManager::selectFrames" << point << " got: " << toBeSelected << endl;
    if(toBeSelected == 0 || (keyState & Qt::ControlModifier) == 0 || ( keyState & Qt::ShiftModifier ) &&
        !(leftClick && (mm == MEANING_TOPLEFT || mm == MEANING_TOPRIGHT || mm == MEANING_TOP ||
        mm == MEANING_LEFT || mm == MEANING_RIGHT || mm == MEANING_MOUSE_MOVE ||
        mm == MEANING_BOTTOMLEFT || mm == MEANING_BOTTOM || mm == MEANING_BOTTOMRIGHT))) {
        // unselect all
        for(Q3ValueListConstIterator<KWFrameView*> frames = m_frames.begin();
                frames != m_frames.end(); ++frames) {
            (*frames)->setSelected(false);
        }
    }
    if(toBeSelected == 0)
        return;
    toBeSelected->setSelected(true, mm);
    slotFrameSelectionChanged();
}

// ********** FrameEvent **** */
KWFrameViewManager::FrameEvent::FrameEvent (ActionType action) {
    m_action = action;
}
KWFrameViewManager::FrameEvent::FrameEvent (ActionType action, KWFrame *frame) {
    m_action = action;
    m_frame = frame;
}
KWFrameViewManager::FrameEvent::FrameEvent (ActionType action, KWFrameSet *frameSet) {
    m_action = action;
    m_frameSet = frameSet;
}


#include "KWFrameViewManager.moc"
