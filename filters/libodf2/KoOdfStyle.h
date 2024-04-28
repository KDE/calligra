/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
    QHash<QString, KoOdfStyleProperties *> properties() const;

    /**
     * @brief Return the list of properties in the selected property set.
     * @param name name of the property set.  Example: "text-properties" or "paragraph-properties"
     */
    KoOdfStyleProperties *properties(const QString &name) const;

    QString property(const QString &propertySet, const QString &property) const;
    void setProperty(const QString &propertySet, const QString &property, const QString &value);

    // Reimplemented from KoOdfStyleBase
    bool readOdf(KoXmlStreamReader &reader) override;
    bool saveOdf(KoXmlWriter *writer) override;

private:
    class Private;
    Private *const d;
};

#endif
