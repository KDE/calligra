/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>
 * SPDX-FileCopyrightText: 2013 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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

    QHash<QString, KoOdfStyleProperties *> properties();
    /**
     * @brief Return the list of properties in the selected property set.
     * @param name name of the property set.  Example: "text-properties" or "paragraph-properties"
     */
    KoOdfStyleProperties *properties(QString &name) const;

    QString property(QString &propertySet, QString &property) const;
    void setProperty(QString &propertySet, QString &property, QString &value);

    bool readProperties(KoXmlStreamReader &reader);

    bool readOdf(KoXmlStreamReader &reader);
    bool saveOdf(KoXmlWriter *writer);

private:
    class Private;
    Private *const d;
};

#endif // KOODFLISTSTYLE_H
