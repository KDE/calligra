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
#include <q3ptrlist.h>
#include <q3valuelist.h>
#include <q3valuevector.h>
#include <defs.h>
#include <QPointer>
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

class QRegion;

/**
 * Class that knows about all the (visible) frames in the document and will act as
 * a manager between the GUI code and the data objects to couple any GUI action to the
 * right frame.
 * All coordinates used in this object are point (pt) based, i.e. the same as in KWFrame. Any
 * coordinates in pixels should first be converted in the KoZoomHandler (in KWord
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

    /**
     * Used to change the behavior of view()<dl>
     * <dt>selected</dt>
     *   <dd>return the first selected with the highest z-ordering (i.e. on top).</dd>
     * <dt>unselected</dt>
     *   <dd>return the first unselected on top.</dd>
     * <dt>nextUnselected</dt>
     *    <dd>return the first unselected directly under a selected frame, or
     *    the top most one if nothing is selected.</dd>
     * <dt>frameOnTop</dt>
     *    <dd>return the frame highest z-ordering, regardless of selection.</dd>
     * </dl>
     */
    enum SelectionType { selected, unselected, nextUnselected, frameOnTop };
    /**
     * Returns a frameView representing a frame positioned at @p point, or 0 when no match found.
     * @param point the position of the frame
     * @param select this alters the behavior of which frame to return if more then one
     *   exist at the appointed location.
     * @param borderOnly If true frames only frames that have the border at the point will be
     *  looked at.
     */
    KWFrameView *view(const KoPoint &point, SelectionType select, bool borderOnly = false) const;
     /** Returns a frameView representing @p frame */
    KWFrameView *view(const KWFrame *frame) const;

    /**
     * Return all currently selected frames.
     */
    Q3ValueList<KWFrameView*> selectedFrames() const;
    /**
     * Returns the first selected frame.
     * Is the same as selectedFrames()[0]
     */
    KWFrameView* selectedFrame() const;

    // this should be changed to a real iterator when Qt4 is used.
    const Q3ValueList<KWFrameView*> frameViewsIterator() const { return m_frames; }

    /**
     * Return the MouseMeaning of what a click of the mouse would do at the @p point.
     * All the frames that have a presence at the point are considered and depending on
     * what kind of frame and where in the frame the mouse is the meaning is calculated.
     * @param point the point where the mouse is hovering.
     * @param keyState the bitmask of keys that are pressed.  Same as Event::state();
     */
    MouseMeaning mouseMeaning( const KoPoint &point, int keyState) const;
    /**
     * Similar to mouseMeaning(), but instead of returning the meaning this will return
     * a mouse cursor to represent the action.
     * @param point the point where the mouse is hovering.
     * @param keyState the bitmask of keys that are pressed.  Same as Event::state();
     */
    QCursor mouseCursor( const KoPoint &point, int keyState ) const;

    /**
     * Show a context-sensitive popup menu based on the location of 'point'.
     * @param point the point at which the mouse was clicked. The context is based on
     *   what is present at that location.
     * @param keyState the bitmask of keys that are pressed.  Same as Event::state();
     * @param popupPoint the point in the same coordinate system as the parent widget of
     *   where the popup menu should be located.
     * @param view the parent widget for the popup.
     */
    void showPopup( const KoPoint &point, KWView *view, int keyState, const QPoint &popupPoint) const;

    // listeners; see the fireEvents method signature for more info.
    void addKWFramesListener(KWFramesListener *listener);
    void removeKWFramesListener(KWFramesListener *listener);

    /**
     * Select frames based on a mouse click at @p point using @p keystate.
     * Handles the click of a mouse and searches for frames at the location selecting and
     * / or unselecting any frames based on this information.
     * @param leftClick true if this select is due to the main button being clicked.
     * @param point the point clicked.
     * @param keyState the bitmask of keys that are pressed.  Same as Event::state();
     */
    void selectFrames(const KoPoint &point, int keyState, bool leftClick);

