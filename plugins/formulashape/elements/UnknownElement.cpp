/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Andrea Rizzi <rizzi@kde.org>
                  Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
         2006 Martin Pfeiffer <hubipete@gmx.net>
   SPDX-FileCopyrightText: 2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "UnknownElement.h"

#include "AttributeManager.h"
#include "FormulaCursor.h"
#include "FormulaDebug.h"

#include <KoXmlReader.h>

#include <QPainter>

UnknownElement::UnknownElement(BasicElement *parent)
    : BasicElement(parent)
{
}

UnknownElement::~UnknownElement() = default;

void UnknownElement::paint(QPainter &painter, AttributeManager *am)
{
    Q_UNUSED(am)
    Q_UNUSED(painter)
}

void UnknownElement::layout(const AttributeManager *am)
{
    Q_UNUSED(am)
}

const QList<BasicElement *> UnknownElement::childElements() const
{
    QList<BasicElement *> tmp;
    return tmp;
}

bool UnknownElement::acceptCursor(const FormulaCursor &cursor)
{
    Q_UNUSED(cursor)
    return false;
}

ElementType UnknownElement::elementType() const
{
    return Unknown;
}

bool UnknownElement::readMathMLAttributes(const KoXmlElement &element)
{
    Q_UNUSED(element)
    return true;
}

bool UnknownElement::readMathMLContent(const KoXmlElement &element)
{
    // TODO - save the mathml content in a string
    Q_UNUSED(element)
    return true;
}

void UnknownElement::writeMathMLAttributes(KoXmlWriter *writer) const
{
    Q_UNUSED(writer)
}

void UnknownElement::writeMathMLContent(KoXmlWriter *writer, const QString &ns) const
{
    Q_UNUSED(writer)
    Q_UNUSED(ns)
    // TODO - write the save mathml content back into writer
}
