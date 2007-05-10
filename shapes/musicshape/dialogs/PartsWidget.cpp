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
#include "PartsWidget.h"
#include "PartDetailsDialog.h"

#include "../MusicTool.h"
#include "../MusicShape.h"

#include "../core/Sheet.h"
#include "../core/Part.h"

#include "../commands/RemovePartCommand.h"

using namespace MusicCore;

PartsWidget::PartsWidget(MusicTool *tool, QWidget *parent)
    : QWidget(parent),
    m_tool(tool)
{
    widget.setupUi(this);

    connect(widget.partsList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(partDoubleClicked(QListWidgetItem*)));
    connect(widget.partsList, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(selectionChanged(QListWidgetItem*,QListWidgetItem*)));
    connect(widget.addPart, SIGNAL(clicked()), this, SLOT(addPart()));
    connect(widget.removePart, SIGNAL(clicked()), this, SLOT(removePart()));
    connect(widget.editPart, SIGNAL(clicked()), this, SLOT(editPart()));
}

void PartsWidget::setShape(MusicShape* shape)
{
    Sheet* sheet = shape->sheet();
    m_shape = shape;
    widget.partsList->clear();
    for (int i = 0; i < sheet->partCount(); i++) {
        widget.partsList->addItem(sheet->part(i)->name());
    }
    m_sheet = sheet;
}

void PartsWidget::partDoubleClicked(QListWidgetItem* item)
{
    int row = widget.partsList->row(item);
    PartDetailsDialog *dlg = new PartDetailsDialog(m_sheet->part(row), this);
    dlg->show();
}

void PartsWidget::selectionChanged(QListWidgetItem* current, QListWidgetItem* prev)
{
    widget.editPart->setEnabled(current);
    widget.removePart->setEnabled(current);
}

void PartsWidget::addPart()
{
    kDebug() << "add part" << endl;
}

void PartsWidget::removePart()
{
    Part* part = m_sheet->part(widget.partsList->currentRow());

    m_tool->addCommand(new RemovePartCommand(m_shape, part));
}

void PartsWidget::editPart()
{
    PartDetailsDialog *dlg = new PartDetailsDialog(m_sheet->part(widget.partsList->currentRow()));
    dlg->show();
}

#include "PartsWidget.moc"
