/* This file is part of the KDE project
   Copyright (C) 2006 Robert Knight <robertknight@gmail.com>
             (C) 2006 Tomas Mecir <mecirt@gmail.com>
             (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002 Ariya Hidayat <ariya@kde.org>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2002 Werner Trobin <trobin@kde.org>
             (C) 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
             (C) 2000 David Faure <faure@kde.org>
             (C) 1998-2000 Torben Weis <weis@kde.org>

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
#include "SortDialog.h"

// KSpread
#include "Map.h"
#include "ui/Selection.h"
#include "Sheet.h"
#include "ValueConverter.h"

// commands
#include "commands/SortManipulator.h"

// ui
#include "ui_SortWidget.h"
#include "ui_SortDetailsWidget.h"

// Qt
#include <QStyledItemDelegate>

using namespace KSpread;

Q_DECLARE_METATYPE(Qt::CaseSensitivity)
Q_DECLARE_METATYPE(Qt::SortOrder)

class SortDialog::Private : public QStyledItemDelegate
{
public:
    Private(SortDialog *parent = 0)
            : QStyledItemDelegate(parent)
    {
    }

    virtual QWidget *createEditor(QWidget *parent,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const
    {
        Q_UNUSED(index)
        Q_UNUSED(option)
        if (mainWidget.m_sortHorizontal->isChecked()) /* data grouped in columns; criteria/header per row */ {
            if (rows.isEmpty()) {
                return 0;
            }
        } else if (columns.isEmpty()) {
            return 0;
        }
        KComboBox *const combo = new KComboBox(parent);
        return combo;
    }

    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        if (!index.isValid()) {
            return;
        }
        KComboBox *const combo = static_cast<KComboBox*>(editor);
        const QAbstractItemModel *const model = index.model();
        const QString itemText = model->data(index, Qt::DisplayRole).toString();
        const int itemIndex = model->data(index, Qt::UserRole).toInt();
        const bool hasHeader = mainWidget.m_useHeader->isChecked();
        Sheet *const sheet = selection->lastSheet();
        ValueConverter *const converter = sheet->map()->converter();

        if (mainWidget.m_sortVertical->isChecked()) /* data grouped in rows; criteria/header per column */ {
            // Put the old item back into the map of available items.
            insertIndex(itemIndex, Qt::Horizontal);

            const int row = selection->lastRange().top();
            const QList<int> indices = columns;
            for (int i = 0; i < indices.count(); ++i) {
                const int col = indices[i];
                const QString columnName = i18n("Column %1", Cell::columnName(col));
                const Value value = Cell(sheet, col, row).value();
                const QString header = converter->asString(value).asString();
                if (hasHeader) {
                    if (header.isEmpty()) {
                        combo->addItem('(' + columnName + ')', col);
                    } else {
                        combo->addItem(header, col);
                        combo->setItemData(combo->count() - 1, columnName, Qt::ToolTipRole);
                    }
                } else {
                    combo->addItem(columnName, col);
                }
                if (col == itemIndex) {
                    combo->setCurrentIndex(i);
                }
            }
        } else /* row headers */ {
            // Put the old item back into the map of available items.
            insertIndex(itemIndex, Qt::Vertical);

            const int col = selection->lastRange().left();
            const QList<int> indices = rows;
            for (int i = 0; i < indices.count(); ++i) {
                const int row = indices[i];
                const QString rowName = i18n("Row %1", row);
                const Value value = Cell(sheet, col, row).value();
                const QString header = converter->asString(value).asString();
                if (hasHeader) {
                    if (header.isEmpty()) {
                        combo->addItem('(' + rowName + ')', row);
                    } else {
                        combo->addItem(header, row);
                        combo->setItemData(combo->count() - 1, rowName, Qt::ToolTipRole);
                    }
                } else {
                    combo->addItem(rowName, row);
                }
                if (row == itemIndex) {
                    combo->setCurrentIndex(i);
                }
            }
        }
    }

    virtual void setModelData(QWidget *editor, QAbstractItemModel *model,
                              const QModelIndex &index) const
    {
        KComboBox *const combo = static_cast<KComboBox*>(editor);
        const int currentIndex = combo->currentIndex();
        model->setData(index, combo->itemText(currentIndex), Qt::DisplayRole);
        model->setData(index, combo->itemData(currentIndex), Qt::UserRole);

        // Remove the current item from the map of available items.
        if (mainWidget.m_sortHorizontal->isChecked()) /* data grouped in columns; criteria/header per row */ {
            rows.removeAll(combo->itemData(currentIndex).toInt());
        } else {
            columns.removeAll(combo->itemData(currentIndex).toInt());
        }
    }

    virtual void updateEditorGeometry(QWidget *editor,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const
    {
        Q_UNUSED(index)
        editor->setGeometry(option.rect);
    }

public: // data
    Selection *selection;
    Ui::SortWidget mainWidget;
    Ui::SortDetailsWidget detailsWidget;
    mutable QList<int> columns;
    mutable QList<int> rows;

public:
    /// \return \c true if all columns/rows have text values
    bool hasHeader(const Region &region, Qt::Orientation orientation) const;
    void createAvailableIndices(const Region &region, Qt::Orientation orientation);
    void insertIndex(int index, Qt::Orientation orientation) const;
    QString itemText(int index, bool useHeader) const;
    void initCriteria(Qt::Orientation orientation, SortDialog *parent);
};

