/* This file is part of the KDE project
Copyright (C) 2009 Adam Pigg <adam@piggz.co.uk>

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

#include "kspreadmigrate.h"

namespace KexiMigration
{

K_EXPORT_KEXIMIGRATE_DRIVER(KSpreadMigrate, "kspread")

KSpreadMigrate::KSpreadMigrate(QObject *parent, const QVariantList &args)
        : KexiMigrate(parent, args)
{
  m_CurSheet = 0;
}

KSpreadMigrate::~KSpreadMigrate()
{
}

bool KSpreadMigrate::drv_connect()
{
  m_FileName = m_migrateData->source->dbPath() + "/" + m_migrateData->source->dbFileName();
  
  if (!QFile::exists(m_FileName)) 
    return false;
  
  m_KSDoc = new KSpread::Doc();
  return m_KSDoc->openUrl(m_FileName);
}

bool KSpreadMigrate::drv_disconnect()
{
  if (m_KSDoc) {
    delete m_KSDoc;
    m_KSDoc = 0;
  }
  return true;
}

bool KSpreadMigrate::drv_tableNames(QStringList& tablenames)
{
  QList<KSpread::Sheet*> sheets = m_KSDoc->map()->sheetList();
  
  kDebug() << sheets.size() << "sheets" << m_KSDoc->map()->sheetList().size();
  
  foreach(KSpread::Sheet *sheet, sheets) {
    tablenames << sheet->sheetName();
  }
  
  return true;
}

bool KSpreadMigrate::drv_readTableSchema(const QString& originalName, KexiDB::TableSchema& tableSchema)
{
  KSpread::Sheet *sheet = m_KSDoc->map()->findSheet(originalName);
  
  if (!sheet)
  {
      kDebug() << "unable to find sheet" << originalName;
      return false;
  }
  
  int row=1, col = 1;
  QString fieldname;
  
  KSpread::Cell *cell;
  KexiDB::Field *fld;
  tableSchema.setName(QString(originalName).replace(" ", "_").toLower());
  tableSchema.setCaption(originalName);
  
  do
  {
      cell = new KSpread::Cell(sheet, col, row);

      fieldname = cell->displayText();
      col++;
      if (!cell->isEmpty())
      {
          fld = new KexiDB::Field(fieldname.replace(" ", "_"), KexiDB::Field::Text);
          fld->setCaption(fieldname);
          tableSchema.addField( fld );
          kDebug() << fieldname;
      }
  }while(!cell->isEmpty());
  
  return true;
}

bool KSpreadMigrate::drv_readFromTable(const QString & tableName)
{
  m_CurSheet = m_KSDoc->map()->findSheet(tableName);
  
  m_Row = 1;
  
  return m_CurSheet;
}

bool KSpreadMigrate::drv_moveNext()
{
  if (!m_CurSheet)
    return false;
  
  KSpread::Cell cell = KSpread::Cell(m_CurSheet, 1, m_Row + 1);
  
  if (!cell.isEmpty())
  {
    m_Row++;
    return true;
  }
  
  return false;
}

bool KSpreadMigrate::drv_movePrevious()
{
  if (!m_CurSheet)
    return false;
  
  if (m_Row > 1)
  {
    m_Row--;
    return true;
  }
  return false;
}

bool KSpreadMigrate::drv_moveFirst()
{
  if (!m_CurSheet)
    return false;
  
  m_Row = 1;
  return drv_moveNext();
}

bool KSpreadMigrate::drv_moveLast()
{
  if (!m_CurSheet)
    return false;
  
  while(drv_moveNext()){}
  
  return true;
}

QVariant KSpreadMigrate::drv_value(uint i)
{
  QString str = KSpread::Cell(m_CurSheet, i+1, m_Row).displayText();
  
  return str;
}

}
