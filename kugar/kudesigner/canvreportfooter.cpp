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

#include "canvreportfooter.h"
#include "property.h"

CanvasReportFooter::CanvasReportFooter(int x, int y, int width, int height, QCanvas * canvas):
    CanvasBand(x, y, width, height, canvas)
{
    props["Height"] = *(new PropPtr(new Property(IntegerValue, "Height", i18n("Report footer's height"), "50")));
}

void CanvasReportFooter::draw(QPainter &painter)
{
    painter.drawText(rect(), AlignVCenter | AlignLeft,
        i18n("Report Footer"));
    CanvasBand::draw(painter);
}

QString CanvasReportFooter::getXml()
{
    return "\t<ReportFooter PrintFrequency=\"0\"" + CanvasBand::getXml() + "\t</ReportFooter>\n\n";
}
