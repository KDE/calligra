/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOCUTCONTROLLER_H
#define KOCUTCONTROLLER_H

#include "KoCopyController.h"
#include "flake_export.h"

/**
 * This class takes care of the cut actions integration into flake.
 * Whenever the copy (KStandardAction::Cut) action is triggered the controller
 * will use the currently selected tool and try to cut to the clipboard using that tool.
 * Additionally; when the tool does not allow copying (KoToolBase::hasSelection() returns false)
 * the signal copyRequested will be emitted for applications to connect to.
 */
class FLAKE_EXPORT KoCutController : public KoCopyController
{
    Q_OBJECT
public:
    /**
     * Constructor
     * @param canvas the canvas this controller will work on.
     * @param cutAction the action that we will listen to and respond to when it is activated.  Additionally, the
     *     action is used as a parent for the QObject for memory management purposes.
     */
    KoCutController(KoCanvasBase *canvas, QAction *cutAction);
};

#endif
