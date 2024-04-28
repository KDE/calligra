/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ActionElement.h"

ActionElement::ActionElement(BasicElement *parent)
    : RowElement(parent)
{
}

ElementType ActionElement::elementType() const
{
    return Action;
}
