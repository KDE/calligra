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

#include "kudesigner_command.h"
#include "kudesigner_doc.h"

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

AddDetailFooterCommand::AddDetailFooterCommand(int level, QString name, KudesignerDoc *doc)
    : KNamedCommand(name), m_level(level), m_doc(doc)
{

}

void AddDetailFooterCommand::execute()
{
    m_section = new CanvasDetailFooter(m_doc->canvas()->templ->props["LeftMargin"]->value().toInt(),
        0, m_doc->canvas()->templ->width() - m_doc->canvas()->templ->props["RightMargin"]->value().toInt() -
        m_doc->canvas()->templ->props["LeftMargin"]->value().toInt(),
        50, m_doc->canvas());
    m_section->props["Level"]->setValue(QString("%1").arg(m_level));
    m_doc->canvas()->templ->details[m_level].first.second = m_section;
    m_doc->canvas()->templ->arrangeSections();
}

void AddDetailFooterCommand::unexecute()
{
    m_doc->canvas()->templ->removeSection(m_section);
}