public slots:
    /// notify this slot if a FrameSet has been created and should become visible.
    void slotFrameSetAdded(KWFrameSet *fs);
    /// notify this slot if a FrameSet has been removed
    void slotFrameSetRemoved(KWFrameSet *fs);
    /// notify this slot if a Frame has been created and should become visible.
    void slotFrameAdded(KWFrame *f);
    /// notify this slot if a Frame has been removed
    void slotFrameRemoved(KWFrame *f);
    /**
     * notify this slot if a Frame has been moved
     * @param f the frame
     * @param previousYPosition the pt-based location of the frame before it was moved.
     *  This is used to update any views in a more intelligent matter.
     */
    void slotFrameMoved(KWFrame *f, double previousYPosition);
    /// notify this slot if a Frame has been resized
    void slotFrameResized(KWFrame *f);
    /// notify this slot if one or more frames have been selected or unselected.
    void slotFrameSelectionChanged();
    /// notify this slot if a frameset was renamed
    void slotFrameSetRenamed(KWFrameSet *fs);

signals:
    /// emitted after one or more incoming slotFrameSelectionChanged events.
    void sigFrameSelectionChanged();
    /// emitted after a frameset that had at least one selected frame was renamed.
    void sigFrameSetRenamed();
    /// emitted after frames were resized
    void sigFrameResized(const Q3ValueList<KWFrame*>&);
    /// emitted after frames were moved
    void sigFrameMoved(const Q3ValueList<KWFrame*>&);

protected slots:
    /**
     * This method will propagate all the events saved up to the listeners.
     * You don't normally want to call this method; call requestFireEvents() instead
     * which will eventually call this method for you.
     */
    void fireEvents();

protected:
    /// prepare or update the singleshot timer to fire events.
#ifndef NDEBUG
    virtual
#endif
    void requestFireEvents();

private:
    /**  Internal class to store FrameEvents in (from the slots) until they are fired later */
    class FrameEvent {
        public:
            enum ActionType { FrameRemoved, FrameAdded, FrameSetRemoved, FrameSetAdded, FrameMoved,
                FrameResized, FrameSelectionChanged, FrameSetRenamed };
            FrameEvent (ActionType action);
            FrameEvent (ActionType action, KWFrame *frame);
            FrameEvent (ActionType action, KWFrameSet *frameSet);
        private:
            ActionType m_action;
            KWFrame *m_frame;
            KWFrameSet *m_frameSet;
        friend class KWFrameViewManager;
    };

    /// make sure the caches for pages and frame-hit positions is uptodate.
    void recalculateFrameCache();
    /// return the KWFrameView for a specific frame
    KWFrameView *getViewFor(KWFrame *frame);

    /**
     * Returns a sorted list of KWFrameView objects that represents frames present at @p point
     * @param point the location the frame should occupy
     * @param borderOnly if true, only return frames that are hit in the border by point.
     */
    Q3ValueVector<KWFrameView*> framesAt(const KoPoint &point, bool borderOnly = false) const;
    /**
     * This is a method used to sort a list using the STL sorting methods.
     * @param f1 the first object
     * @param f2 the second object
     */
    static bool compareFrameViewZOrder(KWFrameView *f1, KWFrameView *f2);

private:
    Q3ValueList<KWFrameView*> m_frames;
    Q3ValueList<KWFramesListener*> m_framesListener;
    Q3ValueList<FrameEvent*> m_frameEvents;
    bool m_queueRequested, m_blockEvents;
};

class KWFramesListener {
public:
	virtual ~KWFramesListener(){}
    virtual void frameSetAdded(KWFrameSet *fs) = 0;
    virtual void frameSetRemoved(KWFrameSet *fs) = 0;
    virtual void frameAdded(KWFrame *f) = 0;
    virtual void frameRemoved(KWFrame *f) = 0;
};

#endif
