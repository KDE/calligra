/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2010 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ANNOTATIONELEMENT_H
#define ANNOTATIONELEMENT_H

#include "BasicElement.h"
#include "koformula_export.h"

/**
 * @short Implementation of the MathML annotation element
 *
 * Support for annotation elements in MathML.
 */

class KOFORMULA_EXPORT AnnotationElement : public BasicElement
{
public:
    /// The standard constructor
    explicit AnnotationElement(BasicElement *parent = nullptr);

    void setContent(const QString &content);
    QString content() const;

    /// @return The element's ElementType
    ElementType elementType() const override;

    bool readMathMLContent(const KoXmlElement &element) override;
    void writeMathMLContent(KoXmlWriter *writer, const QString &ns) const override;

private:
    QString m_content;
};

#endif // ANNOTATIONELEMENT_H
