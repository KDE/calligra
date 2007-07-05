/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include "FilterPopup.h"

#include <QButtonGroup>
#include <QCheckBox>
#include <QHash>
#include <QList>
#include <QScrollArea>
#include <QVBoxLayout>

#include <klocale.h>

#include "CellStorage.h"
#include "Database.h"
#include "Doc.h"
#include "Filter.h"
#include "RowColumnFormat.h"
#include "Region.h"
#include "Sheet.h"
#include "ValueConverter.h"

#include "commands/ApplyFilterCommand.h"

using namespace KSpread;

class FilterPopup::Private
{
public:
    QAbstractButton* allCheckbox;
    QAbstractButton* emptyCheckbox;
    QAbstractButton* notEmptyCheckbox;
    QList<QCheckBox*> checkboxes;
    int fieldNumber;
    Database database;
    bool dirty;

public:
    void initGUI(FilterPopup* parent, const Cell& cell, const Database* database);
};

void FilterPopup::Private::initGUI(FilterPopup* parent, const Cell& cell, const Database* database)
{
    QButtonGroup* buttonGroup = new QButtonGroup(parent);
    buttonGroup->setExclusive(false);
    connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
            parent, SLOT(buttonClicked(QAbstractButton*)));

    QVBoxLayout* layout = new QVBoxLayout(parent);
    layout->setMargin(3);
    layout->setSpacing(0);

    allCheckbox = new QCheckBox(i18n("All"), parent);
    buttonGroup->addButton(allCheckbox);
    layout->addWidget(allCheckbox);
    emptyCheckbox = new QCheckBox(i18n("Empty"), parent);
    emptyCheckbox->setChecked(true);
    buttonGroup->addButton(emptyCheckbox);
    layout->addWidget(emptyCheckbox);
    notEmptyCheckbox = new QCheckBox(i18n("Non-empty"), parent);
    notEmptyCheckbox->setChecked(true);
    buttonGroup->addButton(notEmptyCheckbox);
    layout->addWidget(notEmptyCheckbox);
    layout->addSpacing(3);

    const Sheet* sheet = cell.sheet();
    const QRect range = database->range().lastRange();
    const int start = database->orientation() == Qt::Vertical ? range.top() : range.left();
    const int end = database->orientation() == Qt::Vertical ? range.bottom() : range.right();
    const int j = database->orientation() == Qt::Vertical ? cell.column() : cell.row();
    QHash<QString, bool> items;
    for (int i = start; i <= end; ++i)
    {
        const Value value = database->orientation() == Qt::Vertical
                            ? sheet->cellStorage()->value(j, i)
                            : sheet->cellStorage()->value(i, j);
        const QString string = sheet->doc()->converter()->asString(value).asString();
        const bool isFiltered = database->orientation() == Qt::Vertical
                                ? sheet->rowFormat(i)->isFiltered()
                                : sheet->columnFormat(i)->isFiltered();
        if (string.isEmpty())
        {
            emptyCheckbox->setChecked(emptyCheckbox->isChecked() && !isFiltered);
            continue;
        }
        if (items.contains(string))
            items[string] = items[string] && !isFiltered;
        else
            items[string] = !isFiltered;
    }

    QWidget* scrollWidget = new QWidget(parent);
    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollWidget);
    scrollLayout->setMargin(0);
    scrollLayout->setSpacing(0);

    QList<QString> sortedItems = items.keys();
    qSort(sortedItems);
    bool isAll = true;
    QCheckBox* item;
    for (int i = 0; i < sortedItems.count(); ++i)
    {
        item = new QCheckBox(sortedItems[i], scrollWidget);
        item->setChecked(items[sortedItems[i]]);
        buttonGroup->addButton(item);
        scrollLayout->addWidget(item);
        checkboxes.append(item);
        isAll = isAll && items[sortedItems[i]];
    }
    allCheckbox->setChecked(isAll && emptyCheckbox->isChecked());
    notEmptyCheckbox->setChecked(isAll);

    QScrollArea* scrollArea = new QScrollArea(parent);
    layout->addWidget(scrollArea);
    scrollArea->setWidget(scrollWidget);
}


