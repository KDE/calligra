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
#ifndef TOOL_CONNECTOR_H
#define TOOL_CONNECTOR_H

#include "kivio_mousetool.h"
#include <KoPoint.h>

class QMouseEvent;
class QCursor;

namespace Kivio {
  class MouseToolAction;
}

class KivioView;
class KivioPage;
class Kivio1DStencil;
class KivioCustomDragData;

class ConnectorTool : public Kivio::MouseTool
{
  Q_OBJECT
  public:
    ConnectorTool( KivioView* parent );
    ~ConnectorTool();

    virtual bool processEvent(QEvent* e);

    void connector(QRect);

  public slots:
    void setActivated(bool a);

  protected slots:
    void activateStraight();
    void activatePolyline();

    void makePermanent();

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

    enum {
      StraightConnector,
      PolyLineConnector
    };

  private:
    int m_mode; // Flag to indicate that we are drawing a rubber band
    int m_type; // Type of connector
    QCursor* m_pConnectorCursor1;
    QCursor* m_pConnectorCursor2;
    Kivio1DStencil* m_pStencil;
    KoPoint startPoint;
    KivioCustomDragData* m_pDragData;

    Kivio::MouseToolAction* m_connectorAction;
    Kivio::MouseToolAction* m_polyLineAction;

    bool m_permanent;
};

#endif


