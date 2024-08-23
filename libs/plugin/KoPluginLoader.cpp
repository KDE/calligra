/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2016 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPluginLoader.h"

#include <KConfig>
#include <KConfigGroup>
#include <KPluginFactory>
#include <KPluginMetaData>
#include <KSharedConfig>

#include <QCoreApplication>
#include <QDebug>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QPluginLoader>

const QLoggingCategory &PLUGIN_LOG()
{
    static const QLoggingCategory category("calligra.lib.plugin");
    return category;
}

#define debugPlugin qCDebug(PLUGIN_LOG)
#define warnPlugin qCWarning(PLUGIN_LOG)

class KoPluginLoaderImpl : public QObject
{
    Q_OBJECT
public:
    QStringList loadedDirectories;
};

Q_GLOBAL_STATIC(KoPluginLoaderImpl, pluginLoaderInstance)

void KoPluginLoader::load(const QString &directory, const PluginsConfig &config, QObject *owner)
{
    // Don't load the same plugins again
    if (pluginLoaderInstance->loadedDirectories.contains(directory)) {
        return;
    }
    pluginLoaderInstance->loadedDirectories << directory;

    auto offers = KoPluginLoader::pluginLoaders(directory);
    QVector<KPluginMetaData> plugins;
    bool configChanged = false;
    QList<QString> blacklist; // what we will save out afterwards
    if (!config.whiteList.isEmpty() && !config.blacklist.isEmpty() && !config.group.isEmpty()) {
        debugPlugin << "Loading" << directory << "with checking the config";
        KConfigGroup configGroup(KSharedConfig::openConfig(), config.group);
        QList<QString> whiteList = configGroup.readEntry(config.whiteList, config.defaults);
        QList<QString> knownList;

        // if there was no list of defaults; all plugins are loaded.
        const bool firstStart = !config.defaults.isEmpty() && !configGroup.hasKey(config.whiteList);
        knownList = configGroup.readEntry(config.blacklist, knownList);
        if (firstStart) {
            configChanged = true;
        }
        for (const KPluginMetaData &metaData : std::as_const(offers)) {
            const QString pluginName = metaData.pluginId();
            if (pluginName.isEmpty()) {
                warnPlugin << "Loading plugin" << metaData.fileName() << "failed, has no X-KDE-PluginInfo-Name.";
                continue;
            }
            if (whiteList.contains(pluginName)) {
                plugins.append(metaData);
            } else if (!firstStart && !knownList.contains(pluginName)) { // also load newly installed plugins.
                plugins.append(metaData);
                configChanged = true;
            } else {
                blacklist << pluginName;
            }
        }
    } else {
        plugins = offers;
    }

    QMap<QString, KPluginMetaData> serviceNames;
    for (const KPluginMetaData &loader : std::as_const(plugins)) {
        if (serviceNames.contains(loader.fileName())) { // duplicate
            const QJsonObject json2 = loader.rawData();
            const QVariant pluginVersion2 = json2.value(QLatin1StringView("X-Flake-PluginVersion")).toVariant();
            if (pluginVersion2.isNull()) { // just take the first one found...
                continue;
            }
            const KPluginMetaData currentLoader = serviceNames.value(loader.fileName());
            const QJsonObject json = currentLoader.rawData();
            const QVariant pluginVersion = json.value(QLatin1StringView("X-Flake-PluginVersion")).toVariant();
            if (!pluginVersion.isNull() && pluginVersion.toInt() >= pluginVersion2.toInt()) {
                continue; // replace the old one with this one, since its newer.
            }
        }
        serviceNames.insert(loader.fileName(), loader);
    }

    QList<QString> whiteList;
    for (const KPluginMetaData &metaData : std::as_const(serviceNames)) {
        auto result = KPluginFactory::instantiatePlugin<QObject>(metaData, owner ? owner : pluginLoaderInstance, {});
        if (result.plugin) {
            const QString pluginName = metaData.pluginId();
            whiteList << pluginName;
            debugPlugin << "Loaded plugin" << metaData.fileName() << owner;
            if (!owner) {
                delete result.plugin;
            }
        } else {
            warnPlugin << "Loading plugin" << metaData.fileName() << "failed, " << result.errorString;
        }
    }

    if (configChanged && !config.whiteList.isEmpty() && !config.blacklist.isEmpty() && !config.group.isEmpty()) {
        KConfigGroup configGroup(KSharedConfig::openConfig(), config.group);
        configGroup.writeEntry(config.whiteList, whiteList);
        configGroup.writeEntry(config.blacklist, blacklist);
    }
}

QList<KPluginFactory *> KoPluginLoader::instantiatePluginFactories(const QString &directory)
{
    QList<KPluginFactory *> pluginFactories;

    const QVector<KPluginMetaData> offers = KoPluginLoader::pluginLoaders(directory);

    for (const KPluginMetaData &metaData : offers) {
        auto result = KPluginFactory::loadFactory(metaData);

        if (!result.plugin) {
            warnPlugin << "Loading plugin" << metaData.fileName() << "failed, " << result.errorString;
            continue;
        }

        pluginFactories.append(result.plugin);
    }

    return pluginFactories;
}

QVector<KPluginMetaData> KoPluginLoader::pluginLoaders(const QString &directory, const QString &mimeType)
{
    return KPluginMetaData::findPlugins(directory, [&](const KPluginMetaData &metaData) -> bool {
        debugPlugin << "Trying to load" << metaData.fileName();

        if (!metaData.isValid()) {
            debugPlugin << metaData.fileName() << "is invalid or has no MetaData!";
            return false;
        }

        if (!mimeType.isEmpty()) {
            QStringList mimeTypes = metaData.mimeTypes();
            mimeTypes += metaData.rawData().value(QLatin1StringView("X-KDE-ExtraNativeMimeTypes")).toString().split(',');
            mimeTypes += metaData.rawData().value(QLatin1StringView("X-KDE-NativeMimeType")).toString();
            if (!mimeTypes.contains(mimeType)) {
                return false;
            }
        }
        return true;
    });
}
#include "KoPluginLoader.moc"
