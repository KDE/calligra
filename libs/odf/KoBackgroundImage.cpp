/* This file is part of the KDE project
 *
 * Copyright (C) 2013 Inge Wallin <inge@lysator.liu.se>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

// Own
#include "KoBackgroundImage.h"

// KDE
#include <kdebug.h>

// Calligra
#include <KoXmlReader.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>


// ----------------------------------------------------------------
//                     KoBackgroundImage::Private


class KoBackgroundImage::Private
{
public:
    Private();
    ~Private() {};

    qint8  opacity;            // 0-100%
    KoBackgroundImage::Position verticalPosition;
    KoBackgroundImage::Position horizontalPosition;
    KoBackgroundImage::Repeat repeat;
};

KoBackgroundImage::Private::Private()
    : opacity(100)
    , verticalPosition(Center)
    , horizontalPosition(Center)
    , repeat(DoRepeat)
{
}


// ----------------------------------------------------------------
//                         KoBackgroundImage


KoBackgroundImage::KoBackgroundImage()
    : d(new Private())
{
}

KoBackgroundImage::~KoBackgroundImage()
{
    delete d;
}


// getters
qint8 KoBackgroundImage::opacity() const
{
    return d->opacity;
}

KoBackgroundImage::Position KoBackgroundImage::verticalPosition() const
{
    return d->verticalPosition;
}

KoBackgroundImage::Position KoBackgroundImage::horizontalPosition() const
{
    return d->horizontalPosition;
}

KoBackgroundImage::Repeat KoBackgroundImage::repeat() const
{
    return d->repeat;
}

    
// setters
void KoBackgroundImage::setOpacity(quint8 opacity)
{
    d->opacity = opacity;
}

void KoBackgroundImage::setVerticalPosition(KoBackgroundImage::Position position)
{
    d->verticalPosition = position;
}

void KoBackgroundImage::setHorizontalPosition(KoBackgroundImage::Position position)
{
    d->horizontalPosition = position;
}

void KoBackgroundImage::setRepeat(KoBackgroundImage::Repeat repeat)
{
    d->repeat = repeat;
}



bool KoBackgroundImage::loadOdf(const KoXmlElement &element)
{
    QString dummy;

    // attribute style:position
    d->verticalPosition = Center;
    d->horizontalPosition = Center;
    dummy = element.attributeNS(KoXmlNS::style, "position");
    QStringList positions = dummy.split(' ', QString::SkipEmptyParts);
    foreach (const QString &str, positions) {
        if (str == "top")         d->verticalPosition = Top;
        else if (str == "bottom") d->verticalPosition = Bottom;
        else if (str == "left")   d->horizontalPosition = Left;
        else if (str == "right")  d->horizontalPosition = Right;
        // No need for a general else clause because everything is already initialized to Center
    }

    // attribute style:repeat
    d->repeat = DoRepeat;
    dummy = element.attributeNS(KoXmlNS::style, "repeat");
    if (dummy == "no-repeat") {
        d->repeat = NoRepeat;
    }
    else if (dummy == "stretch") {
        d->repeat = Stretch;
    }

    // attribute draw:opacity
    d->opacity = 100;
    dummy = element.attributeNS(KoXmlNS::draw, "opacity");
    bool ok;
    int  value = dummy.toInt(&ok);
    if (ok) {
        if (value > 100)
            value = 100;
        else if (value < 0)
            value = 0;

        d->opacity = value;
    }

    // FIXME: the actual image

    return true;
}

void KoBackgroundImage::saveOdf(KoXmlWriter &writer) const
{
    writer.startElement("style:background-image");

    // "repeat" is default for attribute style:repeat.
    if (d->repeat != KoBackgroundImage::DoRepeat) {
        writer.addAttribute("style:repeat", d->repeat == NoRepeat ? "no-repeat" : "stretch");
    }

    // "center center" is default for attribute style:position
    if (d->verticalPosition != KoBackgroundImage::Center
        || d->horizontalPosition != KoBackgroundImage::Center)
    {
        QString value;
        if (d->verticalPosition != KoBackgroundImage::Center) {
            value += (d->verticalPosition == Top ? "top" : "bottom");
        }
        if (d->horizontalPosition != KoBackgroundImage::Center) {
            if (!value.isEmpty()) {
                value += ' ';
            }
            value += (d->horizontalPosition == Left ? "left" : "right");
        }

        writer.addAttribute("style:position", value);
    }

    // opacity 100% is default for attribute draw:opacity
    if (d->opacity != 100) {
        writer.addAttribute("draw:opacity", QString("%1").arg(d->opacity));
    }

    // FIXME: the actual Image

    writer.endElement(); // style:background-image
}
