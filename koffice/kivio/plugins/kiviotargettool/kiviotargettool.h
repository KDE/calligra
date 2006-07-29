/* This file is part of the KDE project
   Copyright (C) 2004 Peter Simonsson <psn@linux.se>,

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#ifndef KIVIOKIVIOTARGETTOOL_H
#define KIVIOKIVIOTARGETTOOL_H

#include <KoPoint.h>

#include "kivio_mousetool.h"

class QMouseEvent;

namespace Kivio {
  class MouseToolAction;
}

class KivioStencil;
class KivioView;

namespace Kivio {
  class TargetTool : public MouseTool
  {
    Q_OBJECT
    public:
      TargetTool(KivioView* parent);
      ~TargetTool();
  
      virtual bool processEvent(QEvent* e);
      
    public slots:
      virtual void setActivated(bool);
      virtual void applyToolAction(KivioStencil* stencil, const KoPoint& pos);
    
    protected slots:
      void makePermanent();
    
    protected:
      void mousePress(QMouseEvent* e);
      void mouseMove(QMouseEvent* e);
    
    private:
      Kivio::MouseToolAction* m_targetAction;
      bool m_permanent;
  };
}

#endif
