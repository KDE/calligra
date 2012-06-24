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

#include <QObject>
#include <QSqlDatabase>
#include <QMap>

class QSqlDatabase;
class QSqlTableModel;
class QSqlError;
class QDir;
class QSortFilterProxyModel;

class KoInlineCite;

class BibliographyDb : public QObject
{
    Q_OBJECT
public:
    explicit BibliographyDb(QObject *parent = 0, const QString &path = QString(), const QString &dbName = QString());
    ~BibliographyDb();
    bool openDb();
    bool deleteDb();
    void closeDb();
    QSqlError lastError() const;
    bool isValid() const;
    bool createTable();
    QSqlTableModel* tableModel();
    QSortFilterProxyModel* proxyModel();
    void setSearchFilter(QRegExp);
    void setFilter(QString filter);
    bool insertCitation(KoInlineCite *);
    QMap<QString, KoInlineCite *> citationRecords();

    static QSqlRecord sqlRecord(const KoInlineCite* cite);
    static const QList<QString> dbFields;

private:
    QSqlTableModel *m_model;
    QSortFilterProxyModel *m_filterModel;
    QSqlDatabase m_db;
    QString m_dbName;
    QString m_fullPath;
    bool m_valid;
};

#endif // BIBLIOGRAPHYDB_H
