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

class KWFrameView {
public:
    KWFrameView(KWFrame *frame);

    /// draw the frame background and borders and all the other things that are not actual content
    void drawFrameBase( lotsaargs );
    void drawPadding( foo bar );
    MouseMeaning getMouseMeaning( const KoPoint &nPoint, int keyState );
    QPopup createPopup( const KoPoint &point );

    bool isSelected();
    void setSelected(bool selected);

private:
    KWFrame *m_frame;
    bool m_selected;
    FramePolicy *policy;
};

class FramePolicy {
    MouseMeaning getMouseMeaning( const KoPoint &nPoint, int keyState ) = 0;
    QPopup createPopup( const KoPoint &point ) = 0;
}

class TableFramePolicy : public FramePolicy {
    MouseMeaning getMouseMeaning( const KoPoint &nPoint, int keyState );
    QPopup createPopup( const KoPoint &point );
}

class PartFramePolicy : public FramePolicy {
    MouseMeaning getMouseMeaning( const KoPoint &nPoint, int keyState );
    QPopup createPopup( const KoPoint &point );
}

class TextFramePolicy : public FramePolicy {
    MouseMeaning getMouseMeaning( const KoPoint &nPoint, int keyState );
    QPopup createPopup( const KoPoint &point );
}
