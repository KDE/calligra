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
#ifndef kwframeviewmanager_h
#define kwframeviewmanager_h
#include <qobject.h>
#include <qptrlist.h>
#include <qvaluelist.h>
#include <defs.h>

class KWFrame;
class KWFrameSet;
class KWView;
class KWViewMode;
class KWFramesListener;
class KWFrameSetEdit;
class KWFrameView;

class KoPoint;
class KoRect;

class QPointer;
class QRegion;

/**
 * Class that knows about all the (visible) frames in the document and will act as
 * a manager between the GUI code and the data objects to couple any GUI action to the
 * right frame.
 * All coordinates used in this object are pt based, i.e. the same as the Frames. Any
 * coordinates in pixels should first be converted in the KoTextZoomHandler (in KWord
 * thats the KWDocument)
 */
class KWFrameViewManager : public QObject {
    Q_OBJECT

public:
    KWFrameViewManager();
    virtual ~KWFrameViewManager();

    // listeners; see the fireEvents method signature for more info.
    void addKWFramesListener(KWFramesListener *listener);
    void removeKWFramesListener(KWFramesListener *listener);

public slots:
    /// notify this slot if a FrameSet has been created and should become visible.
    void slotFrameSetAdded(KWFrameSet *fs);
    /// notify this slot if a FrameSet has been removed
    void slotFrameSetRemoved(KWFrameSet *fs);
    /// notify this slot if a Frame has been created and should become visible.
    void slotFrameAdded(KWFrame *f);
    /// notify this slot if a Frame has been removed
    void slotFrameRemoved(KWFrame *f);

protected slots:
    /**
     * Various classes are not, not should be heavyweight QObjects, but still want to
     * keep uptodate about frames being deleted.  This method will propagate all the
     * events saved up to the listeners.
     * Note; this method should be called _after_ all the events from the slots have
     * come in, which is needed to minimize race conditions.  So; first update all the
     * data objects and when that is done all the views will be updated.
     */
    void fireEvents();

protected:
    /// prepare or update the singleshot timer to fire events.
#ifndef NDEBUG
    virtual
#endif
    void requestFireEvents();

private:
    class FrameEvent {
        public:
            enum actionEnum { FrameRemoved, FrameAdded, FrameSetRemoved, FrameSetAdded };
            FrameEvent (actionEnum action, KWFrame *frame);
            FrameEvent (actionEnum action, KWFrameSet *frameSet);
        private:
            actionEnum m_action;
            KWFrame *m_frame;
            KWFrameSet *m_frameSet;
        friend class KWFrameViewManager;
    };

private:
    QValueList<KWFramesListener*> m_framesListener;
    QValueList<FrameEvent*> m_frameEvents;
    bool m_queueRequested;
};

class KWFramesListener {
public:
    virtual void frameSetAdded(KWFrameSet *fs) = 0;
    virtual void frameSetRemoved(KWFrameSet *fs) = 0;
    virtual void frameAdded(KWFrame *f) = 0;
    virtual void frameRemoved(KWFrame *f) = 0;
};

#endif
