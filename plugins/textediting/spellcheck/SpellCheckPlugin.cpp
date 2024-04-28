/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SpellCheckPlugin.h"
#include "SpellCheckFactory.h"

#include <KPluginFactory>

#include <KoTextEditingRegistry.h>

K_PLUGIN_FACTORY_WITH_JSON(SpellCheckPluginFactory, "calligra_textediting_spellcheck.json", registerPlugin<SpellCheckPlugin>();)

SpellCheckPlugin::SpellCheckPlugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoTextEditingRegistry::instance()->add(new SpellCheckFactory());
}

#include <SpellCheckPlugin.moc>
