/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998-99 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <SelectionTool.h>

#include <qkeycode.h>
#include <qbitmap.h>
#include <klocale.h>
#include <kdebug.h>

#include "GDocument.h"
#include "GPage.h"
#include <Canvas.h>
#include "KIllustrator_doc.h"
#include <Coord.h>
#include <TranslateCmd.h>
#include <ScaleCmd.h>
#include <RotateCmd.h>
#include <ShearCmd.h>
#include <CommandHistory.h>
#include <units.h>
#include <PStateManager.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "ToolController.h"

#define resize_ptr_width 16
#define resize_ptr_height 16
#define resize_ptr_x_hot 8
#define resize_ptr_y_hot 9
static unsigned char resize_ptr_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x7c, 0x7c, 0x78, 0x3c, 0x78, 0x3c, 0x7c, 0x7c,
    0x4e, 0xe4, 0x07, 0xc0, 0x03, 0x80, 0x07, 0xc0, 0x4e, 0xe4, 0x7c, 0x7c,
    0x78, 0x3c, 0x78, 0x3c, 0x7c, 0x7c, 0x00, 0x00};

#define resize_ptrmsk_width 16
#define resize_ptrmsk_height 16
static unsigned char resize_ptrmsk_bits[] = {
    0x00, 0x00, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfc, 0x7e, 0xfe, 0xfe,
    0xff, 0xfe, 0xef, 0xee, 0x07, 0xc0, 0xef, 0xee, 0xff, 0xfe, 0xfe, 0xfe,
    0xfc, 0x7e, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe};

SelectionTool::SelectionTool (CommandHistory *history)
:Tool(history)
{
   state = S_Init;
   dragHorizHelpline = dragVertHelpline = -1;
   cursor = new QCursor (QBitmap(resize_ptr_width,
                                 resize_ptr_height,
                                 resize_ptr_bits),
                         QBitmap(resize_ptrmsk_width,
                                 resize_ptrmsk_height,
                                 resize_ptrmsk_bits),
                         resize_ptr_x_hot, resize_ptr_y_hot);
   m_id=ToolSelect;
}

void SelectionTool::processEvent (QEvent* e, GDocument *doc, Canvas* canvas) {
//  if (doc->activePage()->helplineLayerIsActive ()) {
/*    if (e->type () == QEvent::MouseButtonPress)
      processButtonPressForHelpline ((QMouseEvent *) e, doc, canvas);
    else if (e->type () == QEvent::MouseMove)
      processMouseMoveForHelpline ((QMouseEvent *) e, doc, canvas);
    else if (e->type () == QEvent::MouseButtonRelease)
      processButtonReleaseForHelpline ((QMouseEvent *) e, doc, canvas);*/
//  }
//  else {
    if(!doc->document()->isReadWrite())
      return;
    if (e->type () == QEvent::MouseButtonPress)
      processButtonPressEvent ((QMouseEvent *) e, doc, canvas);
    else if (e->type () == QEvent::MouseMove)
      processMouseMoveEvent ((QMouseEvent *) e, doc, canvas);
    else if (e->type () == QEvent::MouseButtonRelease)
      processButtonReleaseEvent ((QMouseEvent *) e, doc, canvas);
    else if (e->type () == QEvent::KeyPress)
      processKeyPressEvent ((QKeyEvent *) e, doc, canvas);
//  }
}

void SelectionTool::processButtonReleaseForHelpline(QMouseEvent *,GDocument *, Canvas* canvas)
{

   if (dragHorizHelpline != -1)
   {
      canvas->updateHelplines ();
      dragHorizHelpline = -1;
      canvas->setCursor(Qt::arrowCursor);
      ctype = C_Arrow;
   }
   else if (dragVertHelpline != -1)
   {
      canvas->updateHelplines ();
      dragVertHelpline = -1;
      canvas->setCursor(Qt::arrowCursor);
      ctype = C_Arrow;
  }
}

void SelectionTool::processButtonPressForHelpline (QMouseEvent *me,
  GDocument *, Canvas* canvas) {
  float xpos = me->x();
  float ypos = me->y();
  dragHorizHelpline = canvas->indexOfHorizHelpline (ypos);
  if (dragHorizHelpline != -1) {
    if (ctype != C_Vert) {
      canvas->setCursor(Qt::sizeVerCursor);
      ctype = C_Vert;
    }
    dragVertHelpline = -1;
  }
  else {
    dragVertHelpline = canvas->indexOfVertHelpline (xpos);
    if (dragVertHelpline != -1 && ctype != C_Horiz) {
      canvas->setCursor(Qt::sizeHorCursor);
      ctype = C_Horiz;
    }
  }
}