bool SortDialog::Private::hasHeader(const Region &region, Qt::Orientation orientation) const
{
    Sheet *const sheet = region.lastSheet();
    const QRect range = region.lastRange();
    if (orientation == Qt::Horizontal) /* check for column headers */ {
        for (int col = range.left(); col <= range.right(); ++col) {
            if (!Cell(sheet, col, range.top()).value().isString())  {
                return false;
            }
        }
    } else /* check for row headers */ {
        for (int row = range.top(); row <= range.bottom(); ++row) {
            if (!Cell(sheet, range.left(), row).value().isString()) {
                return false;
            }
        }
    }
    return true;
}

void SortDialog::Private::createAvailableIndices(const Region &region, Qt::Orientation orientation)
{
    const QRect range = region.lastRange();
    if (orientation == Qt::Horizontal) /* available columns */ {
        for (int col = range.left(); col <= range.right(); ++col) {
            columns.append(col);
        }
    } else /* available rows */ {
        for (int row = range.top(); row <= range.bottom(); ++row) {
            rows.append(row);
        }
    }
}

void SortDialog::Private::insertIndex(int index, Qt::Orientation orientation) const
{
    if (orientation == Qt::Vertical) /* data grouped in columns; criteria/header per row */ {
        Q_ASSERT(1 <= index && index <= KS_colMax);
        QList<int>::Iterator it = qLowerBound(rows.begin(), rows.end(), index);
        if (*it == index) {
            return;
        }
        rows.insert(it, index);
    } else /* data grouped in rows; criteria/header per column */ {
        Q_ASSERT(1 <= index && index <= KS_rowMax);
        QList<int>::Iterator it = qLowerBound(columns.begin(), columns.end(), index);
        if (*it == index) {
            return;
        }
        columns.insert(it, index);
    }
}

QString SortDialog::Private::itemText(int index, bool useHeader) const
{
    Sheet *const sheet = selection->lastSheet();
    ValueConverter *const converter = sheet->map()->converter();

    if (mainWidget.m_sortHorizontal->isChecked()) /* data grouped in columns; criteria/header per row */ {
        const int col = selection->lastRange().left();
        const int row = index;
        const QString rowName = i18n("Row %1", row);
        if (useHeader) {
            const Value value = Cell(sheet, col, row).value();
            const QString header = converter->asString(value).asString();
            if (header.isEmpty()) {
                return QString('(' + rowName + ')');
            } else {
                return header;
            }
        } else {
            return rowName;
        }
    } else /* data grouped in rows; criteria/header per column */ {
        const int col = index;
        const int row = selection->lastRange().top();
        const QString columnName = i18n("Column %1", Cell::columnName(col));
        if (useHeader) {
            const Value value = Cell(sheet, col, row).value();
            const QString header = converter->asString(value).asString();
            if (header.isEmpty()) {
                return QString('(' + columnName + ')');
            } else {
                return header;
            }
        } else {
            return columnName;
        }
    }
}

