/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Robert Knight <robertknight@gmail.com>
             SPDX-FileCopyrightText: 2006 Tomas Mecir <mecirt@gmail.com>
             SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 2002 Werner Trobin <trobin@kde.org>
             SPDX-FileCopyrightText: 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2000 David Faure <faure@kde.org>
             SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "SortDialog.h"

#include "engine/CellBase.h"
#include "engine/Localization.h"

// ui
#include "ui_SortDetailsWidget.h"
#include "ui_SortWidget.h"

#include <KoIcon.h>

// Qt
#include <QStyledItemDelegate>

using namespace Calligra::Sheets;

Q_DECLARE_METATYPE(Qt::CaseSensitivity)
Q_DECLARE_METATYPE(Qt::SortOrder)

class SortDialog::Private : public QStyledItemDelegate
{
public:
    Private(SortDialog *parent = nullptr)
        : QStyledItemDelegate(parent)
    {
    }

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        Q_UNUSED(index)
        Q_UNUSED(option)
        if (mainWidget.m_sortHorizontal->isChecked()) /* data grouped in columns; criteria/header per row */ {
            if (rows.isEmpty()) {
                return nullptr;
            }
        } else if (columns.isEmpty()) {
            return nullptr;
        }
        KComboBox *const combo = new KComboBox(parent);
        return combo;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const override
    {
        if (!index.isValid()) {
            return;
        }
        KComboBox *const combo = static_cast<KComboBox *>(editor);
        const QAbstractItemModel *const model = index.model();
        const int itemIndex = model->data(index, Qt::UserRole).toInt();
        const bool hasHeader = mainWidget.m_useHeader->isChecked();

        if (mainWidget.m_sortVertical->isChecked()) /* data grouped in rows; criteria/header per column */ {
            // Put the old item back into the map of available items.
            insertIndex(itemIndex, Qt::Horizontal);

            const QList<int> indices = columns;
            for (int i = 0; i < indices.count(); ++i) {
                const int col = indices[i];
                const QString columnName = i18n("Column %1", CellBase::columnName(col));
                if (hasHeader) {
                    const QString header = firstRow.at(i).asString();
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

            const QList<int> indices = rows;
            for (int i = 0; i < indices.count(); ++i) {
                const int row = indices[i];
                const QString rowName = i18n("Row %1", row);
                if (hasHeader) {
                    const QString header = firstCol.at(i).asString();
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

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override
    {
        KComboBox *const combo = static_cast<KComboBox *>(editor);
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

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        Q_UNUSED(index)
        editor->setGeometry(option.rect);
    }

public: // data
    QRect rect;
    Ui::SortWidget mainWidget;
    Ui::SortDetailsWidget detailsWidget;
    mutable QList<int> columns;
    mutable QList<int> rows;
    QVector<Value> firstRow, firstCol;

public:
    /// \return \c true if all columns/rows have text values
    bool hasHeader(Qt::Orientation orientation) const;
    void createAvailableIndices(Qt::Orientation orientation);
    void insertIndex(int index, Qt::Orientation orientation) const;
    QString itemText(int index, bool useHeader) const;
    void initCriteria(Qt::Orientation orientation, SortDialog *parent);
};

bool SortDialog::Private::hasHeader(Qt::Orientation orientation) const
{
    if (orientation == Qt::Horizontal) /* check for column headers */
    {
        if (!firstRow.size())
            return false;
        for (Value v : firstRow)
            if (!v.isString())
                return false;
    } else /* check for row headers */
    {
        if (!firstCol.size())
            return false;
        for (Value v : firstCol)
            if (!v.isString())
                return false;
    }
    return true;
}

void SortDialog::Private::createAvailableIndices(Qt::Orientation orientation)
{
    if (orientation == Qt::Horizontal) /* available columns */ {
        for (int col = rect.left(); col <= rect.right(); ++col) {
            columns.append(col);
        }
    } else /* available rows */ {
        for (int row = rect.top(); row <= rect.bottom(); ++row) {
            rows.append(row);
        }
    }
}

void SortDialog::Private::insertIndex(int index, Qt::Orientation orientation) const
{
    if (orientation == Qt::Vertical) /* data grouped in columns; criteria/header per row */ {
        QList<int>::Iterator it = std::lower_bound(rows.begin(), rows.end(), index);
        if (*it == index) {
            return;
        }
        rows.insert(it, index);
    } else /* data grouped in rows; criteria/header per column */ {
        QList<int>::Iterator it = std::lower_bound(columns.begin(), columns.end(), index);
        if (*it == index) {
            return;
        }
        columns.insert(it, index);
    }
}

QString SortDialog::Private::itemText(int index, bool useHeader) const
{
    if (mainWidget.m_sortHorizontal->isChecked()) /* data grouped in columns; criteria/header per row */
    {
        const QString rowName = i18n("Row %1", index);
        if (useHeader) {
            const QString header = firstCol.at(index - rect.top()).asString();
            if (header.isEmpty())
                return QString('(' + rowName + ')');
            return header;
        }
        return rowName;
    }
    /* data grouped in rows; criteria/header per column */
    const QString columnName = i18n("Column %1", CellBase::columnName(index));
    if (useHeader) {
        const QString header = firstRow.at(index - rect.left()).asString();
        if (header.isEmpty())
            return QString('(' + columnName + ')');
        return header;
    }
    return columnName;
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
    if (mainWidget.m_sortHorizontal->isChecked()) /* Sort horizontally */ {
        // data gets sorted horizontally; comparisons per row; columns get exchanged/sorted
        mainWidget.m_useHeader->setText(i18n("&First column contains row headers"));
    } else /* Sort vertically */ {
        // data gets sorted vertically; comparisons per column; rows get exchanged/sorted
        mainWidget.m_useHeader->setText(i18n("&First row contains column headers"));
    }
}

SortDialog::SortDialog(QWidget *parent, QRect rect, const QVector<Value> &firstRow, const QVector<Value> &firstCol)
    : KoDialog(parent)
    , d(new Private(this))
{
    d->rect = rect;
    d->firstRow = firstRow;
    d->firstCol = firstCol;

    setCaption(i18n("Sort"));
    setButtons(Ok | Cancel | Details | Reset);
    setObjectName(QLatin1String("SortDialog"));

    QWidget *widget = new QWidget(this);
    d->mainWidget.setupUi(widget);
    setMainWidget(widget);

    widget = new QWidget(this);
    d->detailsWidget.setupUi(widget);
    setDetailsWidget(widget);

    // UI refinements Designer is not capable of
    d->mainWidget.m_addButton->setIcon(koIcon("list-add"));
    d->mainWidget.m_removeButton->setIcon(koIcon("list-remove"));
    d->mainWidget.m_upButton->setIcon(koIcon("go-up"));
    d->mainWidget.m_downButton->setIcon(koIcon("go-down"));
    QHeaderView *const header = d->mainWidget.m_tableWidget->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    d->mainWidget.m_tableWidget->setItemDelegateForColumn(0, d);

    connect(d->mainWidget.m_useHeader, &QAbstractButton::toggled, this, &SortDialog::useHeaderChanged);
    connect(d->mainWidget.m_sortHorizontal, &QAbstractButton::toggled, this, &SortDialog::orientationChanged);

    connect(d->mainWidget.m_tableWidget, &QTableWidget::itemActivated, this, &SortDialog::itemActivated);
    connect(d->mainWidget.m_tableWidget, &QTableWidget::itemSelectionChanged, this, &SortDialog::itemSelectionChanged);

    connect(d->mainWidget.m_addButton, &QAbstractButton::clicked, this, &SortDialog::addCriterion);
    connect(d->mainWidget.m_removeButton, &QAbstractButton::clicked, this, &SortDialog::removeCriterion);
    connect(d->mainWidget.m_upButton, &QAbstractButton::clicked, this, &SortDialog::moveCriterionUp);
    connect(d->mainWidget.m_downButton, &QAbstractButton::clicked, this, &SortDialog::moveCriterionDown);

    init();
}

SortDialog::~SortDialog()
{
    delete d;
}

void SortDialog::init()
{
    d->mainWidget.m_sortHorizontal->setEnabled(true);
    d->mainWidget.m_sortVertical->setEnabled(true);

    if (!d->firstRow.size()) {
        d->mainWidget.m_sortHorizontal->setEnabled(false);
        d->mainWidget.m_sortVertical->setChecked(true);

        const bool hasHeader = d->hasHeader(Qt::Horizontal);
        d->mainWidget.m_useHeader->setChecked(hasHeader);
    } else if (!d->firstCol.size()) {
        d->mainWidget.m_sortVertical->setEnabled(false);
        d->mainWidget.m_sortHorizontal->setChecked(true);

        const bool hasHeader = d->hasHeader(Qt::Vertical);
        d->mainWidget.m_useHeader->setChecked(hasHeader);
    } else /* both directions are possible */ {
        const bool hasColumnHeader = d->hasHeader(Qt::Horizontal);
        const bool hasRowHeader = d->hasHeader(Qt::Vertical);

        if (d->rect.width() >= d->rect.height()) {
            d->mainWidget.m_sortHorizontal->setChecked(true);
            d->mainWidget.m_useHeader->setChecked(hasRowHeader);
        } else {
            d->mainWidget.m_sortVertical->setChecked(true);
            d->mainWidget.m_useHeader->setChecked(hasColumnHeader);
        }
    }
    // create column indices, if data can be sorted vertically
    if (d->mainWidget.m_sortVertical->isEnabled())
        d->createAvailableIndices(Qt::Horizontal);
    // create row indices, if data can be sorted horizontally
    if (d->mainWidget.m_sortHorizontal->isEnabled())
        d->createAvailableIndices(Qt::Vertical);

    // Initialize the criteria.
    slotButtonClicked(Reset);
}

void SortDialog::setCustomLists(const QStringList &lsts, Localization *locale)
{
    QStringList lst;
    QString e;
    for (int month = 1; month <= 12; ++month) {
        e += locale->monthName(month);
        if (month < 12)
            e += ", ";
    }
    lst << e;

    e = QString();
    for (int day = 1; day <= 7; ++day) {
        e += locale->dayName(day);
        if (day < 7)
            e += ", ";
    }
    lst << e;

    QString tmp;
    for (QStringList::ConstIterator it = lsts.begin(); it != lsts.end(); ++it) {
        if ((*it) != "\\")
            tmp += (*it) + ", ";
        else if (it != lsts.begin()) {
            tmp = tmp.left(tmp.length() - 2);
            lst.append(tmp);
            tmp.clear();
        }
    }
    d->detailsWidget.m_customList->insertItems(0, lst);
}

bool SortDialog::sortRows() const
{
    return d->mainWidget.m_sortVertical->isChecked();
}

bool SortDialog::skipFirst() const
{
    return d->mainWidget.m_useHeader->isChecked();
}

bool SortDialog::copyFormat() const
{
    return d->detailsWidget.m_copyLayout->isChecked();
}

bool SortDialog::isHorizontal() const
{
    return d->mainWidget.m_sortHorizontal->isChecked();
}

bool SortDialog::useCustomList() const
{
    return d->detailsWidget.m_useCustomLists->isChecked();
}

QStringList SortDialog::customList() const
{
    QString txt = d->detailsWidget.m_customList->currentText();
    QStringList clist = txt.split(",");
    QStringList res;
    for (QString chunk : clist)
        res.append(chunk.trimmed());
    return res;
}

int SortDialog::criterionCount() const
{
    QTableWidget *const table = d->mainWidget.m_tableWidget;
    return table->rowCount();
}

int SortDialog::criterionIndex(int row) const
{
    QTableWidget *const table = d->mainWidget.m_tableWidget;
    Q_ASSERT((row >= 0) && (row < table->rowCount()));

    return table->item(row, 0)->data(Qt::UserRole).toInt();
}

Qt::SortOrder SortDialog::criterionSortOrder(int row) const
{
    QTableWidget *const table = d->mainWidget.m_tableWidget;
    Q_ASSERT((row >= 0) && (row < table->rowCount()));

    return table->item(row, 1)->data(Qt::UserRole).value<Qt::SortOrder>();
}

Qt::CaseSensitivity SortDialog::criterionCaseSensitivity(int row) const
{
    QTableWidget *const table = d->mainWidget.m_tableWidget;
    Q_ASSERT((row >= 0) && (row < table->rowCount()));

    return table->item(row, 2)->data(Qt::UserRole).value<Qt::CaseSensitivity>();
}

void SortDialog::orientationChanged(bool horizontal)
{
    // Take the old, i.e. the reverse orientation.
    const Qt::Orientation orientation = horizontal ? Qt::Horizontal : Qt::Vertical;
    d->initCriteria(orientation, this);
}

void SortDialog::slotButtonClicked(int button)
{
    if (button == Reset) {
        const bool horizontal = d->mainWidget.m_sortHorizontal->isChecked();
        const Qt::Orientation orientation = horizontal ? Qt::Vertical : Qt::Horizontal;
        d->initCriteria(orientation, this);
    }
    KoDialog::slotButtonClicked(button);
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
            item->setIcon(koIcon("view-sort-descending"));
            item->setText(i18n("Descending"));
            item->setData(Qt::UserRole, QVariant::fromValue(Qt::DescendingOrder));
        } else {
            item->setIcon(koIcon("view-sort-ascending"));
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
    item = new QTableWidgetItem(koIcon("view-sort-ascending"), i18n("Ascending"));
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
    std::stable_sort(ranges.begin(), ranges.end(), greaterThan);
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
