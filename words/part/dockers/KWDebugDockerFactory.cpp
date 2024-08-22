/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWDebugDockerFactory.h"
#include <dockers/KWDebugDocker.h>

KWDebugDockerFactory::KWDebugDockerFactory() = default;

QString KWDebugDockerFactory::id() const
{
    return QString("Debug");
}

QDockWidget *KWDebugDockerFactory::createDockWidget()
{
    KWDebugDocker *widget = new KWDebugDocker();
    widget->setObjectName(id());
    return widget;
}
