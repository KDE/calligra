/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "FilterPopup.h"

#include <QButtonGroup>
#include <QCheckBox>
#include <QScrollArea>
#include <QVBoxLayout>

#include <KLocalizedString>

#include "engine/CellBase.h"
#include "engine/CellBaseStorage.h"
#include "engine/Region.h"
#include "engine/ValueConverter.h"

#include "core/DataFilter.h"
#include "core/Database.h"
#include "core/Sheet.h"

#include "commands/ApplyFilterCommand.h"

using namespace Calligra::Sheets;

class FilterPopup::Private
{
public:
    QAbstractButton *allCheckbox;
    QAbstractButton *emptyCheckbox;
    QAbstractButton *notEmptyCheckbox;
    QList<QCheckBox *> checkboxes;
    int fieldNumber;
    Database database;
    bool dirty;

public:
    void initGUI(FilterPopup *parent, const CellBase &cell, const Database *database);
};

void FilterPopup::Private::initGUI(FilterPopup *parent, const CellBase &cell, const Database *database)
{
    QButtonGroup *buttonGroup = new QButtonGroup(parent);
    buttonGroup->setExclusive(false);
    connect(buttonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), parent, &FilterPopup::buttonClicked);

    QVBoxLayout *layout = new QVBoxLayout(parent);
    layout->setContentsMargins(3, 3, 3, 3);
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

    const SheetBase *sheet = cell.sheet();
    const QRect range = database->range().lastRange();
    const bool isRowFilter = database->orientation() == Qt::Vertical;
    const int start = isRowFilter ? range.top() : range.left();
    const int end = isRowFilter ? range.bottom() : range.right();
    const int j = isRowFilter ? cell.column() : cell.row();
    QSet<QString> items;
    for (int i = start + (database->containsHeader() ? 1 : 0); i <= end; ++i) {
        const Value value = isRowFilter ? sheet->cellStorage()->value(j, i) : sheet->cellStorage()->value(i, j);
        const QString string = sheet->map()->converter()->asString(value).asString();
        if (!string.isEmpty())
            items.insert(string);
    }

    QWidget *scrollWidget = new QWidget(parent);
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollWidget);
    scrollLayout->setContentsMargins({});
    scrollLayout->setSpacing(0);

    const int fieldNumber = j - (isRowFilter ? range.left() : range.top());
    const QMap<QString, AbstractCondition::Comparison> conditions = database->filter().conditions(fieldNumber);
    const bool defaultCheckState = conditions.isEmpty()
        ? true
        : !(conditions[conditions.keys()[0]] == AbstractCondition::Match || conditions[conditions.keys()[0]] == AbstractCondition::Empty);
    QList<QString> sortedItems = items.values();
    std::sort(sortedItems.begin(), sortedItems.end());
    bool isAll = true;
    QCheckBox *item;
    for (int i = 0; i < sortedItems.count(); ++i) {
        const QString value = sortedItems[i];
        item = new QCheckBox(value, scrollWidget);
        item->setChecked(conditions.contains(value) ? !defaultCheckState : defaultCheckState);
        buttonGroup->addButton(item);
        scrollLayout->addWidget(item);
        checkboxes.append(item);
        isAll = isAll && item->isChecked();
    }
    emptyCheckbox->setChecked(conditions.contains("") ? !defaultCheckState : defaultCheckState);
    allCheckbox->setChecked(isAll && emptyCheckbox->isChecked());
    notEmptyCheckbox->setChecked(isAll);

    QScrollArea *scrollArea = new QScrollArea(parent);
    layout->addWidget(scrollArea);
    scrollArea->setWidget(scrollWidget);
}

FilterPopup::FilterPopup(QWidget *parent, const CellBase &cell, Database *database)
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
    debugSheets << "FilterPopup for fieldNumber" << d->fieldNumber;
}

FilterPopup::~FilterPopup()
{
    delete d;
}

