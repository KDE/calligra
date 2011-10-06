/* This file is part of the KDE project
   Copyright (C) 2001, 2006 David Faure <faure@kde.org>

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

#ifndef ODP2ODT_H
#define ODP2ODT_H

#include <KoFilter.h>
#include <qdom.h>

// odp2odt means: OASIS OpenDocument Presentation to Text
class Odp2Odt : public KoFilter
{

    Q_OBJECT

public:
    Odp2Odt(QObject* parent, const QStringList&);

    virtual ~Odp2Odt() {}

    virtual KoFilter::ConversionStatus convert(const QByteArray& from, const QByteArray& to);

protected:
    void fixBodyTagName(QDomElement& body);
    void processPages(QDomElement& body);
    void copyFile(const QString& fileName);
    void adjustStyles();
    void fixPageLayout(QDomElement& docElem);
    void addSlideTitleStyle(QDomElement& docElem);
    QDomDocument doc;
    //QDomElement frameset;
    //QString titleStyleName;
    //QString titleFont;
};
#endif // ODP2ODT_H
