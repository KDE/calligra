/* This file is part of the KDE project
   Copyright (C) 2000 David Faure <faure@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef GNUMERICEXPORT_TEST_H
#define GNUMERICEXPORT_TEST_H

#include <koFilter.h>
#include <qdom.h>

class KSpreadCell;

class GNUMERICExport : public KoFilter {

    Q_OBJECT

public:
    GNUMERICExport(KoFilter *parent, const char*name, const QStringList&);
    virtual ~GNUMERICExport() {}

    virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );

private:
    QDomElement GetCellStyle(QDomDocument gnumeric_doc, KSpreadCell * cell, int currentcolumn, int currentrow);
    QDomElement GetBorderStyle(QDomDocument gnumeric_doc, KSpreadCell * cell, int currentcolumn, int currentrow);
    QDomElement GetFontStyle(QDomDocument gnumeric_doc, KSpreadCell * cell, int currentcolumn, int currentrow);
    QDomElement GetLinkStyle(QDomDocument gnumeric_doc );
    QDomElement GetValidity( QDomDocument gnumeric_doc, KSpreadCell * cell );

    void addAttributeItem(QDomDocument gnumeric_doc, QDomElement attributes, const QString& type, const QString& name, bool value);
    void addSummaryItem(QDomDocument gnumeric_doc, QDomElement summary, const QString& name, const QString& value);
    bool hasBorder(KSpreadCell *cell, int currentcolumn, int currentrow);
    const QString ColorToString(int red, int green, int blue);
    QString convertVariable( QString headerFooter );
    QString convertRefToRange( const QString & table, const QRect & rect );
    QString convertRefToBase( const QString & table, const QRect & rect );
    bool isLink;
    QString linkText;
    QString linkUrl;
    bool isLinkBold;
    bool isLinkItalic;

};
#endif

