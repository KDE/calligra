/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "AutocorrectPlugin.h"
#include "AutocorrectFactory.h"

#include <KPluginFactory>

#include <KoTextEditingRegistry.h>

K_PLUGIN_FACTORY_WITH_JSON(AutocorrectPluginFactory, "calligra_textediting_autocorrect.json", registerPlugin<AutocorrectPlugin>();)

AutocorrectPlugin::AutocorrectPlugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoTextEditingRegistry::instance()->add(new AutocorrectFactory());
}

#include <AutocorrectPlugin.moc>
