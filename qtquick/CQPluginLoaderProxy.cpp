/*
 * This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "CQPluginLoaderProxy.h"

#include <KoFilterEffectRegistry.h>
#include <KoInlineObjectRegistry.h>
#include <KoShapeRegistry.h>
#include <KoToolRegistry.h>

CQPluginLoaderProxy::CQPluginLoaderProxy(QObject *parent)
    : QObject(parent)
{
}

CQPluginLoaderProxy::~CQPluginLoaderProxy()
{
}

void CQPluginLoaderProxy::loadPlugins()
{
    KoToolRegistry::instance();
    KoShapeRegistry::instance();
    KoInlineObjectRegistry::instance();
    KoFilterEffectRegistry::instance();
}
