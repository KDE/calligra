/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2003 theKompany.com & Dave Marotti,
 *                         Peter Simonsson
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
#ifndef TOOL_TEXT_H
#define TOOL_TEXT_H

#include "kivio_mousetool.h"
#include <koffice_export.h>
class QMouseEvent;
class QCursor;

namespace Kivio {
  class MouseToolAction;
}

class KivioView;
class KivioPage;
class KoPoint;

class KIVIO_EXPORT TextTool : public Kivio::MouseTool
{
  Q_OBJECT
  public:
    TextTool( KivioView* parent );
    ~TextTool();
  
    virtual bool processEvent(QEvent* e);
  
    void text(QRect);
  
  public slots:
    virtual void setActivated(bool a);
    virtual void applyToolAction(QPtrList<KivioStencil>* stencils);
    virtual void applyToolAction(KivioStencil* stencil, const KoPoint& pos);
  
  protected slots:
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
  
    // Text Tool Mode
    enum
    {
      stmNone,
      stmDrawRubber
    };
  
  private:
    // Flag to indicate that we are drawing a rubber band
    int m_mode;
    QCursor* m_pTextCursor;
    Kivio::MouseToolAction* m_textAction;
    bool m_permanent;
};

#endif


