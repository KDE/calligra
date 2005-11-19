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
#include <qvaluevector.h>
#include <defs.h>

class KWDocument;
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
    /**
     * Constructor that takes the already existing frames and framesets from doc and
     * adds them.
     */
    KWFrameViewManager(KWDocument *doc);
    virtual ~KWFrameViewManager();

    enum selectionEnum { selected, unselected, nextUnselected, frameOnTop };
    /**
     * Returns a frame positioned at @p point with the highest z-ordering (i.e. on top).
     * @param point the position of the frame
     */
    KWFrameView *view(const KoPoint &point, selectionEnum select, bool borderOnly = false) const;
    KWFrameView *view(const KWFrame *frame) const;

    QValueList<KWFrameView*> selectedFrames() const;
    KWFrameView* selectedFrame() const;

    // this should be changed to a real iterator when Qt4 is used.
    const QValueList<KWFrameView*> frameViewsIterator() const { return m_frames; }

    MouseMeaning mouseMeaning( const KoPoint &point, int keyState) const;
    QCursor mouseCursor( const KoPoint &point, int keyState ) const;

    void showPopup( const KoPoint &point, KWView *view, int keyState, const QPoint &popupPoint) const;

    // listeners; see the fireEvents method signature for more info.
    void addKWFramesListener(KWFramesListener *listener);
    void removeKWFramesListener(KWFramesListener *listener);

    void selectFrames(KoPoint &point, int keyState);

public slots:
    /// notify this slot if a FrameSet has been created and should become visible.
    void slotFrameSetAdded(KWFrameSet *fs);
    /// notify this slot if a FrameSet has been removed
    void slotFrameSetRemoved(KWFrameSet *fs);
    /// notify this slot if a Frame has been created and should become visible.
    void slotFrameAdded(KWFrame *f);
    /// notify this slot if a Frame has been removed
    void slotFrameRemoved(KWFrame *f);
    void slotFrameMoved(KWFrame *f, double previousYPosition);
    void slotFrameResized(KWFrame *f);
    void slotFrameSelectionChanged();

signals:
    void sigFrameSelectionChanged();

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
            enum actionEnum { FrameRemoved, FrameAdded, FrameSetRemoved, FrameSetAdded, FrameMoved, FrameResized, FrameSelectionChanged };
            FrameEvent (actionEnum action);
            FrameEvent (actionEnum action, KWFrame *frame);
            FrameEvent (actionEnum action, KWFrameSet *frameSet);
        private:
            actionEnum m_action;
            KWFrame *m_frame;
            KWFrameSet *m_frameSet;
        friend class KWFrameViewManager;
    };

    /// make sure the caches for pages and frame-hit positions is uptodate.
    void recalculateFrameCache();
    KWFrameView *getViewFor(KWFrame *frame);

    QValueVector<KWFrameView*> framesAt(const KoPoint &point, bool borderOnly = false) const;
    static bool compareFrameViewZOrder(KWFrameView *f1, KWFrameView *f2);

private:
    QValueList<KWFrameView*> m_frames;
    QValueList<KWFramesListener*> m_framesListener;
    QValueList<FrameEvent*> m_frameEvents;
    bool m_queueRequested, m_blockEvents;
};

class KWFramesListener {
public:
    virtual void frameSetAdded(KWFrameSet *fs) = 0;
    virtual void frameSetRemoved(KWFrameSet *fs) = 0;
    virtual void frameAdded(KWFrame *f) = 0;
    virtual void frameRemoved(KWFrame *f) = 0;
    virtual void frameMoved(KWFrame *f) = 0;
    virtual void frameResized(KWFrame *f) = 0;
};

#endif
