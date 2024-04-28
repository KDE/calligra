/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Fredy Yanardi <fyanardi@gmail.com>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoFind.h"
#include "KoFind_p.h"

#include <KActionCollection>
#include <KoCanvasResourceManager.h>
#include <QAction>

KoFind::KoFind(QWidget *parent, KoCanvasResourceManager *canvasResourceManager, KActionCollection *ac)
    : QObject(parent)
    , d(new KoFindPrivate(this, canvasResourceManager, parent))
{
    connect(canvasResourceManager, &KoCanvasResourceManager::canvasResourceChanged, this, [this](int a, const QVariant &b) {
        d->resourceChanged(a, b);
    });
    ac->addAction(KStandardAction::Find, "edit_find", this, SLOT(findActivated()));
    d->findNext = ac->addAction(KStandardAction::FindNext, "edit_findnext", this, SLOT(findNextActivated()));
    d->findNext->setEnabled(false);
    d->findPrev = ac->addAction(KStandardAction::FindPrev, "edit_findprevious", this, SLOT(findPreviousActivated()));
    d->findPrev->setEnabled(false);
    ac->addAction(KStandardAction::Replace, "edit_replace", this, SLOT(replaceActivated()));
}

KoFind::~KoFind()
{
    delete d;
}

// have to include this because of Q_PRIVATE_SLOT
#include "moc_KoFind.cpp"
