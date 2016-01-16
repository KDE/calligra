/* This file is part of the KDE project
 * Copyright (c) 2006 Boudewijn Rempt <boud@valdyas.org>
 * Copyright (c) 2007 Thomas Zander <zander@kde.org>
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

#include <KConfig>
#include <KSharedConfig>
#include <KConfigGroup>
#include <KPluginFactory>

#include <QCoreApplication>
#include <QDirIterator>
#include <QDir>
#include <QJsonObject>
#include <QJsonArray>
#include <QPluginLoader>
#include <QDebug>


class KoPluginLoaderImpl : public QObject
{
public:
    QString pluginPath;
    QStringList loadedServiceTypes;
};



Q_GLOBAL_STATIC(KoPluginLoaderImpl, pluginLoaderInstance)


void KoPluginLoader::load(const QString & serviceType, const PluginsConfig &config, QObject* owner)
{
    // Don't load the same plugins again
    if (pluginLoaderInstance->loadedServiceTypes.contains(serviceType)) {
        return;
    }
    pluginLoaderInstance->loadedServiceTypes << serviceType;

    QList<QPluginLoader *> offers = KoPluginLoader::pluginLoaders(serviceType);
    QList<QPluginLoader *> plugins;
    bool configChanged = false;
    QList<QString> blacklist; // what we will save out afterwards
    if (config.whiteList && config.blacklist && config.group) {
//        qDebug() << "Loading" << serviceType << "with checking the config";
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
                qWarning() << "Loading plugin" << loader->fileName() << "failed, has no X-KDE-PluginInfo-Name.";
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
//             qDebug() << "Loaded plugin" << loader->fileName() << owner;
            if (!owner) {
                delete plugin;
            }
        } else {
            qWarning() << "Loading plugin" << loader->fileName() << "failed, " << loader->errorString();
        }
    }

    if (configChanged && config.whiteList && config.blacklist && config.group) {
        KConfigGroup configGroup(KSharedConfig::openConfig(), config.group);
        configGroup.writeEntry(config.whiteList, whiteList);
        configGroup.writeEntry(config.blacklist, blacklist);
    }
}

QList<KPluginFactory *> KoPluginLoader::instantiatePluginFactories(const QString & serviceType)
{
    QList<KPluginFactory *> pluginFactories;

    const QList<QPluginLoader *> offers = KoPluginLoader::pluginLoaders(serviceType);

    foreach(QPluginLoader *pluginLoader, offers) {
        QObject* pluginInstance = pluginLoader->instance();
        if (!pluginInstance) {
            qWarning() << "Loading plugin" << pluginLoader->fileName() << "failed, " << pluginLoader->errorString();
            continue;
        }
        KPluginFactory *factory = qobject_cast<KPluginFactory *>(pluginInstance);
        if (factory == 0) {
            qWarning() << "Expected a KPluginFactory, got a" << pluginInstance->metaObject()->className();
            delete pluginInstance;
            continue;
        }

        pluginFactories.append(factory);
    }

    return pluginFactories;
}

QList<QPluginLoader *> KoPluginLoader::pluginLoaders(const QString &serviceType, const QString &mimeType)
{
    if (pluginLoaderInstance->pluginPath.isEmpty()) {

        QList<QDir> searchDirs;

        QDir appDir(qApp->applicationDirPath());
        appDir.cdUp();

        searchDirs << appDir;
        // help plugin trader find installed plugins when run from uninstalled tests
#ifdef CMAKE_INSTALL_PREFIX
        searchDirs << QDir(CMAKE_INSTALL_PREFIX);
#endif

        foreach(const QDir& dir, searchDirs) {
            foreach(const QString &entry, dir.entryList()) {
                QFileInfo info(dir, entry);
                if (info.isDir() && info.fileName().contains("lib")) {
                    QDir libDir(info.absoluteFilePath());

                    // on many systems this will be the actual lib dir (and calligra subdir contains plugins)
                    if (libDir.entryList(QStringList() << "calligra").size() > 0) {
                        pluginLoaderInstance->pluginPath = info.absoluteFilePath() + "/calligra";
                        break;
                    }

                    // on debian at least the actual libdir is a subdir named like "lib/x86_64-linux-gnu"
                    // so search there for the calligra subdir which will contain our plugins
                    foreach(const QString &subEntry, libDir.entryList()) {
                        QFileInfo subInfo(libDir, subEntry);
                        if (subInfo.isDir()) {
                            if (QDir(subInfo.absoluteFilePath()).entryList(QStringList() << "calligra").size() > 0) {
                                pluginLoaderInstance->pluginPath = subInfo.absoluteFilePath() + "/calligra";
                                break; // will only break inner loop so we need the extra check below
                            }
                        }
                    }

                    if (!pluginLoaderInstance->pluginPath.isEmpty()) {
                        break;
                    }
                }
            }

            if(!pluginLoaderInstance->pluginPath.isEmpty()) {
                break;
            }
        }
        //qDebug() << "KoPluginLoader will load its plugins from" << pluginLoaderInstance->pluginPath;

    }

    QList<QPluginLoader *>list;
    QDirIterator dirIter(pluginLoaderInstance->pluginPath, QDirIterator::Subdirectories);
    while (dirIter.hasNext()) {
        dirIter.next();
        if (dirIter.fileInfo().isFile()) {
            QPluginLoader *loader = new QPluginLoader(dirIter.filePath());
            QJsonObject json = loader->metaData().value("MetaData").toObject();

            if (json.isEmpty()) {
                //qDebug() << dirIter.filePath() << "has no json!";
            }
            if (!json.isEmpty()) {
                QJsonObject pluginData = json.value("KPlugin").toObject();
                if (!pluginData.value("ServiceTypes").toArray().contains(QJsonValue(serviceType))) {
                    continue;
                }

                if (!mimeType.isEmpty()) {
#ifdef OLD_PLUGIN_MIMETYPE_DATA
                    QStringList mimeTypes = json.value("MimeType").toString().split(';');
#else
                    QStringList mimeTypes = pluginData.value("MimeTypes").toVariant().toStringList();
#endif
                    mimeTypes += json.value("X-KDE-ExtraNativeMimeTypes").toString().split(',');
                    mimeTypes += json.value("X-KDE-NativeMimeType").toString();
                    if (! mimeTypes.contains(mimeType)) {
                        continue;
                    }
                }
                list.append(loader);
            }
        }

    }

    return list;
}
