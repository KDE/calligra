/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2010 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "TableSource.h"
#include <QAbstractItemModel>
#include <QMap>
#include <QPointer>
#include <QSet>
#include <Qt>

Q_DECLARE_METATYPE(QPointer<QAbstractItemModel>)

using namespace KoChart;

Table::Table(const QString &name, QAbstractItemModel *model)
    : m_name(name)
    , m_model(model)
{
    Q_ASSERT(!name.isEmpty());
    Q_ASSERT(m_model);
}

class TableSource::Private
{
public:
    Private(TableSource *parent);
    ~Private();

    /**
     * Called if a column in the SAM (Sheet Access Model) is changed that
     * previously was invalid.
     * If both a table name and a valid model pointer are found, the table
     * is added and the column is removed from the empty column list.
     */
    void updateEmptySamColumn(int col);

    /// Pointer to owner of this Private instance
    TableSource *const q;

    QAbstractItemModel *sheetAccessModel;

    /// A list of columns in the sheetAccessModel with an empty table name.
    /// Once the name is changed to something non-empty, we'll add the table
    /// in this column. This is needed in case first the column is inserted,
    /// and then the data is set, e.g. when a QStandardItemModel is used.
    QList<int> samEmptyColumns;

    /// All tables, with name as unique identifier
    QMap<QString, Table *> tablesByName;
    /// Redundant (but complete!) list of tables, now with model as UID
    QMap<const QAbstractItemModel *, Table *> tablesByModel;

    /// Set of Table instances owned by this TableSource.
    /// This isn't equivalent to Tables in tablesByName or tablesByModel
    /// as a Table isn't deleted when it is removed from this source (the
    /// model pointer is just set to null).
    QSet<Table *> tables;
};

TableSource::Private::Private(TableSource *parent)
    : q(parent)
    , sheetAccessModel(nullptr)
{
}

TableSource::Private::~Private()
{
    qDeleteAll(tablesByName.values());
}

/**
 * Retrieves and returns the model of a sheet in @a sheetAccessModel in column @a col
 */
static QAbstractItemModel *getModel(QAbstractItemModel *sheetAccessModel, int col)
{
    QModelIndex tableIndex = sheetAccessModel->index(0, col);
    QPointer<QAbstractItemModel> table = sheetAccessModel->data(tableIndex).value<QPointer<QAbstractItemModel>>();

    return table.data();
}

void TableSource::Private::updateEmptySamColumn(int col)
{
    // Check for consistency
    Q_ASSERT(samEmptyColumns.contains(col));

    QString tableName = sheetAccessModel->headerData(col, Qt::Horizontal).toString();
    QAbstractItemModel *model = getModel(sheetAccessModel, col);
    if (tableName.isEmpty() || model == nullptr)
        return;

    // Ok. Column is valid now. Add table in this column.
    samEmptyColumns.removeAll(col);
    q->add(tableName, model);
}

TableSource::TableSource()
    : d(new Private(this))
{
}

TableSource::~TableSource()
{
    delete d;
}

Table *TableSource::get(const QString &tableName) const
{
    if (!d->tablesByName.contains(tableName))
        return nullptr;
    return d->tablesByName[tableName];
}

Table *TableSource::get(const QAbstractItemModel *model) const
{
    if (!d->tablesByModel.contains(model))
        return nullptr;
    return d->tablesByModel[model];
}

TableMap TableSource::tableMap() const
{
    return d->tablesByName;
}

void TableSource::setSheetAccessModel(QAbstractItemModel *model)
{
    // Disconnect slots from signals in old sheetAccessModel
    if (d->sheetAccessModel)
        d->sheetAccessModel->disconnect(this);

    d->sheetAccessModel = model;

    if (model) {
        connect(model, &QAbstractItemModel::columnsInserted, this, &TableSource::samColumnsInserted);
        connect(model, &QAbstractItemModel::columnsAboutToBeRemoved, this, &TableSource::samColumnsRemoved);
        connect(model, &QAbstractItemModel::headerDataChanged, this, &TableSource::samHeaderDataChanged);

        // Process existing data
        samColumnsInserted(QModelIndex(), 0, model->columnCount() - 1);
    }
}

Table *TableSource::add(const QString &name, QAbstractItemModel *model)
{
    Q_ASSERT(!d->tablesByName.contains(name));
    Q_ASSERT(!d->tablesByModel.contains(model));

    Table *table = new Table(name, model);
    d->tablesByName.insert(name, table);
    d->tablesByModel.insert(model, table);
    d->tables.insert(table);

    Q_EMIT tableAdded(table);

    return table;
}

void TableSource::remove(const QString &name)
{
    Q_ASSERT(d->tablesByName.contains(name));

    Table *table = get(name);
    if (table) {
        d->tablesByName.remove(table->m_name);
        d->tablesByModel.remove(table->m_model);
        d->tables.remove(table);
        Q_EMIT tableRemoved(table);
        // Don't delete the Table instance, it might still be in use.
        table->m_model = nullptr;
    }
}

void TableSource::rename(const QString &from, const QString &to)
{
    Q_ASSERT(!d->tablesByName.contains(to));

    Table *table = get(from);
    if (table) {
        d->tablesByName.remove(from);
        d->tablesByName.insert(to, table);
        table->m_name = to;
    }
}

void TableSource::clear()
{
    d->tablesByName.clear();
    d->tablesByModel.clear();
    setSheetAccessModel(nullptr);
}

void TableSource::samColumnsInserted(QModelIndex, int first, int last)
{
    Q_ASSERT(d->sheetAccessModel);

    for (int col = first; col <= last; col++) {
        QString tableName = d->sheetAccessModel->headerData(col, Qt::Horizontal).toString();
        QAbstractItemModel *model = getModel(d->sheetAccessModel, col);
        if (tableName.isEmpty() || model == nullptr)
            d->samEmptyColumns.append(col);
        else
            add(tableName, getModel(d->sheetAccessModel, col));
    }
}

void TableSource::samColumnsRemoved(QModelIndex, int first, int last)
{
    Q_ASSERT(d->sheetAccessModel);

    for (int col = first; col <= last; col++) {
        QString tableName = d->sheetAccessModel->headerData(col, Qt::Horizontal).toString();
        remove(tableName);
    }
}

void TableSource::samDataChanged(const QModelIndex &first, const QModelIndex &last)
{
    // Only the first row contains useful information for us
    if (first.row() != 0)
        return;

    for (int col = first.column(); col <= last.column(); col++) {
        // If this column wasn't valid before check if it is now and update
        if (d->samEmptyColumns.contains(col))
            d->updateEmptySamColumn(col);
        else
            Q_ASSERT("Changing the model of an existing table is not supported!");
    }
}

void TableSource::samHeaderDataChanged(Qt::Orientation orientation, int first, int last)
{
    // There's no useful information for us in vertical headers
    if (orientation == Qt::Vertical)
        return;

    for (int col = first; col <= last; col++) {
        // If this column wasn't valid before check if it is now and update
        if (d->samEmptyColumns.contains(col)) {
            d->updateEmptySamColumn(col);
            continue;
        }

        QAbstractItemModel *model = getModel(d->sheetAccessModel, col);
        Q_ASSERT(model);
        Table *table = get(model);
        Q_ASSERT(table);
        QString newName = d->sheetAccessModel->headerData(col, Qt::Horizontal).toString();
        rename(table->m_name, newName);
    }
}
