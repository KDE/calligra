/* This file is part of the KDE project
   Copyright (C) 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

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

#ifndef EXCELEXPORT_H
#define EXCELEXPORT_H

#include <QHash>
#include <QList>
#include <QString>

#include <KoFilter.h>
#include <KoStore.h>

#include <records.h>
#include <QVariantList>

namespace Calligra {
namespace Sheets {
    class Sheet;
    class Tokens;
}
}

namespace Swinder {
    class SSTRecord;
    class FormulaToken;
}

class ExcelExport : public KoFilter
{

    Q_OBJECT

public:

    ExcelExport(QObject *parent, const QVariantList&);
    ~ExcelExport() override;

    KoFilter::ConversionStatus convert(const QByteArray& from, const QByteArray& to) override;

    QList<Swinder::FormulaToken> compileFormula(const Calligra::Sheets::Tokens& tokens, Calligra::Sheets::Sheet* sheet) const;

    void convertSheet(Calligra::Sheets::Sheet* sheet, const QHash<QString, unsigned>& sst);
    void buildStringTable(Calligra::Sheets::Sheet* sheet, Swinder::SSTRecord& sst, QHash<QString, unsigned>& stringTable);
    void collectStyles(Calligra::Sheets::Sheet* sheet, QList<Swinder::XFRecord>& xfRecords, QHash<QPair<QFont, QColor>, unsigned>& fontMap);
private:
    class Private;
    Private* d;

};

#endif // EXCELEXPORT_H
