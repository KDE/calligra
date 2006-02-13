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

#include "kiviopolylineconnector.h"

#include <kdebug.h>

#include <KoZoomHandler.h>
#include <KoSize.h>

#include "kivio_line_style.h"
#include "kivio_painter.h"
#include "kivio_intra_stencil_data.h"
#include "kivio_connector_point.h"
#include "kivio_custom_drag_data.h"
#include "tkmath.h"
#include "kivio_page.h"
#include "kivio_layer.h"
#include "kivio_common.h"


namespace Kivio {
  PolyLineConnector::PolyLineConnector()
    : Kivio1DStencil()
  {
    m_startArrow = new KivioArrowHead();
    m_endArrow = new KivioArrowHead();

    m_needsWidth = false;
    m_needsText = false; // FIXME add text support

    m_pCanProtect->clearBit(kpAspect);
    m_pCanProtect->clearBit(kpWidth);
    m_pCanProtect->clearBit(kpHeight);
    m_pCanProtect->clearBit(kpX);
    m_pCanProtect->clearBit(kpY);

    // This is a stencil of type connector
    setType(kstConnector);
  }

  PolyLineConnector::~PolyLineConnector()
  {
    delete m_startArrow;
    delete m_endArrow;
  }

  KivioStencil* PolyLineConnector::duplicate()
  {
    PolyLineConnector* connector = new PolyLineConnector();
    copyBasicInto(connector);
    connector->m_points = m_points;

    // Copy the arrow head information
    connector->setStartAHType( m_startArrow->type() );
    connector->setStartAHWidth( m_startArrow->width() );
    connector->setStartAHLength( m_startArrow->length() );

    connector->setEndAHType( m_endArrow->type() );
    connector->setEndAHWidth( m_endArrow->width() );
    connector->setEndAHLength( m_endArrow->length() );

    return connector;
  }

  bool PolyLineConnector::loadCustom(const QDomElement& e)
  {
    QDomNode node = e.firstChild();
    QString nodeName;
    m_points.clear();

    while(!node.isNull()) {
      nodeName = node.nodeName();

      if(nodeName == "KivioArrowHeads") {
        loadArrowHeads(node.toElement());
      } else if(nodeName == "KivioGeometryPoints") {
        QDomNode pointsNode = node.firstChild();
        QDomElement el;

        while(!pointsNode.isNull()) {
          if(pointsNode.nodeName() == "KivioPoint") {
            el = pointsNode.toElement();
            KoPoint p;
            p.setX(XmlReadFloat(el, "x", 1.0f));
            p.setY(XmlReadFloat(el, "y", 1.0f));
            addPoint(p);
          }

          pointsNode = pointsNode.nextSibling();
        }
      }

      node = node.nextSibling();
    }

    return true;
  }

  bool PolyLineConnector::saveCustom(QDomElement& e, QDomDocument& doc)
  {
    e.appendChild(saveArrowHeads(doc));
    QDomElement pointsElement = doc.createElement("KivioGeometryPoints");

    for(QValueList<KoPoint>::iterator it = m_points.begin(); it != m_points.end(); ++it) {
      KoPoint p = (*it);
      QDomElement el = doc.createElement("KivioPoint");
      XmlWriteFloat(el, "x", p.x());
      XmlWriteFloat(el, "y", p.y());
      pointsElement.appendChild(el);
    }

    e.appendChild(pointsElement);

    return true;
  }

  KivioCollisionType PolyLineConnector::checkForCollision(KoPoint* p, double threshold)
  {
    unsigned int i = 0;
    KoPoint point;
    double px = p->x();
    double py = p->y();
    uint count = m_points.count();

    while(i < count) {
      point = m_points[i];

      if(px >= point.x() - threshold && px <= point.x() + threshold &&
        py >= point.y() - threshold && py <= point.y() + threshold)
      {
        return static_cast<KivioCollisionType>(i + kctCustom + 1);
      }

      i++;
    }

    i = 0;
    count--; // As we need current + 1;

    while(i < count) {
      point = m_points[i];

      if(collisionLine(point.x(), point.y(),
        m_points[i + 1].x(), m_points[i + 1].y(), px, py, threshold))
      {
        return kctBody;
      }

      i++;
    }

    return kctNone;
  }

