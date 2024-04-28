/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOODF_PARAGRAPH_PROPERTIES_H
#define KOODF_PARAGRAPH_PROPERTIES_H

#include "KoOdfStyleProperties.h"

#include "koodf2_export.h"

class QString;
class KoXmlStreamReader;
class KoXmlWriter;

struct KoOdfStyleDropCap {
    AttributeSet attributes;
};

struct KoOdfStyleTabStop {
    AttributeSet attributes;
};

class KOODF2_EXPORT KoOdfParagraphProperties : public KoOdfStyleProperties
{
public:
    KoOdfParagraphProperties();
    ~KoOdfParagraphProperties() override;

    KoOdfStyleDropCap &dropCap();
    QList<KoOdfStyleTabStop> &tabStops();

    // Inherited from KoOdfStyleProperties
    void clear() override;

    bool readOdf(KoXmlStreamReader &reader) override;
    bool saveOdf(const QString &propertySet, KoXmlWriter *writer) override;

private:
    class Private;
    Private *const d;
};

#endif
