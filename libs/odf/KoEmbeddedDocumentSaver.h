/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2011 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOEMBEDDEDDOCUMENTSAVER_H
#define KOEMBEDDEDDOCUMENTSAVER_H

#include "KoDocumentBase.h"
#include "koodf_export.h"

#include <QString>

class KoXmlWriter;

/**
 * This class is used to save embedded objects in ODF documents.
 *
 * @see KoEmbeddedFileSaver
 */
class KOODF_EXPORT KoEmbeddedDocumentSaver
{
public:
    KoEmbeddedDocumentSaver();
    ~KoEmbeddedDocumentSaver();

    /**
     * Get a unique file name with the given prefix, to be used as a name for an embedded file in the ODF store.
     * @param the prefix of the filename to be created.
     * return a unique file name for use in the odf store.
     */
    QString getFilename(const QString &prefix);

    /**
     * Adds the object specific attributes to the tag, but does NOT
     * write the content of the embedded document. Saving of the
     * embedded documents themselves is done in @ref save. This
     * function should be called from within KoDocumentBase::saveOdf.
     */
    void embedDocument(KoXmlWriter &writer, KoDocumentBase *doc);

    /**
     * Adds the object specific attributes to the tag, and queues the
     * file for saving into the store.
     *
     * However, it does NOT write the content of the embedded document
     * to the store. Saving of the embedded files themselves is done
     * in @ref saveEmbeddedFiles. This function should be called from
     * within saveOdf in a shape or a document.
     */
    void embedFile(KoXmlWriter &writer, const char *element, const QString &path, const QByteArray &mimeType, const QByteArray &contents);

    /**
     * Queues the file for saving into the store.
     *
     * Saving of the embedded files themselves is done in @ref
     * saveEmbeddedFiles. This function should be called from within
     * saveOdf in a shape or a document if you don't wish to have a
     * reference to the file within content.xml, e.g. when the file is
     * part of an embedded object with embedded files within it.
     */
    void saveFile(const QString &path, const QByteArray &mimeType, const QByteArray &contents);

    /**
     *
     */
    void saveManifestEntry(const QString &fullPath, const QString &mediaType, const QString &version = QString());

    /**
     * Save all embedded documents to the store.
     */
    bool saveEmbeddedDocuments(KoDocumentBase::SavingContext &documentContext);

private:
    class Private;
    Private *const d;
    Q_DISABLE_COPY(KoEmbeddedDocumentSaver)
};

#endif /* KOEMBEDDEDDOCUMENTSAVER_H */
