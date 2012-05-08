/* This file is part of the KDE project
 * Copyright (C) 2011 Smit Patel <smitpatel24@gmail.com>
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

#include "BibliographyDb.h"

#include <KoOdfBibliographyConfiguration.h>
#include <klocale.h>

#include <QDir>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlTableModel>

BibliographyDb::BibliographyDb(QObject *parent, QString path, QString dbName) :
    QObject(parent),
    m_dbName(dbName),
    m_fullPath(path),
    m_db(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE")))
{
#ifdef Q_OS_LINUX
    m_fullPath.append(QDir::separator()).append(m_dbName);
    m_fullPath = QDir::toNativeSeparators(m_fullPath);
    m_db->setDatabaseName(m_fullPath);
#else
    m_db->setDatabaseName(m_dbName);
#endif
    if (openDb()) {
        m_model = new QSqlTableModel(this, *m_db);
        m_model->setTable("bibref");
        m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        m_model->select();

        /*for (int i = 0; KoOdfBibliographyConfiguration::bibDataFields.size(); i++) {
            m_model->setHeaderData(i, Qt::Horizontal, KoOdfBibliographyConfiguration::bibDataFields.at(i));
        }*/
    }
}

bool BibliographyDb::openDb()
{
    if ( m_db->open() && !m_db->tables(QSql::Tables).contains("bibref")) {
        createTable();
    }
    return m_db->isOpen();
}

bool BibliographyDb::deleteDb()
{
    m_db->close();
#ifdef Q_OS_LINUX
    return QFile::remove(m_fullPath);
#else
    return QFile::remove(m_dbName);
#endif
}

QSqlError BibliographyDb::lastError() const
{
    return m_db->lastError();
}

bool BibliographyDb::createTable()
{
    bool ret = false;
    if (!m_db->isOpen()) {
        m_db->open();
    } else {
        QString query("CREATE TABLE bibref"
                      "(id INTEGER PRIMARY KEY,"
                      "identifier TEXT UNIQUE,"
                      "bibliographic_type TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "address TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "annote TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "author TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "booktitle TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "chapter TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "edition TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "editor TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "howpublished TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "institution TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "journal TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "month TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "note TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "number TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "organizations TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "pages TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "publisher TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "school TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "series TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "title TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "report_type TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "volume TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "year TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "url TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "custom1 TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "custom2 TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "custom3 TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "custom4 TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "custom5 TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "isbn TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '',"
                      "issn TEXT NOT NULL ON CONFLICT REPLACE DEFAULT '')");
        m_db->exec(query);
    }
    return ret;
}

QSqlTableModel* BibliographyDb::tableModel()
{
    return m_model;
}
