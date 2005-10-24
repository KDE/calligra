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
#include "KWFrameViewManager.h"
#include <KWFrame.h>
#include <KWFrameSet.h>

#include <qtimer.h>

KWFrameViewManager::KWFrameViewManager() : QObject() {
    m_queueRequested = false;
}
KWFrameViewManager::~KWFrameViewManager() {
    QValueListIterator<FrameEvent *> events = m_frameEvents.begin();
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
    m_frameEvents.append(new FrameEvent(FrameEvent::FrameSetAdded, fs));
    requestFireEvents();
}
void KWFrameViewManager::slotFrameSetRemoved(KWFrameSet *fs) {
    m_frameEvents.append(new FrameEvent(FrameEvent::FrameSetRemoved, fs));
    requestFireEvents();
}
void KWFrameViewManager::slotFrameAdded(KWFrame *f) {
    m_frameEvents.append(new FrameEvent(FrameEvent::FrameAdded, f));
    requestFireEvents();
}
void KWFrameViewManager::slotFrameRemoved(KWFrame *f) {
    m_frameEvents.append(new FrameEvent(FrameEvent::FrameRemoved, f));
    requestFireEvents();
}

void KWFrameViewManager::requestFireEvents() {
    if(m_queueRequested)
        return;
    m_queueRequested = true;
    QTimer::singleShot ( 0, this, SLOT(fireEvents()) );
}

void KWFrameViewManager::fireEvents() {
    QValueList<FrameEvent *> copy(m_frameEvents);
    m_frameEvents.clear();
    m_queueRequested = false;

    QValueList<KWFramesListener *> listenersCopy(m_framesListener);

    QValueListIterator<FrameEvent *> events = copy.begin();
    while(events != copy.end()) {
        FrameEvent *event = *events;
        QValueListIterator<KWFramesListener *> listeners = listenersCopy.begin();
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
}

// ********** FrameEvent **** */
KWFrameViewManager::FrameEvent::FrameEvent (actionEnum action, KWFrame *frame) {
    m_action = action;
    m_frame = frame;
}
KWFrameViewManager::FrameEvent::FrameEvent (actionEnum action, KWFrameSet *frameSet) {
    m_action = action;
    m_frameSet = frameSet;
}


#include "KWFrameViewManager.moc"
