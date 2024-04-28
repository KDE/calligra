/*
* This file is part of the KDE project
*
* SPDX-FileCopyrightText: 2009 Nokia Corporation and /or its subsidiary(-ies).
*
* Contact: Amit Aggarwal <amitcs06@gmail.com>
*
SPDX-License-Identifier: LGPL-2.1-or-later
*
*/
#include "Plugin.h"
#include "PresentationVariableFactory.h"
#include <KPluginFactory>
#include <KoInlineObjectRegistry.h>

K_PLUGIN_FACTORY_WITH_JSON(PluginFactory, "kprvariables.json", registerPlugin<Plugin>();)

Plugin::Plugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoInlineObjectRegistry::instance()->add(new PresentationVariableFactory());
}

#include "Plugin.moc"
