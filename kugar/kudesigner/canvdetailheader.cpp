/* This file is part of the KDE project
   Copyright (C) 2003 Alexander Dymo <cloudtemple@mksat.net>

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
#include <map>

#include <klocale.h>

#include <qpainter.h>

#include "canvdetailheader.h"
#include "property.h"

CanvasDetailHeader::CanvasDetailHeader(int x, int y, int width, int height, QCanvas * canvas):
    CanvasBand(x, y, width, height, canvas)
{
    props["Height"] = *(new PropPtr(new Property(IntegerValue, "Height", i18n("Detail header's height"), "50")));

    props["Level"] = *(new PropPtr(new Property(IntegerValue, "Level", i18n("Detail header's level"), "0")));
}

void CanvasDetailHeader::draw(QPainter &painter)
{
    QString str = QString("%1 %2").arg(i18n("Detail header")).arg(props["Level"]->value().toInt());
    painter.drawText(rect(), AlignVCenter | AlignLeft, str);
    CanvasBand::draw(painter);
}

QString CanvasDetailHeader::getXml()
{
    return "\t<DetailHeader" + CanvasBand::getXml() + "\t</DetailHeader>\n\n";
}
