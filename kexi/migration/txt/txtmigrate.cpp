/* This file is part of the KDE project
Copyright (C) 2004-2009 Adam Pigg <adam@piggz.co.uk>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/

#include "txtmigrate.h"
#include <KDebug>
#include <QDir>

namespace KexiMigration
{

K_EXPORT_KEXIMIGRATE_DRIVER(TxtMigrate, "txt")


TxtMigrate::TxtMigrate(QObject *parent, const QVariantList& args)
        : KexiMigrate(parent, args)
{
  m_DataFile = 0;
}


TxtMigrate::~TxtMigrate()
{
}

bool TxtMigrate::drv_connect()
{
  QDir d;

  m_Folder = m_migrateData->source->dbPath();
  return d.exists(m_Folder);
}

bool TxtMigrate::drv_disconnect()
{
  if (m_DataFile) {
    delete m_DataFile;
    m_DataFile = 0;
  }
  
  return true;
}

bool TxtMigrate::drv_tableNames(QStringList& tablenames)
{
  tablenames << "Table1";
  return true;
}

bool TxtMigrate::drv_readTableSchema(const QString& originalName, KexiDB::TableSchema& tableSchema)
{
  if (drv_readFromTable(originalName))
  {
    for (uint i = 0; i < (uint)m_FieldNames.count(); ++i)
    {
      tableSchema.addField( new KexiDB::Field(m_FieldNames[i], KexiDB::Field::Text) );
    }
    return true;
  }
  return false;
}

bool TxtMigrate::drv_readFromTable(const QString & tableName)
{
  if (m_DataFile) {
    delete m_DataFile;
    m_DataFile = 0;
  }
  
  m_DataFile = new QFile(m_Folder + "/" + tableName);

  kDebug() << m_DataFile->fileName();
  m_Row = -1;
  m_FileRow = -1;

  if (!m_DataFile->open(QIODevice::ReadOnly | QIODevice::Text))
         return false;

  m_LastLine = m_DataFile->readLine();
  m_FieldNames = m_LastLine.split("\t");

  return true;
}

bool TxtMigrate::drv_moveNext()
{
  if (m_Row < m_FileRow)
  {
   m_Row++; 
  }
  else
  {
    if (m_DataFile->atEnd())
      return false;

    m_LastLine = m_DataFile->readLine();
    m_FieldValues.push_back(m_LastLine.split("\t"));
    m_Row++;
    m_FileRow++;
  }
  return true;
}

bool TxtMigrate::drv_movePrevious()
{
  if (m_Row > 0)
  {
    m_Row--;
    return true;
  }
  return false;
}

QVariant TxtMigrate::drv_value(uint i)
{
  return QVariant(m_FieldValues[m_Row][i]);
}

}
