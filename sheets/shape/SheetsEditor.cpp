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
#include <QInputDialog>

#include "TableShape.h"
#include "Sheet.h"
#include "Map.h"

using namespace Calligra::Sheets;

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
    setObjectName(QLatin1String("SheetsEditor"));
    d->tableShape = tableShape;

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    setLayout(layout);

    d->list = new QListWidget(this);
    connect(d->list, &QListWidget::itemSelectionChanged, this, &SheetsEditor::selectionChanged);
    connect(d->list, &QListWidget::itemChanged, this, &SheetsEditor::itemChanged);
    layout->addWidget(d->list);

    Map *map = d->tableShape->map();
    foreach(Sheet* sheet, map->sheetList()) {
        sheetAdded(sheet);
    }
    connect(map, &Map::sheetAdded, this, &SheetsEditor::sheetAdded);

    QVBoxLayout* btnlayout = new QVBoxLayout();
    layout->addLayout(btnlayout);

    d->renamebtn = new QPushButton(/*koIcon("edit-rename"),*/ i18n("Rename"), this);
    connect(d->renamebtn, &QAbstractButton::clicked, this, &SheetsEditor::renameClicked);
    btnlayout->addWidget(d->renamebtn);

    d->addbtn = new QPushButton(/*koIcon("list-add"),*/ i18n("Add"), this);
    connect(d->addbtn, &QAbstractButton::clicked, this, &SheetsEditor::addClicked);
    btnlayout->addWidget(d->addbtn);

    d->rembtn = new QPushButton(/*koIcon("edit-delete"),*/ i18n("Remove"), this);
    connect(d->rembtn, &QAbstractButton::clicked, this, &SheetsEditor::removeClicked);
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
    connect(sheet, &Sheet::nameChanged, this, [this, sheet] (const QString &oldName, const QString &name) { Q_UNUSED(name); sheetNameChanged(sheet, oldName); });
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
    QString name = QInputDialog::getText(0, i18n("Rename"), i18n("Enter Name:"), QLineEdit::Normal, sheet->sheetName());
    if (name.isEmpty())
        return;
    sheet->setSheetName(name);
    d->renamebtn->setText(sheet->sheetName());
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
