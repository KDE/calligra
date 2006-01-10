/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef frame_set_edit_h
#define frame_set_edit_h

#include <qrect.h>
#include <KoPoint.h>
#include <KoRect.h>

class KWCanvas;
class KWFrame;
class KWFrameSet;
class KWView;
class KWViewMode;
class KWFrameViewManager;

class QDragEnterEvent;
class QDragLeaveEvent;
class QDragMoveEvent;
class QDropEvent;
class QIMEvent;
class QKeyEvent;
class QMimeSource;
class QMouseEvent;
class QPainter;
class QPoint;

/**
 * This object is created to edit a particular frameset in a particular view
 * The view's canvas creates it, and destroys it.
 */
class KWFrameSetEdit
{
public:
    KWFrameSetEdit( KWFrameSet * fs, KWCanvas * canvas );
    virtual ~KWFrameSetEdit() {}

    KWFrameSet * frameSet() const { return m_fs; }
    KWCanvas * canvas() const { return m_canvas; }
    KWFrame * currentFrame() const { return m_currentFrame; }

    /**
     * Return the current most-low-level text edit object
     */
    virtual KWFrameSetEdit* currentTextEdit() { return 0L; }

    /**
     * Called before destruction, when terminating edition - use to e.g. hide cursor
     */
    virtual void terminate(bool /*removeselection*/ = true) {}

    /**
     * Paint this frameset in "has focus" mode (e.g. with a cursor)
     * See KWFrameSet for explanation about the arguments.
     * Most framesets don't need to reimplement that (the KWFrameSetEdit gets passed to drawFrame)
     */
    virtual void drawContents( QPainter *, const QRect &,
                               const QColorGroup &, bool onlyChanged, bool resetChanged,
                               KWViewMode *viewMode,
                               KWFrameViewManager *frameViewManager );

    // Events forwarded by the canvas (when being in "edit" mode)
    virtual void keyPressEvent( QKeyEvent * ) {}
    virtual void keyReleaseEvent( QKeyEvent * ) {}
    virtual void imStartEvent( QIMEvent * ) {}
    virtual void imComposeEvent( QIMEvent * ) {}
    virtual void imEndEvent( QIMEvent * ) {}
    virtual void mousePressEvent( QMouseEvent *, const QPoint &, const KoPoint & ) {}
    virtual void mouseMoveEvent( QMouseEvent *, const QPoint &, const KoPoint & ) {} // only called if button is pressed
    virtual void mouseReleaseEvent( QMouseEvent *, const QPoint &, const KoPoint & ) {}
    virtual void mouseDoubleClickEvent( QMouseEvent *, const QPoint &, const KoPoint & ) {}
    virtual void dragEnterEvent( QDragEnterEvent * ) {}
    virtual void dragMoveEvent( QDragMoveEvent *, const QPoint &, const KoPoint & ) {}
    virtual void dragLeaveEvent( QDragLeaveEvent * ) {}
    virtual void dropEvent( QDropEvent *, const QPoint &, const KoPoint &, KWView* ) {}
    virtual void focusInEvent() {}
    virtual void focusOutEvent() {}
    virtual void copy() {}
    virtual void cut() {}
    virtual void paste() {}
    virtual void selectAll() {}
    virtual void pasteData( QMimeSource* /*data*/, int /*provides*/, bool /*drop*/ ) {}


    /// Called if the cursor tries to leave the frameset at its beginning.
    /// Returns true if the frameset was inline, and we did exit into another frameset.
    bool exitLeft();
    /// Called if the cursor tries to leave the frameset at its end.
    /// Returns true if the frameset was inline, and we did exit into another frameset.
    bool exitRight();

protected:
    KWFrameSet * m_fs;
    KWCanvas * m_canvas;
    /**
     * The Frameset-Edit implementation is responsible for updating that one
     * (to the frame where the current "cursor" is)
     */
    KWFrame * m_currentFrame;
};

#endif
