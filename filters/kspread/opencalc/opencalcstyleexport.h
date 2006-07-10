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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#ifndef OPENCALCSTYLEEXPORT_H
#define OPENCALCSTYLEEXPORT_H

#include "kspread/Format.h"

#include <QColor>
#include <QFont>
#include <q3ptrlist.h>
#include <QString>

namespace KSpread
{
  class Cell;
}

class QDomDocument;
class QDomElement;

typedef enum T1 { Boolean, Date, Number, Percentage, Time } NumberType;

class Style
{
 public:
  enum breakBefore { none, automatic, page };

  Style() : breakB( none ), size( 0.0 ) {}

  QString name;
  uint    breakB;
  double  size;
};

class SheetStyle
{
 public:
  SheetStyle() : visible( true ) {}

  void copyData( SheetStyle const & ts ) { visible = ts.visible; }
  static bool isEqual( SheetStyle const * const t1, SheetStyle const & t2 );

  QString     name;
  bool        visible;
};

class NumberStyle
{
 public:
  NumberStyle() {}

  void copyData( NumberStyle const & ts ) { type = ts.type; }
  static bool isEqual( NumberStyle const * const t1, NumberStyle const & t2 );

  QString     name;

  NumberType  type;
  QString     pattern;
};

class CellStyle
{
 public:
  CellStyle();

  void copyData( CellStyle const & ts );
  static bool isEqual( CellStyle const * const t1, CellStyle const & t2 );

  // all except the number style
  static void loadData( CellStyle & cs, KSpread::Cell const * const cell );

  QString     name;

  QFont       font;
  QString     numberStyle;
  QColor      color;
  QColor      bgColor;
  double      indent;
  bool        wrap;
  bool        vertical;
  int         angle;
  bool        print;
  QPen        left;
  QPen        right;
  QPen        top;
  QPen        bottom;
  bool        hideAll;
  bool        hideFormula;
  bool        notProtected;

  KSpread::Style::HAlign  alignX;
  KSpread::Style::VAlign  alignY;
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

  void    writeStyles  ( QDomDocument & doc, QDomElement & autoStyles );
  void    writeFontDecl( QDomDocument & doc, QDomElement & content );

  void    addFont( QFont const & font, bool def = false );

  QString cellStyle( CellStyle const & cs );
  QString columnStyle( ColumnStyle const & cs );
  QString numberStyle( NumberStyle const & ns );
  QString rowStyle( RowStyle const & rs );
  QString sheetStyle( SheetStyle const & ts );

 private:
  Q3PtrList<CellStyle>   m_cellStyles;
  Q3PtrList<ColumnStyle> m_columnStyles;
  Q3PtrList<NumberStyle> m_numberStyles;
  Q3PtrList<RowStyle>    m_rowStyles;
  Q3PtrList<SheetStyle>  m_sheetStyles;
  Q3PtrList<QFont>       m_fontList;

  QFont                 m_defaultFont;

  void addCellStyles( QDomDocument & doc, QDomElement & autoStyles );
  void addColumnStyles( QDomDocument & doc, QDomElement & autoStyles );
  void addNumberStyles( QDomDocument & doc, QDomElement & autoStyles );
  void addRowStyles( QDomDocument & doc, QDomElement & autoStyles );
  void addSheetStyles( QDomDocument & doc, QDomElement & autoStyles );
};



#endif
