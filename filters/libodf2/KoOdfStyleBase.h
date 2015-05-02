/* This file is part of the KDE project
 *
 * Copyright (C) 2013-2014 Inge Wallin <inge@lysator.liu.se>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */


#ifndef KOODF_STYLE_BASE_H
#define KOODF_STYLE_BASE_H

#include "koodf2_export.h"

#include <QHash>

class QString;
class KoXmlStreamReader;
class KoXmlWriter;


class KOODF2_EXPORT KoOdfStyleBase
{
 public:
    enum StyleType {
        StyleStyle,
        PageLayout
        // ... more here
    };

    explicit KoOdfStyleBase(StyleType type);
    virtual ~KoOdfStyleBase();

    StyleType type() const;

    QString name() const;
    void setName(const QString &name);
    QString displayName() const;
    void setDisplayName(const QString &name);

    bool isDefaultStyle() const;
    void setIsDefaultStyle(bool isDefaultStyle);

    bool inUse() const;
    void setInUse(bool inUse);

    bool isFromStylesXml() const;
    void setIsFromStylesXml(bool isFromStylesXml);

    //
    virtual bool readOdf(KoXmlStreamReader &reader) = 0;
    virtual bool saveOdf(KoXmlWriter *writer) = 0;

 private:
    class Private;
    Private * const d;
};


#endif
