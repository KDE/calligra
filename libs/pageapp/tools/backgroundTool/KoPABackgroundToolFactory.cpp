/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPABackgroundToolFactory.h"

// Calligra includes
#include "KoPABackgroundTool.h"

#include <KoIcon.h>
#include <KoPACanvasBase.h>
#include <KoPADocument.h>
#include <KoPAViewBase.h>
#include <KoPageApp.h>

KoPABackgroundToolFactory::KoPABackgroundToolFactory()
    : KoToolFactoryBase("KoPABackgroundTool")
{
    setToolType("calligraflow, calligrastage");
    setActivationShapeId("flake/always");
    setIconName(koIconName("backgroundtool"));
    setPriority(3);
}

KoPABackgroundToolFactory::~KoPABackgroundToolFactory() = default;

KoToolBase *KoPABackgroundToolFactory::createTool(KoCanvasBase *canvas)
{
    // We need the canvas to know in which app we are to turn the tooltip to page or slide design
    if (dynamic_cast<KoPACanvasBase *>(canvas)) {
        KoPAViewBase *view = static_cast<KoPACanvasBase *>(canvas)->koPAView();

        if (view) {
            const QString toolTip = (view->kopaDocument()->pageType() == KoPageApp::Page) ? i18n("Page Design") : i18n("Slide Design");
            setToolTip(toolTip);
        }
        return new KoPABackgroundTool(canvas);
    }
    return nullptr;
}