void FilterPopup::updateFilter(Filter *filter) const
{
    if (d->allCheckbox->isChecked())
        filter->removeConditions(d->fieldNumber); // remove all conditions for this field
    else if (d->notEmptyCheckbox->isChecked()) {
        // emptyCheckbox is not checked, because allCheckbox is not.
        filter->removeConditions(d->fieldNumber);
        filter->addCondition(Filter::AndComposition, d->fieldNumber, AbstractCondition::NotMatch, "");
    } else {
        filter->removeConditions(d->fieldNumber);
        QList<QString> matchList;
        QList<QString> notMatchList;
        if (d->emptyCheckbox->isChecked())
            matchList.append("");
        else
            notMatchList.append("");
        foreach (QCheckBox *checkbox, d->checkboxes) {
            if (checkbox->isChecked())
                matchList.append(checkbox->text());
            else
                notMatchList.append(checkbox->text());
        }
        // be lazy; choose the comparison causing least effort
        const AbstractCondition::Comparison comparison = (matchList.count() < notMatchList.count()) ? AbstractCondition::Match : AbstractCondition::NotMatch;
        const Filter::Composition composition = (comparison == AbstractCondition::Match) ? Filter::OrComposition : Filter::AndComposition;
        const QList<QString> values = (comparison == AbstractCondition::Match) ? matchList : notMatchList;
        debugSheets << "adding conditions for fieldNumber" << d->fieldNumber;
        Filter subFilter;
        for (int i = 0; i < values.count(); ++i)
            subFilter.addCondition(composition, d->fieldNumber, comparison, values[i]);
        filter->addSubFilter(Filter::AndComposition, subFilter);
    }
}

void FilterPopup::closeEvent(QCloseEvent *event)
{
    if (d->dirty) {
        Filter filter = d->database.filter();
        updateFilter(&filter);
        // any real change?
        if (d->database.filter() != filter) {
            ApplyFilterCommand *command = new ApplyFilterCommand();
            Sheet *sheet = dynamic_cast<Sheet *>(d->database.range().lastSheet());
            command->setSheet(sheet);
            command->add(d->database.range());
            command->setOldFilter(d->database.filter());
            d->database.setFilter(filter);
            d->database.dump();
            command->setDatabase(d->database);
            command->execute();
        }
    }
    QFrame::closeEvent(event);
}

void FilterPopup::buttonClicked(QAbstractButton *button)
{
    d->dirty = true;
    if (button == d->allCheckbox) {
        foreach (QCheckBox *checkbox, d->checkboxes)
            checkbox->setChecked(button->isChecked());
        d->emptyCheckbox->setChecked(button->isChecked());
        d->notEmptyCheckbox->setChecked(button->isChecked());
    } else if (button == d->emptyCheckbox) {
        bool isAll = button->isChecked();
        if (isAll) {
            foreach (QCheckBox *checkbox, d->checkboxes) {
                if (!checkbox->isChecked()) {
                    isAll = false;
                    break;
                }
            }
        }
        d->allCheckbox->setChecked(isAll);
    } else if (button == d->notEmptyCheckbox) {
        foreach (QCheckBox *checkbox, d->checkboxes)
            checkbox->setChecked(button->isChecked());
        d->allCheckbox->setChecked(button->isChecked() && d->emptyCheckbox->isChecked());
    } else {
        bool isAll = d->emptyCheckbox->isChecked();
        if (isAll) {
            foreach (QCheckBox *checkbox, d->checkboxes) {
                if (!checkbox->isChecked()) {
                    isAll = false;
                    break;
                }
            }
        }
        d->allCheckbox->setChecked(isAll);
        d->notEmptyCheckbox->setChecked(isAll);
    }
}

void FilterPopup::showPopup(QWidget *parent, const CellBase &cell, const QRect &cellRect, Database *database)
{
    FilterPopup *popup = new FilterPopup(parent, cell, database);
    const QPoint position(database->orientation() == Qt::Vertical ? cellRect.bottomLeft() : cellRect.topRight());
    popup->move(parent->mapToGlobal(position));
    popup->show();
}
