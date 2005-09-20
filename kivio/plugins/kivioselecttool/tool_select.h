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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef TOOL_SELECT_H
#define TOOL_SELECT_H

#include <qptrlist.h>
#include <qvaluelist.h>

#include <koPoint.h>

#include "kivio_mousetool.h"
#include "object.h"

class KivioView;
class KivioPage;
class QMouseEvent;
class QKeyEvent;
class KPopupMenu;
class KRadioAction;
class KAction;

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

  protected slots:
    void editText(QPtrList<KivioStencil>* stencils);
    void editStencilText();

  protected:
    void mouseMove(QMouseEvent*);
    void mouseRelease(const QPoint&);
    void leftDoubleClick(const QPoint&);
    /** This is called when the left mouse button is pressed
      * @p pos current mouse position
      * @p selectMultiple if we should allow more then one selected object
      */
    void leftMouseButtonPressed(const QPoint& pos, bool selectMultiple);

    bool startResizing(const QPoint&);
    bool startDragging(const QPoint&, bool);
    bool startCustomDragging(const QPoint&, bool);
    bool startRubberBanding(const QPoint&);

    void move(const QPoint& pos);
    void endMove();

    void resize(const QPoint& pos);
    void endResize();

    void continueCustomDragging(const QPoint&);
    void continueResizing(const QPoint&, bool ignoreGridGuides = false);
    void continueRubberBanding(const QPoint&);

    void endResizing(const QPoint&);
    void endCustomDragging(const QPoint&);
    void endRubberBanding(const QPoint&);

    void showPopupMenu(const QPoint&);

    void changeMouseCursor(const QPoint&);

    void keyPress(QKeyEvent* e);

  protected:
    typedef enum Mode {
      MSelect,
      MMove,
      MResize,
      MCustom
    };

  private:
    Mode m_mode;

    KRadioAction* m_selectAction;
    KAction* m_arrowHeadAction;
    KAction* m_textEditAction;
    KAction* m_textFormatAction;

    Kivio::CollisionFeedback m_collisionFeedback;
    KoPoint m_previousPos;
    QValueList<Kivio::Object*> m_origObjectList;
    Kivio::Object* m_clickedObject;
};

#endif
