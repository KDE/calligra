/***************************************************************************
                          canvbox.cpp  -  description
                             -------------------
    begin                : 07.06.2002
    copyright            : (C) 2002 by Alexander Dymo
    email                : cloudtemple@mksat.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <klocale.h>

#include <qcanvas.h>
#include <qapplication.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qpainter.h>
#include "canvbox.h"
#include "mycanvas.h"
#include "creportitem.h"

//CanvasBox class

void CanvasBox::scale(int scale)
{
    setSize(width()*scale, height()*scale);
}

void CanvasBox::draw(QPainter &painter)
{
    QCanvasRectangle::draw(painter);
}


//CanvasSection class

void CanvasSection::draw(QPainter &painter)
{
    CanvasBox::draw(painter);
}


//CanvasKugarTemplate class

CanvasKugarTemplate::CanvasKugarTemplate(int x, int y, int width, int height, QCanvas * canvas):
	CanvasSection(x, y, width, height, canvas)
{
    detailsCount = 0;

    setZ(1);
    ((MyCanvas *)canvas)->templ = this;
    reportHeader = 0;
    reportFooter = 0;
    pageHeader = 0;
    pageFooter = 0;

    std::pair<QString, QStringList> propValues;

    propValues.first = "0";
    propValues.second << i18n("CanvasKugarTemplate", "Page size");
    propValues.second << "int_from_list";
    propValues.second << "0 - A4" << "1 - B5" << "2 - Letter"
	    << "3 - Legal" << "4 - Executive" << "5 - A0"
	    << "6 - A1" << "7 - A2" << "8 - A3" << "9 - A5"
	    << "10 - A6" << "11 - A7" << "12 - A8"
	    << "13 - A9" << "14 - B0" << "15 - B1" << "16 - B10"
	    << "17 - B2" << "18 - B3" << "19 - B4" << "20 - B6"
	    << "21 - B7" << "22 - B8" << "23 - B9" << "24 - C5E"
	    << "25 - Comm10E" << "26 - DLE" << "27 - Folio"
	    << "28 - Ledger" << "29 - Tabloid" << "30 - NPageSize";
    props["PageSize"] = propValues;
    propValues.second.clear();

    propValues.first = "0";
    propValues.second << i18n("CanvasKugarTemplate", "Page orientation");
    propValues.second << "int_from_list";
    propValues.second << i18n("CanvasKugarTemplate", "0 - Portrait");
    propValues.second << i18n("CanvasKugarTemplate", "1 - Landscape");
    props["PageOrientation"] = propValues;
    propValues.second.clear();

    propValues.first = "0";
    propValues.second << i18n("CanvasKugarTemplate", "Top margin");
    propValues.second << "int";
    props["TopMargin"] = propValues;
    propValues.second.clear();

    propValues.first = "0";
    propValues.second << i18n("CanvasKugarTemplate", "Bottom margin");
    propValues.second << "int";
    props["BottomMargin"] = propValues;
    propValues.second.clear();

    propValues.first = "0";
    propValues.second << i18n("CanvasKugarTemplate", "Left margin");
    propValues.second << "int";
    props["LeftMargin"] = propValues;
    propValues.second.clear();

    propValues.first = "0";
    propValues.second << i18n("CanvasKugarTemplate", "Right margin");
    propValues.second << "int";
    props["RightMargin"] = propValues;
    propValues.second.clear();
}

CanvasKugarTemplate::~CanvasKugarTemplate()
{
    if (reportHeader)
	delete reportHeader;
    if (pageHeader)
	delete pageHeader;

    std::map<int, DetailBand>::const_iterator it;
    for (it = details.begin(); it != details.end(); ++it)
    {
	if (it->second.first.first)
	    delete it->second.first.first;
	if (it->second.second)
	    delete it->second.second;
	if (it->second.first.second)
	    delete it->second.first.second;
    }
    if (pageFooter)
	delete pageFooter;
    if (reportFooter)
	delete reportFooter;
}

void CanvasKugarTemplate::draw(QPainter &painter)
{
    painter.setPen(QColor(0, 0, 0));
    QPoint p1((int)(x()+props["LeftMargin"].first.toInt()),
	      (int)(y()+props["TopMargin"].first.toInt()));
    QPoint p2((int)(x()+props["LeftMargin"].first.toInt()),
	      (int)y() + height() - props["BottomMargin"].first.toInt());
    QPoint p3((int)x() + width() - props["RightMargin"].first.toInt(),
	      (int)y() + height() - props["BottomMargin"].first.toInt());
    QPoint p4((int)x() + width() - props["RightMargin"].first.toInt(),
	      (int)(y()+props["TopMargin"].first.toInt()));
    painter.moveTo(p1);
    painter.lineTo(p2);
    painter.lineTo(p3);
    painter.lineTo(p4);
    painter.lineTo(p1);
/*    painter.drawRect((int)(x()+props["LeftMargin"].first.toInt()),
		     (int)(y()+props["TopMargin"].first.toInt()),
		     width() - props["LeftMargin"].first.toInt() - props["RightMargin"].first.toInt(),
		     height() - props["TopMargin"].first.toInt() - props["BottomMargin"].first.toInt());*/
    CanvasSection::draw(painter);
}