void SortDialog::Private::initCriteria(Qt::Orientation orientation, SortDialog *parent)
{
    // Put the items back into the map of available items.
    for (int row = mainWidget.m_tableWidget->rowCount() - 1; row >= 0; --row) {
        QTableWidgetItem *const item = mainWidget.m_tableWidget->item(row, 0);
        const int index = item->data(Qt::UserRole).toInt();
        insertIndex(index, orientation);
        mainWidget.m_tableWidget->removeRow(row);
    }

    // (Re-)Insert the criteria.
    if (mainWidget.m_sortHorizontal->isChecked()) {
        while (rows.count()) {
            parent->addCriterion();
        }
    } else {
        while (columns.count()) {
            parent->addCriterion();
        }
    }

    // Setup the buttons.
    mainWidget.m_addButton->setEnabled(false);
    mainWidget.m_removeButton->setEnabled(false);
    mainWidget.m_upButton->setEnabled(false);
    mainWidget.m_downButton->setEnabled(false);

    // Adjust the header usage text.
    if (mainWidget.m_sortHorizontal->isChecked()) /* Sort horizontally */  {
        // data gets sorted horizontally; comparisons per row; columns get exchanged/sorted
        mainWidget.m_useHeader->setText(i18n("&First column contains row headers"));
    } else /* Sort vertically */ {
        // data gets sorted vertically; comparisons per column; rows get exchanged/sorted
        mainWidget.m_useHeader->setText(i18n("&First row contains column headers"));
    }
}


SortDialog::SortDialog(QWidget* parent, Selection* selection)
        : KDialog(parent)
        , d(new Private(this))
{
    d->selection = selection;

    setCaption(i18n("Sort"));
    setButtons(Ok | Cancel | Details | Reset);
    setObjectName("SortDialog");

    QWidget *widget = new QWidget(this);
    d->mainWidget.setupUi(widget);
    setMainWidget(widget);

    widget = new QWidget(this);
    d->detailsWidget.setupUi(widget);
    setDetailsWidget(widget);

    // UI refinements Designer is not capable of
    d->mainWidget.m_addButton->setIcon(KIcon("list-add"));
    d->mainWidget.m_removeButton->setIcon(KIcon("list-remove"));
    d->mainWidget.m_upButton->setIcon(KIcon("go-up"));
    d->mainWidget.m_downButton->setIcon(KIcon("go-down"));
    QHeaderView *const header = d->mainWidget.m_tableWidget->horizontalHeader();
    header->setResizeMode(QHeaderView::ResizeToContents);
    header->setResizeMode(0, QHeaderView::Stretch);
    d->mainWidget.m_tableWidget->setItemDelegateForColumn(0, d);

    connect(d->mainWidget.m_useHeader, SIGNAL(toggled(bool)),
            this, SLOT(useHeaderChanged(bool)));
    connect(d->mainWidget.m_sortHorizontal, SIGNAL(toggled(bool)),
            this, SLOT(orientationChanged(bool)));

    connect(d->mainWidget.m_tableWidget, SIGNAL(itemActivated(QTableWidgetItem*)),
            this, SLOT(itemActivated(QTableWidgetItem*)));
    connect(d->mainWidget.m_tableWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(itemSelectionChanged()));

    connect(d->mainWidget.m_addButton, SIGNAL(clicked()),
            this, SLOT(addCriterion()));
    connect(d->mainWidget.m_removeButton, SIGNAL(clicked()),
            this, SLOT(removeCriterion()));
    connect(d->mainWidget.m_upButton, SIGNAL(clicked()),
            this, SLOT(moveCriterionUp()));
    connect(d->mainWidget.m_downButton, SIGNAL(clicked()),
            this, SLOT(moveCriterionDown()));

    init();
}

SortDialog::~SortDialog()
{
    delete d;
}

