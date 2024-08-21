/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
   SPDX-FileCopyrightText: 2002 Rob Buis <buis@kde.org>
   SPDX-FileCopyrightText: 2004 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2005-2006 Tim Beaulen <tbscope@gmail.com>
   SPDX-FileCopyrightText: 2005 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2005 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2005-2008 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2006 C. Boemann <cbo@boemann.dk>
   SPDX-FileCopyrightText: 2012 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "SnapGuideDocker.h"

#include <KLocalizedString>

#include <KoCanvasBase.h>
#include <KoCanvasObserverBase.h>
#include <KoSnapGuide.h>

class SnapGuideDocker::Private
{
public:
    Private()
        : canvas(nullptr)
        , mainWidget(nullptr)
    {
    }

    KoCanvasBase *canvas;
    QWidget *mainWidget;
};

SnapGuideDocker::SnapGuideDocker()
    : d(new Private())
{
    setWindowTitle(i18n("Snap Settings"));
}

SnapGuideDocker::~SnapGuideDocker()
{
    delete d;
}

void SnapGuideDocker::setCanvas(KoCanvasBase *canvas)
{
    setEnabled(canvas != nullptr);

    if (d->canvas) {
        d->canvas->disconnectCanvasObserver(this); // "Every connection you make emits a signal, so duplicate connections emit two signals"
    }

    if (canvas) {
        d->mainWidget = canvas->createSnapGuideConfigWidget();
    }

    d->canvas = canvas;
    setWidget(d->mainWidget);
}

void SnapGuideDocker::unsetCanvas()
{
    setEnabled(false);
    setWidget(nullptr);
    d->canvas = nullptr;
}

void SnapGuideDocker::locationChanged(Qt::DockWidgetArea area)
{
    Q_UNUSED(area);
}
