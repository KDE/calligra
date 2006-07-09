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
#ifndef SML_TOOL_CONNECTOR_H
#define SML_TOOL_CONNECTOR_H

#include "kivio_mousetool.h"
#include <KoPoint.h>

class QMouseEvent;
class QCursor;

class KToggleAction;

class KivioView;
class KivioPage;
class KivioSMLConnector;
class KivioCustomDragData;

class SMLConnector : public Kivio::MouseTool
{ Q_OBJECT
public:
  SMLConnector( KivioView* view );
  ~SMLConnector();

  virtual bool processEvent( QEvent* );

  void connector(QRect);
  
public slots:
  virtual void setActivated(bool a);

signals:
  void operationDone();

protected:
  void mousePress(QMouseEvent *);
  void mouseMove(QMouseEvent *);
  void mouseRelease(QMouseEvent *);

  bool startRubberBanding(QMouseEvent*);
  void continueRubberBanding(QMouseEvent *);
  void endRubberBanding(QMouseEvent *);



  QPoint m_startPoint, m_releasePoint;

  // Connector Tool Mode
  enum
  {
      stmNone,
      stmDrawRubber
  };

private:
  // Flag to indicate that we are drawing a rubber band
  int m_mode;
  QCursor* m_pConnectorCursor1;
  QCursor* m_pConnectorCursor2;
  KivioSMLConnector* m_pStencil;
  KoPoint startPoint;
  KivioCustomDragData* m_pDragData;

  KToggleAction* m_connectorAction;
};

#endif


