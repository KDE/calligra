/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "PhantomElement.h"

PhantomElement::PhantomElement(BasicElement *parent)
    : RowElement(parent)
{
}

ElementType PhantomElement::elementType() const
{
    return Phantom;
}
