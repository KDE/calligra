/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2008-2012 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2009 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KoDockerManager_p_h
#define KoDockerManager_p_h

#include "KoDockerManager.h"
#include "KoDockFactoryBase.h"

#include <KLocalizedString>
#include <KConfig>
#include <KConfigGroup>
#include <MainDebug.h>

#include "KoToolDocker.h"

#include "KoView.h"
#include "KoMainWindow.h"

class Q_DECL_HIDDEN KoDockerManager::Private
{
public:
    Private(KoMainWindow *mw) :
        mainWindow(mw)
        ,ignore(true)
        ,showOptionsDocker(true)
    {
    }

    KoToolDocker *toolOptionsDocker;
    KoMainWindow *mainWindow;
    bool ignore;
    bool showOptionsDocker;

    void restoringDone()
    {
        if (ignore) {
            ignore = false;
            toolOptionsDocker->setVisible(showOptionsDocker);
        }
    }
};

#endif
