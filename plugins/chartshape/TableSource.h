/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2010 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_TABLESOURCE_H
#define KCHART_TABLESOURCE_H

// Qt
#include <QAbstractItemModel>
#include <QObject>

// KoChart
#include "ChartShape.h"

class QString;

namespace KoChart
{

class Table
{
    friend class TableSource;

public:
    QAbstractItemModel *model() const
    {
        return m_model;
    }
    QString name() const
    {
        return m_name;
    }

private:
    Table(const QString &name, QAbstractItemModel *model);

    QString m_name;
    QAbstractItemModel *m_model;
};

typedef QMap<QString, Table *> TableMap;

class TableSource : public QObject
{
    Q_OBJECT

public:
    TableSource();
    ~TableSource();

    /**
     * Returns the table (model/name pair) associated with @a tableName.
     *
     * Note: The table name will be updated automatically when changed
     * by some "table source".
     */
    Table *get(const QString &tableName) const;

    /**
     * Returns the table (model/name pair) associated with @a model.
     *
     * Note: The table name will be updated automatically when changed
     * by some "table source".
     */
    Table *get(const QAbstractItemModel *model) const;

    /**
     * Returns a map of all name/table pairs in this source.
     * Mostly for debugging purposes.
     */
    TableMap tableMap() const;

    /**
     * Sets the KSpread::SheetAccessModel instance to use to get notified
     * about added/removed/renamed sheets in Calligra Sheets.
     *
     * This method is only relevant if the chart is embedded in Calligra Sheets or
     * somehow needs access to Calligra Sheets' sheets.
     */
    void setSheetAccessModel(QAbstractItemModel *model);

    /**
     * Adds a named model to this source.
     *
     * @return Pointer to new table (name/model pair) instance
     */
    Table *add(const QString &name, QAbstractItemModel *model);

    /**
     * Makes sure that the name of the specified table always stays unique.
     *
     * Use this for programmatically added tables (like internal chart table).
     * Whenever another table with the same name is added/renamed, the
     * table specified will be renamed (to a sane similar name) to not
     * collide with the new name.
     */
    // TODO
    // void setRenameOnNameClash(const QString &tableName);
    // or
    // void setRenameOnNameClash(Table *table);

    /**
     * Removes a table from this source.
     */
    void remove(const QString &name);

    /**
     * Renames a table that has previously been added.
     */
    void rename(const QString &from, const QString &to);

    /**
     * Removes all tables and the sheetAccessModel.
     *
     * Note that all Table* pointers from this source are invalid after
     * calling this method!
     */
    void clear();

Q_SIGNALS:
    /**
     * Emitted whenever a table is added to this source.
     */
    void tableAdded(KoChart::Table *table);

    /**
     * Emitted whenever a table is removed from this source
     *
     * Note that right after this signal is emitted, the Table* instance
     * is deleted, thus you can't use it anymore afterwards.
     */
    void tableRemoved(KoChart::Table *table);

private Q_SLOTS:
    /**
     * Methods that react on changes in the SheetAccessModel ("sam")
     */
    void samColumnsInserted(QModelIndex, int, int);
    void samColumnsRemoved(QModelIndex, int, int);
    void samDataChanged(const QModelIndex &first, const QModelIndex &last);
    void samHeaderDataChanged(Qt::Orientation, int, int);

private:
    class Private;
    Private *const d;
};

} // namespace KoChart

#endif // KCHART_TABLESOURCE_H
