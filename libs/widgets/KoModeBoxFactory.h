/*
 * SPDX-FileCopyrightText: 2006 Peter Simonsson <peter.simonsson@gmail.com>
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2011 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOMODEBOXFACTORY_H
#define KOMODEBOXFACTORY_H

#include "kowidgets_export.h"
#include <KoDockFactoryBase.h>

#include <QDockWidget>
#include <QString>

class KoCanvasControllerWidget;

/**
 * Factory class to create a new KoModeBox that contains a QToolBox which acts
 * as a replacement for KoToolBox and KoDockerManagers options docker.
 */
class KOWIDGETS_EXPORT KoModeBoxFactory : public KoDockFactoryBase
{
public:
    explicit KoModeBoxFactory(KoCanvasControllerWidget *canvas, const QString &applicationName, const QString &appName);
    ~KoModeBoxFactory() override;

    QString id() const override;
    KoDockFactoryBase::DockPosition defaultDockPosition() const override;
    QDockWidget *createDockWidget() override;
    bool isCollapsable() const override
    {
        return false;
    }

private:
    class Private;
    Private *const d;
};

#endif
