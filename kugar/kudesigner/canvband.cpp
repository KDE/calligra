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

#include <kglobalsettings.h>

#include <qcanvas.h>
#include <qrect.h>
#include <qpainter.h>

#include "canvband.h"
#include "canvbox.h"
#include "canvkutemplate.h"
#include "mycanvas.h"
#include "creportitem.h"


void CanvasBand::draw(QPainter &painter)
{
    setX(((MyCanvas*)canvas())->templ->props["LeftMargin"]->value().toInt());
    setSize(((MyCanvas*)canvas())->templ->width()
        - ((MyCanvas*)canvas())->templ->props["RightMargin"]->value().toInt()
        - ((MyCanvas*)canvas())->templ->props["LeftMargin"]->value().toInt(),
        props["Height"]->value().toInt());
    CanvasSection::draw(painter);
}

//arrange band and all sublings (items)
void CanvasBand::arrange(int base, bool destructive)
{
    int diff = base - (int)y();
    setY(base);
    if (!destructive)
        return;
    for (QCanvasItemList::Iterator it=items.begin(); it!=items.end(); ++it)
    {
        (*it)->moveBy(0, diff);
    //  ( (CanvasReportItem *)(*it) )->updateGeomProps();
        canvas()->update();
        (*it)->hide();
        (*it)->show();
    }
}

int CanvasBand::minHeight()
{
	int result=y()+10;
	for (QCanvasItemList::Iterator it=items.begin(); it!=items.end(); ++it)
	{
		result=QMAX(result,((QCanvasRectangle*)(*it))->y()+
			((QCanvasRectangle*)(*it))->height());
	}
	return result-y();
}

QString CanvasBand::getXml()
{
    QString result = "";
    std::map<QString, PropPtr >::const_iterator it;
    for (it = props.begin(); it != props.end(); ++it)
    {
	if (it->second->allowSaving())
        result += " " + it->first + "=" + "\"" + it->second->value() + "\"";
    }
    result += ">\n";
    for (QCanvasItemList::Iterator it=items.begin(); it!=items.end(); ++it)
    {
        result += ((CanvasReportItem *)(*it))->getXml();
    }
    return result;
}

int CanvasBand::isInHolder(const QPoint p)
{
    if (bottomMiddleResizableRect().contains(p)) return (ResizeBottom);
    return ResizeNothing;
}

void CanvasBand::drawHolders(QPainter &painter)
{
    painter.setPen(QColor(0, 0, 0));
    painter.setBrush(KGlobalSettings::highlightColor());
    painter.drawRect(bottomMiddleResizableRect());
}

QRect CanvasBand::bottomMiddleResizableRect()
{
    return QRect((int)(x()+width()/2-HolderSize/2.), (int)(y()+height()-HolderSize), HolderSize, HolderSize);
}

void CanvasBand::updateGeomProps()
{
    props["Height"]->setValue(QString("%1").arg(height()));
    ((MyCanvas *)canvas())->templ->arrangeSections();
}

CanvasBand::~CanvasBand()
{
    for (QCanvasItemList::Iterator it = items.begin(); it != items.end(); ++it)
    {
//  (*it)->hide();
        ((MyCanvas *)canvas())->selected.remove((CanvasBox*)(*it));
        (*it)->setSelected(false);
        delete (*it);
    }
    items.clear();
}
