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
#include <klocale.h>

#include <qcanvas.h>
#include <qpainter.h>
#include <qprinter.h>
#include <qpaintdevicemetrics.h>

#include "canvkutemplate.h"
#include "property.h"
#include "mycanvas.h"
#include "creportitem.h"

#include "canvkutemplate.h"
#include "canvreportheader.h"
#include "canvreportfooter.h"
#include "canvpageheader.h"
#include "canvpagefooter.h"
#include "canvdetailheader.h"
#include "canvdetailfooter.h"
#include "canvdetail.h"


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

    std::map<QString, QString> m;
    m["A4"] = "0";
    m["B5"] = "1";
    m["Letter"] = "2";
    m["Legal"] = "3";
    m["Executive"] = "4";
    m["A0"] = "5";
    m["A1"] = "6";
    m["A2"] = "7";
    m["A3"] = "8";
    m["A5"] = "9";
    m["A6"] = "10";
    m["A7"] = "11";
    m["A8"] = "12";
    m["A9"] = "13";
    m["B0"] = "14";
    m["B1"] = "15";
    m["B10"] = "16";
    m["B2"] = "17";
    m["B3"] = "18";
    m["B4"] = "19";
    m["B6"] = "20";
    m["B7"] = "21";
    m["B8"] = "22";
    m["B9"] = "23";
    m["C5E"] = "24";
    m["Comm10E"] = "25";
    m["DLE"] = "26";
    m["Folio"] = "27";
    m["Ledger"] = "28";
    m["Tabloid"] = "29";
    m["NPageSize"] = "30";
    props["PageSize"] = *(new PropPtr(new Property("PageSize", m, i18n("Page size"), "0")));
    m.clear();

    m["Portrait"] = "0";
    m["Landscape"] = "1";
    props["PageOrientation"] = *(new PropPtr(new Property("PageOrientation",
        m, i18n("Page orientation"), "0")));
    m.clear();

    props["TopMargin"] = *(new PropPtr(new Property(IntegerValue, "TopMargin", i18n("Top margin"), "0")));

    props["BottomMargin"] = *(new PropPtr(new Property(IntegerValue, "BottomMargin", i18n("Bottom margin"), "0")));

    props["LeftMargin"] = *(new PropPtr(new Property(IntegerValue, "LeftMargin", i18n("Left margin"), "0")));

    props["RightMargin"] = *(new PropPtr(new Property(IntegerValue, "RightMargin", i18n("Right margin"), "0")));
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
    painter.setPen(QPen(QColor(160, 160, 160), 0, Qt::SolidLine));
    QPoint p1((int)(x()+props["LeftMargin"]->value().toInt()),
        (int)(y()+props["TopMargin"]->value().toInt()));
    QPoint p2((int)(x()+props["LeftMargin"]->value().toInt()),
        (int)y() + height() - props["BottomMargin"]->value().toInt());
    QPoint p3((int)x() + width() - props["RightMargin"]->value().toInt(),
        (int)y() + height() - props["BottomMargin"]->value().toInt());
    QPoint p4((int)x() + width() - props["RightMargin"]->value().toInt(),
        (int)(y()+props["TopMargin"]->value().toInt()));
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

void CanvasKugarTemplate::updatePaperProps()
{
    QPrinter* printer;

    // Set the page size
    printer = new QPrinter();
    printer->setFullPage(true);
    printer->setPageSize((QPrinter::PageSize)props["PageSize"]->value().toInt());
    printer->setOrientation((QPrinter::Orientation)props["PageOrientation"]->value().toInt());

    // Get the page metrics and set appropriate wigth and height
    QPaintDeviceMetrics pdm(printer);
    canvas()->resize(pdm.width(), pdm.height());
    setSize(pdm.width(), pdm.height());

    //this is not needed anymore
    delete printer;
}

