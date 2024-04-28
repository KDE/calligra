/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "VariablesPlugin.h"
#include "ChapterVariableFactory.h"
#include "DateVariableFactory.h"
#include "InfoVariableFactory.h"
#include "PageVariableFactory.h"
#include "UserVariableFactory.h"
#include <KPluginFactory>

#include <KoInlineObjectRegistry.h>

K_PLUGIN_FACTORY_WITH_JSON(VariablesPluginFactory, "calligra_textinlineobject_variables.json", registerPlugin<VariablesPlugin>();)

VariablesPlugin::VariablesPlugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoInlineObjectRegistry::instance()->add(new PageVariableFactory());
    KoInlineObjectRegistry::instance()->add(new DateVariableFactory());
    KoInlineObjectRegistry::instance()->add(new InfoVariableFactory());
    KoInlineObjectRegistry::instance()->add(new ChapterVariableFactory());
    KoInlineObjectRegistry::instance()->add(new UserVariableFactory());
}

#include <VariablesPlugin.moc>
