/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2008 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOCOPYCONTROLLER_P_H
#define KOCOPYCONTROLLER_P_H

#include "KoCopyController.h"

class KoCanvasBase;
class QAction;

// KoCopyControllerPrivate
class KoCopyControllerPrivate
{
public:
    KoCopyControllerPrivate(KoCopyController *p, KoCanvasBase *c, QAction *a);

    // request to start the actual copy
    void copy();

    // request to start the actual cut
    void cut();

    void selectionChanged(bool hasSelection);

    KoCopyController *parent;
    KoCanvasBase *canvas;
    QAction *action;
    bool appHasSelection;
};

#endif