void SelectionTool::processMouseMoveForHelpline (QMouseEvent *me,
  GDocument *, Canvas* canvas) {
  float xpos = me->x();
  float ypos = me->y();

  if (dragHorizHelpline != -1) {
    canvas->updateHorizHelpline (dragHorizHelpline, ypos);
  }
  else if (dragVertHelpline != -1) {
    canvas->updateVertHelpline (dragVertHelpline, xpos);
  }
  else if (canvas->indexOfHorizHelpline (ypos) != -1) {
    if (ctype != C_Vert) {
      canvas->setCursor (Qt::sizeVerCursor);
      ctype = C_Vert;
    }
  }
  else if (canvas->indexOfVertHelpline (xpos) != -1) {
    if (ctype != C_Horiz) {
      canvas->setCursor (Qt::sizeHorCursor);
      ctype = C_Horiz;
    }
  }
  else {
    if (ctype != C_Arrow) {
      canvas->setCursor(Qt::arrowCursor);
      ctype = C_Arrow;
    }
  }
}

void SelectionTool::processButtonReleaseEvent (QMouseEvent *me, GDocument *doc, Canvas* canvas)
{
  if (dragHorizHelpline != -1)
   {
      canvas->updateHelplines ();
      dragHorizHelpline = -1;
      canvas->setCursor(Qt::arrowCursor);
      ctype = C_Arrow;
      return;
   }
   else if (dragVertHelpline != -1)
   {
      canvas->updateHelplines ();
      dragVertHelpline = -1;
      canvas->setCursor(Qt::arrowCursor);
      ctype = C_Arrow;
      return;
  }

  canvas->setCursor(Qt::arrowCursor);
  ctype = C_Arrow;
  Handle::Mode mode = Handle::HMode_Default;

  doc->activePage()->handle ().show (true);
  float xpos = me->x (), ypos = me->y ();

  /**********
   * S_Rubberband
   */
  if (state == S_Rubberband) {
    QList<GObject> olist;
    Rect selRect (selPoint[0], selPoint[1]);
    if (doc->activePage()->findObjectsContainedIn (selRect.normalize (), olist)) {
      QListIterator<GObject> it (olist);
      doc->setAutoUpdate (false);
      for (; it.current (); ++it)
        doc->activePage()->selectObject (it.current ());
      state = S_Pick;
      doc->setAutoUpdate (true);
    }
    else {
      // no object found - repaint canvas to remove the rubberband
      canvas->repaint ();
      state = S_Init;
    }
  }
  /**********
   * S_Pick
   */
  else if (state == S_Pick) {
    // ?
  }
  /**********
   * S_Scale, S_Translate
   */
  else if (state == S_Scale) {
    state = S_Pick;
    //    canvas->snapPositionToGrid (xpos, ypos);
    float xoff = xpos - firstpos.x ();
    float yoff = ypos - firstpos.y ();
    if (me->state () & Qt::ControlButton) {
      if (fabs (xoff) > fabs (yoff)) {
        yoff = xoff;
        if ((oldmask & (Handle::HPos_Left | Handle::HPos_Bottom)) ||
            (oldmask & (Handle::HPos_Right | Handle::HPos_Top)))
          yoff = -yoff;
      }
      else {
        xoff = yoff;
        if ((oldmask & (Handle::HPos_Left | Handle::HPos_Bottom)) ||
            (oldmask & (Handle::HPos_Right | Handle::HPos_Top)))
          xoff = -xoff;
      }
    }
    if(oldmask == (Handle::HPos_Left | Handle::HPos_Bottom) ||
	     oldmask == (Handle::HPos_Left | Handle::HPos_Top) ||
	     oldmask == (Handle::HPos_Right | Handle::HPos_Bottom) ||
	     oldmask == (Handle::HPos_Right | Handle::HPos_Top))
     scale (doc, canvas, oldmask, xoff, yoff, 1, true);
    else
     scale (doc, canvas, oldmask, xoff, yoff, 0, true);
  }
  else if (state == S_Translate) {
    state = S_Pick;
    if (me->state () & Qt::ControlButton) {
      if (fabs (xpos - firstpos.x ()) > fabs (ypos - firstpos.y ()))
        ypos = firstpos.y ();
      else
        xpos = firstpos.x ();
    }
    translate (doc, canvas, xpos - firstpos.x (), ypos - firstpos.y (),
               true, true);
  }
  /**********
   * S_Intermediate1
   */
  else if (state == S_Intermediate1) {
    state = S_RotateSelect;
    mode = Handle::HMode_Rotate;
  }
  /**********
   * S_Rotate
   */
  else if (state == S_Rotate) {
    state = S_RotateSelect;
    mode = Handle::HMode_Rotate;
    canvas->snapPositionToGrid (xpos, ypos);
    rotate (doc, xpos - firstpos.x (), ypos - firstpos.y (),
            me->x (), me->y (), true);
  }
  else if (state == S_Shear) {
    state = S_RotateSelect;
    mode = Handle::HMode_Rotate;
    canvas->snapPositionToGrid (xpos, ypos);
    shear (doc, oldmask, xpos - firstpos.x (), ypos - firstpos.y (),
           true);
  }
  /**********
   * S_MoveRotCenter
   */
  else if (state == S_MoveRotCenter) {
    state = S_RotateSelect;
    rotCenter = doc->activePage()->handle().rotCenter ();
    return;
  }
  /**********
   * S_Intermediate2
   */
  else if (state == S_Intermediate2) {
    if (doc->activePage()->findContainingObject (me->x (), me->y ()) == 0L) {
      doc->activePage()->unselectAllObjects ();
      state = S_Init;
    }
    else
      state = S_Pick;
    //    doc->activePage()->handle ().setMode (Handle::HMode_Default);
  }
  doc->activePage()->handle ().setMode (mode, true);
  if (doc->activePage()->selectionIsEmpty ())
  {
    m_toolController->emitModeSelected (m_id,i18n ("Selection Mode"));
  }
  else {
    Rect box = doc->activePage()->boundingBoxForSelection ();
    MeasurementUnit unit =
      PStateManager::instance ()->defaultMeasurementUnit ();
    QString u = unitToString (unit);
    float x, y, w, h;
    x = cvtPtToUnit (unit, box.x ());
    y = cvtPtToUnit (unit, box.y ());
    w = cvtPtToUnit (unit, box.width ());
    h = cvtPtToUnit (unit, box.height ());
    if (doc->activePage()->selectionCount () > 1) {
        msgbuf=i18n("Multiple Selection");
        msgbuf+=" [";
        msgbuf+=QString::number(x, 'f', 3);
        msgbuf+=QString(" ") + u + QString(", ");
        msgbuf+=QString::number(y, 'f', 3);
        msgbuf+=QString(" ") + u + QString(", ");
        msgbuf+=QString::number(w, 'f', 3);
        msgbuf+=QString(" ") + u + QString(", ");
        msgbuf+=QString::number(h, 'f', 3);
        msgbuf+=QString(" ") + u + QString("]");
    }
    else {
      GObject *sobj = doc->activePage()->getSelection ().first();
      msgbuf=sobj->typeName();
      msgbuf+=" [";
      msgbuf+=QString::number(x, 'f', 3);
      msgbuf+=QString(" ") + u + QString(", ");
      msgbuf+=QString::number(y, 'f', 3);
      msgbuf+=QString(" ") + u + QString(", ");
      msgbuf+=QString::number(w, 'f', 3);
      msgbuf+=QString(" ") + u + QString(", ");
      msgbuf+=QString::number(h, 'f', 3);
      msgbuf+=QString(" ") + u + QString("]");
    }
    m_toolController->emitModeSelected (m_id,msgbuf);
  }
  origbox = doc->activePage()->boundingBoxForSelection ();
}

