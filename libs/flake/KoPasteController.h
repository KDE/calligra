/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOPASTECONTROLLER_H
#define KOPASTECONTROLLER_H

#include "flake_export.h"

#include <QObject>

class QAction;
class KoCanvasBase;

/**
 * This class takes care of the paste actions integration into flake.
 * Whenever the paste (KStandardAction::Paste) action is triggered the controller
 * will use the currently selected tool and try to paste using that tool.
 * Additionally; when the tool does not allow pasting (KoToolBase::hasSelection() returns false)
 * the signal pasteRequested will be emitted for applications to connect to.
 */
class FLAKE_EXPORT KoPasteController : public QObject
{
    Q_OBJECT
public:
    /**
     * Constructor
     * @param canvas the canvas this controller will work on.
     * @param pasteAction the action that we will listen to and respond to when it is activated.  Additionally, the
     *     action is used as a parent for the QObject for memory management purposes.
     */
    KoPasteController(KoCanvasBase *canvas, QAction *pasteAction);
    ~KoPasteController() override;

private:
    Q_PRIVATE_SLOT(d, void paste())

    class Private;
    Private *const d;
};

#endif
