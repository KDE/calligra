/* This file is part of the KDE project
 * Copyright (C) 2012 Smit Patel <smitpatel24@gmail.com>
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

#ifndef BIBLIOGRAPHYDB_H
#define BIBLIOGRAPHYDB_H

#include "kotext_export.h"

#include <db/connection.h>

#include <QObject>
#include <QSqlDatabase>
#include <QMap>

class QSqlDatabase;
class QSqlTableModel;
class QSqlError;
class QDir;
class QSortFilterProxyModel;
class BibliographyTableModel;
class BibDbFilter;

class KoInlineCite;

class KOTEXT_EXPORT BibliographyDb : public QObject
{
    Q_OBJECT
public:
    explicit BibliographyDb(QObject *parent = 0, const QString &path = QString(), const QString &dbName = QString());
    ~BibliographyDb();

    QString getDbPath() const;
    bool connect();
    bool deleteDb();
    void closeDb();
    QString lastError() const;
    bool isValid() const;
    bool isLastErrorValid() const;
    int rowCount() const;
    void removeRow(int index);
    void submitAll();
    bool createTable();
    BibliographyTableModel *tableModel();
    QSortFilterProxyModel* proxyModel();
    void setSearchFilter(QRegExp);
    void setFilter(QList<BibDbFilter *> *filters);
    bool insertCitation(KoInlineCite *);
    QMap<QString, KoInlineCite *> citationRecords();

    static const QList<QString> dbFields;
    static const QDir tableDir;
    static KexiDB::DriverManager manager;

private:
    BibliographyTableModel *m_model;
    QSortFilterProxyModel *m_filterModel;
    QString m_dbName;
    QString m_fullPath;
    bool m_valid;

    KexiDB::Driver *m_driver;
    KexiDB::ConnectionData m_cdata;
    KexiDB::Connection *m_conn;
    KexiDB::TableSchema *m_schema;

    void init();
};

#endif // BIBLIOGRAPHYDB_H
