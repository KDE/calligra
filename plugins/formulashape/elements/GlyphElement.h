/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2009 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef GLYPHELEMENT_H
#define GLYPHELEMENT_H

#include "TokenElement.h"
#include "koformula_export.h"
#include <QPainterPath>

/**
 * @short Implementation of the MathML mglyph element
 *
 * GlyphElement uses the Qt font database classes to load the additional fonts to
 * display its contents.
 */
class KOFORMULA_EXPORT GlyphElement : public TokenElement
{
public:
    /// The standard constructor
    explicit GlyphElement(BasicElement *parent = nullptr);

    /// @return The element's ElementType
    ElementType elementType() const override;

    /// Process @p raw and render it to @p path
    QRectF renderToPath(const QString &raw, QPainterPath &path) const override;

    /// get width of character, for layouting
    qreal getWidth(const AttributeManager *am);

private:
    bool readMathMLAttributes(const KoXmlElement &element) override;
    void writeMathMLAttributes(KoXmlWriter *writer) const override;
    void writeMathMLContent(KoXmlWriter *writer, const QString &ns) const override;

    QChar m_char; // Char to be shown
    QString m_fontFamily; // Font family to use
    QString m_alt; // Alternative text if font family not found
    bool m_hasFont; // Whether required font is available
};

#endif // GLYPHELEMENT_H
