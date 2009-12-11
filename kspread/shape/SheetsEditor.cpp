/* This file is part of the KDE project
   Copyright 2007 Sebastian Sauer <mail@dipe.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Local
#include "SheetsEditor.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>

#include <KInputDialog>

#include "TableShape.h"
#include "Sheet.h"
#include "Map.h"

using namespace KSpread;

class SheetsEditor::Private
{
public:
    TableShape* tableShape;
    QListWidget* list;
    QPushButton *renamebtn, *addbtn, *rembtn;
};

SheetsEditor::SheetsEditor(TableShape* tableShape, QWidget* parent)
        : QWidget(parent)
        , d(new Private)
{
    setObjectName("SheetsEditor");
    d->tableShape = tableShape;

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    setLayout(layout);

    d->list = new QListWidget(this);
    connect(d->list, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
    connect(d->list, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(itemChanged(QListWidgetItem*)));
    layout->addWidget(d->list);

    Map *map = d->tableShape->map();
    foreach(Sheet* sheet, map->sheetList()) {
        sheetAdded(sheet);
    }
    connect(map, SIGNAL(sheetAdded(Sheet*)), this, SLOT(sheetAdded(Sheet*)));

    QVBoxLayout* btnlayout = new QVBoxLayout(this);
    layout->addLayout(btnlayout);

    d->renamebtn = new QPushButton(/*KIcon("rename"),*/ i18n("Rename"), this);
    connect(d->renamebtn, SIGNAL(clicked()), this, SLOT(renameClicked()));
    btnlayout->addWidget(d->renamebtn);

    d->addbtn = new QPushButton(/*KIcon("list-add"),*/ i18n("Add"), this);
    connect(d->addbtn, SIGNAL(clicked()), this, SLOT(addClicked()));
    btnlayout->addWidget(d->addbtn);

    d->rembtn = new QPushButton(/*KIcon("edit-delete"),*/ i18n("Remove"), this);
    connect(d->rembtn, SIGNAL(clicked()), this, SLOT(removeClicked()));
    btnlayout->addWidget(d->rembtn);

    btnlayout->addStretch(1);
    selectionChanged();
}

SheetsEditor::~SheetsEditor()
{
    delete d;
}

void SheetsEditor::sheetAdded(Sheet* sheet)
{
    Q_ASSERT(sheet);
    QListWidgetItem* item = new QListWidgetItem(sheet->sheetName());
    item->setCheckState(sheet->isHidden() ? Qt::Unchecked : Qt::Checked);
    d->list->addItem(item);
    connect(sheet, SIGNAL(sig_nameChanged(Sheet*, QString)), this, SLOT(sheetNameChanged(Sheet*, QString)));
}

void SheetsEditor::sheetNameChanged(Sheet* sheet, const QString& old_name)
{
    for (int i = 0; i < d->list->count(); ++i)
        if (d->list->item(i)->text() == old_name)
            d->list->item(i)->setText(sheet->sheetName());
}

void SheetsEditor::selectionChanged()
{
    d->renamebtn->setEnabled(d->list->currentItem());

    d->rembtn->setEnabled(d->list->currentItem());
}

void SheetsEditor::itemChanged(QListWidgetItem* item)
{
    Q_ASSERT(item);
    Map *map = d->tableShape->map();
    Sheet* sheet = map->findSheet(item->text());
    if (sheet)
        sheet->setHidden(item->checkState() != Qt::Checked);
}

void SheetsEditor::renameClicked()
{
    QListWidgetItem* item = d->list->currentItem();
    if (! item)
        return;
    Map *map = d->tableShape->map();
    Sheet* sheet = map->findSheet(item->text());
    if (! sheet)
        return;
    QString name = KInputDialog::getText(i18n("Rename"), i18n("Enter Name:"), sheet->sheetName());
    if (name.isEmpty())
        return;
    sheet->setSheetName(name);
}

void SheetsEditor::addClicked()
{
    d->tableShape->map()->addNewSheet();
}

void SheetsEditor::removeClicked()
{
    QListWidgetItem* item = d->list->currentItem();
    if (! item)
        return;
    Map *map = d->tableShape->map();
    Sheet* sheet = map->findSheet(item->text());
    if (! sheet)
        return;
    map->removeSheet(sheet);
    delete item;
}

#include "SheetsEditor.moc"
