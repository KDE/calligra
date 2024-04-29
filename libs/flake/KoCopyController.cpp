/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2008 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoCopyController.h"

#include <KoCanvasBase.h>
#include <KoToolBase.h>
#include <KoToolProxy.h>
#include <KoToolSelection.h>

#include "KoCopyController_p.h"
#include <FlakeDebug.h>
#include <QAction>

KoCopyControllerPrivate::KoCopyControllerPrivate(KoCopyController *p, KoCanvasBase *c, QAction *a)
    : parent(p)
    , canvas(c)
    , action(a)
{
    appHasSelection = false;
}

void KoCopyControllerPrivate::copy()
{
    if (canvas->toolProxy()->hasSelection())
        // means the copy can be done by a flake tool
        canvas->toolProxy()->copy();
    else // if not; then the application gets a request to do the copy
        Q_EMIT parent->copyRequested();
}

void KoCopyControllerPrivate::cut()
{
    if (canvas->toolProxy()->hasSelection()) {
        canvas->toolProxy()->cut();
    } else {
        Q_EMIT parent->copyRequested();
    }
}

void KoCopyControllerPrivate::selectionChanged(bool hasSelection)
{
    action->setEnabled(appHasSelection || hasSelection);
}

// KoCopyController
KoCopyController::KoCopyController(KoCanvasBase *canvas, QAction *copyAction)
    : QObject(copyAction)
    , d(new KoCopyControllerPrivate(this, canvas, copyAction))
{
    connect(canvas->toolProxy(), &KoToolProxy::selectionChanged, this, [this](bool v) {
        d->selectionChanged(v);
    });
    connect(copyAction, &QAction::triggered, this, [this]() {
        d->copy();
    });
    hasSelection(false);
}

KoCopyController::~KoCopyController()
{
    delete d;
}

void KoCopyController::hasSelection(bool selection)
{
    d->appHasSelection = selection;
    d->action->setEnabled(d->appHasSelection || d->canvas->toolProxy()->hasSelection());
}

// have to include this because of Q_PRIVATE_SLOT
#include "moc_KoCopyController.cpp"
