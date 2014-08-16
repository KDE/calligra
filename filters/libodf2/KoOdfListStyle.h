/* This file is part of the KDE project
  *
  * Copyright (C) 2013 Inge Wallin <inge@lysator.liu.se>
  * Copyright (C) 2013 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
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

#ifndef KOODFLISTSTYLE_H
#define KOODFLISTSTYLE_H

#include "koodf2_export.h"

#include <QHash>

class QString;
class KoXmlStreamReader;
class KoOdfStyleProperties;
class KoXmlWriter;

class KOODF2_EXPORT KoOdfListStyle
{
public:
    KoOdfListStyle();
    ~KoOdfListStyle();

    QString name() const;
    void setName(QString &name);
    QString displayName() const;
    void setDisplayName(QString &name);
    QString listLevelStyleType() const;
    void setListLevelStyleType(QString &name);

    bool inUse() const;
    void setInUse(bool inUse);

    QHash<QString, KoOdfStyleProperties*> properties();
    /**
      * @brief Return the list of properties in the selected property set.
      * @param name name of the property set.  Example: "text-properties" or "paragraph-properties"
      */
    KoOdfStyleProperties *properties(QString &name) const;

    QString property(QString &propertySet, QString &property) const;
    void    setProperty(QString &propertySet, QString &property, QString &value);

    bool readProperties(KoXmlStreamReader &reader);

    bool readOdf(KoXmlStreamReader &reader);
    bool saveOdf(KoXmlWriter *writer);

private:
    class Private;
    Private * const d;
};

#endif // KOODFLISTSTYLE_H
