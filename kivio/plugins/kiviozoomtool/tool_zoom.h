/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
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
#ifndef TOOL_ZOOM_H
#define TOOL_ZOOM_H

#include "tool.h"
#include "kivio_rect.h"

class KivioView;
class KivioPage;
class KivioCanvas;
class KActionMenu;

class KRadioAction;
class KAction;

class QCursor;

class ZoomTool : public Tool
{ Q_OBJECT
public:
  ZoomTool(KivioView* view);
  virtual ~ZoomTool();

  virtual void processEvent(QEvent*);
  virtual void activate();
  virtual void deactivate();
  virtual void configure();

  virtual void activateGUI(KXMLGUIFactory*);
  virtual void deactivateGUI(KXMLGUIFactory*);

  void zoomRect(QRect);

public slots:
  void zoomActivated();
  void handActivated();

  void zoomPlus();
  void zoomMinus();
  void zoomWidth();
  void zoomHeight();
  void zoomPage();
  void zoomSelected();
  void zoomAllobjects();

protected:
  void buildMenu();
  void showPopupMenu(QPoint);

signals:
  void operationDone();

private:
  KToolBar* m_pToolBar;
  KRadioAction* m_z1;
  KRadioAction* m_z2;

  KActionMenu* m_pMenu;

  KAction* m_pMinus;
  KAction* m_pPlus;
  KAction* m_pZoomWidth;
  KAction* m_pZoomHeight;
  KAction* m_pZoomSelected;
  KAction* m_pZoomPage;
  KAction* m_pZoomAllObjects;

  KAction* m_pCurrent;

  QCursor* m_pPlusCursor;
  QCursor* m_pMinusCursor;
  QCursor* m_handCursor;

  bool m_bDrawRubber;
  bool m_bHandMode;
  bool isHandMousePressed;
  bool m_bLockKeyboard;
  QPoint mousePos;
};

#endif
