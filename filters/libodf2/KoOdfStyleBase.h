/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013-2014 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
    Private *const d;
};

#endif
