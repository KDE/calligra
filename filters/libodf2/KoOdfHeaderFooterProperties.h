/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2014 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOODF_HEADER_FOOTER_PROPERTIES_H
#define KOODF_HEADER_FOOTER_PROPERTIES_H

#include "KoOdfStyleProperties.h"

#include "koodf2_export.h"

class QString;
class KoXmlStreamReader;
class KoXmlWriter;

class KOODF2_EXPORT KoOdfHeaderFooterProperties : public KoOdfStyleProperties
{
public:
    KoOdfHeaderFooterProperties();
    ~KoOdfHeaderFooterProperties() override;

    // Inherited from KoOdfStyleProperties
    void clear() override;

    bool readOdf(KoXmlStreamReader &reader) override;
    bool saveOdf(const QString &propertySet, KoXmlWriter *writer) override;

private:
    class Private;
    Private *const d;
};

#endif
