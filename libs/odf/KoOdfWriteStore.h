/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KOODFWRITESTORE_H
#define KOODFWRITESTORE_H

class QIODevice;
class KoXmlWriter;
class KoStore;

/**
 * Helper class around KoStore for writing out ODF files.
 * This class helps solving the problem that automatic styles must be before
 * the body, but it's easier to iterate over the application's objects only
 * once. So we open a KoXmlWriter into a memory buffer, write the body into it,
 * collect automatic styles while doing that, write out automatic styles,
 * and then copy the body XML from the buffer into the real KoXmlWriter.
 *
 * The typical use of this class is therefore:
 *   - write body into bodyWriter() and collect auto styles
 *   - write auto styles into contentWriter()
 *   - call closeContentWriter()
 *   - write other files into the store (styles.xml, settings.xml etc.)
 *
 *
 * TODO: maybe we could encapsulate a bit more things, to e.g. handle
 * adding manifest entries automatically.
 *
 * @author: David Faure <faure@kde.org>
 */
#include "koodf_export.h"

class KOODF_EXPORT KoOdfWriteStore
{
public:
    /// @param store recontents the property of the caller
    explicit KoOdfWriteStore(KoStore *store);

    ~KoOdfWriteStore();

    /**
     * Return an XML writer for saving Oasis XML into the device @p dev,
     * including the XML processing instruction,
     * and the root element with all its namespaces.
     * You can add more namespaces afterwards with addAttribute.
     *
     * @param dev the device into which the XML will be written.
     * @param rootElementName the tag name of the root element.
     *    This is either office:document, office:document-content,
     *    office:document-styles, office:document-meta or office:document-settings
     * @return the KoXmlWriter instance. It becomes owned by the caller, which
     * must delete it at some point.
     *
     * Once done with writing the contents of the root element, you
     * will need to call endElement(); endDocument(); before destroying the KoXmlWriter.
     */
    static KoXmlWriter *createOasisXmlWriter(QIODevice *dev, const char *rootElementName);

    KoStore *store() const;

    /**
     * Open contents.xml for writing and return the KoXmlWriter
     */
    KoXmlWriter *contentWriter();

    /**
     * Open another KoXmlWriter for writing out the contents
     * into a temporary file, to collect automatic styles while doing that.
     */
    KoXmlWriter *bodyWriter();

    /**
     * This will copy the body into the content writer,
     * delete the bodyWriter and the contentWriter, and then
     * close contents.xml.
     */
    bool closeContentWriter();

    // For other files in the store, use open/addManifestEntry/KoStoreDevice/createOasisXmlWriter/close

    /**
     * Create and return a manifest writer. It will write to a memory buffer.
     */
    KoXmlWriter *manifestWriter(const char *mimeType);

    /**
     * Return the manifest writer. It has to be created by manifestWriter( mimeType ) before you can use
     * this function.
     */
    KoXmlWriter *manifestWriter();

    /**
     * Close the manifest writer.
     * @param writeMainfest If true then on closing we also write the contents to the manifest.xml else
     *    we only close the writer and don't write the content to the manifest.xml
     */
    bool closeManifestWriter(bool writeMainfest = true);

private:
    struct Private;
    Private *const d;
};

#endif /* KOODFWRITESTORE_H */
