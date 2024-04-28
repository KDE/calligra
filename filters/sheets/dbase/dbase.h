/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Thomas Franke and Andreas Pietzowski <andreas@pietzowski.de>
                         Ariya Hidayat <ariyahidayat@yahoo.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DBASE_H
#define DBASE_H

#include <QDataStream>
#include <QDateTime>
#include <QFile>
#include <QList>
#include <QString>
#include <QStringList>

class DBaseField
{
public:
    QString name;
    enum { Unknown, Character, Date, Numeric, Logical, Memo } type;
    unsigned length;
    unsigned decimals;
};

class DBase
{
public:
    DBase();
    ~DBase();

    QList<DBaseField *> fields;

    bool load(const QString &filename);
    QStringList readRecord(unsigned recno);
    void close();

    unsigned recordCount()
    {
        return m_recordCount;
    }
    int version()
    {
        return m_version;
    }
    QDate lastUpdate()
    {
        return m_lastUpdate;
    }

private:
    QFile m_file;
    QDataStream m_stream;
    int m_version;
    QDate m_lastUpdate;
    unsigned m_recordCount;
    unsigned m_headerLength;
    unsigned m_recordLength;
};

#endif