  void PolyLineConnector::paint(KivioIntraStencilData* data)
  {
    if(m_points.count() < 2) {
      kdDebug(43000) << "ARGH! We're missing points in this connector!" << endl;
      return;
    }

    KoZoomHandler* zoom = data->zoomHandler;
    KivioPainter* painter = data->painter;

    painter->setLineStyle(m_pLineStyle);
    painter->setLineWidth(zoom->zoomItY(m_pLineStyle->width()));

    QPointArray pa(m_points.count());
    QValueList<KoPoint>::iterator it;
    int i = 0;

    for(it = m_points.begin(); it != m_points.end(); ++it) {
      pa.setPoint(i, zoom->zoomPoint(*it));
      i++;
    }

    KoPoint startVec = m_points[1] - m_points[0];
    KoPoint endVec = m_points.last() - m_points[m_points.count() - 2];
    double startLen = startVec.manhattanLength();
    double endLen = endVec.manhattanLength();

    if(startLen) {
      startVec.setX(startVec.x() / startLen);
      startVec.setY(startVec.y() / startLen);
      QPoint p = pa[0];
      p.setX(p.x() + qRound(startVec.x() * zoom->zoomItX(m_startArrow->cut())));
      p.setY(p.y() + qRound(startVec.y() * zoom->zoomItY(m_startArrow->cut())));
    }

    if(endLen) {
      endVec.setX(endVec.x() / endLen);
      endVec.setY(endVec.y() / endLen);
      QPoint p = pa[m_points.count() - 1];
      p.setX(p.x() + qRound(endVec.x() * zoom->zoomItX(m_endArrow->cut())));
      p.setY(p.y() + qRound(endVec.y() * zoom->zoomItY(m_endArrow->cut())));
    }

    painter->drawPolyline(pa);
    painter->setBGColor(m_pFillStyle->color());

    if(startLen) {
      m_startArrow->paint(painter, m_points[0].x(), m_points[0].y(), -startVec.x(), -startVec.y(), zoom);
    }

    if(endLen) {
      m_endArrow->paint(painter, m_points.last().x(), m_points.last().y(), endVec.x(), endVec.y(), zoom);
    }
  }

  void PolyLineConnector::paintOutline(KivioIntraStencilData* data)
  {
    paint(data);
  }

  void PolyLineConnector::paintSelectionHandles( KivioIntraStencilData* data )
  {
    KivioPainter* painter = data->painter;
    KoZoomHandler* zoomHandler = data->zoomHandler;
    QValueList<KoPoint>::Iterator it;
    int x, y, flag;
    x = y = flag = 0;

    for(it = m_points.begin(); it != m_points.end(); ++it) {
      x = zoomHandler->zoomItX((*it).x());
      y = zoomHandler->zoomItY((*it).y());    

      if((*it) == m_pEnd->position()) {
        flag = ((m_pEnd->target()) ? KivioPainter::cpfConnected : 0) | KivioPainter::cpfEnd;
      } else if((*it) == m_pStart->position()) {
        flag = ((m_pStart->target()) ? KivioPainter::cpfConnected : 0) | KivioPainter::cpfStart;
      } else {
        flag = 0;
      }

      painter->drawHandle(x, y, flag);
    }
  }

  void PolyLineConnector::addPoint(const KoPoint& p)
  {
    if(m_points.count() == 0) {
      m_pStart->setPosition(p.x(), p.y(), false);
      m_pStart->disconnect();
    } else {
      m_pEnd->setPosition(p.x(), p.y(), false);
      m_pEnd->disconnect();
    }

    m_points.append(p);
  }

  void PolyLineConnector::movePoint(unsigned int index, double xOffset, double yOffset)
  {
    if(index >= m_points.count()) {
      return;
    }

    KoPoint p(xOffset, yOffset);
    m_points[index] += p;

    if(index == (m_points.count() - 1)) {
      m_pEnd->setPosition(m_points[index].x(), m_points[index].y(), false);
      m_pEnd->disconnect();
    } else if(index == 0) {
      m_pStart->setPosition(m_points[index].x(), m_points[index].y(), false);
      m_pStart->disconnect();
    }
  }

  void PolyLineConnector::movePointTo(unsigned int index, const KoPoint& p)
  {
    m_points[index] = p;

    if(index == (m_points.count() - 1)) {
      m_pEnd->setPosition(p.x(), p.y(), false);
      m_pEnd->disconnect();
    } else if(index == 0) {
      m_pStart->setPosition(p.x(), p.y(), false);
      m_pStart->disconnect();
    }
  }

  void PolyLineConnector::moveLastPointTo(const KoPoint& p)
  {
    movePointTo(m_points.count() - 1, p);
  }

  void PolyLineConnector::customDrag(KivioCustomDragData* data)
  {
    KoPoint pos(data->x, data->y);
    setCustomIDPoint(data->id, pos, data->page);
  }

  void PolyLineConnector::move(double xOffset, double yOffset)
  {
    for(unsigned int i = 0; i < m_points.count(); i++) {
      movePoint(i, xOffset, yOffset);
    }
  }

