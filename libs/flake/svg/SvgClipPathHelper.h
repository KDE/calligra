/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SVGCLIPPATHHELPER_H
#define SVGCLIPPATHHELPER_H

#include <KoXmlReader.h>

class SvgClipPathHelper
{
public:
    enum Units { UserSpaceOnUse, ObjectBoundingBox };

    SvgClipPathHelper();
    ~SvgClipPathHelper();

    /// Set the clip path units type
    void setClipPathUnits(Units clipPathUnits);
    /// Returns the clip path units type
    Units clipPathUnits() const;

    /// Sets the dom element containing the clip path
    void setContent(const KoXmlElement &content);
    /// Return the clip path element
    KoXmlElement content() const;

private:
    Units m_clipPathUnits;
    KoXmlElement m_content;
};

#endif // SVGCLIPPATHHELPER_H
