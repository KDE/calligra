/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "TextEditingPluginContainer.h"
#include "TextTool.h"
#include <KoTextEditingPlugin.h>
#include <KoTextEditingRegistry.h>

#include <QDebug>

TextEditingPluginContainer::TextEditingPluginContainer(QObject *parent)
    : QObject(parent)
{
    foreach (const QString &key, KoTextEditingRegistry::instance()->keys()) {
        KoTextEditingFactory *factory = KoTextEditingRegistry::instance()->value(key);
        Q_ASSERT(factory);
        if (m_textEditingPlugins.contains(factory->id())) {
            qWarning() << "Duplicate id for textEditingPlugin, ignoring one! (" << factory->id() << ")";
            continue;
        }
        KoTextEditingPlugin *plugin = factory->create();
        if (plugin) {
            m_textEditingPlugins.insert(factory->id(), plugin);
        }
    }
}

TextEditingPluginContainer::~TextEditingPluginContainer()
{
    qDeleteAll(m_textEditingPlugins);
    m_textEditingPlugins.clear();
}

KoTextEditingPlugin *TextEditingPluginContainer::spellcheck() const
{
    return plugin("spellcheck");
}
