/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1999 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef GNUMERICIMPORT_H
#define GNUMERICIMPORT_H

#include <KoFilter.h>

#include <QDate>
#include <QVariantList>

class QDomElement;
class QDomNode;

namespace Calligra
{
namespace Sheets
{
class Cell;
class Sheet;
}
}

class GNUMERICFilter : public KoFilter
{
    Q_OBJECT
public:
    GNUMERICFilter(QObject *parent, const QVariantList &);
    ~GNUMERICFilter() override = default;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;

    enum borderStyle { Left, Right, Top, Bottom, Diagonal, Revdiagonal };

private:
    class GnumericDate : public QDate
    {
    public:
        static uint greg2jul(int y, int m, int d);
        static void jul2greg(double num, int &y, int &m, int &d);
        static QTime getTime(double num);
    };

    void dateInit();
    QString convertVars(QString const &str, Calligra::Sheets::Sheet *table) const;
    void ParsePrintInfo(QDomNode const &printInfo, Calligra::Sheets::Sheet *table);
    void ParseFormat(QString const &formatString, const Calligra::Sheets::Cell &kspread_cell);
    void setStyleInfo(QDomNode *sheet, Calligra::Sheets::Sheet *table);
    bool setType(const Calligra::Sheets::Cell &kspread_cell, QString const &formatString, QString &cell_content);
    void convertFormula(QString &formula) const;
    void importBorder(QDomElement border, borderStyle _style, const Calligra::Sheets::Cell &cell);
    void ParseBorder(QDomElement &gmr_styleborder, const Calligra::Sheets::Cell &kspread_cell);
    double parseAttribute(const QDomElement &_element);

    void setText(Calligra::Sheets::Sheet *sheet, int row, int column, const QString &text, bool asString = false);
};
#endif // GNUMERICIMPORT_H
