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

#include "kiviopolylineconnector.h"

#include <kozoomhandler.h>

#include "kivio_line_style.h"
#include "kivio_painter.h"
#include "kivio_intra_stencil_data.h"


namespace Kivio {
  PolyLineConnector::PolyLineConnector()
    : Kivio1DStencil()
  {
    m_startArrow = new KivioArrowHead();
    m_endArrow = new KivioArrowHead();
  
    // This is a stencil of type connector
    setType(kstConnector);
  }
  
  PolyLineConnector::~PolyLineConnector()
  {
    delete m_startArrow;
    delete m_endArrow;
  }

  KivioCollisionType PolyLineConnector::checkForCollision(KoPoint* p, double threshold)
  {
  }

  void PolyLineConnector::paint(KivioIntraStencilData* data)
  {
    KoZoomHandler* zoom = data->zoomHandler;
    KivioPainter* painter = data->painter;
    
    painter->setLineStyle(m_pLineStyle);
    painter->setLineWidth(zoom->zoomItY(m_pLineStyle->width()));
    
    QPointArray pa(m_points.count());
    QValueList<KoPoint>::iterator it;
    int i = 0;
    
    for(it = m_points.begin(); it != m_points.end(); it++) {
      pa.setPoint(i, zoom->zoomPoint(*it));
      i++;
    }
    
    painter->drawLineArray(pa);
  }
  
  void PolyLineConnector::paintOutline(KivioIntraStencilData* data)
  {
  }
  
  void PolyLineConnector::addPoint(const KoPoint& p)
  {
    m_points.append(p);
  }
  
  void PolyLineConnector::movePoint(int index, double xOffset, double yOffset)
  {
  }

  void PolyLineConnector::movePointTo(int index, const KoPoint& p)
  {
    m_points[index] = p;
  }
  
  void PolyLineConnector::moveLastPointTo(const KoPoint& p)
  {
    movePointTo(m_points.count() - 1, p);
  }
}
