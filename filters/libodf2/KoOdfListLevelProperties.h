/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>
 * SPDX-FileCopyrightText: 2013 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOODFLISTLEVELPROPERTIES_H
#define KOODFLISTLEVELPROPERTIES_H

#include "KoOdfStyleProperties.h"

#include "koodf2_export.h"

class QString;
class KoXmlStreamReader;
class KoXmlWriter;

typedef QHash<QString, QString> AttributeSet; // name, value

class KOODF2_EXPORT KoOdfListLevelProperties : public KoOdfStyleProperties
{
public:
    KoOdfListLevelProperties();
    ~KoOdfListLevelProperties() override;

    // Inherited from KoOdfStyleProperties
    void clear() override;

    bool readOdf(KoXmlStreamReader &reader) override;
    bool saveOdf(const QString &propertySet, KoXmlWriter *writer) override;

private:
    class Private;
    Private *const d;
};

#endif // KOODFLISTLEVELPROPERTIES_H
