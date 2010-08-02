/* This file is part of the KDE project
 * Copyright (C) 2006-2008 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KWFRAMELAYOUT_H
#define KWFRAMELAYOUT_H

#include "KWord.h"

#include "kword_export.h"

#include <QList>
#include <QObject>
#include <QRectF>

class KWPageManager;
class KWFrameSet;
class KWTextFrameSet;
class KWPageStyle;
class KWPage;
class KWFrame;
class KWDocument;

class KoShape;

/**
 * This class is a controller class for frame layouting.
 * For automatically generated frames this class creates new frames and places them on a page.
 * For normal frames this class will be able to (re)move them on page resize/deletion or create
 * followup frames for new pages.
 */
class KWORD_TEST_EXPORT KWFrameLayout : public QObject
{
    Q_OBJECT
public:
    /**
     * Create a new frameLayout instance
     * @param pageManager the manager of pages
     * @param frameSets all the framesets registered in the document.
     */
    KWFrameLayout(const KWPageManager *pageManager, const QList<KWFrameSet*> &frameSets);
    ~KWFrameLayout() {}
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
    void layoutFramesOnPage(int pageNumber);
    //void relayoutFrames(old layout, new layout); // per page ? Or per doc?
    /**
     *  delete any unneeded header/footer frames (but not their contents) based on
     * the document settings
     */
    void cleanupHeadersFooters();

    /**
     * For the one frameset create all the frames that would be auto-placed
     * on the target pagenumber if the page was appended.  Will only place
     * frames if the already present frames in the frameset plus the
     * newFrameBehavior state it should.
     * @param fs the frameset to attach a new textframe to
     * @param pageNumber the (already existing) page where the new frame(s) will show
     */
    void createNewFrameForPage(KWTextFrameSet *fs, int pageNumber);

    /// Set the document to be passed to new instances of the KWTextFrameSet
    void setDocument(KWDocument *document) {
        m_document = document;
    }

    /// return the main text frameset of the document
    KWTextFrameSet *mainFrameSet() const;

signals:
    /**
     * Signal emitted when a frameset is created
     */
    void newFrameSet(KWFrameSet *fs);
    /**
     * Signal emitted when a frameset is removed
     */
    void removedFrameSet(KWFrameSet *fs);

private slots:
    // called when a frame from the main text is removed to check if we should clear the page of other auto-created frames
    void mainframeRemoved(KWFrame *frame);

private:
    friend class TestFrameLayout;
    struct FrameSets {
        FrameSets() : oddHeaders(0), evenHeaders(0), oddFooters(0), evenFooters(0), pageBackground(0) {}
        KWTextFrameSet *oddHeaders;
        KWTextFrameSet *evenHeaders;
        KWTextFrameSet *oddFooters;
        KWTextFrameSet *evenFooters;
        KWTextFrameSet *pageBackground;
    };

    KoShape *createTextShape(const KWPage &page);
    KWTextFrameSet *getOrCreate(KWord::TextFrameSetType type, const KWPage &page);
    QList<KWFrame *> framesInPage(const QRectF &page);
    void setup();
    bool shouldHaveHeaderOrFooter(int pageNumber, bool header, KWord::TextFrameSetType *origin);
    bool hasFrameOn(KWTextFrameSet *fs, int pageNumber);
    void cleanFrameSet(KWTextFrameSet *fs);
    KWFrame* createCopyFrame(KWFrameSet *fs, const KWPage &page);


    const KWPageManager *m_pageManager;
    const QList<KWFrameSet *> &m_frameSets;
    QHash<KWPageStyle, FrameSets> m_pageStyles;

    KWTextFrameSet *m_maintext;

    const KWDocument *m_document;

    bool m_setup;
};

#endif
