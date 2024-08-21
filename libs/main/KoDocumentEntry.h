/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2007 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __ko_document_entry_h__
#define __ko_document_entry_h__

#include <KPluginMetaData>
#include <QList>
#include <QSharedPointer>
#include <QString>

#include "komain_export.h"

class KoDocument;
class KoPart;

/**
 *  Represents an available Calligra component
 *  that supports the document interface.
 */
class KOMAIN_EXPORT KoDocumentEntry
{
public:
    /**
     * Represents an invalid entry (as returned by queryByMimeType for instance)
     */
    explicit KoDocumentEntry();
    /**
     * Represents a valid entry
     * @param metaData plugin metadata for the service
     */
    explicit KoDocumentEntry(const KPluginMetaData &metaData);
    ~KoDocumentEntry();

    QJsonObject metaData() const;

    QString fileName() const;

    /**
     * @return TRUE if the service pointer is null
     */
    bool isEmpty() const;

    /**
     * @return name of the associated service
     */
    QString name() const;

    /**
     *  Mimetypes (and other service types) which this document can handle.
     */
    QStringList mimeTypes() const;

    /**
     *  @return TRUE if the document can handle the requested mimetype.
     */
    bool supportsMimeType(const QString &_mimetype) const;

    /**
     *  Uses the factory of the component to create
     *  a part. If that is not possible, 0 is returned.
     */
    KoPart *createKoPart(QString *errorMsg = nullptr) const;

    /**
     *  This function will query ksycoca to find all available components for the requested mimetype.
     *  The result will only contain parts, which are embeddable into a document
     *
     *  @param mimetype is the MIME type expression as used by KTrader.
     *                 You can use it to set additional restrictions on the available
     *                 components.
     */
    static QList<KoDocumentEntry> query(const QString &mimetype = QString());

    /**
     *  This is a convenience function.
     *
     *  @return a document entry for the Calligra component that supports
     *          the requested mimetype and fits the user best.
     */
    static KoDocumentEntry queryByMimeType(const QString &mimetype);

private:
    KPluginMetaData m_metaData;
};

#endif
