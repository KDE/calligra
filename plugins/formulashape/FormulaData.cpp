/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Jeremias Epperlein <jeeree@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "FormulaData.h"
#include "FormulaCommand.h"
#include "FormulaCursor.h"
#include "FormulaElement.h"
#include "KoFormulaShape.h"
#include <KoXmlWriter.h>

FormulaData::FormulaData(std::unique_ptr<FormulaElement> element)
    : QObject()
    , m_element(std::move(element))
{
}

FormulaData::~FormulaData()
{
}

void FormulaData::notifyDataChange(FormulaCommand *command, bool undo)
{
    Q_EMIT dataChanged(command, undo);
}

void FormulaData::setFormulaElement(std::unique_ptr<FormulaElement> element)
{
    m_element = std::move(element);
}

std::unique_ptr<FormulaElement> FormulaData::takeFormulaElement()
{
    return std::move(m_element);
}

void FormulaData::swapFormulaElement(std::unique_ptr<FormulaElement> &element)
{
    m_element.swap(element);
}

FormulaElement *FormulaData::formulaElement() const
{
    return m_element.get();
}

void FormulaData::writeElementTree()
{
    m_element->writeElementTree();
}

void FormulaData::saveMathML(KoShapeSavingContext &context)
{
    context.xmlWriter().startDocument("math", "http://www.w3.org/1998/Math/MathML");
    formulaElement()->writeMathML(&context.xmlWriter());
    context.xmlWriter().endDocument();
}
