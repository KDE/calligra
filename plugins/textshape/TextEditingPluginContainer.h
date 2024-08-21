/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef TEXTEDITINGPLUGINCONTAINER_H
#define TEXTEDITINGPLUGINCONTAINER_H

#include <QHash>
#include <QObject>
#include <QString>
#include <QVariant>

class KoTextEditingPlugin;

/// This class holds on to the text editing plugins.
/// the goal of this class is to have one plugin instance per calligra-document
/// instead of one per tool.
class TextEditingPluginContainer : public QObject
{
    Q_OBJECT
public:
    enum ResourceManagerId { ResourceId = 345681743 };

    explicit TextEditingPluginContainer(QObject *parent = nullptr);
    ~TextEditingPluginContainer() override;

    KoTextEditingPlugin *spellcheck() const;

    KoTextEditingPlugin *plugin(const QString &pluginId) const
    {
        if (m_textEditingPlugins.contains(pluginId)) {
            return m_textEditingPlugins.value(pluginId);
        }
        return nullptr;
    }

    QList<KoTextEditingPlugin *> values() const
    {
        return m_textEditingPlugins.values();
    }

private:
    QHash<QString, KoTextEditingPlugin *> m_textEditingPlugins;
};

Q_DECLARE_METATYPE(TextEditingPluginContainer *)

#endif
