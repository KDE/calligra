/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoCutController.h"

#include <QAction>

#include "KoCopyController_p.h"

KoCutController::KoCutController(KoCanvasBase *canvas, QAction *cutAction)
    : KoCopyController(canvas, cutAction)
{
    disconnect(cutAction, &QAction::triggered, this, nullptr); // countering the super
    connect(cutAction, &QAction::triggered, this, [this]() {
        d->cut();
    });
}
