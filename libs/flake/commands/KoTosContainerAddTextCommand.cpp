/* This file is part of the KDE project
 * Copyright (C) 2014 Yue Liu <yue.liu@mail.com>
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

#include "KoTosContainerAddTextCommand.h"

#include <KoTosContainer.h>
#include <KoDocumentResourceManager.h>

#include <QList>
#include <vector>

class KoTosContainerAddTextCommand::Private
{
public:
    Private(const QList<KoTosContainer*> &list, KoDocumentResourceManager* res)
        : containers(list)
        , changes(list.count(), false)
        , resourceManager(res)
    {}
    QList<KoTosContainer*> containers;
    std::vector<bool> changes;
    KoDocumentResourceManager* resourceManager;

};

KoTosContainerAddTextCommand::KoTosContainerAddTextCommand(const QList<KoTosContainer*> &tosContainers,
                                                           KoDocumentResourceManager* resourceManager,
                                                           KUndo2Command * parent)
    : KUndo2Command(parent), d(new Private(tosContainers, resourceManager))
{
}

KoTosContainerAddTextCommand::~KoTosContainerAddTextCommand()
{
    delete d;
}

void KoTosContainerAddTextCommand::redo()
{
    KUndo2Command::redo();

    for (int i = 0; i < d->changes.size(); ++i) {
        KoTosContainer * container = d->containers[i];
        container->update();
        container->createTextShape(d->resourceManager);
        d->changes[i] = true;
        container->update();
    }
}

void KoTosContainerAddTextCommand::undo()
{
    KUndo2Command::undo();

    for (int i = 0; i < d->changes.size(); ++i) {
        if (d->changes[i]) {
            d->containers[i]->update();
            //TODO d->containers[i]->deleteTextShape();
            d->containers[i]->update();
        }
    }
}