void SelectionTool::processMouseMoveEvent (QMouseEvent *me, GDocument *doc,
                                           Canvas* canvas)
{
  int hmask;

  float xpos = me->x (), ypos = me->y ();

  if (dragHorizHelpline != -1) {
    canvas->updateHorizHelpline (dragHorizHelpline, ypos);
    return;
  }
  else if (dragVertHelpline != -1) {
    canvas->updateVertHelpline (dragVertHelpline, xpos);
    return;
  }
  else if (canvas->indexOfHorizHelpline (ypos) != -1) {
    if (ctype != C_Vert) {
      canvas->setCursor (Qt::sizeVerCursor);
      ctype = C_Vert;
    }
    return;
  }
  else if (canvas->indexOfVertHelpline (xpos) != -1) {
    if (ctype != C_Horiz) {
      canvas->setCursor (Qt::sizeHorCursor);
      ctype = C_Horiz;
    }
    return;
  }
  else { // out of a helpline [but keep current cursor type if size/move/rotate]
    if (ctype == C_Horiz || ctype == C_Vert) {
      canvas->setCursor(Qt::arrowCursor);
      ctype = C_Arrow;
      return;
    }
  }

  if (state == S_Inactive)
    return;
  /**********
   * S_Rubberband
   */
  else if (state == S_Rubberband)
  {
    selPoint[1].x (me->x ());
    selPoint[1].y (me->y ());
    canvas->repaint ();
    QPainter painter;
    painter.save ();
    QPen pen (Qt::blue, 1, Qt::DotLine);
    painter.begin (canvas);
    painter.setPen (pen);
    painter.translate(canvas->relativePaperArea().left(), canvas->relativePaperArea().top());
    float sfactor = canvas->scaleFactor ();
    painter.scale (sfactor, sfactor);
    Rect selRect (selPoint[0], selPoint[1]);
    painter.drawRect ((int) selRect.x (), (int) selRect.y (),
                      (int) selRect.width (), (int) selRect.height ());
    painter.restore ();
    painter.end ();
    return;
  }

  if (! doc->activePage()->selectionIsEmpty ())
  {
    /**********
     * S_Pick
     */
    if (state == S_Pick)
    {
      hmask = doc->activePage()->handle ().contains (Coord (me->x (), me->y ()));
      if (hmask && hmask != Handle::HPos_Center)
      {
         if (ctype != C_Size)
         {
            ctype = C_Size;
            switch(hmask)
            {
            case (Handle::HPos_Left | Handle::HPos_Top):
               canvas->setCursor(Qt::sizeFDiagCursor);
               break;
            case (Handle::HPos_Top):
               canvas->setCursor(Qt::sizeVerCursor);
               break;
            case (Handle::HPos_Top | Handle::HPos_Right):
               canvas->setCursor(Qt::sizeBDiagCursor);
               break;
            case (Handle::HPos_Right):
               canvas->setCursor(Qt::sizeHorCursor);
               break;
            case (Handle::HPos_Right | Handle::HPos_Bottom):
               canvas->setCursor(Qt::sizeFDiagCursor);
               break;
            case (Handle::HPos_Bottom):
               canvas->setCursor(Qt::sizeVerCursor);
               break;
            case (Handle::HPos_Bottom | Handle::HPos_Left):
               canvas->setCursor(Qt::sizeBDiagCursor);
               break;
            case (Handle::HPos_Left):
               canvas->setCursor(Qt::sizeHorCursor);
               break;
            default:
               canvas->setCursor(Qt::sizeAllCursor);
               break;
            };
         }
      }
      else if (ctype != C_Arrow)
      {
         ctype = C_Arrow;
         canvas->setCursor(Qt::arrowCursor);
      }

      if (me->state () & Qt::LeftButton)
        state = S_Translate;
    }
    /**********
     * S_Intermediate1
     */
    else if (state == S_Intermediate1) {
       if (me->state () & Qt::LeftButton) {
        //      hmask = doc->activePage()->handle ().contains (Coord (me->x (), me->y ()));
        if (ctype == C_Size)
          state = S_Scale;
        else
          state = S_Translate;
      }
    }
    /**********
     * S_MoveRotCenter
     */
    else if (state == S_MoveRotCenter) {
      doc->activePage()->handle ().setRotCenter (Coord (me->x (), me->y ()));
      return;
    }
    /**********
     * S_Rotate
     */
    else if (state == S_RotateSelect || state == S_Rotate) {
      hmask = doc->activePage()->handle ().contains (Coord (me->x (), me->y ()));
      if (hmask)
      {
         if (ctype != C_Size)
         {
            ctype = C_Size;
            switch(hmask)
            {
            case (Handle::HPos_Left | Handle::HPos_Top):
               canvas->setCursor(Qt::sizeBDiagCursor);
               break;
            case (Handle::HPos_Top):
               canvas->setCursor(Qt::sizeHorCursor);
               break;
            case (Handle::HPos_Top | Handle::HPos_Right):
               canvas->setCursor(Qt::sizeFDiagCursor);
               break;
            case (Handle::HPos_Right):
               canvas->setCursor(Qt::sizeVerCursor);
               break;
            case (Handle::HPos_Right | Handle::HPos_Bottom):
               canvas->setCursor(Qt::sizeBDiagCursor);
               break;
            case (Handle::HPos_Bottom):
               canvas->setCursor(Qt::sizeHorCursor);
               break;
            case (Handle::HPos_Bottom | Handle::HPos_Left):
               canvas->setCursor(Qt::sizeFDiagCursor);
               break;
            case (Handle::HPos_Left):
               canvas->setCursor(Qt::sizeVerCursor);
               break;
            case (Handle::HPos_Center):
            default:
               canvas->setCursor(Qt::sizeAllCursor);
               break;
            };
        }
      }
      else if (ctype != C_Arrow) {
        ctype = C_Arrow;
        canvas->setCursor(Qt::arrowCursor);
      }
    }
    if (me->state () & Qt::LeftButton) {
      //      canvas->snapPositionToGrid (xpos, ypos);
      float xoff = xpos - firstpos.x ();
      float yoff = ypos - firstpos.y ();

      doc->activePage()->handle ().show (false);

      switch (state) {
      case S_Scale:
        {
           if (ctype != C_Size)
           {
            ctype = C_Size;
            //canvas->setCursor(Qt::forbiddenCursor);
            canvas->setCursor(*cursor);
          }
          if (me->state () & Qt::ControlButton) {
            if (fabs (xoff) > fabs (yoff)) {
              yoff = xoff;
              if ((oldmask & (Handle::HPos_Left | Handle::HPos_Bottom)) ||
                  (oldmask & (Handle::HPos_Right | Handle::HPos_Top)))
                yoff = -yoff;
            }
            else {
              xoff = yoff;
              if ((oldmask & (Handle::HPos_Left | Handle::HPos_Bottom)) ||
                  (oldmask & (Handle::HPos_Right | Handle::HPos_Top)))
                xoff = -xoff;
            }
          }
	  if(oldmask == (Handle::HPos_Left | Handle::HPos_Bottom) ||
	     oldmask == (Handle::HPos_Left | Handle::HPos_Top) ||
	     oldmask == (Handle::HPos_Right | Handle::HPos_Bottom) ||
	     oldmask == (Handle::HPos_Right | Handle::HPos_Top))
           scale (doc, canvas, oldmask, xoff, yoff, 1);
          else
           scale (doc, canvas, oldmask, xoff, yoff, 0);
          break;
        }
      case S_Translate:
        if (ctype != C_Move) {
          ctype = C_Move;
          canvas->setCursor(Qt::sizeAllCursor);
        }
        if (me->state () & Qt::ControlButton) {
          if (fabs (xoff) > fabs (yoff))
            yoff = 0;
          else
            xoff = 0;
        }
        translate (doc, canvas, xoff, yoff, true);
        break;
      case S_Rotate:
        rotate (doc, xoff, yoff, me->x (), me->y ());
        break;
      case S_Shear:
        shear (doc, oldmask, xoff, yoff);
        break;
      default:
        break;
      }
    }
  }
}