void SortDialog::init()
{
    QStringList lst;
    lst << i18n("January") + ',' + i18n("February") + ',' + i18n("March") +
    ',' + i18n("April") + ',' + i18n("May") + ',' + i18n("June") +
    ',' + i18n("July") + ',' + i18n("August") + ',' + i18n("September") +
    ',' + i18n("October") + ',' + i18n("November") +
    ',' + i18n("December");

    lst << i18n("Monday") + ',' + i18n("Tuesday") + ',' + i18n("Wednesday") +
    ',' + i18n("Thursday") + ',' + i18n("Friday") + ',' + i18n("Saturday") +
    ',' + i18n("Sunday");

    KSharedConfigPtr config = KGlobal::activeComponent().config();
    const QStringList other = config->group("Parameters").readEntry("Other list", QStringList());
    QString tmp;
    for (QStringList::ConstIterator it = other.begin(); it != other.end(); ++it) {
        if ((*it) != "\\")
            tmp += (*it) + ", ";
        else if (it != other.begin()) {
            tmp = tmp.left(tmp.length() - 2);
            lst.append(tmp);
            tmp = "";
        }
    }
    d->detailsWidget.m_customList->insertItems(0, lst);

    Sheet *const sheet = d->selection->lastSheet();
    const QRect range = d->selection->lastRange();
    const Region region(range, sheet);

    if (region.isColumnSelected()) /* entire columns */ {
        d->mainWidget.m_sortHorizontal->setEnabled(false);
        d->mainWidget.m_sortVertical->setChecked(true);

        const bool hasHeader = d->hasHeader(region, Qt::Horizontal);
        d->mainWidget.m_useHeader->setChecked(hasHeader);
        d->createAvailableIndices(region, Qt::Horizontal);
    }
    else if (region.isRowSelected()) /* entire rows */ {
        d->mainWidget.m_sortVertical->setEnabled(false);
        d->mainWidget.m_sortHorizontal->setChecked(true);

        const bool hasHeader = d->hasHeader(region, Qt::Vertical);
        d->mainWidget.m_useHeader->setChecked(hasHeader);
        d->createAvailableIndices(region, Qt::Vertical);
    } else /* ordinary cell range */ {
        if (range.top() == range.bottom()) /* only one row */{
            d->mainWidget.m_sortVertical->setEnabled(false);
            d->mainWidget.m_sortHorizontal->setChecked(true);
        } else if (range.left() == range.right()) /* only one column */ {
            d->mainWidget.m_sortHorizontal->setEnabled(false);
            d->mainWidget.m_sortVertical->setChecked(true);
        } else {
            const bool hasColumnHeader = d->hasHeader(region, Qt::Horizontal);
            const bool hasRowHeader = d->hasHeader(region, Qt::Vertical);

#if 0 // TODO
            if (hasColumnHeader && range.top() + 1 == range.bottom()) /* only one data row */ {
                d->mainWidget.m_sortVertical->setEnabled(false);
            }
            if (hasRowHeader && range.left() + 1 == range.right()) /* only one data column */ {
                d->mainWidget.m_sortHorizontal->setEnabled(false);
            }
#endif

            if (range.width() >= range.height()) {
                d->mainWidget.m_sortHorizontal->setChecked(true);
                d->mainWidget.m_useHeader->setChecked(hasRowHeader);
            } else {
                d->mainWidget.m_sortVertical->setChecked(true);
                d->mainWidget.m_useHeader->setChecked(hasColumnHeader);
            }
        }

        if (d->mainWidget.m_sortHorizontal->isEnabled()) {
            d->createAvailableIndices(region, Qt::Horizontal);
        }
        if (d->mainWidget.m_sortVertical->isEnabled()) {
            d->createAvailableIndices(region, Qt::Vertical);
        }
    }

    // Initialize the criteria.
    slotButtonClicked(Reset);
}

void SortDialog::orientationChanged(bool horizontal)
{
    // Take the old, i.e. the reverse orientation.
    const Qt::Orientation orientation = horizontal ? Qt::Horizontal : Qt::Vertical;
    d->initCriteria(orientation, this);
}

