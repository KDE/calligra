/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
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

#include "KoRulerController.h"
#include "KoRulerController_p.h"
#include "KoText.h"
#include "styles/KoParagraphStyle.h"

#include <KoCanvasResourceManager.h>
#include <KoTextDocument.h>

#include <WidgetsDebug.h>

#include <QVariant>
#include <QTextOption>
#include <QTextDocument>
#include <QTextBlock>
#include <QTextBlockFormat>
#include <QTextLayout>
#include <QTextCursor>

#include <KoRuler.h>

KoRulerController::KoRulerController(KoRuler *horizontalRuler, KoCanvasResourceManager *crp)
        : QObject(horizontalRuler),
        d(new Private(horizontalRuler, crp))
{
    connect(crp, &KoCanvasResourceManager::canvasResourceChanged, this, [this] (int key, const QVariant &value) {
        Q_UNUSED(value);
        d->canvasResourceChanged(key);
    });
    connect(horizontalRuler, &KoRuler::indentsChanged, this, [this] (bool final) { Q_UNUSED(final); d->indentsChanged(); });
    connect(horizontalRuler, &KoRuler::aboutToChange, this, [this] () { d->tabChangeInitiated(); });
    connect(horizontalRuler, &KoRuler::tabChanged, this, [this] (int id, KoRuler::Tab *tab) { d->tabChanged(id, tab); });
}

KoRulerController::~KoRulerController()
{
    delete d;
}

//have to include this because of Q_PRIVATE_SLOT
#include <moc_KoRulerController.cpp>