void SelectionTool::processButtonPressEvent (QMouseEvent *me, GDocument *doc,
                                             Canvas* canvas) {
  int hmask;
  GObject *obj = 0L;

  float xpos = me->x();
  float ypos = me->y();
  //  canvas->snapPositionToGrid (xpos, ypos);

  /*****[HELPLINES]*****/
  dragHorizHelpline = canvas->indexOfHorizHelpline (ypos);
  if (dragHorizHelpline != -1) {
    if (ctype != C_Vert) {
      canvas->setCursor(Qt::sizeVerCursor);
      ctype = C_Vert;
      return;
     }
     dragVertHelpline = -1;
  }
  else {
    dragVertHelpline = canvas->indexOfVertHelpline (xpos);
    if (dragVertHelpline != -1 && ctype != C_Horiz) {
      canvas->setCursor(Qt::sizeHorCursor);
      ctype = C_Horiz;
      return;
    }
  }
  /*********************/

  firstpos.x (xpos);
  firstpos.y (ypos);

  hmask = doc->activePage()->handle ().contains (Coord (me->x (), me->y ()));
  bool shiftFlag = me->state () & Qt::ShiftButton;

  if (state == S_Inactive)
    state = S_Init;

  /************
   * S_Init
   */
  if (state == S_Init) {
    obj = doc->activePage()->findContainingObject (me->x (), me->y ());
    if (obj) {
      // an object will be selected
      state = S_Pick;
      if (!shiftFlag)
        doc->activePage()->unselectAllObjects ();
      // add the object to the selection
      doc->activePage()->selectObject (obj);
      origbox = doc->activePage()->boundingBoxForSelection ();
    }
    else {
      // no object
      state = S_Rubberband;
      doc->activePage()->unselectAllObjects ();
      selPoint[0].x(me->x ()); selPoint[0].y(me->y ());
      selPoint[1].x(me->x ()); selPoint[1].y(me->y ());
    }
  }
  /************
   * S_Pick
   */
  else if (state == S_Pick) {
    origbox = doc->activePage()->boundingBoxForSelection ();
    if (hmask) {
      state = S_Intermediate1;
      oldmask = hmask;
    }
    else {
      obj = doc->activePage()->findContainingObject (me->x (), me->y ());
      if (obj) {
        if (obj->isSelected ()) {

          //
          // a ugly workaround, because cliparts cannot be rotated (WHY NOT ?)
          //
          if (doc->activePage()->selectionCount () == 1) {
            GObject* selObj = doc->activePage()->getSelection ().first();
            if (selObj->isA ("GClipart")) {
              // the selected object is a clipart,
              // so don't show rotation handles
              return;
            }
            else if (selObj->isA ("GPart")) {
              kdDebug(38000) << "activate part !!!" << endl;
              state = S_Inactive;
              m_toolController->emitPartSelected (m_id,selObj);
              return;
            }
          }

          // the object is already selected
          if (shiftFlag)
            // remove it from the selection
            doc->activePage()->unselectObject (obj);
          else
            state = S_Intermediate1;
        }
        else {
          if (!shiftFlag)
            doc->activePage()->unselectAllObjects ();
          // add the object to the selection
          doc->activePage()->selectObject (obj);
        }
      }
      else {
        // nothing selected
        // unselect all
        doc->activePage()->unselectAllObjects ();

        // and switch to rubberband mode
        state = S_Rubberband;
        selPoint[0].x(me->x ()); selPoint[0].y(me->y ());
        selPoint[1].x(me->x ()); selPoint[1].y(me->y ());
      }
    }
  }
  /************
   * S_RotateSelect
   */
  else if (state == S_RotateSelect) {
    if (hmask) {
      origbox = doc->activePage()->boundingBoxForSelection ();
      oldmask = hmask;
      if (hmask == (Handle::HPos_Top | Handle::HPos_Left) ||
          hmask == (Handle::HPos_Bottom | Handle::HPos_Left) ||
          hmask == (Handle::HPos_Top | Handle::HPos_Right) ||
          hmask == (Handle::HPos_Bottom | Handle::HPos_Right)) {
        state = S_Rotate;
        // rotCenter = doc->boundingBoxForSelection ().center ();
        rotCenter = doc->activePage()->handle().rotCenter ();
      }
      else if (hmask == Handle::HPos_Center) {
        state = S_MoveRotCenter;
      }
      else {
        state = S_Shear;
        rotCenter = doc->activePage()->handle().rotCenter ();
      }
    }
    else
      state = S_Intermediate2;
  }
}