void SortDialog::accept()
{
    Sheet *const sheet = d->selection->activeSheet();

    SortManipulator *const command = new SortManipulator();
    command->setSheet(sheet);

    // set parameters
    command->setSortRows(d->mainWidget.m_sortVertical->isChecked());
    command->setSkipFirst(d->mainWidget.m_useHeader->isChecked());
    command->setCopyFormat(d->detailsWidget.m_copyLayout->isChecked());

    const bool horizontal = d->mainWidget.m_sortHorizontal->isChecked();
    const QRect range = d->selection->lastRange();
    const int offset = horizontal ? range.top() : range.left();

    // retrieve sorting order
    QTableWidget *const table = d->mainWidget.m_tableWidget;
    for (int i = 0; i < table->rowCount(); ++i) {
        const int index = table->item(i, 0)->data(Qt::UserRole).toInt();
        const Qt::SortOrder order = table->item(i, 1)->data(Qt::UserRole).value<Qt::SortOrder>();
        const Qt::CaseSensitivity caseSensitivity = table->item(i, 2)->data(Qt::UserRole).value<Qt::CaseSensitivity>();
        command->addCriterion(index - offset, order, caseSensitivity);
    }

    if (d->detailsWidget.m_useCustomLists->isChecked()) {
        // add custom list if any
        QStringList clist;
        QString list = d->detailsWidget.m_customList->currentText();
        QString tmp;
        int l = list.length();
        for (int i = 0; i < l; ++i) {
            if (list[i] == ',') {
                clist.append(tmp.trimmed());
                tmp = "";
            } else
                tmp += list[i];
        }

        command->setUseCustomList(true);
        command->setCustomList(clist);
    }
    command->add(d->selection->lastRange());
    command->execute(d->selection->canvas());

    d->selection->emitModified();
    KDialog::accept();
}

void SortDialog::slotButtonClicked(int button)
{
    if (button == Reset) {
        const bool horizontal = d->mainWidget.m_sortHorizontal->isChecked();
        const Qt::Orientation orientation = horizontal ? Qt::Vertical : Qt::Horizontal;
        d->initCriteria(orientation, this);
    }
    KDialog::slotButtonClicked(button);
}

void SortDialog::useHeaderChanged(bool enable)
{
    // Rename the list items.
    QTableWidget *const table = d->mainWidget.m_tableWidget;
    for (int row = 0; row < table->rowCount(); ++row) {
        QTableWidgetItem *const item = table->item(row, 0);
        const int index = item->data(Qt::UserRole).toInt();
        item->setText(d->itemText(index, enable));
    }
}

void SortDialog::itemActivated(QTableWidgetItem *item)
{
    if (item->column() == 1) /* Sort Order */ {
        if (item->data(Qt::UserRole).value<Qt::SortOrder>() == Qt::AscendingOrder) {
            item->setIcon(KIcon("view-sort-descending"));
            item->setText(i18n("Descending"));
            item->setData(Qt::UserRole, QVariant::fromValue(Qt::DescendingOrder));
        } else {
            item->setIcon(KIcon("view-sort-ascending"));
            item->setText(i18n("Ascending"));
            item->setData(Qt::UserRole, QVariant::fromValue(Qt::AscendingOrder));
        }
    } else if (item->column() == 2) /* Case Sensitivity */ {
        if (item->checkState() == Qt::Checked) {
            item->setCheckState(Qt::Unchecked);
            item->setText(i18n("Case Insensitive"));
            item->setData(Qt::UserRole, QVariant::fromValue(Qt::CaseInsensitive));
        } else {
            item->setCheckState(Qt::Checked);
            item->setText(i18n("Case Sensitive"));
            item->setData(Qt::UserRole, QVariant::fromValue(Qt::CaseSensitive));
        }
    }
}

void SortDialog::itemSelectionChanged()
{
    QTableWidget *const table = d->mainWidget.m_tableWidget;
    QList<QTableWidgetSelectionRange> ranges = table->selectedRanges();
    if (ranges.count() == 0) {
        d->mainWidget.m_removeButton->setEnabled(false);
        d->mainWidget.m_upButton->setEnabled(false);
        d->mainWidget.m_downButton->setEnabled(false);
    } else {
        d->mainWidget.m_removeButton->setEnabled(true);
        bool first = false;
        bool last = false;
        for (int i = 0; i < ranges.count(); ++i) {
            if (ranges[i].topRow() == 0) {
                first = true;
            }
            if (ranges[i].bottomRow() == table->rowCount() - 1) {
                last = true;
            }
            if (first && last) {
                break;
            }
        }
        d->mainWidget.m_upButton->setEnabled(!first);
        d->mainWidget.m_downButton->setEnabled(!last);
    }
}

