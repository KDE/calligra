/* This file is part of the KDE project
   Copyright (C) 2002 Ariya Hidayat <ariyahidayat@yahoo.de>

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

#ifndef __PALMDB_H
#define __PALMDB_H

#include <q3cstring.h>
#include <QDateTime>
#include <q3ptrlist.h>
#include <QString>

class PalmDB
{
  public:

    PalmDB();
    virtual ~PalmDB();

    virtual bool load( const char* filename );
    virtual bool save( const char* filename );

    QString name(){ return m_name; }
    void setName( const QString& n ){ m_name = n; }

    int attributes(){ return m_attributes; }
    void setAttributes( int a ){ m_attributes = a; }

    int version(){ return m_version; }
    void setVersion( int v ){ m_version = v; }

    QDateTime creationDate(){ return m_creationDate; }
    void setCreationDate( QDateTime d ){ m_creationDate = d; }

    QDateTime modificationDate(){ return m_modificationDate; }
    void setModificationDate( QDateTime d ){ m_modificationDate = d; }

    QDateTime lastBackupDate(){ return m_lastBackupDate; }
    void setLastBackupDate( QDateTime d ){ m_lastBackupDate = d; }

    QString type(){ return m_type; }
    QString creator(){ return m_creator; }

    void setType( const QString& t );
    void setCreator( const QString& c );

    int uniqueIDSeed(){ return m_uniqueIDSeed; }
    void setUniqueIDSeed( int i ){ m_uniqueIDSeed= i; }

    Q3PtrList<QByteArray> records;

  private:

    QString m_name;
    int m_attributes, m_version;
    QDateTime m_creationDate;
    QDateTime m_modificationDate;
    QDateTime m_lastBackupDate;
    QString m_type;
    QString m_creator;
    int m_uniqueIDSeed;
};

#endif
