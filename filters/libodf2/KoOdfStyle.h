/* This file is part of the KDE project
 *
 * Copyright (C) 2013 Inge Wallin <inge@lysator.liu.se>
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


#ifndef KOODF_STYLE_H
#define KOODF_STYLE_H

#include "KoOdfStyleBase.h"
#include "koodf2_export.h"

#include <QHash>

class QString;
class KoXmlStreamReader;
class KoOdfStyleProperties;
class KoXmlWriter;


class KOODF2_EXPORT KoOdfStyle : public KoOdfStyleBase
{
 public:
    KoOdfStyle();
    ~KoOdfStyle() override;

    QString family() const;
    void setFamily(const QString &family);
    QString parent() const;
    void setParent(const QString &parent);

    /**
     * @brief Return all property sets in the style.
     */
    QHash<QString, KoOdfStyleProperties*> properties() const;

    /**
     * @brief Return the list of properties in the selected property set.
     * @param name name of the property set.  Example: "text-properties" or "paragraph-properties"
     */
    KoOdfStyleProperties *properties(const QString &name) const;

    QString property(const QString &propertySet, const QString &property) const;
    void    setProperty(const QString &propertySet, const QString &property, const QString &value);

    // Reimplemented from KoOdfStyleBase
    bool readOdf(KoXmlStreamReader &reader) override;
    bool saveOdf(KoXmlWriter *writer) override;

 private:
    class Private;
    Private * const d;
};


#endif
