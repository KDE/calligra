/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 KO GmbH <cbo@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoTableColumnAndRowStyleManager.h"

#include "styles/KoTableCellStyle.h"
#include "styles/KoTableColumnStyle.h"
#include "styles/KoTableRowStyle.h"
#include "styles/KoTableStyle.h"

#include <QTextTable>
#include <QVariant>
#include <QVector>

#include "TextDebug.h"

class Q_DECL_HIDDEN KoTableColumnAndRowStyleManager::Private : public QSharedData
{
public:
    Private() = default;
    ~Private() = default;
    QVector<KoTableColumnStyle> tableColumnStyles;
    QVector<KoTableRowStyle> tableRowStyles;

    QVector<KoTableCellStyle *> defaultRowCellStyles;
    QVector<KoTableCellStyle *> defaultColumnCellStyles;
};

KoTableColumnAndRowStyleManager::KoTableColumnAndRowStyleManager()
    : d(new Private())
{
}

KoTableColumnAndRowStyleManager::KoTableColumnAndRowStyleManager(const KoTableColumnAndRowStyleManager &rhs)

    = default;

KoTableColumnAndRowStyleManager &KoTableColumnAndRowStyleManager::operator=(const KoTableColumnAndRowStyleManager &rhs) = default;

KoTableColumnAndRowStyleManager::~KoTableColumnAndRowStyleManager() = default;

KoTableColumnAndRowStyleManager KoTableColumnAndRowStyleManager::getManager(QTextTable *table)
{
    QTextTableFormat tableFormat = table->format();

    if (tableFormat.hasProperty(KoTableStyle::ColumnAndRowStyleManager)) {
        return tableFormat.property(KoTableStyle::ColumnAndRowStyleManager).value<KoTableColumnAndRowStyleManager>();
    } else {
        KoTableColumnAndRowStyleManager carsManager;

        QVariant var;
        var.setValue(carsManager);
        tableFormat.setProperty(KoTableStyle::ColumnAndRowStyleManager, var);
        table->setFormat(tableFormat);
        return carsManager;
    }
}

void KoTableColumnAndRowStyleManager::setColumnStyle(int column, const KoTableColumnStyle &columnStyle)
{
    Q_ASSERT(column >= 0);

    if (column < 0) {
        return;
    }

    if (column < d->tableColumnStyles.size() && d->tableColumnStyles.value(column) == columnStyle) {
        return;
    }

    // TODO: just resize() if needed should work as well
    d->tableColumnStyles.reserve(column + 1);
    while (column >= d->tableColumnStyles.size())
        d->tableColumnStyles.append(KoTableColumnStyle());

    d->tableColumnStyles.replace(column, columnStyle);
}

void KoTableColumnAndRowStyleManager::insertColumns(int column, int numberColumns, const KoTableColumnStyle &columnStyle)
{
    Q_ASSERT(column >= 0);
    Q_ASSERT(numberColumns >= 0);

    if (column < 0 || numberColumns < 0) {
        return;
    }

    // TODO: just resize() if needed should work as well
    d->tableColumnStyles.reserve(column + numberColumns);
    while (column > d->tableColumnStyles.size())
        d->tableColumnStyles.append(KoTableColumnStyle());

    d->tableColumnStyles.insert(column, numberColumns, columnStyle);
}

void KoTableColumnAndRowStyleManager::removeColumns(int column, int numberColumns)
{
    Q_ASSERT(column >= 0);
    Q_ASSERT(numberColumns >= 0);

    if (column >= d->tableColumnStyles.size() || column < 0 || numberColumns < 0) {
        return;
    }

    d->tableColumnStyles.remove(column, numberColumns);
}

KoTableColumnStyle KoTableColumnAndRowStyleManager::columnStyle(int column) const
{
    Q_ASSERT(column >= 0);

    if (column < 0) {
        return KoTableColumnStyle();
    }

    return d->tableColumnStyles.value(column);
}

void KoTableColumnAndRowStyleManager::setRowStyle(int row, const KoTableRowStyle &rowStyle)
{
    Q_ASSERT(row >= 0);

    if (row < 0) {
        return;
    }

    if (row < d->tableRowStyles.size() && d->tableRowStyles.value(row) == rowStyle) {
        return;
    }

    // TODO: just resize() if needed should work as well
    d->tableRowStyles.reserve(row + 1);
    while (row >= d->tableRowStyles.size())
        d->tableRowStyles.append(KoTableRowStyle());

    d->tableRowStyles.replace(row, rowStyle);
}

void KoTableColumnAndRowStyleManager::insertRows(int row, int numberRows, const KoTableRowStyle &rowStyle)
{
    Q_ASSERT(row >= 0);
    Q_ASSERT(numberRows >= 0);

    if (row < 0 || numberRows < 0) {
        return;
    }

    // TODO: just resize() if needed should work as well
    d->tableRowStyles.reserve(row + numberRows);
    while (row > d->tableRowStyles.size())
        d->tableRowStyles.append(KoTableRowStyle());

    d->tableRowStyles.insert(row, numberRows, rowStyle);
}

void KoTableColumnAndRowStyleManager::removeRows(int row, int numberRows)
{
    Q_ASSERT(row >= 0);
    Q_ASSERT(numberRows >= 0);

    if (row >= d->tableRowStyles.size() || row < 0 || numberRows < 0) {
        return;
    }

    d->tableRowStyles.remove(row, numberRows);
}

KoTableRowStyle KoTableColumnAndRowStyleManager::rowStyle(int row) const
{
    Q_ASSERT(row >= 0);

    if (row < 0) {
        return KoTableRowStyle();
    }

    return d->tableRowStyles.value(row);
}

KoTableCellStyle *KoTableColumnAndRowStyleManager::defaultColumnCellStyle(int column) const
{
    Q_ASSERT(column >= 0);

    return d->defaultColumnCellStyles.value(column);
}

void KoTableColumnAndRowStyleManager::setDefaultColumnCellStyle(int column, KoTableCellStyle *cellStyle)
{
    Q_ASSERT(column >= 0);

    if (column < d->defaultColumnCellStyles.size() && d->defaultColumnCellStyles.value(column) == cellStyle) {
        return;
    }

    while (column > d->defaultColumnCellStyles.size())
        d->defaultColumnCellStyles.append(nullptr);

    d->defaultColumnCellStyles.append(cellStyle);
}

KoTableCellStyle *KoTableColumnAndRowStyleManager::defaultRowCellStyle(int row) const
{
    Q_ASSERT(row >= 0);

    return d->defaultRowCellStyles.value(row);
}

void KoTableColumnAndRowStyleManager::setDefaultRowCellStyle(int row, KoTableCellStyle *cellStyle)
{
    Q_ASSERT(row >= 0);

    if (row < d->defaultRowCellStyles.size() && d->defaultRowCellStyles.value(row) == cellStyle) {
        return;
    }

    while (row > d->defaultRowCellStyles.size())
        d->defaultRowCellStyles.append(nullptr);

    d->defaultRowCellStyles.append(cellStyle);
}
