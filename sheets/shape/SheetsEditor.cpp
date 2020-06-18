/* This file is part of the KDE project
 * Copyright 2020 Dag Andersen <dag.andersen@kdemail.net>
 * Copyright 2007 Sebastian Sauer <mail@dipe.org>
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

// Local
#include "SheetsEditor.h"

#include "ui_SheetsEditor.h"

#include <QStandardItemModel>
#include <QPushButton>

#include "TableShape.h"
#include "Sheet.h"
#include "Map.h"

using namespace Calligra::Sheets;

class SheetsEditor::Private
{
public:
    Map* map;
    Ui::SheetsEditor ui;
};

SheetsEditor::SheetsEditor(TableShape* tableShape, QWidget* parent)
    : QDialog(parent)
    , d(new Private)
{
    setObjectName(QLatin1String("SheetsEditor"));
    d->map = tableShape->map();

    d->ui.setupUi(this);
    d->ui.rembtn->setEnabled(false);

    QStandardItemModel *m = new QStandardItemModel(this);
    foreach(Sheet* sheet, d->map->sheetList()) {
        QStandardItem *item = new QStandardItem(sheet->sheetName());
        item->setCheckable(true);
        item->setCheckState(sheet->isHidden() ? Qt::Unchecked : Qt::Checked);
        m->appendRow(item);
    }
    d->ui.listView->setModel(m);
    connect(d->ui.listView->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(slotCurrentChanged(const QModelIndex&)));
    connect(m, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)), this, SLOT(slotDataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)));

    connect(d->map, SIGNAL(sheetAdded(Sheet*)), this, SLOT(slotSheetAdded(Sheet*)));

    connect(d->ui.addbtn, SIGNAL(clicked()), this, SLOT(addClicked()));
    connect(d->ui.rembtn, SIGNAL(clicked()), this, SLOT(removeClicked()));
    connect(d->ui.closebtn, SIGNAL(clicked()), this, SLOT(accept()));
}

SheetsEditor::~SheetsEditor()
{
    delete d;
}

void SheetsEditor::slotCurrentChanged(const QModelIndex &idx)
{
    if (idx.isValid() && d->map->count() > 1) {
        bool visible = idx.data(Qt::CheckStateRole).toBool();
        if (!visible || d->map->visibleSheets().count() > 1) {
            // hidden sheets can always be removed
            // visible sheets can only be removed if there is another visible sheet
            d->ui.rembtn->setEnabled(true);
        }
    } else {
        d->ui.rembtn->setEnabled(false);
    }
}

void SheetsEditor::slotDataChanged(const QModelIndex &topLeft, const QModelIndex&, const QVector<int>&)
{
    const QString name = topLeft.data().toString();
    Q_ASSERT(!name.isEmpty());
    Sheet *sheet = d->map->sheet(topLeft.row());
    Q_ASSERT(sheet);
    bool changed = false;
    if (name != sheet->sheetName()) {
        // names must be unique
        if (!d->map->findSheet(name)) {
            sheet->setSheetName(name);
            changed = true;
        } else {
            d->ui.listView->model()->setData(topLeft, sheet->sheetName());
        }
    }
    bool visible = topLeft.data(Qt::CheckStateRole).toBool();
    if (visible) {
        sheet->setHidden(false);
        changed = true;
    } else if (d->map->visibleSheets().count() > 1) {
        // only hide if not the only visible sheet
        sheet->setHidden(true);
        changed = true;
    } else if (!visible) {
        // reset check state
        d->ui.listView->model()->setData(topLeft, Qt::Checked, Qt::CheckStateRole);
    }
    if (changed) {
        emit sheetModified(sheet);
    }
}

void SheetsEditor::slotSheetAdded(Sheet* sheet)
{
    Q_ASSERT(sheet);
    QStandardItemModel *m = qobject_cast<QStandardItemModel*>(d->ui.listView->model());
    disconnect(m, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)), this, SLOT(slotDataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)));
    QStandardItem *item = new QStandardItem(sheet->sheetName());
    item->setCheckable(true);
    item->setCheckState(sheet->isHidden() ? Qt::Unchecked : Qt::Checked);
    int row = d->map->indexOf(sheet);
    m->insertRow(row, item);
    connect(m, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)), this, SLOT(slotDataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)));
    emit sheetAdded(sheet);
}

void SheetsEditor::addClicked()
{
    d->map->addNewSheet();
}

void SheetsEditor::removeClicked()
{
    int row = d->ui.listView->currentIndex().row();
    if (row < 0) {
        return;
    }
    Sheet* sheet = d->map->sheet(row);
    Q_ASSERT(sheet);

    d->map->removeSheet(sheet);
    d->ui.listView->model()->removeRow(row);
    emit sheetRemoved(row);
}
