/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2000-2006 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2005-2011 Sebastian Sauer <mail@dipe.org>
 * SPDX-FileCopyrightText: 2005-2006, 2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWTEXTFRAMESET_H
#define KWTEXTFRAMESET_H

#include "../KWPageStyle.h"
#include "../words_export.h"
#include "KWFrameSet.h"

class QTextDocument;
class KWPageManager;
class KWDocument;
class KWRootAreaProviderBase;
class KWTextFrame;

/**
 * A frameset with a TextDocument backing it.
 */
class WORDS_EXPORT KWTextFrameSet : public KWFrameSet
{
    Q_OBJECT
public:
    /**
     * Constructor with a type of text specified
     * @param wordsDocument the document this frameset belongs to.
     * @param type the type of frameSet; this can indicate headers, footers etc.
     */
    explicit KWTextFrameSet(KWDocument *wordsDocument, Words::TextFrameSetType type = Words::OtherTextFrameSet);
    ~KWTextFrameSet() override;

    /// return the type of frameSet this is
    Words::TextFrameSetType textFrameSetType()
    {
        return m_textFrameSetType;
    }

    /// return the document with the text that belongs to this frameset.
    QTextDocument *document() const
    {
        return m_document;
    }

    /// return the rootAreaProvider that is responsible for providing rootArea's to the KoTextDocumentLayout for this frameset.
    KWRootAreaProviderBase *rootAreaProvider() const
    {
        return m_rootAreaProvider;
    }

    /// return the pageManager for this frameSet
    KWPageManager *pageManager()
    {
        return m_pageManager;
    }
    /// return the document for this frameset
    KWDocument *wordsDocument()
    {
        return m_wordsDocument;
    }

    void setPageStyle(const KWPageStyle &style);
    const KWPageStyle &pageStyle() const;

protected:
    friend class TestTextFrameSorting;
    friend class TestTextFrameManagement;

    void setupShape(KoShape *shape) override;
    void cleanupShape(KoShape *shape) override;

private:
    void setupDocument();

private:
    QTextDocument *m_document;
    Words::TextFrameSetType m_textFrameSetType;
    KWPageManager *m_pageManager;
    KWDocument *m_wordsDocument;
    KWPageStyle m_pageStyle; // the page Style this frameset is associated with.
    KWRootAreaProviderBase *m_rootAreaProvider;
};

#endif
