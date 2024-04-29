/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef OPERATORELEMENT_H
#define OPERATORELEMENT_H

#include "Dictionary.h"
#include "TokenElement.h"
#include "koformula_export.h"

/**
 * @short Implementation of the MathML mo element
 *
 * The mo element uses the Dictionary class to look up the attributes of the single
 * operator. Processed in the renderToPath method the operator element renders its
 * contents to the path. It also respects the right and left spaces.
 */
class KOFORMULA_EXPORT OperatorElement : public TokenElement
{
public:
    /// The standard constructor
    explicit OperatorElement(BasicElement *parent = nullptr);

    /**
     * Used by FenceElement to render its open, close fences as well as the separators
     * @param raw The raw string which is supposed to be rendered - might be entity
     * @param form Indicates whether raw is interpreted as fence or separator
     * @return The painter path with the rendered content
     */
    QPainterPath renderForFence(const QString &raw, Form form);

    /// @return The element's ElementType
    ElementType elementType() const override;

    /// Process @p raw and render it to @p path
    QRectF renderToPath(const QString &raw, QPainterPath &path) const override;

    /// Inherited from TokenElement
    bool insertText(int position, const QString &text) override;

    /// Inherited from TokenElement
    bool readMathMLContent(const KoXmlElement &parent) override;

    /** Reimplemented from BaseElement
     *  Sets the height() and baseLine() of the element based on the parent size
     */
    void stretch() override;

private:
    /// @return The Form value that was passed as QString @p value
    Form parseForm(const QString &value) const;
    Dictionary m_dict;

private:
    Form determineOperatorForm() const;
};

#endif // OPERATORELEMENT_H