/*arrange sections on page automatically*/
void CanvasKugarTemplate::arrangeSections(bool destructive)
{
    int base = props["TopMargin"].first.toInt();
    if (reportHeader)
    {
	reportHeader->arrange(base, destructive);
	base += reportHeader->props["Height"].first.toInt();
	reportHeader->show();
    }
    if (pageHeader)
    {
	pageHeader->arrange(base, destructive);
	base += pageHeader->props["Height"].first.toInt();
	pageHeader->show();
    }

    std::map<int, DetailBand>::const_iterator it;
    for (it = details.begin(); it != details.end(); ++it)
    {
	//arranging detail header
	if (it->second.first.first)
	{
	    it->second.first.first->arrange(base, destructive);
	    base += it->second.first.first->props["Height"].first.toInt();
	    it->second.first.first->show();
	}
	//arranging detail
	if (it->second.second)
	{
	    it->second.second->arrange(base, destructive);
	    base += it->second.second->props["Height"].first.toInt();
	    it->second.second->show();
	}
    }
    std::map<int, DetailBand>::reverse_iterator itr;
    for (itr = details.rbegin(); itr != details.rend(); ++itr)
    {
	//arranging detail footer
	if (itr->second.first.second)
	{
	    itr->second.first.second->arrange(base, destructive);
	    base += itr->second.first.second->props["Height"].first.toInt();
	    itr->second.first.second->show();
	}
    }

    if (pageFooter)
    {
	pageFooter->arrange(base, destructive);
	base += pageFooter->props["Height"].first.toInt();
	pageFooter->show();
    }
    if (reportFooter)
    {
	reportFooter->arrange(base, destructive);
	base += reportFooter->props["Height"].first.toInt();
	reportFooter->show();
    }
}

QString CanvasKugarTemplate::getXml()
{
    QString result = "";
    result += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    result += "<KugarTemplate";
    for (std::map<QString, std::pair<QString, QStringList> >::const_iterator it = props.begin();
        it != props.end(); it++ )
    {
	result += " " + it->first + "=" + "\"" + it->second.first + "\"";
    }
    result += ">\n";

    if (reportHeader)
	result += reportHeader->getXml();
    if (pageHeader)
	result += pageHeader->getXml();

    std::map<int, DetailBand>::const_iterator it;
    for (it = details.begin(); it != details.end(); ++it)
    {
	//getting xml from detail header
	if (it->second.first.first)
	    result += it->second.first.first->getXml();
	//getting xml from detail
	if (it->second.second)
	    result += it->second.second->getXml();
	//getting xml from detail footer
	if (it->second.first.second)
	    result += it->second.first.second->getXml();
    }
    if (pageFooter)
	result += pageFooter->getXml();
    if (reportFooter)
	result += reportFooter->getXml();

    result += "</KugarTemplate>\n";

    return result;
}

