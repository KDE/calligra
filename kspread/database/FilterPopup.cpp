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
#include <QVBoxLayout>

#include <klocale.h>

#include "CellStorage.h"
#include "DatabaseRange.h"
#include "Doc.h"
#include "Filter.h"
#include "Region.h"
#include "Sheet.h"
#include "ValueConverter.h"

#include "commands/ApplyFilterCommand.h"

using namespace KSpread;

class FilterPopup::Private
{
public:
    QAbstractButton* allCheckbox;
    QList<QCheckBox*> checkboxes;
    QHash<QString, int> items;
    int fieldNumber;
    DatabaseRange database;

public:
    void createItemList(const Cell& cell, const DatabaseRange* database);
};

void FilterPopup::Private::createItemList(const Cell& cell, const DatabaseRange* database)
{
    const Sheet* sheet = cell.sheet();
    const QRect range = database->range().lastRange();
    const int start = database->orientation() == Qt::Vertical ? range.top() : range.left();
    const int end = database->orientation() == Qt::Vertical ? range.bottom() : range.right();
    const int j = database->orientation() == Qt::Vertical ? cell.column() : cell.row();
    for (int i = start; i <= end; ++i)
    {
        const Value value = database->orientation() == Qt::Vertical
                            ? sheet->cellStorage()->value(j, i)
                            : sheet->cellStorage()->value(i, j);
        if (value.isEmpty() || sheet->doc()->converter()->asString(value).asString().isEmpty())
            continue;
        items[sheet->doc()->converter()->asString(value).asString()]++;
    }
}


FilterPopup::FilterPopup(QWidget* parent, const Cell& cell, DatabaseRange* database)
    : QFrame(parent, Qt::Popup)
    , d(new Private)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setBackgroundRole(QPalette::Base);
    setFrameStyle(QFrame::Panel | QFrame::Raised);

    d->database = *database;

    QButtonGroup* buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(false);
    connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(buttonClicked(QAbstractButton*)));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(3);
    layout->setSpacing(0);

    QCheckBox* item;
    d->allCheckbox = new QCheckBox(i18n("All"), this);
    d->allCheckbox->setChecked(true);
    buttonGroup->addButton(d->allCheckbox);
    layout->addWidget(d->allCheckbox);
//     item = new QCheckBox(i18n("Top 10"), this);
//     buttonGroup->addButton(item);
//     layout->addWidget(item);
//     item = new QCheckBox(i18n("Empty"), this);
//     buttonGroup->addButton(item);
//     layout->addWidget(item);
//     item = new QCheckBox(i18n("Non-empty"), this);
//     buttonGroup->addButton(item);
//     layout->addWidget(item);
    layout->addSpacing(3);

    d->createItemList(cell, database);
    QList<QString> items = d->items.keys();
    qSort(items);
    for (int i = 0; i < items.count(); ++i)
    {
        item = new QCheckBox(items[i], this);
        item->setChecked(true);
        buttonGroup->addButton(item);
        layout->addWidget(item);
        d->checkboxes.append(item);
    }

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
        filter->removeConditions(); // remove all conditions
    else
    {
        filter->removeConditions(d->fieldNumber);
        QList<QString> matchList;
        QList<QString> notMatchList;
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
    updateFilter(d->database.filter());
    ApplyFilterCommand* command = new ApplyFilterCommand();
    command->setSheet((*d->database.range().constBegin())->sheet());
    command->add(d->database.range());
d->database.dump();
    command->setDatabase(&d->database); // FIXME Stefan: Really needed?
    command->execute();
    QFrame::closeEvent(event);
}

void FilterPopup::buttonClicked(QAbstractButton* button)
{
    if (button == d->allCheckbox)
    {
        foreach (QCheckBox* checkbox, d->checkboxes)
            checkbox->setChecked(button->isChecked());
    }
    else
    {
        bool isAll = true;
        foreach (QCheckBox* checkbox, d->checkboxes)
        {
            if (!checkbox->isChecked())
            {
                isAll = false;
                break;
            }
        }
        d->allCheckbox->setChecked(isAll);
    }
}

#include "FilterPopup.moc"
