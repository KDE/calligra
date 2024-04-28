/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoRulerController.h"
#include "KoRulerController_p.h"
#include "KoText.h"
#include "styles/KoParagraphStyle.h"

#include <KoCanvasResourceManager.h>
#include <KoTextDocument.h>

#include <WidgetsDebug.h>

#include <QTextBlock>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextLayout>
#include <QTextOption>
#include <QVariant>

#include <KoRuler.h>

KoRulerController::KoRulerController(KoRuler *horizontalRuler, KoCanvasResourceManager *crp)
    : QObject(horizontalRuler)
    , d(new Private(horizontalRuler, crp))
{
    connect(crp, &KoCanvasResourceManager::canvasResourceChanged, this, [this](int key, const QVariant &value) {
        Q_UNUSED(value);
        d->canvasResourceChanged(key);
    });
    connect(horizontalRuler, &KoRuler::indentsChanged, this, [this](bool final) {
        Q_UNUSED(final);
        d->indentsChanged();
    });
    connect(horizontalRuler, &KoRuler::aboutToChange, this, [this]() {
        d->tabChangeInitiated();
    });
    connect(horizontalRuler, &KoRuler::tabChanged, this, [this](int id, KoRuler::Tab *tab) {
        d->tabChanged(id, tab);
    });
}

KoRulerController::~KoRulerController()
{
    delete d;
}

// have to include this because of Q_PRIVATE_SLOT
#include <moc_KoRulerController.cpp>
