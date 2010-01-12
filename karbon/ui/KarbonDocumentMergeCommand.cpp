/* This file is part of the KDE project
 * Copyright (C) 2009 Jan Hambrecht <jaham@gmx.net>
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

#include "KarbonDocumentMergeCommand.h"
#include "KarbonPart.h"
#include "KarbonDocument.h"
#include "KoShapeLayer.h"
#include <KLocale>

class KarbonDocumentMergeCommand::Private
{
public:
    Private() : hasMerged(false)
    {
    }

    ~Private()
    {
        if (!hasMerged) {
            qDeleteAll(layers);
            qDeleteAll(shapes);
        }
    }

    KarbonPart * targetPart;
    QList<KoShapeLayer*> layers;
    QList<KoShape*> shapes;
    bool hasMerged;
};

KarbonDocumentMergeCommand::KarbonDocumentMergeCommand(KarbonPart * targetPart, KarbonPart * sourcePart)
        : QUndoCommand(0), d(new Private())
{
    d->targetPart = targetPart;
    d->layers = sourcePart->document().layers();
    d->shapes = sourcePart->document().shapes();
    foreach(KoShapeLayer * layer, d->layers) {
        sourcePart->removeShape(layer);
    }
    foreach(KoShape * shape, d->shapes) {
        sourcePart->removeShape(shape);
    }
    setText(i18n("Insert graphics"));
}

KarbonDocumentMergeCommand::~KarbonDocumentMergeCommand()
{
    delete d;
}

void KarbonDocumentMergeCommand::redo()
{
    if (!d->hasMerged) {
        foreach(KoShapeLayer * layer, d->layers) {
            d->targetPart->addShape(layer);
        }
        foreach(KoShape * shape, d->shapes) {
            d->targetPart->addShape(shape);
        }
        d->hasMerged = true;
    }

    QUndoCommand::redo();
}

void KarbonDocumentMergeCommand::undo()
{
    QUndoCommand::undo();

    if (d->hasMerged) {
        foreach(KoShapeLayer * layer, d->layers) {
            d->targetPart->removeShape(layer);
        }
        foreach(KoShape * shape, d->shapes) {
            d->targetPart->removeShape(shape);
        }
        d->hasMerged = false;
    }
}