void SelectionTool::processKeyPressEvent (QKeyEvent *ke, GDocument *doc,
                                             Canvas* canvas) {
  if (doc->activePage()->selectionIsEmpty ())
    return;

  if (ke->key () == Qt::Key_Escape) {
      // clear selection
      doc->activePage()->unselectAllObjects ();
      return;
  }

  float big_step = PStateManager::instance ()->bigStepSize ();
  float small_step = PStateManager::instance ()->smallStepSize ();
  float dx = 0, dy = 0;
  bool shift = ke->state () & Qt::ShiftButton;

  switch (ke->key ()) {
  case Qt::Key_Left:
    dx = (shift ? -small_step : -big_step);
    break;
  case Qt::Key_Right:
    dx = (shift ? small_step : big_step);
    break;
  case Qt::Key_Up:
    dy = (shift ? -small_step : -big_step);
    break;
  case Qt::Key_Down:
    dy = (shift ? small_step : big_step);
    break;
  case Qt::Key_Tab:
      kdDebug(38000) << "<tab>" << endl;
  default:
    break;
  }
  if (dx != 0 || dy != 0)
    translate (doc, canvas, dx, dy, false, true);
}

void SelectionTool::translate (GDocument* doc, Canvas* canvas,
                               float dx, float dy, bool snap,
                               bool permanent) {

  if (snap) {
    const Rect& obox = origbox;
    Rect newbox = canvas->snapTranslatedBoxToGrid (obox.translate (dx, dy));
    if (! (newbox == obox)) {
      dx = newbox.x () - obox.x ();
      dy = newbox.y () - obox.y ();
    }
  }
  if (dx == 0 && dy == 0)  return;
//  kdDebug(0) << "snap=" << snap << endl;
//kdDebug(0) << "DX=" << dx << " DY=" << dy << endl;
  if (permanent) {
      QListIterator<GObject> it(doc->activePage()->getSelection());
      for( ; it.current(); ++it)
          (*it)->setWorkInProgress (false);
    TranslateCmd *cmd = new TranslateCmd (doc, dx, dy);
    history->addCommand (cmd, true);
  }
  else {
    QListIterator<GObject> it(doc->activePage()->getSelection());
    QWMatrix m;
    m.translate (dx, dy);
    for ( ; it.current(); ++it) {
      (*it)->setWorkInProgress (true);
      (*it)->initTmpMatrix ();
      (*it)->ttransform (m, true);
    }
  }

  MeasurementUnit unit =
    PStateManager::instance ()->defaultMeasurementUnit ();
  QString u = unitToString (unit);
  float xval, yval;
  xval = cvtPtToUnit (unit, dx);
  yval = cvtPtToUnit (unit, dy);

  msgbuf=i18n("Translate");
  msgbuf+=" [";
  msgbuf+=QString::number(xval, 'f', 3);
  msgbuf+=QString(" ") + u + QString(", ");
  msgbuf+=QString::number(yval, 'f', 3);
  msgbuf+=QString(" ") + u + QString("]");
  m_toolController->emitModeSelected (m_id,msgbuf);
}

