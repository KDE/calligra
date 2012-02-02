/*
 * Copyright (C) 2012 Cyrille Berger <cberger@cberger.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 sure
 * USA
 */

#include "kto_canvas.h"

#include <QDebug>
#include <QPainter>

KtoCanvas::KtoCanvas(QDeclarativeItem* parent): QDeclarativeItem(parent), m_doc(0)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    qDebug() << "oh";
}

KtoCanvas::~KtoCanvas()
{

}

void KtoCanvas::paint(QPainter* painter, const QStyleOptionGraphicsItem* , QWidget* )
{
    if(m_doc)
    {
        
    } else {
        painter->fillRect(boundingRect(), Qt::gray);
    }
}

void KtoCanvas::setDocument(KisDoc2* _doc)
{
    m_doc = _doc;
}
