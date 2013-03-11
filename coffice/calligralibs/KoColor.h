/*
 *  Copyright (c) 2005 Boudewijn Rempt <boud@valdyas.org>
 *  Copyright (C) 2007 Thomas Zander <zander@kde.org>
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
#ifndef CALLIGRADOCUMENTS_KOCOLOR_H
#define CALLIGRADOCUMENTS_KOCOLOR_H

#include <QColor>
#include <QMetaType>
//class KoColorProfile;
class KoColorSpace;
class KoColorProfile;

const quint8 OPACITY_TRANSPARENT_U8 = 0;
const quint8 OPACITY_OPAQUE_U8 = UCHAR_MAX;
const qreal OPACITY_TRANSPARENT_F = 0.0;
const qreal OPACITY_OPAQUE_F = 1.0;

class KoColor
{
public:
    KoColor(const QColor &color = QColor()) : m_color(color) {}
    KoColor(const QColor & color, const KoColorSpace * colorSpace) : m_color(color) {}
    KoColor(const KoColorSpace * colorSpace) {}
    KoColor(const KoColor &src, const KoColorSpace * colorSpace) : m_color(src.m_color) {}
    KoColor(const quint8 * data, const KoColorSpace * colorSpace) {}

    KoColor &operator=(const KoColor &other) { m_color = other.m_color; return *this; }
    bool operator==(const KoColor &other) const { return m_color == other.m_color; }

    const KoColorSpace * colorSpace() const { return 0; }
    const KoColorProfile *  profile() const { return 0; }

    void convertTo(const KoColorSpace * cs) {}

    void setColor(const quint8 * data, const KoColorSpace * colorSpace = 0) {}

    void fromKoColor(const KoColor& src) { m_color = src.m_color; }

    QColor toQColor() const { return m_color; }
    void toQColor(QColor *c) const { *c = m_color; }

    void setOpacity(quint8 alpha) { m_color.setAlpha((int)alpha); }
    void setOpacity(qreal alpha) { m_color.setAlphaF(alpha); }
    quint8 opacityU8() const { return (quint8)m_color.alpha(); }
    qreal opacityF() const { return m_color.alphaF(); }

    void fromQColor(const QColor& c) const { m_color = c; }

    quint8 * data() { Q_ASSERT(false); return 0; }
    const quint8 * data() const { Q_ASSERT(false); return 0; }

#if 0
    /**
     * Serialize this color following Create's swatch color specification available
     * at http://create.freedesktop.org/wiki/index.php/Swatches_-_colour_file_format
     *
     * This function doesn't create the <color /> element but rather the <CMYK />,
     * <sRGB />, <RGB /> ... elements. It is assumed that colorElt is the <color />
     * element.
     *
     * @param colorElt root element for the serialization, it is assumed that this
     *                 element is <color />
     * @param doc is the document containing colorElt
     */
    void toXML(QDomDocument& doc, QDomElement& colorElt) const;

    /**
     * Unserialize a color following Create's swatch color specification available
     * at http://create.freedesktop.org/wiki/index.php/Swatches_-_colour_file_format
     *
     * @param elt the element to unserialize (<CMYK />, <sRGB />, <RGB />)
     * @param bitDepthId the bit depth is unspecified by the spec, this allow to select
     *                   a preferred bit depth for creating the KoColor object (if that
     *                   bit depth isn't available, this function will randomly select
     *                   an other bit depth)
     * @param profileAliases alias between the profile name specified by the "space"
     *                       attribute and the profile name used inside pigment
     * @return the unserialize color, or an empty color object if the function failed
     *         to unserialize the color
     */
    static KoColor fromXML(const QDomElement& elt, const QString & bitDepthId, const QHash<QString, QString> & aliases);
#endif

private:
    mutable QColor m_color;
};

Q_DECLARE_METATYPE(KoColor)

#endif
