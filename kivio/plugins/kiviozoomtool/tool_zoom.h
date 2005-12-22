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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef TOOL_ZOOM_H
#define TOOL_ZOOM_H

#include "kivio_mousetool.h"

class KivioView;
class KivioPage;
class KivioCanvas;
class KPopupMenu;

class KRadioAction;
class KAction;

class QCursor;

class ZoomTool : public Kivio::MouseTool
{
  Q_OBJECT
  public:
    ZoomTool(KivioView* parent);
    virtual ~ZoomTool();
  
    virtual bool processEvent(QEvent* e);
  
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
    
    virtual void setActivated(bool a);
  
  protected:
    void showPopupMenu(const QPoint&);
  
  signals:
    void operationDone();
  
  private:
    KRadioAction* m_zoomAction;
    KRadioAction* m_panAction;
  
    KPopupMenu* m_pMenu;
  
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
