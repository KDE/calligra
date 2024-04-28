/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ThesaurusPlugin.h"
#include "ThesaurusFactory.h"

#include <KPluginFactory>

#include <KoTextEditingRegistry.h>

K_PLUGIN_FACTORY_WITH_JSON(ThesaurusPluginFactory, "calligra_textediting_thesaurus.json", registerPlugin<ThesaurusPlugin>();)

ThesaurusPlugin::ThesaurusPlugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoTextEditingRegistry::instance()->add(new ThesaurusFactory());
}

#include <ThesaurusPlugin.moc>
