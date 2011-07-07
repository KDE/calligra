/* This file is part of the KDE project
 * Copyright (C) 2011 Jan Hambrecht <jaham@gmx.net>
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

#include "SvgSavingContext.h"
#include <KoXmlWriter.h>
#include <QtCore/QBuffer>

class SvgSavingContext::Private
{
public:
    Private(QIODevice &outputDevice)
        : output(outputDevice), styleWriter(0), shapeWriter(0)
    {
        styleWriter = new KoXmlWriter(&styleBuffer, 1);
        styleWriter->startElement("defs");
        shapeWriter = new KoXmlWriter(&shapeBuffer, 1);
    }

    ~Private()
    {
        delete styleWriter;
        delete shapeWriter;
    }

    QIODevice &output;
    QBuffer styleBuffer;
    QBuffer shapeBuffer;
    KoXmlWriter *styleWriter;
    KoXmlWriter *shapeWriter;
};

SvgSavingContext::SvgSavingContext(QIODevice &outputDevice)
    : d(new Private(outputDevice))
{

}

SvgSavingContext::~SvgSavingContext()
{
    d->styleWriter->endElement();
    d->output.write(d->styleBuffer.data());
    d->output.write("\n");
    d->output.write(d->shapeBuffer.data());

    delete d;
}

KoXmlWriter &SvgSavingContext::styleWriter()
{
    return *d->styleWriter;
}

KoXmlWriter &SvgSavingContext::shapeWriter()
{
    return *d->shapeWriter;
}
