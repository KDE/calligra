/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Eva Brucherseifer <eva@kde.org>
   SPDX-FileCopyrightText: 2005 Bram Schoenmakers <bramschoenmakers@kde.nl>
   based on kspread csv export filter by David Faure

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef HTMLEXPORT_TEST_H
#define HTMLEXPORT_TEST_H

#include <KoFilter.h>
#include <QByteArray>
#include <QLoggingCategory>
#include <QVariantList>

class ExportDialog;
class KoDocument;

namespace Calligra
{
namespace Sheets
{
class Sheet;
}
}

Q_DECLARE_LOGGING_CATEGORY(lcHtml)

class HTMLExport : public KoFilter
{
    Q_OBJECT
public:
    HTMLExport(QObject *parent, const QVariantList &);
    ~HTMLExport() override;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;

private:
    /** Writes the top of the page in HTML to @par str */
    void openPage(Calligra::Sheets::Sheet *sheet, KoDocument *document, QString &str);

    /** Closes a page in HTML */
    void closePage(QString &);

    /**
      Converts @par sheet to HTML and writes to @par str.
     */
    void convertSheet(Calligra::Sheets::Sheet *sheet, QString &str, int, int);

    /** Writes a bar and a link to the top to @par str. */
    void createSheetSeparator(QString &);

    /** Writes the table of contents */
    void writeTOC(const QStringList &, const QString &, QString &);

    /**
      Returns a filename based on the @par base filename and the options
      defined in the dialog.
    */
    QString fileName(const QString &base, const QString &, bool);

    /**
      Detects which rows and columns of the given @par sheet are used and
      writes the number of them to @par row and @par column.
     */
    void detectFilledCells(Calligra::Sheets::Sheet *sheet, int &rows, int &columns);

private:
    ExportDialog *m_dialog;

    typedef QMap<QString, int> Rows;
    Rows m_rowmap;
    typedef QMap<QString, int> Columns;
    Columns m_columnmap;
};

#endif
