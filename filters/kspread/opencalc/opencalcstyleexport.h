/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres <nandres@web.de>

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


#ifndef OPENCALCSTYLEEXPORT_H
#define OPENCALCSTYLEEXPORT_H

#include <qptrlist.h>
#include <qstring.h>

class QDomDocument;
class QDomElement;

typedef enum T1 { Date, Number, Percentage, Time } NumberType;

class Style
{
 public:
  enum breakBefore { none, automatic, page };

  Style() : breakB( none ), size( 0.0 ) {}

  QString name;
  uint    breakB;
  double  size;
};

class TableStyle
{
 public:
  TableStyle() : visible( true ) {}

  void copyData( TableStyle const & ts ) { visible = ts.visible; }
  static bool isEqual( TableStyle const * const t1, TableStyle const & t2 );

  QString     name;
  bool        visible;
};

class CellStyle
{
 public:
  CellStyle() {}

  QString     name;
};

class NumberStyle
{
 public:
  NumberStyle() {}

  QString     name;
  NumberType  type;
};

class ColumnStyle : public Style 
{
 public:
  ColumnStyle() : Style() {}

  void copyData( ColumnStyle const & cs );
  static bool isEqual( ColumnStyle const * const c1, ColumnStyle const & c2 );
};

class RowStyle : public Style 
{
 public:
  RowStyle() : Style() {}

  void copyData( RowStyle const & cs );
  static bool isEqual( RowStyle const * const c1, RowStyle const & c2 );
};

class OpenCalcStyles
{
 public:
  OpenCalcStyles();
  ~OpenCalcStyles();

  void writeStyles( QDomDocument & doc, QDomElement & autoStyles );

  QString cellStyle( CellStyle const & cs );
  QString columnStyle( ColumnStyle const & cs );
  QString numberStyle( NumberStyle const & ns );
  QString rowStyle( RowStyle const & rs );
  QString tableStyle( TableStyle const & ts );

 private:
  QPtrList<CellStyle>   m_cellStyles;
  QPtrList<ColumnStyle> m_columnStyles;
  QPtrList<NumberStyle> m_numberStyles;
  QPtrList<RowStyle>    m_rowStyles;
  QPtrList<TableStyle>  m_tableStyles;

  void addCellStyles( QDomDocument & doc, QDomElement & autoStyles );
  void addColumnStyles( QDomDocument & doc, QDomElement & autoStyles );
  void addNumberStyles( QDomDocument & doc, QDomElement & autoStyles );
  void addRowStyles( QDomDocument & doc, QDomElement & autoStyles );
  void addTableStyles( QDomDocument & doc, QDomElement & autoStyles );
};

#endif
