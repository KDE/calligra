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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KIVIOKIVIOPOLYLINECONNECTOR_H
#define KIVIOKIVIOPOLYLINECONNECTOR_H

#include <qvaluelist.h>

#include <koPoint.h>

#include "kivio_1d_stencil.h"
#include "kivio_arrowhead.h"

class KivioIntraStencilData;

namespace Kivio {
  class PolyLineConnector : public Kivio1DStencil
  {
    public:
      PolyLineConnector();
      virtual ~PolyLineConnector();
      
      virtual KivioCollisionType checkForCollision(KoPoint* p, double threshold);
  
      virtual void paint(KivioIntraStencilData* data);
      virtual void paintOutline(KivioIntraStencilData* data);
      
      /** Append a new point to the list.
       *
       * @param p The point to append.
       */
      void addPoint(const KoPoint& p);
      
      /** Move point with index @param index.
       *
       * @param index Which point to move.
       * @param xOffset How far to move the point horizontaly.
       * @param yOffset How far to move the point verticaly.
       */
      void movePoint(int index, double xOffset, double yOffset);
      
      void movePointTo(int index, const KoPoint& p);
      
      void moveLastPointTo(const KoPoint& p);

      // Arrow head properties
      virtual void setStartAHType(int i) { m_startArrow->setType(i); }
      virtual int startAHType() { return m_startArrow->type(); }
      
      virtual void setStartAHWidth(double f) { m_startArrow->setWidth(f); }
      virtual double startAHWidth() { return m_startArrow->width(); }
      
      virtual void setStartAHLength(double f) { m_startArrow->setLength(f); }
      virtual double startAHLength() { return m_startArrow->length(); }
      
      virtual void setEndAHType(int i) { m_endArrow->setType(i); }
      virtual int endAHType() { return m_endArrow->type(); }
      
      virtual void setEndAHWidth(double f) { m_endArrow->setWidth(f); }
      virtual double endAHWidth() { return m_endArrow->width(); }
      
      virtual void setEndAHLength(double f) { m_endArrow->setLength(f); }
      virtual double endAHLength() { return m_endArrow->length(); }
  
    private:
      KivioArrowHead* m_startArrow; // Arrow head for the first point.
      KivioArrowHead* m_endArrow; // Arrow head for the last point.
      
      QValueList<KoPoint> m_points;
  };
};

#endif
