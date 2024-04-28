/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "PartsWidget.h"
#include "PartDetailsDialog.h"
#include "PartsListModel.h"

#include "../MusicShape.h"
#include "../MusicTool.h"

#include "../core/Part.h"
#include "../core/Sheet.h"

#include "../commands/AddPartCommand.h"
#include "../commands/RemovePartCommand.h"

#include <KoIcon.h>

using namespace MusicCore;

PartsWidget::PartsWidget(MusicTool *tool, QWidget *parent)
    : QWidget(parent)
    , m_tool(tool)
{
    widget.setupUi(this);

    widget.addPart->setIcon(koIcon("list-add"));
    widget.removePart->setIcon(koIcon("list-remove"));
    widget.editPart->setIcon(koIcon("document-properties"));

    connect(widget.partsList, &QAbstractItemView::doubleClicked, this, &PartsWidget::partDoubleClicked);
    // connect(widget.partsList, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this,
    // SLOT(selectionChanged(QListWidgetItem*,QListWidgetItem*)));
    connect(widget.addPart, &QAbstractButton::clicked, this, &PartsWidget::addPart);
    connect(widget.removePart, &QAbstractButton::clicked, this, &PartsWidget::removePart);
    connect(widget.editPart, &QAbstractButton::clicked, this, &PartsWidget::editPart);
}

void PartsWidget::setShape(MusicShape *shape)
{
    Sheet *sheet = shape->sheet();
    m_shape = shape;
    //    widget.partsList->clear();
    //    for (int i = 0; i < sheet->partCount(); i++) {
    //        widget.partsList->addItem(sheet->part(i)->name());
    //    }
    widget.partsList->setModel(new PartsListModel(sheet));
    connect(widget.partsList->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &PartsWidget::selectionChanged);
    m_sheet = sheet;
}

void PartsWidget::partDoubleClicked(const QModelIndex &index)
{
    int row = index.row();
    PartDetailsDialog::showDialog(m_tool, m_sheet->part(row), this);
}

void PartsWidget::selectionChanged(const QModelIndex &current, const QModelIndex &prev)
{
    Q_UNUSED(prev);
    widget.editPart->setEnabled(current.isValid());
    widget.removePart->setEnabled(current.isValid());
}

void PartsWidget::addPart()
{
    m_tool->addCommand(new AddPartCommand(m_shape));
}

void PartsWidget::removePart()
{
    Part *part = m_sheet->part(widget.partsList->currentIndex().row());

    m_tool->addCommand(new RemovePartCommand(m_shape, part));
}

void PartsWidget::editPart()
{
    int row = widget.partsList->currentIndex().row();
    PartDetailsDialog::showDialog(m_tool, m_sheet->part(row), this);
}
