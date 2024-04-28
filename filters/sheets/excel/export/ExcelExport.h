/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef EXCELEXPORT_H
#define EXCELEXPORT_H

#include <QHash>
#include <QList>
#include <QString>

#include <KoFilter.h>
#include <KoStore.h>

#include <QVariantList>
#include <records.h>

namespace Calligra
{
namespace Sheets
{
class Sheet;
class Tokens;
}
}

namespace Swinder
{
class SSTRecord;
class FormulaToken;
}

class ExcelExport : public KoFilter
{
    Q_OBJECT

public:
    ExcelExport(QObject *parent, const QVariantList &);
    ~ExcelExport() override;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;

    QList<Swinder::FormulaToken> compileFormula(const Calligra::Sheets::Tokens &tokens, Calligra::Sheets::Sheet *sheet) const;

    void convertSheet(Calligra::Sheets::Sheet *sheet, const QHash<QString, unsigned> &sst);
    void buildStringTable(Calligra::Sheets::Sheet *sheet, Swinder::SSTRecord &sst, QHash<QString, unsigned> &stringTable);
    void collectStyles(Calligra::Sheets::Sheet *sheet, QList<Swinder::XFRecord> &xfRecords, QHash<QPair<QFont, QColor>, unsigned> &fontMap);

private:
    class Private;
    Private *d;
};

#endif // EXCELEXPORT_H
