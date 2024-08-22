/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef GNUMERICEXPORT_TEST_H
#define GNUMERICEXPORT_TEST_H

#include <KoFilter.h>
#include <QByteArray>
#include <QDomDocument>
#include <QVariantList>

namespace Calligra
{
namespace Sheets
{
class Cell;
}
}

class QRect;

class GNUMERICExport : public KoFilter
{
    Q_OBJECT

public:
    GNUMERICExport(QObject *parent, const QVariantList &);
    ~GNUMERICExport() override = default;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;

private:
    QDomElement GetCellStyle(QDomDocument gnumeric_doc, const Calligra::Sheets::Cell &cell, int currentcolumn, int currentrow);
    QDomElement GetFontStyle(QDomDocument gnumeric_doc, const Calligra::Sheets::Cell &cell, int currentcolumn, int currentrow);
    QDomElement GetLinkStyle(QDomDocument gnumeric_doc);
    QDomElement GetValidity(QDomDocument gnumeric_doc, const Calligra::Sheets::Cell &cell);

    void addAttributeItem(QDomDocument gnumeric_doc, QDomElement attributes, const QString &type, const QString &name, bool value);
    void addSummaryItem(QDomDocument gnumeric_doc, QDomElement summary, const QString &name, const QString &value);
    bool hasBorder(const Calligra::Sheets::Cell &cell, int currentcolumn, int currentrow);
    QString convertVariable(QString headerFooter);
    QString convertRefToRange(const QString &table, const QRect &rect);
    QString convertRefToBase(const QString &table, const QRect &rect);
    bool isLink;
    QString linkText;
    QString linkUrl;
    bool isLinkBold;
    bool isLinkItalic;
};
#endif