void SortDialog::addCriterion()
{
    QTableWidgetItem *item;
    const bool useHeader = d->mainWidget.m_useHeader->isChecked();
    // Take the first item from the map of available items.
    if (d->mainWidget.m_sortVertical->isChecked()) /* data grouped in rows; criteria/header per column */ {
        const QList<int> keys = d->columns;
        if (keys.isEmpty()) {
            return;
        } else if (keys.count() == 1) {
            d->mainWidget.m_addButton->setEnabled(false);
        }
        const int col = d->columns.takeFirst();
        item = new QTableWidgetItem(d->itemText(col, useHeader));
        item->setData(Qt::UserRole, col);
    } else {
        const QList<int> keys = d->rows;
        if (keys.isEmpty()) {
            return;
        } else if (keys.count() == 1) {
            d->mainWidget.m_addButton->setEnabled(false);
        }
        const int row = d->rows.takeFirst();
        item = new QTableWidgetItem(d->itemText(row, useHeader));
        item->setData(Qt::UserRole, row);
    }
    // Insert the item and its default attributes in a new row.
    const int row = d->mainWidget.m_tableWidget->rowCount();
    d->mainWidget.m_tableWidget->insertRow(row);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
    d->mainWidget.m_tableWidget->setItem(row, 0, item);
    item = new QTableWidgetItem(KIcon("view-sort-ascending"), i18n("Ascending"));
    item->setData(Qt::UserRole, Qt::AscendingOrder);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
    d->mainWidget.m_tableWidget->setItem(row, 1, item);
    item = new QTableWidgetItem(i18n("Case Sensitive"));
    item->setCheckState(Qt::Checked);
    item->setData(Qt::UserRole, Qt::CaseSensitive);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
    d->mainWidget.m_tableWidget->setItem(row, 2, item);
}

bool greaterThan(const QTableWidgetSelectionRange &r1, const QTableWidgetSelectionRange &r2)
{
    return r1.topRow() > r2.topRow();
}

void SortDialog::removeCriterion()
{
    QTableWidget *const table = d->mainWidget.m_tableWidget;
    QList<QTableWidgetSelectionRange> ranges = table->selectedRanges();
    if (ranges.isEmpty()) {
        return;
    }
    qStableSort(ranges.begin(), ranges.end(), greaterThan);
    for (int i = 0; i < ranges.count(); ++i) {
        for (int row = ranges[i].bottomRow(); row >= ranges[i].topRow(); --row) {
            // Reinsert the item to be removed into the map of available items.
            const int index = table->item(row, 0)->data(Qt::UserRole).toInt();
            if (d->mainWidget.m_sortHorizontal->isChecked()) {
                d->insertIndex(index, Qt::Vertical);
            } else {
                d->insertIndex(index, Qt::Horizontal);
            }
            // Remove the item from the list.
            table->removeRow(row);
        }
    }
    d->mainWidget.m_addButton->setEnabled(true);
}

void SortDialog::moveCriterionUp()
{
    QTableWidget *const table = d->mainWidget.m_tableWidget;
    const QList<QTableWidgetSelectionRange> ranges = table->selectedRanges();
    for (int i = 0; i < ranges.count(); ++i) {
        if (ranges[i].topRow() > 0) {
            const int srcRow = ranges[i].topRow() - 1;
            const int dstRow = ranges[i].bottomRow() + 1;
            table->insertRow(dstRow);
            for (int col = 0; col <= 2; ++col) {
                table->setItem(dstRow, col, table->takeItem(srcRow, col));
            }
            table->removeRow(srcRow);
        }
    }
    itemSelectionChanged();
}

void SortDialog::moveCriterionDown()
{
    QTableWidget *const table = d->mainWidget.m_tableWidget;
    const QList<QTableWidgetSelectionRange> ranges = table->selectedRanges();
    for (int i = 0; i < ranges.count(); ++i) {
        if (ranges[i].bottomRow() < table->rowCount() - 1) {
            const int srcRow = ranges[i].bottomRow() + 2;
            const int dstRow = ranges[i].topRow();
            table->insertRow(dstRow);
            for (int col = 0; col <= 2; ++col) {
                table->setItem(dstRow, col, table->takeItem(srcRow, col));
            }
            table->removeRow(srcRow);
        }
    }
    itemSelectionChanged();
}

#include "SortDialog.moc"
