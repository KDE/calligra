/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Andrea Rizzi <rizzi@kde.org>
                      Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
   SPDX-FileCopyrightText: 2006-2007 Martin Pfeiffer <hubipete@gmx.net>
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>
                 2009 Jeremias Epperlein <jeeree@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "FormulaElement.h"
#include "ElementFactory.h"
#include "FormulaCursor.h"
#include <KoXmlWriter.h>

FormulaElement::FormulaElement()
    : RowElement(nullptr)
{
}

ElementType FormulaElement::elementType() const
{
    return Formula;
}

void FormulaElement::writeMathMLAttributes(KoXmlWriter *writer) const
{
    writer->addAttribute("xmlns", "http://www.w3.org/1998/Math/MathML");
}
