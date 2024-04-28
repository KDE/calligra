/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPasteController.h"
#include "KoPasteController_p.h"

#include <KoCanvasBase.h>
#include <KoToolProxy.h>

#include <FlakeDebug.h>
#include <QAction>

KoPasteController::KoPasteController(KoCanvasBase *canvas, QAction *pasteAction)
    : QObject(pasteAction)
    , d(new Private(this, canvas, pasteAction))
{
    // connect(canvas->toolProxy(), SIGNAL(selectionChanged(bool)), this, SLOT(selectionChanged(bool)));
    connect(pasteAction, &QAction::triggered, this, [this]() {
        d->paste();
    });
}

KoPasteController::~KoPasteController()
{
    delete d;
}

// have to include this because of Q_PRIVATE_SLOT
#include "moc_KoPasteController.cpp"
