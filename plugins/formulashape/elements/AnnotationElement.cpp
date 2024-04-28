/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2010 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "AnnotationElement.h"
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

AnnotationElement::AnnotationElement(BasicElement *parent)
    : BasicElement(parent)
{
}

ElementType AnnotationElement::elementType() const
{
    return Annotation;
}

void AnnotationElement::setContent(const QString &content)
{
    m_content = content;
}

QString AnnotationElement::content() const
{
    return m_content;
}

bool AnnotationElement::readMathMLContent(const KoXmlElement &element)
{
    KoXmlNode node = element.firstChild();
    while (!node.isNull()) {
        if (node.isText()) {
            m_content.append(node.toText().data());
        }
        node = node.nextSibling();
    }
    return BasicElement::readMathMLContent(element);
}

void AnnotationElement::writeMathMLContent(KoXmlWriter *writer, const QString &ns) const
{
    writer->addTextNode(m_content);

    BasicElement::writeMathMLContent(writer, ns);
}
