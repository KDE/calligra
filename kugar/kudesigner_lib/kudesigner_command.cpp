/* This file is part of the KDE project
   Copyright (C) 2002 Alexander Dymo <cloudtemple@mksat.net>

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

#ifndef PURE_QT
#include <klocale.h>
#include <kdebug.h>
#else
#include "qlocale.h"
#endif

#include "kudesigner_command.h"
//#include "kudesigner_doc.h"

#include "canvdefs.h"

#include "cv.h"
#include "cfield.h"
#include "ccalcfield.h"
#include "clabel.h"
#include "cline.h"
#include "cspecialfield.h"
#include "mycanvas.h"
#include "propertyeditor.h"
#include "property.h"

#include "canvkutemplate.h"
#include "canvreportheader.h"
#include "canvreportfooter.h"
#include "canvpageheader.h"
#include "canvpagefooter.h"
#include "canvdetailheader.h"
#include "canvdetailfooter.h"
#include "canvdetail.h"

//AddDetailFooterCommand

AddDetailFooterCommand::AddDetailFooterCommand(int level, MyCanvas *doc)
    : KNamedCommand(i18n("Insert Detail Footer Section")), m_level(level), m_doc(doc)
{

}

void AddDetailFooterCommand::execute()
{
    m_section = new CanvasDetailFooter(m_doc->templ->props["LeftMargin"]->value().toInt(),
        0, m_doc->templ->width() - m_doc->templ->props["RightMargin"]->value().toInt() -
        m_doc->templ->props["LeftMargin"]->value().toInt(),
        50, m_level, m_doc);
    m_section->props["Level"]->setValue(QString("%1").arg(m_level));
    m_doc->templ->details[m_level].first.second = m_section;
    m_doc->templ->arrangeSections();
}

void AddDetailFooterCommand::unexecute()
{
    m_doc->templ->removeReportItem(m_section);
}




//AddDetailCommand

AddDetailCommand::AddDetailCommand(int level, MyCanvas *doc)
    : KNamedCommand(i18n("Insert Detail Section")), m_level(level), m_doc(doc)
{

}

void AddDetailCommand::execute()
{
    m_section = new CanvasDetail(m_doc->templ->props["LeftMargin"]->value().toInt(),
        0, m_doc->templ->width() - m_doc->templ->props["RightMargin"]->value().toInt() -
        m_doc->templ->props["LeftMargin"]->value().toInt(),
        50, m_level, m_doc);
    m_section->props["Level"]->setValue(QString("%1").arg(m_level));
    m_doc->templ->details[m_level].second = m_section;
    m_doc->templ->arrangeSections();
    m_doc->templ->detailsCount++;
}

void AddDetailCommand::unexecute()
{
    m_doc->templ->removeReportItem(m_section);
}





//AddDetailHeaderCommand
AddDetailHeaderCommand::AddDetailHeaderCommand(int level, MyCanvas *doc)
    : KNamedCommand(i18n("Insert Detail Header Section")), m_level(level), m_doc(doc)
{

}

void AddDetailHeaderCommand::execute()
{
    m_section = new CanvasDetailHeader(m_doc->templ->props["LeftMargin"]->value().toInt(),
        0, m_doc->templ->width() - m_doc->templ->props["RightMargin"]->value().toInt() -
        m_doc->templ->props["LeftMargin"]->value().toInt(),
        50, m_level, m_doc);
    m_section->props["Level"]->setValue(QString("%1").arg(m_level));
    m_doc->templ->details[m_level].first.first = m_section;
    m_doc->templ->arrangeSections();
}

void AddDetailHeaderCommand::unexecute()
{
    m_doc->templ->removeReportItem(m_section);
}




//AddPageFooterCommand

AddPageFooterCommand::AddPageFooterCommand(MyCanvas *doc)
    : KNamedCommand(i18n("Insert Page Footer Section")), m_doc(doc)
{

}

void AddPageFooterCommand::execute()
{
    m_section = new CanvasPageFooter(m_doc->templ->props["LeftMargin"]->value().toInt(),
        0, m_doc->templ->width() - m_doc->templ->props["RightMargin"]->value().toInt() -
        m_doc->templ->props["LeftMargin"]->value().toInt(),
        50, m_doc);
    m_doc->templ->pageFooter = m_section;
    m_doc->templ->arrangeSections();
}

void AddPageFooterCommand::unexecute()
{
    m_doc->templ->removeReportItem(m_section);
}



//AddPageHeaderCommand

AddPageHeaderCommand::AddPageHeaderCommand(MyCanvas *doc)
    : KNamedCommand(i18n("Insert Page Header Section")), m_doc(doc)
{

}

void AddPageHeaderCommand::execute()
{
    m_section = new CanvasPageHeader(m_doc->templ->props["LeftMargin"]->value().toInt(),
        0, m_doc->templ->width() - m_doc->templ->props["RightMargin"]->value().toInt() -
        m_doc->templ->props["LeftMargin"]->value().toInt(),
        50, m_doc);
    m_doc->templ->pageHeader = m_section;
    m_doc->templ->arrangeSections();
}

void AddPageHeaderCommand::unexecute()
{
    m_doc->templ->removeReportItem(m_section);
}



//AddReportFooterCommand

AddReportFooterCommand::AddReportFooterCommand(MyCanvas *doc)
    : KNamedCommand(i18n("Insert Report Footer Section")), m_doc(doc)
{

}

void AddReportFooterCommand::execute()
{
    m_section = new CanvasReportFooter(m_doc->templ->props["LeftMargin"]->value().toInt(),
        0, m_doc->templ->width() - m_doc->templ->props["RightMargin"]->value().toInt() -
        m_doc->templ->props["LeftMargin"]->value().toInt(),
        50, m_doc);
    m_doc->templ->reportFooter = m_section;
    m_doc->templ->arrangeSections();
}

void AddReportFooterCommand::unexecute()
{
    m_doc->templ->removeReportItem(m_section);
}



//AddReportHeaderCommand

AddReportHeaderCommand::AddReportHeaderCommand(MyCanvas *doc)
    : KNamedCommand(i18n("Insert Report Header Section")), m_doc(doc)
{
}

void AddReportHeaderCommand::execute()
{
    m_section = new CanvasReportHeader(m_doc->templ->props["LeftMargin"]->value().toInt(),
        0, m_doc->templ->width() - m_doc->templ->props["RightMargin"]->value().toInt() -
        m_doc->templ->props["LeftMargin"]->value().toInt(),
        50, m_doc);
    m_doc->templ->reportHeader = m_section;
    m_doc->templ->arrangeSections();
}

void AddReportHeaderCommand::unexecute()
{
    m_doc->templ->removeReportItem(m_section);
}




//AddReportItemCommand

AddReportItemCommand::AddReportItemCommand(MyCanvas *doc, ReportCanvas *rc, int x, int y, KuDesignerCanvasRtti section, int sectionLevel):
    KNamedCommand(i18n("Insert Report Item")), m_doc(doc), m_rc(rc), m_x(x), m_y(y), m_section(section), m_sectionLevel(sectionLevel)
{
    m_rtti = m_rc->itemToInsert;
    setName(KuDesigner::insertItemActionName(m_rc->itemToInsert));
}

void AddReportItemCommand::execute()
{
    kdDebug() << "Execute: rtti = " << m_rtti << endl;
    switch( m_rtti ){
        case KuDesignerRttiCanvasLabel:
            m_item = new CanvasLabel(0, 0, defaultItemWidth, defaultItemHeight, m_doc);
            break;
        case KuDesignerRttiCanvasField:
            m_item = new CanvasField(0, 0, defaultItemWidth, defaultItemHeight, m_doc);
            break;
        case KuDesignerRttiCanvasSpecial:
            m_item = new CanvasSpecialField(0, 0, defaultItemWidth, defaultItemHeight, m_doc);
            break;
        case KuDesignerRttiCanvasCalculated:
            m_item = new CanvasCalculatedField(0, 0, defaultItemWidth, defaultItemHeight, m_doc);
            break;
        case KuDesignerRttiCanvasLine:
            m_item = new CanvasLine(0, 0, defaultItemWidth, defaultItemHeight, m_doc);
            break;
        default:
            m_item = 0;
            return;
    }

    m_item->setX(m_x);
    m_item->setY(m_y);
    kdDebug() << "Execute: item created" << endl;
    m_item->setSection(m_doc->templ->band(m_section, m_sectionLevel));
    m_item->updateGeomProps();

    m_rc->selectItem(m_item, false);

    m_item->show();
    m_doc->templ->band(m_section, m_sectionLevel)->items.append(m_item);
}

void AddReportItemCommand::unexecute()
{
    if (m_item)
    {
        m_rc->unselectItem(m_item);
        m_doc->templ->removeReportItem(m_item);
    }
}
