/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "StyleElement.h"

StyleElement::StyleElement(BasicElement *parent)
    : RowElement(parent)
{
}

ElementType StyleElement::elementType() const
{
    return Style;
}
