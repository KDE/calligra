/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2007 David Faure <faure@kde.org>

SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoFilterEntry.h"

#include "KoDocument.h"
#include "KoFilter.h"

#include <KoPluginLoader.h>
#include <MainDebug.h>

#include <KPluginFactory>
#include <QFile>

#include <limits.h> // UINT_MAX

KoFilterEntry::KoFilterEntry(const KPluginMetaData &metaData)
    : m_metaData(metaData)
{
    QJsonObject metadata = metaData.rawData();
    import = metadata.value("X-KDE-Import").toString().split(',');
    export_ = metadata.value("X-KDE-Export").toString().split(',');
    int w = metadata.value("X-KDE-Weight").toInt();
    weight = w < 0 ? UINT_MAX : static_cast<unsigned int>(w);
    available = metadata.value("X-KDE-Available").toString();
}

KoFilterEntry::~KoFilterEntry() = default;

QString KoFilterEntry::fileName() const
{
    return m_metaData.fileName();
}

QList<KoFilterEntry::Ptr> KoFilterEntry::query()
{
    QList<KoFilterEntry::Ptr> lst;

    const auto datas = KoPluginLoader::pluginLoaders(QStringLiteral("calligra/formatfilters"));

    for (const auto &data : datas) {
        lst.append(KoFilterEntry::Ptr(new KoFilterEntry(data)));
    }

    return lst;
}

KoFilter *KoFilterEntry::createFilter(KoFilterChain *chain, QObject *parent)
{
    auto result = KPluginFactory::instantiatePlugin<KoFilter>(m_metaData, parent);
    if (!result.plugin) {
        return nullptr;
    }

    KoFilter *filter = result.plugin;
    filter->m_chain = chain;
    return filter;
}