void SelectionTool::rotate (GDocument* doc, float , float ,
                            float xp, float yp, bool permanent) {
  //  float adx = fabs (dx);
  //  float ady = fabs (dy);
  float angle = 0;

  //  Rect& r = origbox;

  /*
  if (adx > ady) {
    angle = adx / r.width () * 180.0;
    if ((yp > rotCenter.y () && dx > 0) ||
        (yp < rotCenter.y () && dx < 0))
      angle = -angle;
  }
  else if (adx < ady) {
    angle = ady / r.height () * 180.0;
    if ((xp > rotCenter.x () && dy < 0) ||
        (xp < rotCenter.x () && dy > 0))
      angle = -angle;
  }
  */
  // proposed by Stefan Eickeler <eickeler@fb9-ti.uni-duisburg.de>
  angle=atan2 ((float)rotCenter.y()-yp,(float)rotCenter.x()-xp);
  kdDebug()<<"SelectionTool:rotate() angle=="<<angle<<" a: "<<(float)rotCenter.y()-yp<<" b: "<<(float)rotCenter.x()-xp<<endl;
  angle-=atan2 ((float)(rotCenter.y()-firstpos.y()),(float)(rotCenter.x()-firstpos.x()));
  kdDebug()<<"SelectionTool:rotate() angle=="<<angle<<" a: "<<(float)(rotCenter.y()-firstpos.y())<<" b: "<<(float)(rotCenter.x()-firstpos.x())<<endl;

  kdDebug()<<"SelectionTool:rotate() angle=="<<angle<<" a: "<<(float)rotCenter.y()-yp<<" b: "<<(float)rotCenter.x()-xp<<endl;
  kdDebug()<<"SelectionTool:rotate() angle=="<<angle<<" a: "<<(float)(rotCenter.y()-firstpos.y())<<" b: "<<(float)(rotCenter.x()-firstpos.x())<<endl;



  angle*=180.0/3.141;
  if (angle<180.0) angle+=360.0;
  if (angle>180.0) angle-=360.0;

  if (permanent) {
      QListIterator<GObject> it(doc->activePage()->getSelection());
      for( ; it.current(); ++it)
          (*it)->setWorkInProgress(false);
      RotateCmd *cmd = new RotateCmd (doc, rotCenter, angle);
      history->addCommand (cmd, true);
  }
  else {
    QWMatrix m1, m2, m3;
    m1.translate (-rotCenter.x (), -rotCenter.y ());
    m2.rotate (angle);
    m3.translate (rotCenter.x (), rotCenter.y ());

    for (QListIterator<GObject> it(doc->activePage()->getSelection()); it.current(); ++it) {
      (*it)->setWorkInProgress (true);
      (*it)->initTmpMatrix ();
      (*it)->ttransform (m1);
      (*it)->ttransform (m2);
      (*it)->ttransform (m3, true);
    }
  }
  MeasurementUnit unit =
    PStateManager::instance ()->defaultMeasurementUnit ();
  QString u = unitToString (unit);
  float xval, yval;
  xval = cvtPtToUnit (unit, rotCenter.x ());
  yval = cvtPtToUnit (unit, rotCenter.y ());

  msgbuf=i18n("Rotate");
  msgbuf+=" [";
  msgbuf+=QString::number(angle, 'f', 3);
  msgbuf+=QString(" - ");
  msgbuf+=QString::number(xval, 'f', 3);
  msgbuf+=QString(" ") + u + QString(", ");
  msgbuf+=QString::number(yval, 'f', 3);
  msgbuf+=QString(" ") + u + QString("]");
  m_toolController->emitModeSelected (m_id,msgbuf);
}

