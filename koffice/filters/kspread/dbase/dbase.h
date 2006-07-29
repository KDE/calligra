/* This file is part of the KDE project
   Copyright (C) 2002 by Thomas Franke and Andreas Pietzowski <andreas@pietzowski.de>
                         Ariya Hidayat <ariyahidayat@yahoo.de>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef DBASE_H
#define DBASE_H

#include <qdatastream.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qstring.h>
#include <qstringlist.h>

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

    QPtrList<DBaseField> fields;

    bool load( const QString& filename );
    QStringList readRecord( unsigned recno );
    void close();

    unsigned recordCount(){ return m_recordCount; }
    int version(){ return m_version; } 
    QDate lastUpdate(){ return m_lastUpdate; }

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
