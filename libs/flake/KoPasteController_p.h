/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOPASTECONTROLLER_P_H
#define KOPASTECONTROLLER_P_H

#include "KoPasteController.h"

#include <KoCanvasBase.h>
#include <KoToolProxy.h>

#include <FlakeDebug.h>
#include <QAction>

class Q_DECL_HIDDEN KoPasteController::Private
{
public:
    Private(KoPasteController *p, KoCanvasBase *c, QAction *a)
        : parent(p)
        , canvas(c)
        , action(a)
    {
    }

    void paste()
    {
        debugFlake << "Paste!";
        canvas->toolProxy()->paste();
    }

    KoPasteController *parent;
    KoCanvasBase *canvas;
    QAction *action;
};

#endif
