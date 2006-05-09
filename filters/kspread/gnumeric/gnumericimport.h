/* This file is part of the KDE project
   Copyright (C) 1999 David Faure <faure@kde.org>

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

#ifndef GNUMERICFILTER_H
#define GNUMERICFILTER_H

#include <KoFilter.h>

#include <qdatetime.h>
#include <qdom.h>
//Added by qt3to4:
#include <Q3CString>

namespace KSpread
{
class Cell;
class Sheet;
}

class GNUMERICFilter : public KoFilter
{
    Q_OBJECT
 public:
    GNUMERICFilter(QObject* parent, const QStringList&);
    virtual ~GNUMERICFilter() {}

    virtual KoFilter::ConversionStatus convert( const QByteArray& from, const QByteArray& to );

enum borderStyle { Left, Right, Top, Bottom, Diagonal, Revdiagonal};
 private:
  class GnumericDate : public QDate
  {
   public:
    static uint greg2jul( int y, int m, int d );
    static void jul2greg( double num, int & y, int & m, int & d );
    static QTime getTime( double num );

  };

  void dateInit();
  QString convertVars( QString const & str, KSpread::Sheet * table ) const;
  void ParsePrintInfo( QDomNode const & printInfo, KSpread::Sheet * table );
  void ParseFormat(QString const & formatString, KSpread::Cell* kspread_cell);
  void setStyleInfo(QDomNode * sheet, KSpread::Sheet * table);
  bool setType( KSpread::Cell* kspread_cell, QString const & formatString, QString & cell_content );
  void convertFormula( QString & formula ) const;
    void importBorder( QDomElement  border, borderStyle _style,  KSpread::Cell*cell);
    void ParseBorder( QDomElement & gmr_styleborder, KSpread::Cell* kspread_cell );
    double parseAttribute( const QDomElement &_element );

};
#endif // GNUMERICFILTER_H
