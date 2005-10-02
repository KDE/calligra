/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#include "kptchartdataset.h"

namespace KPlato
{

KPTChartDataSetItem::KPTChartDataSetItem(const QDateTime &x1, const QDateTime &x2, double y1, double y2) 
    : m_parent(0),
      m_prio(0.0),
      m_pen(0),
      m_brush(0) {
    m_x1 = x1; m_x2 = x2; m_y1 = y1; m_y2 = y2;
}

KPTChartDataSetItem::KPTChartDataSetItem(double y, QPen *pen) 
    : m_parent(0),
      m_prio(-100.0),
      m_pen(pen),
      m_brush(0) {
    m_y1 = m_y2 = y;
}

KPTChartDataSetItem::KPTChartDataSetItem(const QDateTime &x, QPen *pen) 
    : m_parent(0),
      m_prio(-100.0),
      m_pen(pen),
      m_brush(0) {
    m_x1 = m_x2 = x;
}

KPTChartDataSetItem::~KPTChartDataSetItem() { 
    delete m_pen; delete m_brush; 
}
    
void KPTChartDataSetItem::setPrio(double prio) { 
    m_prio = prio; 
}

QPen *KPTChartDataSetItem::pen() const {
    return m_pen ? m_pen : m_parent->pen(); 
}

void KPTChartDataSetItem::setPen(QPen *pen) {
    delete m_pen; m_pen = pen; 
}

QBrush *KPTChartDataSetItem::brush() const { 
    return m_brush ? m_brush : m_parent->brush(); 
}

void KPTChartDataSetItem::setBrush(QBrush *brush) { 
    delete m_brush; m_brush = brush; 
}
    
KPTChartDataSet::KPTChartDataSet(QPen *pen, QBrush *brush) 
    : m_mode(Mode_Default),
      m_pen(pen),
      m_brush(brush) 
{
    m_dataItems.setAutoDelete(true);
}

KPTChartDataSet::~KPTChartDataSet() { 
    delete m_pen; delete m_brush; 
}
        
void KPTChartDataSet::append(KPTChartDataSetItem *item) {
    item->setParent(this); m_dataItems.append(item);
}

void KPTChartDataSet::clear() {
    m_dataItems.clear();
}

}  //KPlato namespace
