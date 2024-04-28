/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ShapeApplicationData.h"

#include <Qt> // Q_DECL_HIDDEN

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN ShapeApplicationData::Private
{
public:
    bool anchoredToCell;
};

ShapeApplicationData::ShapeApplicationData()
    : KoShapeApplicationData()
    , d(new Private)
{
    d->anchoredToCell = true;
}

ShapeApplicationData::~ShapeApplicationData()
{
    delete d;
}

void ShapeApplicationData::setAnchoredToCell(bool state)
{
    d->anchoredToCell = state;
}

bool ShapeApplicationData::isAnchoredToCell() const
{
    return d->anchoredToCell;
}
