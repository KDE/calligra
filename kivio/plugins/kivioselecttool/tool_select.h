/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 * Copyright (C) 2003-2005 Peter Simonsson <psn@linux.se>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef TOOL_SELECT_H
#define TOOL_SELECT_H

#include <qptrlist.h>
#include <KoPoint.h>
#include <KoRect.h>
#include "kivio_mousetool.h"

class KivioView;
class KivioPage;
class QMouseEvent;
class QKeyEvent;
class KPopupMenu;
class KRadioAction;
class KAction;

class KivioStencil;

class KivioSelectDragData
{
  public:
    KoRect rect;
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

class SelectTool : public Kivio::MouseTool
{
  Q_OBJECT
  public:
    SelectTool( KivioView* parent );
    ~SelectTool();

    virtual bool processEvent(QEvent* e);

    void select(const QRect&);

  public slots:
    void setActivated(bool a);

  signals:
    void operationDone();

  protected slots:
    void editText(QPtrList<KivioStencil>* stencils);
    void showProperties();
    void editStencilText();

  protected:
    void mousePress(const QPoint&);
    void mouseMove(QMouseEvent*);
    void mouseRelease(const QPoint&);
    void leftDoubleClick(const QPoint&);

    bool startResizing(const QPoint&);
    bool startDragging(const QPoint&, bool);
    bool startCustomDragging(const QPoint&, bool);
    bool startRubberBanding(const QPoint&);

    void continueDragging(const QPoint&, bool ignoreGridGuides = false);
    void continueCustomDragging(const QPoint&);
    void continueResizing(const QPoint&, bool ignoreGridGuides = false);
    void continueRubberBanding(const QPoint&);

    void endResizing(const QPoint&);
    void endDragging(const QPoint&);
    void endCustomDragging(const QPoint&);
    void endRubberBanding(const QPoint&);

    void showPopupMenu(const QPoint&);

    void changeMouseCursor(const QPoint&);
    int isOverResizeHandle( KivioStencil *pStencil, const double x, const double y );
    
    void keyPress(QKeyEvent* e);
    
    QPoint m_startPoint, m_releasePoint;
    KoPoint m_lastPoint;
    KoPoint m_origPoint;

    // Select Tool Mode
    enum {
      stmNone,
      stmDrawRubber,
      stmDragging,
      stmCustomDragging,
      stmResizing
    };

  private:
    int m_mode;     // Flag to indicate that we are drawing a rubber band
    KivioStencil *m_pResizingStencil;
    KivioStencil *m_pCustomDraggingStencil;
    int m_resizeHandle;
    bool m_controlKey;
    int m_customDragID;
    QPtrList <KivioSelectDragData> m_lstOldGeometry;
    KoRect m_selectedRect;
    KoPoint m_customDragOrigPoint;

    KRadioAction* m_selectAction;
    KAction* m_arrowHeadAction;
    KAction* m_textEditAction;

    bool m_firstTime;
};

#endif
