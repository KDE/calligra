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
#ifndef TOOL_CONNECTOR_H
#define TOOL_CONNECTOR_H

#include "tool.h"
#include "tkunits.h"

class QMouseEvent;
class QCursor;

class KivioView;
class KivioPage;
class KivioStraightConnector;
class KivioCustomDragData;

class ConnectorTool : public Tool
{ Q_OBJECT
public:
  ConnectorTool( KivioView* view );
  ~ConnectorTool();

  virtual void processEvent( QEvent* );
  virtual void activate();
  virtual void deactivate();
  virtual void configure();

  void connector(QRect);

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
      stmDrawRubber,
  };
    
private:
  // Flag to indicate that we are drawing a rubber band
  int m_mode;
  QCursor* m_pConnectorCursor1;
  QCursor* m_pConnectorCursor2;
  KivioStraightConnector* m_pStencil;
  TKPoint startPoint;
  KivioCustomDragData* m_pDragData;
};

#endif


