/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KORULERCONTROLLER_H
#define KORULERCONTROLLER_H

#include "kowidgets_export.h"
#include <QObject>

class KoRuler;
class KoCanvasResourceManager;

/**
 * This class combines text options with the KoRuler object.
 * Any usage of a horizontal ruler should consider using this class to show the
 * text indent and tabs on the ruler, and allow to edit them.
 * The code to do this is pretty trivial; just instantiate this class and you can
 * forget about it.  It'll do what you want.
 */
class KOWIDGETS_EXPORT KoRulerController : public QObject
{
    Q_OBJECT
public:
    /**
     * Constructor.
     * @param horizontalRuler the ruler to monitor and update.
     *  Will also be used as QObject parent for memory management purposes.
     * @param crp the resource provider for the canvas this ruler and the text tool belong to.
     */
    KoRulerController(KoRuler *horizontalRuler, KoCanvasResourceManager *crp);
    ~KoRulerController() override;

private:
    class Private;
    Private *const d;

    Q_PRIVATE_SLOT(d, void canvasResourceChanged(int))
    Q_PRIVATE_SLOT(d, void indentsChanged())
    Q_PRIVATE_SLOT(d, void tabChanged(int, KoRuler::Tab *tab))
    Q_PRIVATE_SLOT(d, void tabChangeInitiated())
};

#endif
