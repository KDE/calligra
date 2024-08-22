/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2004 Nicolas GOUTTE <goutte@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CSVEXPORT_H
#define CSVEXPORT_H

#include <KoFilter.h>
#include <QByteArray>
#include <QLoggingCategory>
#include <QVariantList>

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
    CSVExport(QObject *parent, const QVariantList &);
    ~CSVExport() override = default;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;

private:
    QString exportCSVCell(const Calligra::Sheets::Doc *doc, Calligra::Sheets::Sheet *sheet, int col, int row, QChar const &textQuote, QChar csvDelimiter);

private:
    QString m_eol; ///< End of line (LF, CR or CRLF)
};

#endif // CSVEXPORT_H
