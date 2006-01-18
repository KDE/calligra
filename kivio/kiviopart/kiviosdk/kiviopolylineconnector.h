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

#ifndef KIVIOKIVIOPOLYLINECONNECTOR_H
#define KIVIOKIVIOPOLYLINECONNECTOR_H

#include <qvaluelist.h>

#include <KoPoint.h>
#include <KoRect.h>

#include "kivio_1d_stencil.h"
#include "kivio_arrowhead.h"
#include <koffice_export.h>
class KivioIntraStencilData;
class KivioCustomDragData;
class KivioPage;

namespace Kivio {
  class KIVIO_EXPORT PolyLineConnector : public Kivio1DStencil
  {
    public:
      PolyLineConnector();
      virtual ~PolyLineConnector();
      
      virtual KivioStencil* duplicate();
      
      virtual bool loadCustom(const QDomElement& e);
      virtual bool saveCustom(QDomElement& e, QDomDocument& doc);
      
      virtual KivioCollisionType checkForCollision(KoPoint* p, double threshold);
  
      virtual void paint(KivioIntraStencilData* data);
      virtual void paintOutline(KivioIntraStencilData* data);
      virtual void paintSelectionHandles(KivioIntraStencilData* data);
      
      /** Append a new point to the list.
       *
       * @param p The point to append.
       */
      void addPoint(const KoPoint& p);
      /// Remove a point from the list.
      void removePoint(unsigned int index);
      /// Remove the last point from the list
      void removeLastPoint();
      
      unsigned int pointCount() const { return m_points.count(); }
      
      /** Move point with index @param index.
       *
       * @param index Which point to move.
       * @param xOffset How far to move the point horizontaly.
       * @param yOffset How far to move the point verticaly.
       */
      void movePoint(unsigned int index, double xOffset, double yOffset);
      
      void movePointTo(unsigned int index, const KoPoint& p);
      
      void moveLastPointTo(const KoPoint& p);
      
      virtual void customDrag(KivioCustomDragData* data);
      virtual void move(double xOffset, double yOffset);
      virtual double x();
      virtual void setX(double x);
      virtual double y();
      virtual void setY(double y);
      virtual double w() { return 0; }
      virtual double h() { return 0; }
      virtual KoRect rect();
      virtual bool isInRect(const KoRect& rect);
      virtual void setCustomIDPoint(int customID, const KoPoint& point, KivioPage* page);
      virtual KoPoint customIDPoint(int customID);


      virtual void updateConnectorPoints(KivioConnectorPoint* cp, double /*oldX*/, double /*oldY*/);
      
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
    
    protected:
      void checkForConnection(KivioConnectorPoint* cp, KivioPage* page);
  
      bool loadArrowHeads(const QDomElement& e);
      QDomElement saveArrowHeads(QDomDocument& doc);

    private:
      KivioArrowHead* m_startArrow; // Arrow head for the first point.
      KivioArrowHead* m_endArrow; // Arrow head for the last point.
      
      QValueList<KoPoint> m_points;
  };
}

#endif