  double PolyLineConnector::x()
  {
    if(m_points.count() == 0) {
      return 0;
    }

    return rect().x();
  }

  void PolyLineConnector::setX(double newX)
  {
    double dx = newX - x();
    move(dx, 0);
  }

  double PolyLineConnector::y()
  {
    if(m_points.count() == 0) {
      return 0;
    }

    return rect().y();
  }

  void PolyLineConnector::setY(double newY)
  {
    double dy = newY - y();
    move(0, dy);
  }

  void PolyLineConnector::checkForConnection(KivioConnectorPoint* cp, KivioPage* page)
  {
    if(cp->connectable()) {
      KivioLayer* currentLayer = page->curLayer();
      KivioLayer* layer = page->firstLayer();
      bool found = false;

      while(layer && !found) {
        if((layer != currentLayer) && (!layer->connectable() || !layer->visible())) {
          layer = page->nextLayer();
          continue;
        }

        if(layer->connectPointToTarget(cp, 8.0f)) {
          found = true;
        }

        layer = page->nextLayer();
      }

      if(!found) {
        cp->disconnect();
      }
    }
  }

  void PolyLineConnector::updateConnectorPoints(KivioConnectorPoint* cp, double /*oldX*/, double /*oldY*/)
  {
    if(cp == m_pStart) {
      m_points[0] = m_pStart->position();
    } else if(cp == m_pEnd) {
      m_points[m_points.count() - 1] = m_pEnd->position();
    }
  }

  KoRect PolyLineConnector::rect()
  {
    KoPoint p = m_points.first();
    KoPoint topLeft(p.x(), p.y()), bottomRight;
    QValueList<KoPoint>::iterator it;
    QValueList<KoPoint>::iterator itEnd = m_points.end();

    for(it = m_points.begin(); it != itEnd; ++it) {
      p = (*it);
      topLeft.setX(QMIN(p.x(), topLeft.x()));
      topLeft.setY(QMIN(p.y(), topLeft.y()));
      bottomRight.setX(QMAX(p.x(), bottomRight.x()));
      bottomRight.setY(QMAX(p.y(), bottomRight.y()));
    }

    KoRect rect;
    rect.moveTopLeft(topLeft);
    rect.setWidth(bottomRight.x() - topLeft.x());
    rect.setHeight(bottomRight.y() - topLeft.y());
    return rect;
  }

  bool PolyLineConnector::isInRect(const KoRect& rect)
  {
    QValueList<KoPoint>::Iterator it;
    bool retVal = true;

    for(it = m_points.begin(); it != m_points.end(); ++it) {
      retVal = retVal && rect.contains((*it));
    }

    return retVal;
  }

  bool PolyLineConnector::loadArrowHeads(const QDomElement& e)
  {
    bool first = true;
    QDomNode node = e.firstChild();

    while(!node.isNull()) {
      if(node.nodeName() == "KivioArrowHead") {
        if(first) {
          m_startArrow->loadXML(node.toElement());
          first = false;
        } else {
          m_endArrow->loadXML(node.toElement());
        }
      }

      node = node.nextSibling();
    }

    return true;
  }

  QDomElement PolyLineConnector::saveArrowHeads(QDomDocument& doc)
  {
    QDomElement e = doc.createElement("KivioArrowHeads");

    e.appendChild( m_startArrow->saveXML(doc) );
    e.appendChild( m_endArrow->saveXML(doc) );

    return e;
  }

  void PolyLineConnector::removePoint(unsigned int index)
  {
    if(index >= m_points.count()) {
      return;
    }

    m_points.remove(m_points.at(index));
  }

  void PolyLineConnector::removeLastPoint()
  {
    removePoint(m_points.count() - 1);
  }

  void PolyLineConnector::setCustomIDPoint(int customID, const KoPoint& point, KivioPage* page)
  {
    int index = customID - (kctCustom + 1);

    if((index < 0) || index >= (int)m_points.count()) {
      kdDebug(43000) << "PolyLineConnector::setCustomIDPoint: Index out of range! Index = " << index << endl;
      return;
    }

    movePointTo(index, point);
    KivioConnectorPoint* cp;

    if(index == 0) {
      cp = m_pStart;
    } else if(index == ((int)m_points.count() - 1)) {
      cp = m_pEnd;
    } else {
      return;
    }

    checkForConnection(cp, page);
  }

  KoPoint PolyLineConnector::customIDPoint(int customID)
  {
    int index = customID - (kctCustom + 1);

    if((index < 0) || index >= (int)m_points.count()) {
      kdDebug(43000) << "PolyLineConnector::customIDPoint: Index out of range! Index = " << index << endl;
      return KoPoint();
    }

    return m_points[index];
  }

}
