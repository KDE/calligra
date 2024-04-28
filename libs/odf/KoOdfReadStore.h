/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KOODFREADSTORE_H
#define KOODFREADSTORE_H

#include "KoXmlReaderForward.h"
#include "koodf_export.h"

class QString;
class QIODevice;
class KoStore;
class KoOdfStylesReader;

/**
 * Helper class around KoStore for reading out ODF files.
 *
 * The class loads and parses files from the KoStore.
 *
 * @author: David Faure <faure@kde.org>
 */
class KOODF_EXPORT KoOdfReadStore
{
public:
    /// @param store recontents the property of the caller
    explicit KoOdfReadStore(KoStore *store);

    ~KoOdfReadStore();

    /**
     * Get the store
     */
    KoStore *store() const;

    /**
     * Get the styles
     *
     * To get a usable result loadAndParse( QString ) has to be called first.
     *
     * @return styles
     */
    KoOdfStylesReader &styles();

    /**
     * Get the content document
     *
     * To get a usable result loadAndParse( QString ) has to be called first.
     *
     * This gives you the content of the content.xml file
     */
    KoXmlDocument contentDoc() const;

    /**
     * Get the settings document
     *
     * To get a usable result loadAndParse( QString ) has to be called first.
     *
     * This gives you the content of the settings.xml file
     */
    KoXmlDocument settingsDoc() const;

    /**
     * Load and parse
     *
     * This function loads and parses the content.xml, styles.xml and the settings.xml
     * file in the store. The styles are already parsed.
     *
     * After this function is called you can access the data via
     * styles()
     * contentDoc()
     * settingsDoc()
     *
     * @param errorMessage The errorMessage is set in case an error is encountered.
     * @return true if loading and parsing was successful, false otherwise. In case of an error
     * the errorMessage is updated accordingly.
     */
    bool loadAndParse(QString &errorMessage);

    /**
     * Load a file from an odf store
     */
    bool loadAndParse(const QString &fileName, KoXmlDocument &doc, QString &errorMessage);

    /**
     * Load a file and parse from a QIODevice
     * filename argument is just used for debug message
     */
    static bool loadAndParse(QIODevice *fileDevice, KoXmlDocument &doc, QString &errorMessage, const QString &fileName);

private:
    class Private;
    Private *const d;
};

#endif /* KOODFREADSTORE_H */
