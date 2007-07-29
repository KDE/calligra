/* This file is part of the KDE project
 * Copyright (C) 2007 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "PartDetailsDialog.h"

#include "../MusicTool.h"

#include "../core/Part.h"

#include "../commands/ChangePartNameCommand.h"
#include "../commands/ChangePartAbbreviationCommand.h"

using namespace MusicCore;

PartDetailsDialog::PartDetailsDialog(MusicTool* tool, Part* part, QWidget* parent)
    : KDialog(parent),
    m_tool(tool),
    m_part(part)
{
    setCaption(i18n("Part details"));
    setButtons( Close );
    setDefaultButton( Close );
    QWidget* w = new QWidget(this);
    widget.setupUi(w);
    setMainWidget(w);
    
    widget.nameEdit->setText(part->name());
    widget.shortNameEdit->setText(part->shortName());
    widget.staffCount->setValue(part->staffCount());

    connect(part, SIGNAL(nameChanged(const QString&)), widget.nameEdit, SLOT(setText(const QString&)));
    connect(part, SIGNAL(shortNameChanged(const QString&)), widget.shortNameEdit, SLOT(setText(const QString&)));
    
    connect(widget.nameEdit, SIGNAL(textEdited(const QString&)), this, SLOT(nameChanged(const QString&)));
    connect(widget.shortNameEdit, SIGNAL(textEdited(const QString&)), this, SLOT(shortNameChanged(const QString&)));
}

void PartDetailsDialog::nameChanged(const QString& text)
{
    m_tool->addCommand(new ChangePartNameCommand(m_tool->shape(), m_part, text));     
}

void PartDetailsDialog::shortNameChanged(const QString& text)
{
    m_tool->addCommand(new ChangePartAbbreviationCommand(m_tool->shape(), m_part, text));
}

void PartDetailsDialog::staffCountChanged(int count)
{
}

#include "PartDetailsDialog.moc"
