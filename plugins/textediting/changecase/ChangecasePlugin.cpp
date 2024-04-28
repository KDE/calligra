/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "ChangecasePlugin.h"
#include "ChangecaseFactory.h"

#include <KPluginFactory>

#include <KoTextEditingRegistry.h>

K_PLUGIN_FACTORY_WITH_JSON(ChangecasePluginFactory, "calligra_textediting_changecase.json", registerPlugin<ChangecasePlugin>();)

ChangecasePlugin::ChangecasePlugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoTextEditingRegistry::instance()->add(new ChangecaseFactory());
}

#include <ChangecasePlugin.moc>
