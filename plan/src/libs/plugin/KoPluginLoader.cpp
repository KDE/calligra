/* This file is part of the KDE project
 * Copyright (c) 2006 Boudewijn Rempt <boud@valdyas.org>
 * Copyright (c) 2007 Thomas Zander <zander@kde.org>
 * Copyright (c) 2016 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KoPluginLoader.h"

#include <config.h> // CALLIGRA_OLD_PLUGIN_METADATA

#include <KConfig>
#include <KSharedConfig>
#include <KConfigGroup>
#include <KPluginFactory>
#include <KPluginLoader>

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

    QList<QPluginLoader *> offers = KoPluginLoader::pluginLoaders(directory);
    QList<QPluginLoader *> plugins;
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
        foreach(QPluginLoader *loader, offers) {
            QJsonObject json = loader->metaData().value("MetaData").toObject();
            json = json.value("KPlugin").toObject();
            const QString pluginName = json.value("Id").toString();
            if (pluginName.isEmpty()) {
                warnPlugin << "Loading plugin" << loader->fileName() << "failed, has no X-KDE-PluginInfo-Name.";
                continue;
            }
            if (whiteList.contains(pluginName)) {
                plugins.append(loader);
            } else if (!firstStart && !knownList.contains(pluginName)) { // also load newly installed plugins.
                plugins.append(loader);
                configChanged = true;
            } else {
                blacklist << pluginName;
            }
        }
    } else {
        plugins = offers;
    }

    QMap<QString, QPluginLoader *> serviceNames;
    foreach(QPluginLoader *loader, plugins) {
        if (serviceNames.contains(loader->fileName())) { // duplicate
            QJsonObject json2 = loader->metaData().value("MetaData").toObject();
            QVariant pluginVersion2 = json2.value("X-Flake-PluginVersion").toVariant();
            if (pluginVersion2.isNull()) { // just take the first one found...
                continue;
            }
            QPluginLoader *currentLoader = serviceNames.value(loader->fileName());
            QJsonObject json = currentLoader->metaData().value("MetaData").toObject();
            QVariant pluginVersion = json.value("X-Flake-PluginVersion").toVariant();
            if (!(pluginVersion.isNull() || pluginVersion.toInt() < pluginVersion2.toInt())) {
                continue; // replace the old one with this one, since its newer.
            }
        }
        serviceNames.insert(loader->fileName(), loader);
    }

    QList<QString> whiteList;
    foreach(QPluginLoader *loader, serviceNames) {
        KPluginFactory *factory = qobject_cast<KPluginFactory *>(loader->instance());
        QObject *plugin = factory->create<QObject>(owner ? owner : pluginLoaderInstance, QVariantList());
        if (plugin) {
            QJsonObject json = loader->metaData().value("MetaData").toObject();
            json = json.value("KPlugin").toObject();
            const QString pluginName = json.value("Id").toString();
            whiteList << pluginName;
            debugPlugin << "Loaded plugin" << loader->fileName() << owner;
            if (!owner) {
                delete plugin;
            }
        } else {
            warnPlugin << "Loading plugin" << loader->fileName() << "failed, " << loader->errorString();
        }
    }

    if (configChanged && config.whiteList && config.blacklist && config.group) {
        KConfigGroup configGroup(KSharedConfig::openConfig(), config.group);
        configGroup.writeEntry(config.whiteList, whiteList);
        configGroup.writeEntry(config.blacklist, blacklist);
    }

    qDeleteAll(offers);
}

QList<KPluginFactory *> KoPluginLoader::instantiatePluginFactories(const QString & directory)
{
    QList<KPluginFactory *> pluginFactories;

    const QList<QPluginLoader *> offers = KoPluginLoader::pluginLoaders(directory);

    foreach(QPluginLoader *pluginLoader, offers) {
        QObject* pluginInstance = pluginLoader->instance();
        if (!pluginInstance) {
            warnPlugin << "Loading plugin" << pluginLoader->fileName() << "failed, " << pluginLoader->errorString();
            continue;
        }
        KPluginFactory *factory = qobject_cast<KPluginFactory *>(pluginInstance);
        if (factory == 0) {
            warnPlugin << "Expected a KPluginFactory, got a" << pluginInstance->metaObject()->className();
            delete pluginInstance;
            continue;
        }

        pluginFactories.append(factory);
    }
    qDeleteAll(offers);

    return pluginFactories;
}

QList<QPluginLoader *> KoPluginLoader::pluginLoaders(const QString &directory, const QString &mimeType)
{
    QList<QPluginLoader *>list;
    KPluginLoader::forEachPlugin(directory, [&](const QString &pluginPath) {
        debugPlugin << "Trying to load" << pluginPath;
        QPluginLoader *loader = new QPluginLoader(pluginPath);
        QJsonObject metaData = loader->metaData().value("MetaData").toObject();

        if (metaData.isEmpty()) {
            debugPlugin << pluginPath << "has no MetaData!";
            return;
        }

        if (!mimeType.isEmpty()) {
#ifdef CALLIGRA_OLD_PLUGIN_METADATA
            QStringList mimeTypes = metaData.value("MimeType").toString().split(';');
            mimeTypes += metaData.value("X-KDE-ExtraNativeMimeTypes").toString().split(QLatin1Char(','));
#else
            QJsonObject pluginData = metaData.value("KPlugin").toObject();
            QStringList mimeTypes = pluginData.value("MimeTypes").toVariant().toStringList();
            mimeTypes += metaData.value("X-KDE-ExtraNativeMimeTypes").toVariant().toStringList();
#endif
            mimeTypes += metaData.value("X-KDE-NativeMimeType").toString();
            if (! mimeTypes.contains(mimeType)) {
                return;
            }
        }

        list.append(loader);
    });

    return list;
}
#include "KoPluginLoader.moc"
