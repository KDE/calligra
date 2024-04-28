/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "TextLabelDummy.h"

void TextLabelDummy::paint(QPainter &, const KoViewConverter &, KoShapePaintingContext &)
{
}

bool TextLabelDummy::loadOdf(const KoXmlElement &, KoShapeLoadingContext &)
{
    return false;
}

void TextLabelDummy::saveOdf(KoShapeSavingContext &) const
{
}
