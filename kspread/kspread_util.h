/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __kspread_util_h__
#define __kspread_util_h__

#include <qstring.h>
#include <qrect.h>
#include <qdatetime.h>
#include <kspread_cell.h>

class KSpreadMap;
class KSpreadTable;
//class KSpreadCell;
class KLocale;

struct KSpreadPoint
{
public:
  KSpreadPoint() { pos.setX( -1 ); table = 0; columnFixed = false; rowFixed = false; }
  KSpreadPoint( const QString& );
  KSpreadPoint( const QString&, KSpreadMap*, KSpreadTable* default_table = 0 );
  KSpreadPoint( const KSpreadPoint& c ) {
    pos = c.pos;
    table = c.table; tableName = c.tableName;
    columnFixed = c.columnFixed;
    rowFixed = c.rowFixed;
  }

  bool isValid() const { return ( pos.x() >= 0 && ( table != 0 || tableName.isEmpty() ) ); }
  bool isTableKnown() const { return ( !tableName.isEmpty() || table != 0 ); }

  KSpreadCell* cell();

  /*
    TODO
  bool columnFixed() const { return m_columnFixed; }
  bool rowFixed() const { return m_rowFixed; }
  QPoint pos() const { return m_pos; }
  QString tableName() const { return m_tableName; }
  KSpreadTable* table() const { return m_table; }

private:
  */
  KSpreadTable* table;
  QString tableName;
  QPoint pos;
  bool columnFixed;
  bool rowFixed;

private:
  void init( const QString& );
};

struct KSpreadRange
{
  KSpreadRange() { table = 0; range.setLeft( -1 ); }
  KSpreadRange( const QString& );
  KSpreadRange( const QString&, KSpreadMap*, KSpreadTable* default_table = 0 );
  KSpreadRange( const KSpreadRange& r ) {
    table = r.table;
    tableName = r.tableName;
    range = r.range;
  }
  KSpreadRange( const KSpreadPoint& ul, const KSpreadPoint& lr )
  {
    range = QRect( ul.pos, lr.pos );
    if ( ul.tableName != lr.tableName )
    {
      range.setLeft( -1 );
      return;
    }
    tableName = ul.tableName;
    table = ul.table;
    leftFixed = ul.columnFixed;
    rightFixed = lr.columnFixed;
    topFixed = ul.rowFixed;
    bottomFixed = lr.rowFixed;
  }

  bool isValid() const { return ( range.left() >= 0 && range.right() >= 0 && ( table != 0 || tableName.isEmpty() ) ); }
  bool isTableKnown() const { return ( !tableName.isEmpty() || table != 0 ); }

  KSpreadTable* table;
  QString tableName;
  QRect range;
  bool leftFixed;
  bool rightFixed;
  bool topFixed;
  bool bottomFixed;
};

QString util_cellName( KSpreadTable*, int _col, int _row );
QString util_cellName( int _col, int _row );
QString util_rangeName( QRect _area );
QString util_rangeName( KSpreadTable *_table, QRect _area );
QString util_columnLabel( int column );

QString util_dateFormat( KLocale* locale, QDate _date, KSpreadCell::formatNumber _tmpFormat); 

QString util_timeFormat( KLocale* locale, QTime _time, KSpreadCell::formatNumber _tmpFormat); 

QString util_fractionFormat( double value , KSpreadCell::formatNumber _tmpFormat); 

#endif

