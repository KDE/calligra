/* This file is part of the KDE project
   Copyright (C) 2000, 2001 Thomas Zander <zander@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef resizehandles_h
#define resizehandles_h

#include <qwidget.h>

class KWCanvas;
class KWFrame;

/**
 * Class: KWResizeHandle
 * Each of the 8 visible resize corners for a selected frame is an instance of KWResizeHandle.
 * TODO: why is this a widget? Might be easier with it being part of the frame painting and mouse handling.
 */
class KWResizeHandle : public QWidget
{
    Q_OBJECT

public:
    enum Direction {
        LeftUp = 0,
        Up,
        RightUp,
        Right,
        RightDown,
        Down,
        LeftDown,
        Left
    };

    KWResizeHandle( KWCanvas *p, Direction d, KWFrame *frm );
    virtual ~KWResizeHandle();
    void updateGeometry();
    KWCanvas *getCanvas() const { return m_canvas; }

    bool isResizingEnabled() const;
    void applyCursorType();
protected:
    virtual void mouseMoveEvent( QMouseEvent *e );
    virtual void mousePressEvent( QMouseEvent *e );
    virtual void mouseReleaseEvent( QMouseEvent *e );
    virtual void paintEvent( QPaintEvent *e );

private:
    bool mousePressed;
    int oldX, oldY;
    KWCanvas *m_canvas;
    Direction direction;
    KWFrame *frame;
};

#endif
