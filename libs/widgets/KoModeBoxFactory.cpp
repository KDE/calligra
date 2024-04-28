/*
 * SPDX-FileCopyrightText: 2006 Peter Simonsson <peter.simonsson@gmail.com>
 * SPDX-FileCopyrightText: 2011 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoModeBoxFactory.h"
#include "KoModeBoxDocker_p.h"
#include "KoModeBox_p.h"
#include <KLocalizedString>

class Q_DECL_HIDDEN KoModeBoxFactory::Private
{
public:
    KoCanvasControllerWidget *canvasController;
    QString applicationName;
};

KoModeBoxFactory::KoModeBoxFactory(KoCanvasControllerWidget *canvasController, const QString &applicationName, const QString & /*title*/)
    : d(new Private())
{
    d->canvasController = canvasController;
    d->applicationName = applicationName;
}

KoModeBoxFactory::~KoModeBoxFactory()
{
    delete d;
}

QString KoModeBoxFactory::id() const
{
    return QString("ModeBox");
}

KoDockFactoryBase::DockPosition KoModeBoxFactory::defaultDockPosition() const
{
    return KoDockFactoryBase::DockRight;
}

QDockWidget *KoModeBoxFactory::createDockWidget()
{
    KoModeBox *box = new KoModeBox(d->canvasController, d->applicationName);
    QDockWidget *docker = new KoModeBoxDocker(box);

    return docker;
}
