/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Nokia

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "writer.h"

#include "KoGenStyles.h"

Writer::Writer(KoXmlWriter &xmlWriter, KoGenStyles &kostyles, bool stylesxml_)
    : xOffset(0)
    , yOffset(0)
    , scaleX(1)
    , scaleY(1)
    , g_rotation(0)
    , g_flipH(0)
    , g_flipV(0)
    , xml(xmlWriter)
    , styles(kostyles)
    , stylesxml(stylesxml_)
{
}

Writer Writer::transform(const QRectF &oldCoords, const QRectF &newCoords) const
{
    Writer w(xml, styles, stylesxml);
    w.xOffset = xOffset + oldCoords.x() * scaleX;
    w.yOffset = yOffset + oldCoords.y() * scaleY;
    w.scaleX = scaleX * oldCoords.width() / newCoords.width();
    w.scaleY = scaleY * oldCoords.height() / newCoords.height();
    w.xOffset -= w.scaleX * newCoords.x();
    w.yOffset -= w.scaleY * newCoords.y();
    w.g_rotation = g_rotation;
    w.g_flipH = g_flipH;
    w.g_flipV = g_flipV;
    return w;
}

qreal Writer::vLength(qreal length) const
{
    return length * scaleY;
}

qreal Writer::hLength(qreal length) const
{
    return length * scaleX;
}

qreal Writer::vOffset(qreal offset) const
{
    return yOffset + offset * scaleY;
}

qreal Writer::hOffset(qreal offset) const
{
    return xOffset + offset * scaleX;
}
