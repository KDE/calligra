/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2007 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoDocumentEntry.h"

#include "KoDocument.h"
#include "KoFilter.h"
#include "KoPart.h"
#include <MainDebug.h>

#include <KoPluginLoader.h>

#include <KPluginFactory>

#include <QCoreApplication>

#include <limits.h> // UINT_MAX

KoDocumentEntry::KoDocumentEntry() = default;

KoDocumentEntry::KoDocumentEntry(const KPluginMetaData &metaData)
    : m_metaData(metaData)
{
}

KoDocumentEntry::~KoDocumentEntry() = default;

QJsonObject KoDocumentEntry::metaData() const
{
    return m_metaData.isValid() ? m_metaData.rawData() : QJsonObject();
}

QString KoDocumentEntry::fileName() const
{
    return m_metaData.isValid() ? m_metaData.fileName() : QString();
}

/**
 * @return TRUE if the service pointer is null
 */
bool KoDocumentEntry::isEmpty() const
{
    return !m_metaData.isValid();
}

/**
 * @return name of the associated service
 */
QString KoDocumentEntry::name() const
{
    return m_metaData.name();
}

/**
 *  Mimetypes (and other service types) which this document can handle.
 */
QStringList KoDocumentEntry::mimeTypes() const
{
    return m_metaData.mimeTypes();
}

/**
 *  @return TRUE if the document can handle the requested mimetype.
 */
bool KoDocumentEntry::supportsMimeType(const QString &_mimetype) const
{
    return mimeTypes().contains(_mimetype);
}

KoPart *KoDocumentEntry::createKoPart(QString *errorMsg) const
{
    if (!m_metaData.isValid()) {
        return nullptr;
    }

    auto result = KPluginFactory::instantiatePlugin<KoPart>(m_metaData, nullptr, {});
    if (!result.plugin) {
        if (errorMsg)
            *errorMsg = result.errorString;
        return nullptr;
    }

    return result.plugin;
}

KoDocumentEntry KoDocumentEntry::queryByMimeType(const QString &mimetype)
{
    QList<KoDocumentEntry> vec = query(mimetype);

    if (vec.isEmpty()) {
        warnMain << "Got no results with " << mimetype;
        // Fallback to the old way (which was probably wrong, but better be safe)
        vec = query(mimetype);

        if (vec.isEmpty()) {
            // Still no match. Either the mimetype itself is unknown, or we have no service for it.
            // Help the user debugging stuff by providing some more diagnostics
            // if (!KServiceType::serviceType(mimetype)) {
            //    errorMain << "Unknown Calligra MimeType " << mimetype << "." << Qt::endl;
            //    errorMain << "Check your installation (for instance, run 'kde4-config --path mime' and check the result)." << Qt::endl;
            //} else {
            //    errorMain << "Found no Calligra part able to handle " << mimetype << "!" << Qt::endl;
            //    errorMain << "Check your installation (does the desktop file have X-KDE-NativeMimeType and Calligra/Part, did you install Calligra in a
            //    different prefix than KDE, without adding the prefix to /etc/kderc ?)" << Qt::endl;
            //}
            return KoDocumentEntry();
        }
    }

    // Filthy hack alert -- this'll be properly fixed in the mvc branch.
    if (qApp->applicationName() == "flow" && vec.size() == 2) {
        return KoDocumentEntry(vec[1]);
    }

    return KoDocumentEntry(vec[0]);
}

QList<KoDocumentEntry> KoDocumentEntry::query(const QString &mimetype)
{
    QList<KoDocumentEntry> lst;

    // Query the trader
    const auto metaDatas = KoPluginLoader::pluginLoaders(QStringLiteral("calligra/parts"), mimetype);

    for (const auto &metadata : metaDatas) {
        lst.append(KoDocumentEntry(metadata));
    }

    if (lst.count() > 1 && !mimetype.isEmpty()) {
        warnMain << "KoDocumentEntry::query " << mimetype << " got " << lst.count() << " offers!";
        foreach (const KoDocumentEntry &entry, lst) {
            warnMain << entry.name();
        }
    }

    return lst;
}
