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
class KWFrameView;
class KWFrameViewManager;
class KWFrame;
class KWView;
class KoZoomHandler;
class KoPoint;
class KActionSeparator;
class KAction;
class QPopupMenu;

class FramePolicy {
public:
    FramePolicy(KWFrameView *view);
    virtual MouseMeaning mouseMeaning( const KoPoint &point, int keyState ) = 0;
    virtual QPopupMenu* createPopup( const KoPoint &point, KWView *view ) = 0;
    virtual void setSelected(MouseMeaning) { };

protected:
    virtual MouseMeaning mouseMeaningOnBorder(const KoPoint &point, int keyState);
    void addFloatingAction(KWView *view, QPtrList<KAction> &actionList);

    KWFrameView *m_view;
    KActionSeparator *m_separator;

    friend class KWFrameView; // so it can call mouseMeaningOnBorder which assumes some stuff
};

class KWFrameView {
public:
    KWFrameView(KWFrameViewManager *parent, KWFrame *frame);
    virtual ~KWFrameView();

    bool selected() const { return m_selected; }
    void setSelected(bool selected, MouseMeaning selectPolicy = MEANING_MOUSE_SELECT);

    KWFrame *frame() const { return m_frame; }

    bool isBorderHit(const KoPoint &point) const;
    bool contains(const KoPoint &point, bool withBorders = true) const;

    MouseMeaning mouseMeaning( const KoPoint &point, int keyState );

    KWFrameViewManager *parent() { return m_manager; }

    void showPopup( const KoPoint &point, KWView *view, const QPoint &popupPoint) const;
    void paintFrameAtributes(QPainter *painter, const QRect &crect, KoZoomHandler *zh);

private:
    bool hit(const KoPoint &point, bool withBorders , bool borderOnly) const;

private:
    KWFrame *m_frame;
    bool m_selected;
    FramePolicy *m_policy;
    KWFrameViewManager *m_manager;
};

class TableFramePolicy : public FramePolicy {
public:
    TableFramePolicy(KWFrameView *view);
    MouseMeaning mouseMeaning( const KoPoint &nPoint, int keyState );
    QPopupMenu* createPopup( const KoPoint &point, KWView *view );
    void setSelected(MouseMeaning selectPolicy);

protected:
    MouseMeaning mouseMeaningOnBorder( const KoPoint &point, int keyState);
};

class PartFramePolicy : public FramePolicy {
public:
    PartFramePolicy(KWFrameView *view);
    MouseMeaning mouseMeaning( const KoPoint &nPoint, int keyState );
    QPopupMenu* createPopup( const KoPoint &point, KWView *view );
};

class TextFramePolicy : public FramePolicy {
public:
    TextFramePolicy(KWFrameView *view);
    MouseMeaning mouseMeaning( const KoPoint &nPoint, int keyState );
    QPopupMenu* createPopup( const KoPoint &point, KWView *view );
};

class ImageFramePolicy : public FramePolicy {
public:
    ImageFramePolicy(KWFrameView *view);
    MouseMeaning mouseMeaning( const KoPoint &nPoint, int keyState );
    QPopupMenu* createPopup( const KoPoint &point, KWView *view );
};
#endif