void SelectionTool::scale (GDocument* doc, Canvas* canvas,
                           int mask, float dx, float dy, int type,
                           bool permanent) {
  Rect& r = origbox;
  Rect newbox (origbox);
  float sx = 1, sy = 1;
  float xoff = r.x (), yoff = r.y ();
  float xback = xoff, yback = yoff;

  if (mask & Handle::HPos_Right)
    newbox.right (newbox.right () + dx);
  if (mask & Handle::HPos_Bottom)
    newbox.bottom (newbox.bottom () + dy);
  if (mask & Handle::HPos_Left)
    newbox.left (newbox.left () + dx);
  if (mask & Handle::HPos_Top)
    newbox.top (newbox.top () + dy);

  Rect sbox = canvas->snapScaledBoxToGrid (newbox, mask);
  sx = sbox.width () / origbox.width ();
  sy = sbox.height () / origbox.height ();

  if(type == 1)
   sx = sy;
  if (mask & Handle::HPos_Left) {
    xback = r.left () + r.width () * (1 - sx);
  }
  if (mask & Handle::HPos_Top) {
    yback = r.top () + r.height () * (1 - sy);
  }
  if (permanent) {
      QListIterator<GObject> it(doc->activePage()->getSelection());
      for( ; it.current(); ++it)
          (*it)->setWorkInProgress(false);
      ScaleCmd *cmd = new ScaleCmd (doc, oldmask, sx, sy, r);
      history->addCommand (cmd, true);
  }
  else {
    QWMatrix m1, m2, m3;

    m1.translate (-xoff, -yoff);
    m2.scale (sx, sy);
    m3.translate (xback, yback);

    for (QListIterator<GObject> it(doc->activePage()->getSelection ()); it.current(); ++it) {
      (*it)->setWorkInProgress (true);
      (*it)->initTmpMatrix ();

      (*it)->ttransform (m1);
      (*it)->ttransform (m2);
      (*it)->ttransform (m3, true);
    }
  }
  msgbuf=i18n("Scale");
  msgbuf+=" [";
  msgbuf+=QString::number(sx * 100.0, 'f', 3);
  msgbuf+=QString(" %, ");
  msgbuf+=QString::number(sy * 100.0, 'f', 3);
  msgbuf+=QString(" %]");
  m_toolController->emitModeSelected (m_id,msgbuf);
}

