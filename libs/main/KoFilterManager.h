/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
                 2000, 2001 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 2004 Nicolas Goutte <goutte@kde.org>

SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __KO_FILTER_MANAGER_H__
#define __KO_FILTER_MANAGER_H__

#include <QByteArray>
#include <QMap>
#include <QObject>

#include "KoFilterChain.h"
#include "KoFilterGraph.h"

#include "komain_export.h"
class KoDocument;
class KoProgressUpdater;

/**
 *  @brief The class managing all the filters.
 *
 *  This class manages all filters for a Calligra application. Normally
 *  you will not have to use it, since KoMainWindow takes care of loading
 *  and saving documents.
 *
 *  KoFilter
 *
 *  @author Kalle Dalheimer <kalle@kde.org>
 *  @author Torben Weis <weis@kde.org>
 *  @author Werner Trobin <trobin@kde.org>
 */
class KOMAIN_EXPORT KoFilterManager : public QObject
{
    Q_OBJECT
public:
    /**
     * This enum is used to distinguish the import/export cases
     */
    enum Direction {
        Import = 1,
        Export = 2
    };

    /**
     * Create a filter manager for a document
     */
    explicit KoFilterManager(KoDocument *document, KoProgressUpdater *progressUpdater = nullptr);

    /**
     * Create a filter manager for the Shape Collection docker.
     * @param mimeType the mimetype to import to.
     */
    explicit KoFilterManager(const QByteArray &mimeType);

    /**
     * Create a filter manager for a filter which wants to embed something.
     * The url it passes is the file to convert, obviously. You cannot use
     * the @ref importDocument() method -- use @ref exportDocument() to convert
     * the file to the destination mimetype you prefer.
     *
     * @param url The file you want to export
     * @param mimetypeHint The mimetype of the file you want to export. You have
     *        to specify this information only if the automatic detection will
     *        fail because e.g. you saved an embedded stream to a *.tmp file.
     *        Most likely you do not have to care about that.
     * @param parentChain The parent filter chain of this filter manager. Used
     *        to allow embedding for filters. Most likely you do not have to care.
     */
    explicit KoFilterManager(const QString &url, const QByteArray &mimetypeHint = QByteArray(), KoFilterChain *const parentChain = nullptr);

    ~KoFilterManager() override;

    /**
     * Imports the passed URL and returns the resultant filename
     * (most likely some file in /tmp).
     * @p documentMimeType gives importDocument a hint about what type
     * the document may be. It can be left empty.
     * The @p status variable signals the success/error of the conversion
     * If the QString which is returned isEmpty() and the status is OK,
     * then we imported the file directly into the document.
     */
    QString importDocument(const QString &url, const QString &documentMimeType, KoFilter::ConversionStatus &status);

    /**
     * @brief Exports the given file/document to the specified URL/mimetype.
     *
     * If @p mimeType is empty, then the closest matching Calligra part is searched
     * and when the method returns @p mimeType contains this mimetype.
     * Oh, well, export is a C++ keyword ;)
     */
    KoFilter::ConversionStatus exportDocument(const QString &url, QByteArray &mimeType);

    ///@name Static API
    //@{
    /**
     * Suitable for passing to KoFileDialog::setMimeTypeFilters. The default mime
     * gets set by the "users" of this method, as we do not have enough
     * information here.
     * Optionally, @p extraNativeMimeTypes are added after the native mimetype.
     */
    static QStringList mimeFilter(const QByteArray &mimetype, Direction direction, const QStringList &extraNativeMimeTypes = QStringList());

    /**
     * The same method as KoFilterManager::mimeFilter but suited for KoShell.
     * We do not need the mimetype, as we will simply use all available
     * %Calligra mimetypes. The Direction enum is omitted, as we only
     * call this for importing. When saving from KoShell we already
     * know the Calligra part we are using.
     */
    static QStringList mimeFilter();

    /**
     * Method used to check if that filter is available at all.
     * @note Slow, but cached
     */
    static bool filterAvailable(KoFilterEntry::Ptr entry);

    //@}

    /**
     * Set the filter manager is batch mode (no dialog shown)
     * instead of the interactive mode (dialog shown)
     */
    void setBatchMode(const bool batch);

    /**
     * Get if the filter manager is batch mode (true)
     * or in interactive mode (true)
     */
    bool getBatchMode(void) const;

    /**
     * Return the KoProgressUpdater or nullptr if there is none.
     **/
    KoProgressUpdater *progressUpdater() const;

    /**
     * Set up a progress updater.
     */
    void setProgressUpdater(KoProgressUpdater *updater);

private:
    // === API for KoFilterChains === (internal)
    // The friend methods are private in KoFilterChain and
    // just forward calls to the methods here. Should be
    // pretty safe.
    friend QString KoFilterChain::filterManagerImportFile() const;
    QString importFile() const
    {
        return m_importUrl;
    }
    friend QString KoFilterChain::filterManagerExportFile() const;
    QString exportFile() const
    {
        return m_exportUrl;
    }
    friend KoDocument *KoFilterChain::filterManagerKoDocument() const;
    KoDocument *document() const
    {
        return m_document;
    }
    friend int KoFilterChain::filterManagerDirection() const;
    int direction() const
    {
        return static_cast<int>(m_direction);
    }
    friend KoFilterChain *KoFilterChain::filterManagerParentChain() const;
    KoFilterChain *parentChain() const
    {
        return m_parentChain;
    }

    // Private API
    KoFilterManager(const KoFilterManager &rhs);
    KoFilterManager &operator=(const KoFilterManager &rhs);

    void importErrorHelper(const QString &mimeType, const bool suppressDialog = false);

    KoDocument *m_document;
    KoFilterChain *const m_parentChain;
    QString m_importUrl, m_exportUrl;
    QByteArray m_importUrlMimetypeHint; ///< suggested mimetype
    CalligraFilter::Graph m_graph;
    Direction m_direction;

    /// A static cache for the availability checks of filters
    static QMap<QString, bool> m_filterAvailable;

    class Private;
    Private *const d;
};

#endif // __KO_FILTER_MANAGER_H__
