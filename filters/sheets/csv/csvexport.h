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
#include <QByteArray>
#include <KoFilter.h>
#include <QVariantList>
#include <QDebug>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(lcCsvExport)

namespace Calligra
{
namespace Sheets
{
class Sheet;
class Doc;
}
}

class CSVExport : public KoFilter
{
    Q_OBJECT

public:
    CSVExport(QObject* parent, const QVariantList &);
    ~CSVExport() override {}

    KoFilter::ConversionStatus convert(const QByteArray & from, const QByteArray & to) override;

private:
    QString exportCSVCell(const Calligra::Sheets::Doc* doc, Calligra::Sheets::Sheet const * const sheet,
                          int col, int row, QChar const & textQuote, QChar csvDelimiter);

private:
    QString m_eol; ///< End of line (LF, CR or CRLF)
};

#endif // CSVEXPORT_H