void CanvasKugarTemplate::removeSection(CanvasBand *section,
	CanvasDetailHeader **header, CanvasDetailFooter **footer)
{
    *header = 0;
    *footer = 0;
    if (section == reportHeader)
	reportHeader = 0;
    if (section == reportFooter)
	reportFooter = 0;
    if (section == pageHeader)
	pageHeader = 0;
    if (section == pageFooter)
	pageFooter = 0;
    for (std::map<int, DetailBand>::iterator i = details.begin(); i != details.end(); i++)
    {
	if (i->second.second == section)
	{
	    //delete not only detail but it's header and footer
	    i->second.second = 0;
	    *header = i->second.first.first;
	    i->second.first.first = 0;
	    *footer = i->second.first.second;
	    i->second.first.second = 0;
	    detailsCount--;
	    detailsCount < 0 ? detailsCount = 0 : detailsCount=detailsCount; // ATTENTION: Always false (unsigned!)
	}
	if (i->second.first.first == section)
	    i->second.first.first = 0;
	if (i->second.first.second == section)
	    i->second.first.second = 0;
    }
}


//CanvasBand class

void CanvasBand::draw(QPainter &painter)
{
    setSize(((MyCanvas*)canvas())->templ->width()
	    - ((MyCanvas*)canvas())->templ->props["RightMargin"].first.toInt()
	    - ((MyCanvas*)canvas())->templ->props["LeftMargin"].first.toInt(),
	    props["Height"].first.toInt());
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
	( (CanvasReportItem *)(*it) )->updateGeomProps();
	canvas()->update();
    }
}

QString CanvasBand::getXml()
{
    QString result = "";
    std::map<QString, std::pair<QString, QStringList> >::const_iterator it;
    for (it = props.begin(); it != props.end(); ++it)
    {
	result += " " + it->first + "=" + "\"" + it->second.first + "\"";
    }
    result += ">\n";
    for (QCanvasItemList::Iterator it=items.begin(); it!=items.end(); ++it)
    {
	result += ((CanvasReportItem *)(*it))->getXml();
    }
    return result;
}

CanvasBand::~CanvasBand()
{
    for (QCanvasItemList::Iterator it = items.begin(); it != items.end(); ++it)
    {
//	(*it)->hide();
	delete (*it);
    }
    items.clear();
}

//CanvasReportHeader class

CanvasReportHeader::CanvasReportHeader(int x, int y, int width, int height, QCanvas * canvas):
	CanvasBand(x, y, width, height, canvas)
{
    std::pair<QString, QStringList> propValues;

    propValues.first = "50";
    propValues.second = "Report header's height";
    propValues.second << "int";
    props["Height"] = propValues;
    propValues.second.clear();
};

void CanvasReportHeader::draw(QPainter &painter)
{
    painter.drawText(rect(), AlignVCenter | AlignLeft,
		     i18n("CanvasReportHeader", "Report Header"));
    CanvasBand::draw(painter);
}

QString CanvasReportHeader::getXml()
{
    return "\t<ReportHeader" + CanvasBand::getXml() + "\t</ReportHeader>\n\n";
}


//CanvasReportFooter class

CanvasReportFooter::CanvasReportFooter(int x, int y, int width, int height, QCanvas * canvas):
	CanvasBand(x, y, width, height, canvas)
{
    std::pair<QString, QStringList> propValues;

    propValues.first = "50";
    propValues.second = "Report footer's height";
    propValues.second << "int";
    props["Height"] = propValues;
    propValues.second.clear();
}

void CanvasReportFooter::draw(QPainter &painter)
{
    painter.drawText(rect(), AlignVCenter | AlignLeft,
		     i18n("CanvasReportFooter", "Report Footer"));
    CanvasBand::draw(painter);
}

QString CanvasReportFooter::getXml()
{
    return "\t<ReportFooter" + CanvasBand::getXml() + "\t</ReportFooter>\n\n";
}


//CanvasPageHeader class

CanvasPageHeader::CanvasPageHeader(int x, int y, int width, int height, QCanvas * canvas):
	CanvasBand(x, y, width, height, canvas)
{
    std::pair<QString, QStringList> propValues;

    propValues.first = "50";
    propValues.second = "Page header's height";
    propValues.second << "int";
    props["Height"] = propValues;
    propValues.second.clear();
}

void CanvasPageHeader::draw(QPainter &painter)
{
    painter.drawText(rect(), AlignVCenter | AlignLeft,
		     i18n("CanvasPageHeader", "Page Header"));
    CanvasBand::draw(painter);
}

