/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 * Copyright (C) 2005 Peter Simonsson <psn@linux.se>
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
#ifndef KIVIOBIRDEYEPANEL_H
#define KIVIOBIRDEYEPANEL_H
#include "kivio_birdeye_panel_base.h"

class QPixmap;

class KivioView;
class KivioCanvas;
class KivioDoc;
class KivioPage;
class KAction;
class KoZoomHandler;

class KivioBirdEyePanel : public KivioBirdEyePanelBase
{ Q_OBJECT
public:
  KivioBirdEyePanel(KivioView* view, QWidget* parent=0, const char* name=0);
  ~KivioBirdEyePanel();

  bool eventFilter(QObject*, QEvent*);

public slots:
  void zoomMinus();
  void zoomPlus();

protected slots:
  void updateView();
  void canvasZoomChanged(int zoom);
  void slotUpdateView(KivioPage*);

protected:
  void handleMouseMove(QPoint);
  void handleMouseMoveAction(QPoint);
  void handleMousePress(QPoint);

private:
  KivioView* m_pView;
  KivioCanvas* m_pCanvas;
  KivioDoc* m_pDoc;

  KAction* zoomIn;
  KAction* zoomOut;
  QPixmap* m_buffer;

  QRect varea;
  AlignmentFlags apos;
  bool handlePress;
  QPoint lastPos;
  KoZoomHandler* m_zoomHandler;
};

#endif
