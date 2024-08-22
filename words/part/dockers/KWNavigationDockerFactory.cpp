/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWNavigationDockerFactory.h"
#include <dockers/KWNavigationDocker.h>

KWNavigationDockerFactory::KWNavigationDockerFactory() = default;

QString KWNavigationDockerFactory::id() const
{
    return QString("Navigation");
}

QDockWidget *KWNavigationDockerFactory::createDockWidget()
{
    KWNavigationDocker *widget = new KWNavigationDocker();
    widget->setObjectName(id());
    return widget;
}
