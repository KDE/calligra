/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2016 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPluginLoader.h"

#include <KConfig>
#include <KSharedConfig>
#include <KConfigGroup>
#include <KPluginFactory>
#include <KPluginLoader>
#include <KPluginMetaData>

#include <QCoreApplication>
#include <QJsonObject>
#include <QPluginLoader>
#include <QLoggingCategory>
#include <QDebug>


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


void KoPluginLoader::load(const QString & directory, const PluginsConfig &config, QObject* owner)
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
    if (config.whiteList && config.blacklist && config.group) {
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
        for(KPluginMetaData metaData : offers) {
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
    for (KPluginMetaData loader : plugins) {
        if (serviceNames.contains(loader.fileName())) { // duplicate
            QJsonObject json2 = loader.rawData().value("MetaData").toObject();
            QVariant pluginVersion2 = json2.value("X-Flake-PluginVersion").toVariant();
            if (pluginVersion2.isNull()) { // just take the first one found...
                continue;
            }
            KPluginMetaData currentLoader = serviceNames.value(loader.fileName());
            QJsonObject json = currentLoader.rawData().value("MetaData").toObject();
            QVariant pluginVersion = json.value("X-Flake-PluginVersion").toVariant();
            if (!(pluginVersion.isNull() || pluginVersion.toInt() < pluginVersion2.toInt())) {
                continue; // replace the old one with this one, since its newer.
            }
        }
        serviceNames.insert(loader.fileName(), loader);
    }

    QList<QString> whiteList;
    for (KPluginMetaData metaData : serviceNames) {
        auto result = KPluginFactory::instantiatePlugin<QObject >(metaData, owner ? owner : pluginLoaderInstance, {});
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

    if (configChanged && config.whiteList && config.blacklist && config.group) {
        KConfigGroup configGroup(KSharedConfig::openConfig(), config.group);
        configGroup.writeEntry(config.whiteList, whiteList);
        configGroup.writeEntry(config.blacklist, blacklist);
    }
}

QList<KPluginFactory *> KoPluginLoader::instantiatePluginFactories(const QString & directory)
{
    QList<KPluginFactory *> pluginFactories;

    const QVector<KPluginMetaData> offers = KoPluginLoader::pluginLoaders(directory);

    for(KPluginMetaData metaData : offers) {
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
            mimeTypes += metaData.rawData().value("X-KDE-ExtraNativeMimeTypes").toVariant().toStringList();
            mimeTypes += metaData.rawData().value("X-KDE-NativeMimeType").toString();
            if (! mimeTypes.contains(mimeType)) {
                return false;
            }
        }
        return true;
    });
}
#include "KoPluginLoader.moc"
