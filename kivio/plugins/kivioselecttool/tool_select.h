/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef TOOL_SELECT_H
#define TOOL_SELECT_H

#include <qptrlist.h>
#include "tool.h"
#include "tkunits.h"

#include "kivio_rect.h"

class KivioView;
class KivioPage;
class QMouseEvent;
class KActionMenu;

class KivioStencil;

class KivioSelectDragData
{
public:
    KivioRect rect;
};

enum {
    stCut=1,
    stCopy,
    stPaste,
    stSendToBack,
    stBringToFront,
    stGroup,
    stUngroup
};

class SelectTool : public Tool
{ Q_OBJECT
public:
  SelectTool( KivioView* view );
  ~SelectTool();

  virtual void processEvent( QEvent* );
  virtual void activate();
  virtual void deactivate();
  virtual void configure();

  void select(const QRect&);

signals:
  void operationDone();

protected:
    void mousePress(const QPoint&);
    void mouseMove(const QPoint&);
    void mouseRelease(const QPoint&);

    void leftDoubleClick(const QPoint&);

    bool startResizing(const QPoint&);
    bool startDragging(const QPoint&, bool);
    bool startCustomDragging(const QPoint&, bool);
    bool startRubberBanding(const QPoint&);

    void continueDragging(const QPoint&);
    void continueCustomDragging(const QPoint&);
    void continueResizing(const QPoint&);
    void continueRubberBanding(const QPoint&);

    void endResizing(const QPoint&);
    void endDragging(const QPoint&);
    void endCustomDragging(const QPoint&);
    void endRubberBanding(const QPoint&);

    void buildPopupMenu();
    void showPopupMenu(const QPoint&);

    void changeMouseCursor(const QPoint&);
    int isOverResizeHandle( KivioStencil *pStencil, const float x, const float y );
    QPoint m_startPoint, m_releasePoint;
    TKPoint m_lastPoint;
    TKPoint m_origPoint;

    // Select Tool Mode
    enum {
        stmNone,
        stmDrawRubber,
        stmDragging,
        stmCustomDragging,
        stmResizing
    };
private:
    // Flag to indicate that we are drawing a rubber band
    int m_mode;
    KivioStencil *m_pResizingStencil;
    KivioStencil *m_pCustomDraggingStencil;
    int m_resizeHandle;
    char m_keys[32];
    bool m_shiftKey;
    int m_customDragID;
    QPtrList <KivioSelectDragData> m_lstOldGeometry;

    KActionMenu *m_pMenu;
};

#endif


