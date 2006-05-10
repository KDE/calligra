/* This file is part of the KDE project
   Copyright (C) 2000 David Faure <faure@kde.org>
   Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>

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

#ifndef CSVEXPORT_H
#define CSVEXPORT_H

#include <QString>
//Added by qt3to4:
#include <Q3CString>
#include <KoFilter.h>

namespace KSpread
{
class Sheet;
}

class CSVExport : public KoFilter
{
  Q_OBJECT

  public:
  CSVExport(QObject* parent, const QStringList &);
  virtual ~CSVExport() {}

  virtual KoFilter::ConversionStatus convert( const QByteArray & from, const QByteArray & to );

  private:
  QString exportCSVCell( KSpread::Sheet const * const sheet, int col, int row, QChar const & textQuote );

  private:
  QString m_eol; ///< End of line (LF, CR or CRLF)
};

#endif // CSVEXPORT_H