/*arrange sections on page automatically*/
void CanvasKugarTemplate::arrangeSections(bool destructive)
{
    int base = props["TopMargin"]->value().toInt();
    if (reportHeader)
    {
        reportHeader->arrange(base, destructive);
        base += reportHeader->props["Height"]->value().toInt();
        reportHeader->show();
    }
    if (pageHeader)
    {
        pageHeader->arrange(base, destructive);
        base += pageHeader->props["Height"]->value().toInt();
        pageHeader->show();
    }

    std::map<int, DetailBand>::const_iterator it;
    for (it = details.begin(); it != details.end(); ++it)
    {
        //arranging detail header
        if (it->second.first.first)
        {
            it->second.first.first->arrange(base, destructive);
            base += it->second.first.first->props["Height"]->value().toInt();
            it->second.first.first->show();
        }
        //arranging detail
        if (it->second.second)
        {
            it->second.second->arrange(base, destructive);
            base += it->second.second->props["Height"]->value().toInt();
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
            base += itr->second.first.second->props["Height"]->value().toInt();
            itr->second.first.second->show();
        }
    }

    if (pageFooter)
    {
        pageFooter->arrange(base, destructive);
        base += pageFooter->props["Height"]->value().toInt();
        pageFooter->show();
    }
    if (reportFooter)
    {
        reportFooter->arrange(base, destructive);
        base += reportFooter->props["Height"]->value().toInt();
        reportFooter->show();
    }
}

QString CanvasKugarTemplate::getXml()
{
    QString result = "";
    result += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n";
    result += "<!DOCTYPE KugarTemplate SYSTEM \"kugartemplate.dtd\">\n\n";
    result += "<KugarTemplate";
    for (std::map<QString, PropPtr >::const_iterator it = props.begin();
        it != props.end(); it++ )
    {
	if (it->second->allowSaving())
        result += " " + it->first + "=" + "\"" + it->second->value() + "\"";
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


bool CanvasKugarTemplate::removeReportItem(QCanvasItem *item)
{
    if (item->rtti() > 2000)
    {

        item->hide();
/*        qDebug("%d items of %p in canvas()", canvas()->allItems().remove( section ), section );
        qDebug("%d items of %p removed from list",canvas()->allItems().remove( section ), section);
        qDebug("%d items of %p in canvas()", canvas()->allItems().remove( section ), section );
        qDebug("%d items of %p removed from list",canvas()->allItems().remove( section ), section);
        int i = canvas()->allItems().remove( section );
        int j = ((CanvasReportItem *)section)->section()->items.remove(section);
        qDebug("Deleting section %p (%d removed, %d removed)", section, i,j);
        qDebug("Items :%d, items:%d", canvas()->allItems().contains( section ),  ((CanvasReportItem *)section)->section()->items.contains( section));*/
        CanvasReportItem *ritem = dynamic_cast<CanvasReportItem*>(item);
        if (ritem != 0)
        {
            ritem->section()->items.remove(ritem);
            qWarning("good");
        }
        delete item;
//        section = 0;
        canvas()->update();

        return true;
    }
    if (item->rtti() > 1800)
    {
    /*    if ((*it)->rtti() == KuDesignerRttiDetail)
        {
            CanvasDetail *det = (CanvasDetail*)(*it);
            if ( det->props["Level"].first.toInt() <
                ((MyCanvas*)(canvas()))->templ->detailsCount - 1)
                return;
        }*/

        CanvasBand *section = dynamic_cast<CanvasBand *>(item);

        CanvasDetailHeader *header = 0;
        CanvasDetailFooter *footer = 0;
        qWarning("1");
        removeSection(section, &header, &footer);
        qWarning("2");
        section->hide();
        qWarning("3");
        delete section;
        qWarning("4");

        if (header)
        {
        qWarning("5");
            header->hide();
            delete header;
        qWarning("6");
        }
        if (footer)
        {
        qWarning("7");
            footer->hide();
            delete footer;
        qWarning("8");
        }
        qWarning("9");
        arrangeSections();
        qWarning("10");
        canvas()->update();
        qWarning("11");

        return true;
    }
    
    return false;
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
        }
        if (i->second.first.first == section)
            i->second.first.first = 0;
        if (i->second.first.second == section)
            i->second.first.second = 0;
    }
}