FilterPopup::FilterPopup(QWidget* parent, const Cell& cell, Database* database)
    : QFrame(parent, Qt::Popup)
    , d(new Private)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setBackgroundRole(QPalette::Base);
    setFrameStyle(QFrame::Panel | QFrame::Raised);

    d->database = *database;
    d->dirty = false;

    d->initGUI(this, cell, database);

    if (database->orientation() == Qt::Vertical)
        d->fieldNumber = cell.column() - database->range().lastRange().left();
    else // Qt::Horizontal
        d->fieldNumber = cell.row() - database->range().lastRange().top();
    kDebug() << "FilterPopup::fieldNumber: " << d->fieldNumber << endl;
}

FilterPopup::~FilterPopup()
{
    delete d;
}

void FilterPopup::updateFilter(Filter* filter) const
{
    if (d->allCheckbox->isChecked())
        filter->removeConditions(d->fieldNumber); // remove all conditions for this field
    else if (d->notEmptyCheckbox->isChecked())
    {
        // emptyCheckbox is not checked, because allCheckbox is not.
        filter->removeConditions(d->fieldNumber);
        filter->addCondition(Filter::AndComposition, d->fieldNumber, Filter::NotMatch, "");
    }
    else
    {
        filter->removeConditions(d->fieldNumber);
        QList<QString> matchList;
        QList<QString> notMatchList;
        if (d->emptyCheckbox->isChecked())
            matchList.append("");
        foreach (QCheckBox* checkbox, d->checkboxes)
        {
            if (checkbox->isChecked())
                matchList.append(checkbox->text());
            else
                notMatchList.append(checkbox->text());
        }
        // be lazy; choose the comparison causing least effort
        const Filter::Comparison comparison = (matchList.count() < notMatchList.count())
                                            ? Filter::Match : Filter::NotMatch;
        const QList<QString> values = (comparison == Filter::Match) ? matchList : notMatchList;
        for (int i = 0; i < values.count(); ++i)
        {
            kDebug() << "adding condition for fieldNumber " << d->fieldNumber << endl;
            filter->addCondition(Filter::AndComposition, d->fieldNumber, comparison, values[i]);
        }
    }
}

void FilterPopup::closeEvent(QCloseEvent* event)
{
    if (d->dirty)
    {
        updateFilter(d->database.filter());
        ApplyFilterCommand* command = new ApplyFilterCommand();
        command->setSheet((*d->database.range().constBegin())->sheet());
        command->add(d->database.range());
        command->setDatabase(d->database);
        command->execute();
    }
    QFrame::closeEvent(event);
}

void FilterPopup::buttonClicked(QAbstractButton* button)
{
    d->dirty = true;
    if (button == d->allCheckbox)
    {
        foreach (QCheckBox* checkbox, d->checkboxes)
            checkbox->setChecked(button->isChecked());
        d->emptyCheckbox->setChecked(button->isChecked());
        d->notEmptyCheckbox->setChecked(button->isChecked());
    }
    else if (button == d->emptyCheckbox)
    {
        bool isAll = button->isChecked();
        if (isAll)
        {
            foreach (QCheckBox* checkbox, d->checkboxes)
            {
                if (!checkbox->isChecked())
                {
                    isAll = false;
                    break;
                }
            }
        }
        d->allCheckbox->setChecked(isAll);
    }
    else if (button == d->notEmptyCheckbox)
    {
        foreach (QCheckBox* checkbox, d->checkboxes)
            checkbox->setChecked(button->isChecked());
        d->allCheckbox->setChecked(button->isChecked() && d->emptyCheckbox->isChecked());
    }
    else
    {
        bool isAll = d->emptyCheckbox->isChecked();
        if (isAll)
        {
            foreach (QCheckBox* checkbox, d->checkboxes)
            {
                if (!checkbox->isChecked())
                {
                    isAll = false;
                    break;
                }
            }
        }
        d->allCheckbox->setChecked(isAll);
        d->notEmptyCheckbox->setChecked(isAll);
    }
}

#include "FilterPopup.moc"
