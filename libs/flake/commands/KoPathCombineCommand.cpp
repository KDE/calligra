/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006, 2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPathCombineCommand.h"
#include "KoShapeBasedDocumentBase.h"
#include "KoShapeContainer.h"
#include <KLocalizedString>

class Q_DECL_HIDDEN KoPathCombineCommand::Private
{
public:
    Private(KoShapeBasedDocumentBase *c, const QList<KoPathShape *> &p)
        : controller(c)
        , paths(p)
        , combinedPath(nullptr)
        , combinedPathParent(nullptr)
        , isCombined(false)
    {
        foreach (KoPathShape *path, paths)
            oldParents.append(path->parent());
    }
    ~Private()
    {
        if (isCombined && controller) {
            foreach (KoPathShape *path, paths)
                delete path;
        } else
            delete combinedPath;
    }

    KoShapeBasedDocumentBase *controller;
    QList<KoPathShape *> paths;
    QList<KoShapeContainer *> oldParents;
    KoPathShape *combinedPath;
    KoShapeContainer *combinedPathParent;
    bool isCombined;
};

KoPathCombineCommand::KoPathCombineCommand(KoShapeBasedDocumentBase *controller, const QList<KoPathShape *> &paths, KUndo2Command *parent)
    : KUndo2Command(parent)
    , d(new Private(controller, paths))
{
    setText(kundo2_i18n("Combine paths"));

    d->combinedPath = new KoPathShape();
    d->combinedPath->setStroke(d->paths.first()->stroke());
    d->combinedPath->setShapeId(d->paths.first()->shapeId());
    // combine the paths
    foreach (KoPathShape *path, d->paths) {
        d->combinedPath->combine(path);
        if (!d->combinedPathParent && path->parent())
            d->combinedPathParent = path->parent();
    }
}

KoPathCombineCommand::~KoPathCombineCommand()
{
    delete d;
}

void KoPathCombineCommand::redo()
{
    KUndo2Command::redo();

    if (!d->paths.size())
        return;

    d->isCombined = true;

    if (d->controller) {
        QList<KoShapeContainer *>::iterator parentIt = d->oldParents.begin();
        foreach (KoPathShape *p, d->paths) {
            d->controller->removeShape(p);
            if (*parentIt)
                (*parentIt)->removeShape(p);
            ++parentIt;
        }
        if (d->combinedPathParent)
            d->combinedPathParent->addShape(d->combinedPath);
        d->controller->addShape(d->combinedPath);
    }
}

void KoPathCombineCommand::undo()
{
    if (!d->paths.size())
        return;

    d->isCombined = false;

    if (d->controller) {
        d->controller->removeShape(d->combinedPath);
        if (d->combinedPath->parent())
            d->combinedPath->parent()->removeShape(d->combinedPath);
        QList<KoShapeContainer *>::iterator parentIt = d->oldParents.begin();
        foreach (KoPathShape *p, d->paths) {
            d->controller->addShape(p);
            p->setParent(*parentIt);
            ++parentIt;
        }
    }
    KUndo2Command::undo();
}
