/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2000-2006 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2005-2011 Sebastian Sauer <mail@dipe.org>
 * SPDX-FileCopyrightText: 2005-2006, 2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWFRAMELAYOUT_H
#define KWFRAMELAYOUT_H

#include "../KWPageStyle.h"
#include "../Words.h"

#include "../words_export.h"

#include <QList>
#include <QObject>
#include <QRectF>

class KWPageManager;
class KWFrameSet;
class KWTextFrameSet;
class KWPage;
class KWDocument;
class KWFrame;

class KoShape;

/**
 * This class is a controller class for frame layouting.
 *
 * For automatically generated frames this class creates new frames and places them on a page.
 * For normal frames this class will be able to (re)move them on page resize/deletion or create
 * followup frames for new pages.
 *
 * NOTE This class only handles header, footer and the mainframes. It does NOT handle any
 * other framesets (neither Words::OtherFrameSet nor Words::OtherTextFrameSet). This class
 * doesn't even know about them.
 */
class WORDS_TEST_EXPORT KWFrameLayout : public QObject
{
    Q_OBJECT
public:
    /**
     * Create a new frameLayout instance
     * @param pageManager the manager of pages
     * @param frameSets all the framesets registered in the document.
     */
    KWFrameLayout(const KWPageManager *pageManager, const QList<KWFrameSet *> &frameSets);
    ~KWFrameLayout() override = default;
    /**
     * As soon as a new page is created you should call this method to auto-create all frames
     * needed on that new page, with a call to layoutFramesOnPage done directly afterwards.
     * Note that the creation of a page-spread counts as one page, even though it takes two
     *  page numbers.
     * @param pageNumber the new page number.
     */
    void createNewFramesForPage(int pageNumber);

    /**
     * Whenever the page layout is changed, call this method to move and resize the frames.
     * Note that a page-spread counts as one page, even though it takes two page numbers.
     * @param pageNumber the number of the page to re-layout.
     */
    void layoutFramesOnPage(KWPage page, int pageNumber);

    static void proposeShapeMove(const KoShape *shape, QPointF &delta, const KWPage &page);

    /// Set the document to be passed to new instances of the KWTextFrameSet
    void setDocument(KWDocument *document)
    {
        m_document = document;
    }

    /// return the main text frameset of the document
    KWTextFrameSet *mainFrameSet() const;

    QList<KoShape *> sequencedShapesOnPage(const QRectF &page) const;
    QList<KoShape *> sequencedShapesOnPage(int pageNumber) const;
    QList<KoShape *> sequencedShapesOnPage(KWFrameSet *fs, const QRectF &page) const;
    QList<KoShape *> sequencedShapesOnPage(KWFrameSet *fs, int pageNumber) const;

    KoShape *sequencedShapeOn(KWFrameSet *fs, KWPage page) const;
    KoShape *sequencedShapeOn(KWFrameSet *fs, int pageNumber) const;
    QList<KoShape *> sequencedShapesOn(KWFrameSet *fs, int pageNumber) const;

    QList<KWTextFrameSet *> getFrameSets(const KWPageStyle &pageStyle) const;
    KWTextFrameSet *getFrameSet(Words::TextFrameSetType type, const KWPageStyle &pageStyle) const;

    KWFrame *createCopyFrame(KWFrameSet *fs, const KWPage &page);

Q_SIGNALS:
    /**
     * Signal emitted when a frameset is created
     */
    void newFrameSet(KWFrameSet *fs);
    /**
     * Signal emitted when a frameset is removed
     */
    void removedFrameSet(KWFrameSet *fs);

private Q_SLOTS:
    // called when a shape from the main text is removed to check if we should clear the page of other auto-created shape
    void mainShapeRemoved(KoShape *shape);

private:
    struct FrameSets {
        FrameSets()
            : oddHeaders(nullptr)
            , evenHeaders(nullptr)
            , oddFooters(nullptr)
            , evenFooters(nullptr)
            , pageBackground(nullptr)
        {
        }
        KWTextFrameSet *oddHeaders;
        KWTextFrameSet *evenHeaders;
        KWTextFrameSet *oddFooters;
        KWTextFrameSet *evenFooters;
        KWTextFrameSet *pageBackground;
    };

    KoShape *createTextShape(const KWPage &page);

    /**
     * Get or create a text frameset of the specified \a type on the specified \a page.
     * A page follows a KWPageStyle and we have individual framesets for each style.
     * This means that in a document there can be many framesets of type odd-header, at
     * most one for each page style. The same for all other text framesets.
     * This method figures out which frameset fits with the page and returns it, or
     * if it did not exist yet it creates it.
     * \note the main text frameset is consistent across all pages and page styles.
     */
    KWTextFrameSet *getOrCreate(Words::TextFrameSetType type, const KWPage &page);

    void setup();
    bool shouldHaveHeaderOrFooter(int pageNumber, bool header, Words::TextFrameSetType *origin);
    bool shouldHaveHeaderOrFooter(int pageNumber, const KWPageStyle &pageStyle, bool header, Words::TextFrameSetType *origin);
    void cleanFrameSet(KWTextFrameSet *fs);

    const KWPageManager *m_pageManager;
    const QList<KWFrameSet *> &m_frameSets;
    QHash<KWPageStyle, FrameSets> m_pageStyles;

    KWTextFrameSet *m_maintext;
    KWFrameSet *m_backgroundFrameSet;
    KWDocument *m_document;

    bool m_setup;
};

#endif
