/* This file is part of the KDE libraries
   SPDX-FileCopyrightText: 2009 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only
*/

#include "KoStandardAction.h"

#include <KActionCollection>
#include <KLocalizedString>
#include <ktoggleaction.h>

QAction *KoStandardAction::create(StandardAction id, const QObject *recvr, const char *slot, QObject *parent)
{
    QAction *newAction = nullptr;

    switch (id) {
    case ShowGuides: {
        KToggleAction *toggle = new KToggleAction(i18n("Show Guides"), parent);
        toggle->setToolTip(i18n("Shows or hides guides"));
        newAction = toggle;
        break;
    }
    case ActionNone:
        return nullptr;
    }

    Q_ASSERT(newAction);
    newAction->setObjectName(name(id));

    if (recvr && slot)
        QObject::connect(newAction, SIGNAL(triggered(bool)), recvr, slot);

    KActionCollection *collection = qobject_cast<KActionCollection *>(parent);
    if (collection)
        collection->addAction(newAction->objectName(), newAction);

    return newAction;
}

const char *KoStandardAction::name(StandardAction id)
{
    switch (id) {
    case ShowGuides:
        return "view_show_guides";
    default:
        return nullptr;
    };
}

KToggleAction *KoStandardAction::showGuides(const QObject *receiver, const char *slot, QObject *parent)
{
    return static_cast<KToggleAction *>(create(ShowGuides, receiver, slot, parent));
}
