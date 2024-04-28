/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOODF_STYLE_MANAGER_H
#define KOODF_STYLE_MANAGER_H

#include "koodf2_export.h"

#include <QList>

class QString;
class KoStore;
class KoOdfStyle;
class KoOdfListStyle;
class KoXmlStreamReader;
class KoXmlWriter;

class KOODF2_EXPORT KoOdfStyleManager
{
public:
    KoOdfStyleManager();
    ~KoOdfStyleManager();

    KoOdfStyle *style(const QString &name, const QString &family) const;
    void setStyle(const QString &name, KoOdfStyle *style);

    KoOdfListStyle *listStyle(const QString &name) const;
    void setListStyle(const QString &name, KoOdfListStyle *listStyle);

    KoOdfStyle *defaultStyle(const QString &family) const;
    void setDefaultStyle(const QString &family, KoOdfStyle *style);

    void clear();

    bool loadStyles(KoStore *odfStore);
    bool saveNamedStyles(KoXmlWriter *writer);

    QList<KoOdfStyle *> styles() const;
    QList<KoOdfStyle *> defaultStyles() const;

private:
    // FIXME: Move to private class.
    void collectStyleSet(KoXmlStreamReader &reader, bool fromStylesXml);

private:
    class Private;
    Private *const d;
};

#endif
