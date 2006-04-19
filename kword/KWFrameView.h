/* This file is part of the KOffice project
 * Copyright (C) 2005 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; version 2.
 *
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
#ifndef kwframeview_h
#define kwframeview_h

#include "defs.h"
//Added by qt3to4:
#include <Q3PopupMenu>
#include <Q3PtrList>
class KWFrameView;
class KWFrameViewManager;
class KWFrame;
class KWView;
class KWViewMode;
class KoZoomHandler;
class KoPoint;
class KActionSeparator;
class KAction;
class Q3PopupMenu;

/**
 * This base class is an interface for Policies used in the KWFrameView.
 */
class FramePolicy {
public:
    /** Constructor */
    FramePolicy(KWFrameView *view);
    virtual ~FramePolicy() {}
    /**
     * Return the MouseMeaning enum value for the parent view.
     * @param point the point where the mouse is hovering.
     * @param keyState the bitmask of keys that are pressed.  Same as Event::state();
     */
    virtual MouseMeaning mouseMeaning( const KoPoint &point, int keyState ) = 0;
    /**
     * Return a fully initialized popup for the context of frame at @p point
     * @param point the point where the mouse is hovering.
     * @param view the parent view widget
     */
    virtual Q3PopupMenu* createPopup( const KoPoint &point, KWView *view ) = 0;
    /**
     * Override this method to do more than select the parent frame-view
     */
    virtual void setSelected(MouseMeaning) { }

protected:
    /**
     * Shared method for all policies called when the border of the frame is clicked.
     * @param point the point where the mouse is hovering.
     * @param keyState the bitmask of keys that are pressed.  Same as Event::state();
     */
    virtual MouseMeaning mouseMeaningOnBorder(const KoPoint &point, int keyState);
    /**
     * Shared method for all policies to add the 'set floating' action to a popup menu.
     * @param view the parent view widget
     * @param actionList the list of actions where the floating action should be added to
     */
    void addFloatingAction(KWView *view, QList<KAction*> &actionList);

    KWFrameView *m_view;
    KActionSeparator *m_separator;

    friend class KWFrameView; // so it can call mouseMeaningOnBorder which assumes some stuff
};

/**
 * an instance of a KWFrameView represents the view in the MVC model of a frame; there can be
 * multiple KWFrameView objects per frame. Typically there is one per view (KWView).
 * This class registers selectedness and has methods to show the frame and its interaction methods
 */
class KWFrameView {
public:
    /**
     * Constructor
     * @param parent the parent
     * @param frame the frame this view represents.
     */
    KWFrameView(KWFrameViewManager *parent, KWFrame *frame);
    virtual ~KWFrameView();

    /// returns if this frameView is selected.  A selected frame is shown differently on screen.
    bool selected() const { return m_selected; }
    /**
     * Set the selected state of this frameView.
     * @param selected the new selected state
     * @param selectPolicy an optionaly mouseMeaning at which the mouse was clicked to
     *  make this selection happen.  See the FramePolicy::setSelected() for more info.
     */
    void setSelected(bool selected, MouseMeaning selectPolicy = MEANING_MOUSE_SELECT);

    /// returns the frame this frameView represents
    KWFrame *frame() const { return m_frame; }

    /// returns if the @p point is on the border
    bool isBorderHit(const KoPoint &point) const;
    /// returns if the @p point is anywhere in te frame, if fuzzy is true; also a little outside
    bool contains(const KoPoint &point, bool fuzzy = true) const;

    /**
     * Return the MouseMeaning enum value for the parent view.
     * @param point the point where the mouse is hovering.
     * @param keyState the bitmask of keys that are pressed.  Same as Event::state();
     */
    MouseMeaning mouseMeaning( const KoPoint &point, int keyState );

    /// Return the parent KWFrameViewManager this frameView belongs to
    KWFrameViewManager *parent() const { return m_manager; }

    /**
     * Show a context-sensitive popup menu based on the location of 'point'.
     * @param point the point at which the mouse was clicked. The context is based on
     *   what is present at that location.
     * @param popupPoint the point in the same coordinate system as the parent widget of
     *   where the popup menu should be located.
     * @param view the parent widget for the popup.
     */
    void showPopup( const KoPoint &point, KWView *view, const QPoint &popupPoint) const;

    /**
     * Paint view based items of the frameView, like selected state
     * @param painter the painter to paint to
     * @param crect the clip rect; nothing outside this rect is important
     * @param zh the zoomHandler to convert from internal coordinates to view coordinates
     * @param vm the current Viewmode.
     */
    void paintFrameAttributes(QPainter *painter, const QRect &crect, KWViewMode *vm, KoZoomHandler *zh);

private:
    /**
     * Helper method for isBorderHit() and contains()
     * @param point the point
     * @param borderOnly if true; exclude the range inside the frame
     * @param fuzzy don't take all the coordinates too strict, allow for some margin
     */
    bool hit(const KoPoint &point, bool fuzzy, bool borderOnly) const;

private:
    KWFrame *m_frame;
    bool m_selected;
    FramePolicy *m_policy;
    KWFrameViewManager *m_manager;
};

/**  A policy for Table Frames */
class TableFramePolicy : public FramePolicy {
public:
	virtual ~TableFramePolicy(){}
    TableFramePolicy(KWFrameView *view);
    MouseMeaning mouseMeaning( const KoPoint &point, int keyState );
    Q3PopupMenu* createPopup( const KoPoint &point, KWView *view );
    void setSelected(MouseMeaning selectPolicy);

protected:
    MouseMeaning mouseMeaningOnBorder( const KoPoint &point, int keyState);
};

/**  A policy for Part Frames */
class PartFramePolicy : public FramePolicy {
public:
	virtual ~PartFramePolicy() {}
    PartFramePolicy(KWFrameView *view);
    MouseMeaning mouseMeaning( const KoPoint &point, int keyState );
    Q3PopupMenu* createPopup( const KoPoint &point, KWView *view );
};

/**  A policy for Text Frames */
class TextFramePolicy : public FramePolicy {
public:
	virtual ~TextFramePolicy(){}		
    TextFramePolicy(KWFrameView *view);
    MouseMeaning mouseMeaning( const KoPoint &point, int keyState );
    Q3PopupMenu* createPopup( const KoPoint &point, KWView *view );
};

/**  A policy for Image (aka Picture) Frames */
class ImageFramePolicy : public FramePolicy {
public:
	virtual ~ImageFramePolicy() {}
    ImageFramePolicy(KWFrameView *view);
    MouseMeaning mouseMeaning( const KoPoint &point, int keyState );
    Q3PopupMenu* createPopup( const KoPoint &point, KWView *view );
};
#endif