void SelectionTool::shear (GDocument* doc, int mask, float dx, float dy,
                           bool permanent) {
  Rect& r = origbox;
  float sx = 0.0, sy = 0.0;
  if (mask == Handle::HPos_Top)
    sx = -dx / r.width ();
  else if (mask == Handle::HPos_Bottom)
    sx = dx / r.width ();
  else if (mask == Handle::HPos_Left)
    sy = -dy / r.height ();
  else if (mask == Handle::HPos_Right)
    sy = dy / r.height ();

  if (permanent) {
      QListIterator<GObject> it(doc->activePage()->getSelection());
      for( ; it.current(); ++it)
          (*it)->setWorkInProgress(false);
      ShearCmd *cmd = new ShearCmd (doc, rotCenter, sx, sy);
      history->addCommand (cmd, true);
  }
  else {
    QWMatrix m1, m2, m3;

    m1.translate (-rotCenter.x (), -rotCenter.y ());
    m2.shear (sx, sy);
    m3.translate (rotCenter.x (), rotCenter.y ());

    for (QListIterator<GObject> it(doc->activePage()->getSelection()); it.current(); ++it) {
      (*it)->setWorkInProgress (true);
      (*it)->initTmpMatrix ();

      (*it)->ttransform (m1);
      (*it)->ttransform (m2);
      (*it)->ttransform (m3, true);
    }
  }
  msgbuf=i18n("Shear");
  msgbuf+=" [";
  msgbuf+=QString::number(sx * 100.0, 'f', 3);
  msgbuf+=QString(" %, ");
  msgbuf+=QString::number(sy * 100.0, 'f', 3);
  msgbuf+=QString(" %]");
  m_toolController->emitModeSelected (m_id,msgbuf);
}

void SelectionTool::processTabKeyEvent (GDocument* doc, Canvas*) {
  Handle::Mode mode = Handle::HMode_Default;
  doc->activePage()->selectNextObject ();
  doc->activePage()->handle ().show (true);
  doc->activePage()->handle ().setMode (mode, true);
  state = S_Pick;
}

void SelectionTool::activate (GDocument* doc, Canvas *canvas)
{
   canvas->setCursor(Qt::arrowCursor);

   dragHorizHelpline = dragVertHelpline = -1;

    doc->activePage()->handle ().show (true);
    if (doc->activePage()->lastObject ()) {
        if (doc->activePage()->selectionIsEmpty ())
            doc->activePage()->selectObject (doc->activePage()->lastObject ());
        else
            doc->setAutoUpdate (true);
        state = S_Pick;
    }
    else
        state = S_Init;
    ctype = C_Arrow;

    if (doc->activePage()->selectionIsEmpty ()) {
        m_toolController->emitModeSelected (m_id,i18n("Selection Mode"));
    }
    else {
        Rect box = doc->activePage()->boundingBoxForSelection ();
        MeasurementUnit unit =
            PStateManager::instance ()->defaultMeasurementUnit ();
        QString u = unitToString (unit);
        float x, y, w, h;
        x = cvtPtToUnit (unit, box.x ());
        y = cvtPtToUnit (unit, box.y ());
        w = cvtPtToUnit (unit, box.width ());
        h = cvtPtToUnit (unit, box.height ());
        if (doc->activePage()->selectionCount () > 1) {
            msgbuf=i18n("Multiple Selection");
            msgbuf+=" [";
            msgbuf+=QString::number(x, 'f', 3);
            msgbuf+=QString(" ") + u + QString(", ");
            msgbuf+=QString::number(y, 'f', 3);
            msgbuf+=QString(" ") + u + QString(", ");
            msgbuf+=QString::number(w, 'f', 3);
            msgbuf+=QString(" ") + u + QString(", ");
            msgbuf+=QString::number(h, 'f', 3);
            msgbuf+=QString(" ") + u + QString("]");
        }
        else {
            GObject *sobj = doc->activePage()->getSelection ().first();
            msgbuf=sobj->typeName();
            msgbuf+=" [";
            msgbuf+=QString::number(x, 'f', 3);
            msgbuf+=QString(" ") + u + QString(", ");
            msgbuf+=QString::number(y, 'f', 3);
            msgbuf+=QString(" ") + u + QString(", ");
            msgbuf+=QString::number(w, 'f', 3);
            msgbuf+=QString(" ") + u + QString(", ");
            msgbuf+=QString::number(h, 'f', 3);
            msgbuf+=QString(" ") + u + QString("]");
        }
        m_toolController->emitModeSelected (m_id,msgbuf);
    }
    canvas->repaint();
}

void SelectionTool::deactivate (GDocument* doc, Canvas* canvas)
{
  doc->activePage()->handle ().show (false);
  canvas->repaint ();
}

