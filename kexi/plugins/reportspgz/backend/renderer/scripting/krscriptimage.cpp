/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */
#include "krscriptimage.h"
#include <krimagedata.h>
#include <QBuffer>
#include <kcodecs.h>
#include <kdebug.h>

namespace Scripting
{

Image::Image(KRImageData *i)
{
    _image = i;
}


Image::~Image()
{
}

QPointF Image::position()
{
    return _image->_pos.toPoint();
}
void Image::setPosition(const QPointF& p)
{
    _image->_pos.setPointPos(p);
}

QSizeF Image::size()
{
    return _image->_size.toPoint();
}
void Image::setSize(const QSizeF& s)
{
    _image->_size.setPointSize(s);
}

QString Image::resizeMode()
{
    return _image->_resizeMode->value().toString();
}

void Image::setResizeMode(const QString &rm)
{
    if (rm == "Stretch") {
        _image->_resizeMode->setValue("Stretch");
    } else {
        _image->_resizeMode->setValue("Clip");
    }
}

void Image::setInlineImage(const QByteArray &ba)
{
    _image->setInlineImageData(ba);
}

void Image::loadFromFile(const QVariant &pth)
{
    QPixmap img;

    QString str = pth.toString();
    _image->setInlineImageData(QByteArray(), str);
}
}
