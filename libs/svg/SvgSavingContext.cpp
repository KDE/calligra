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
#include <KoShape.h>
#include <KoShapeGroup.h>
#include <KoShapeLayer.h>
#include <QtCore/QBuffer>
#include <QtCore/QHash>

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

    QHash<QString, int> uniqueNames;
    QHash<const KoShape*, QString> shapeIds;
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

QString SvgSavingContext::createUID(const QString &base)
{
    QString idBase = base.isEmpty() ? "defitem" : base;
    int counter = d->uniqueNames.value(idBase);
    d->uniqueNames.insert(idBase, counter+1);

    return idBase + QString("%1").arg(counter);
}

QString SvgSavingContext::getID(const KoShape *obj)
{
    QString id;
    // do we have already an id for this object ?
    if (d->shapeIds.contains(obj)) {
        // use existing id
        id = d->shapeIds[obj];
    } else {
        // initialize from object name
        id = obj->name();
        // if object name is not empty and was not used already
        // we can use it as is
        if (!id.isEmpty() && !d->uniqueNames.contains(id)) {
            // add to unique names so it does not get reused
            d->uniqueNames.insert(id, 1);
        } else {
            if (id.isEmpty()) {
                // differentiate a little between shape types
                if (dynamic_cast<const KoShapeGroup*>(obj))
                    id = "group";
                else if (dynamic_cast<const KoShapeLayer*>(obj))
                    id = "layer";
                else
                    id = "shape";
            }
            // create a compeletely new id based on object name
            // or a generic name
            id = createUID(id);
        }
        // record id for this shape
        d->shapeIds.insert(obj, id);
    }
    return id;
}

