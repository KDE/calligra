/*
 * This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "CQPluginLoaderProxy.h"

#include <KoToolRegistry.h>
#include <KoShapeRegistry.h>
#include <KoInlineObjectRegistry.h>
#include <KoFilterEffectRegistry.h>

CQPluginLoaderProxy::CQPluginLoaderProxy(QObject* parent)
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
