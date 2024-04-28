/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2005 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2007-2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2007 Sebastian Sauer <mail@dipe.org>
 * SPDX-FileCopyrightText: 2007-2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWODFLOADER_H
#define KWODFLOADER_H

#include "KoXmlReaderForward.h"
#include "Words.h"

#include <KoTextLoader.h>
#include <QPointer>

class KWDocument;
class KoOdfReadStore;
class KoOdfLoadingContext;
class KoShapeLoadingContext;
class KWPageStyle;
class QTextDocument;
class QTextCursor;

/**
 * Class that has a lot of the OpenDocument (ODF) loading code for Words.
 */
class KWOdfLoader : public QObject
{
    Q_OBJECT
public:
    /**
     * Constructor
     * @param document the document this loader will work for.
     */
    explicit KWOdfLoader(KWDocument *document);
    ~KWOdfLoader() override;

    KWDocument *document() const;

    /**
     *  @brief Loads an OASIS OpenDocument from a store.
     *  This implements the KoDocument::loadOdf method.
     */
    bool load(KoOdfReadStore &odfStore);

Q_SIGNALS:
    /**
     * This signal is emitted during loading with a percentage within 1-100 range
     * \param percent the progress as a percentage
     */
    void progressUpdate(int percent);

private:
    enum HFLoadType { LoadHeader, LoadFooter };

    void loadSettings(const KoXmlDocument &settings, QTextDocument *textDoc);
    void loadMasterPageStyles(KoShapeLoadingContext &context);
    void loadHeaderFooter(KoShapeLoadingContext &context, KWPageStyle &pageStyle, const KoXmlElement &masterPageStyle, HFLoadType headerFooter);
    void loadFinished(KoOdfLoadingContext &context, QTextCursor &cursor);

    /// helper function to create a KWTextFrameSet+KWFrame for a header/footer.
    void loadHeaderFooterFrame(KoShapeLoadingContext &context, const KWPageStyle &pageStyle, const KoXmlElement &elem, Words::TextFrameSetType fsType);

private:
    /// The Words document.
    QPointer<KWDocument> m_document;
};

#endif