QString CanvasPageHeader::getXml()
{
    return "\t<PageHeader" + CanvasBand::getXml() + "\t</PageHeader>\n\n";
}


//CanvasReportFooter class

CanvasPageFooter::CanvasPageFooter(int x, int y, int width, int height, QCanvas * canvas):
	CanvasBand(x, y, width, height, canvas)
{
    std::pair<QString, QStringList> propValues;

    propValues.first = "50";
    propValues.second = "Page footer's height";
    propValues.second << "int";
    props["Height"] = propValues;
    propValues.second.clear();
}

void CanvasPageFooter::draw(QPainter &painter)
{
    painter.drawText(rect(), AlignVCenter | AlignLeft,
		     i18n("CanvasPageFooter", "Page Footer"));
    CanvasBand::draw(painter);
}

QString CanvasPageFooter::getXml()
{
    return "\t<PageFooter" + CanvasBand::getXml() + "\t</PageFooter>\n\n";
}


//CanvasDetailHeader class

CanvasDetailHeader::CanvasDetailHeader(int x, int y, int width, int height, QCanvas * canvas):
	CanvasBand(x, y, width, height, canvas)
{
    std::pair<QString, QStringList> propValues;

    propValues.first = "50";
    propValues.second = "Detail header's height";
    propValues.second << "int";
    props["Height"] = propValues;
    propValues.second.clear();

    propValues.first = "0";
    propValues.second = "Detail header's level";
    propValues.second << "int";
    props["Level"] = propValues;
    propValues.second.clear();

}

void CanvasDetailHeader::draw(QPainter &painter)
{
    QString str = QString("Detail Header %1").arg(props["Level"].first.toInt());
    painter.drawText(rect(), AlignVCenter | AlignLeft, str);
//		     i18n("CanvasDetailHeader", "Detail Header"));
    CanvasBand::draw(painter);
}

QString CanvasDetailHeader::getXml()
{
    return "\t<DetailHeader" + CanvasBand::getXml() + "\t</DetailHeader>\n\n";
}


//CanvasDetail class

CanvasDetail::CanvasDetail(int x, int y, int width, int height, QCanvas * canvas):
	CanvasBand(x, y, width, height, canvas)
{
    std::pair<QString, QStringList> propValues;

    propValues.first = "50";
    propValues.second = "Detail height";
    propValues.second << "int";
    props["Height"] = propValues;
    propValues.second.clear();

    propValues.first = "0";
    propValues.second = "Detail level";
    propValues.second << "int";
    props["Level"] = propValues;
    propValues.second.clear();
}

void CanvasDetail::draw(QPainter &painter)
{
    QString str = QString("Detail %1").arg(props["Level"].first.toInt());
    painter.drawText(rect(), AlignVCenter | AlignLeft, str);
//		     i18n("CanvasDetail", "Detail"));
    CanvasBand::draw(painter);
}

QString CanvasDetail::getXml()
{
    return "\t<Detail" + CanvasBand::getXml() + "\t</Detail>\n\n";
}


//CanvasDetailFooter class

CanvasDetailFooter::CanvasDetailFooter(int x, int y, int width, int height, QCanvas * canvas):
	CanvasBand(x, y, width, height, canvas)
{
    std::pair<QString, QStringList> propValues;

    propValues.first = "50";
    propValues.second = "Detail footer's height";
    propValues.second << "int";
    props["Height"] = propValues;
    propValues.second.clear();

    propValues.first = "0";
    propValues.second = "Detail footer's level";
    propValues.second << "int";
    props["Level"] = propValues;
    propValues.second.clear();
}

void CanvasDetailFooter::draw(QPainter &painter)
{
    QString str = QString("Detail footer %1").arg(props["Level"].first.toInt());
    painter.drawText(rect(), AlignVCenter | AlignLeft, str);
//		     i18n("CanvasDetailFooter", "Detail Footer"));
    CanvasBand::draw(painter);
}

QString CanvasDetailFooter::getXml()
{
    return "\t<DetailFooter" + CanvasBand::getXml() + "\t</DetailFooter>\n\n";
}
