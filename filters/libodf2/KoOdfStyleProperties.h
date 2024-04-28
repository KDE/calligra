/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOODF_STYLE_PROPERTIES_H
#define KOODF_STYLE_PROPERTIES_H

// Qt
#include <QHash>
#include <QString>

#include "KoXmlStreamReader.h"
#include "koodf2_export.h"

class QString;
class KoXmlWriter;

typedef QHash<QString, QString> AttributeSet; // name, value

class KOODF2_EXPORT KoOdfStyleProperties
{
public:
    KoOdfStyleProperties();
    virtual ~KoOdfStyleProperties();

    QString attribute(const QString &property) const;
    void setAttribute(const QString &property, const QString &value);

    virtual void clear();

    virtual bool readOdf(KoXmlStreamReader &reader);
    virtual bool saveOdf(const QString &propertySet, KoXmlWriter *writer);

    void copyPropertiesFrom(const KoOdfStyleProperties &sourceProperties);

protected:
    /// Read all attributes from the XML element.
    /// This function is normally called from readOdf().
    bool readAttributes(KoXmlStreamReader &reader);
    bool saveAttributes(KoXmlWriter *writer);

private:
    class Private;
    Private *const d;
};

void copyAttributes(KoXmlStreamReader &reader, AttributeSet &attributes);
void saveAttributes(AttributeSet &attributes, KoXmlWriter *writer);

#endif
