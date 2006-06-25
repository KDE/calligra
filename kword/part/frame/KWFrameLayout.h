/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
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

#ifndef KWBASICFRAMELAYOUT_H
#define KWBASICFRAMELAYOUT_H

#include "KWord.h"

#include "kword_export.h"

#include <QList>
#include <QObject>
#include <QRectF>

class KWPageManager;
class KWFrameSet;
class KWTextFrameSet;
class KWPageSettings;
class KWPage;
class KWFrame;

class KoTextShape;

/**
 * This class is a controller class for frame layouting.
 * For automatically generated frames this class creates new frames and places them on a page.
 * For normal frames this class will be able to (re)move them on page resize/deletion or create
 * followup frames for new pages.
 */
class KWORD_TEST_EXPORT KWFrameLayout : public QObject {
    Q_OBJECT
public:
    /**
     * Create a new frameLayout instance
     * @param pageManager the manager of pages
     * @param frameSets all the framesets registred in the document.
     * @param pageSettings the settings used for auto-layout of frames.
     */
    KWFrameLayout(const KWPageManager *pageManager, QList<KWFrameSet*> frameSets, const KWPageSettings *pageSettings);
    ~KWFrameLayout() {}
    /**
     * As soon as a new page is created you should call this method to auto-create all frames
     * needed on that new page.  You should only call this method one time per new page!
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
    void updateFramesAfterDelete(int deletedPage);
    //void relayoutFrames(old layout, new layout); // per page ? Or per doc?

signals:
    /**
     * Signal emitted when a frameset is created
     */
    void newFrameSet(KWFrameSet *fs);

private:
    KoTextShape *createTextShape(KWPage *page);
    KWTextFrameSet *getOrCreate(KWord::TextFrameSetType type);
    QList<KWFrame *> framesInPage(QRectF page);
    void setup();
    bool shouldCreateHeaderOrFooter(int pageNumber, bool header, KWord::TextFrameSetType *origin);

private:
    friend class TestBasicLayout;
    const KWPageManager *m_pageManager;
    const KWPageSettings *m_pageSettings;
    QList<KWFrameSet *> m_frameSets;

    KWTextFrameSet *m_oddHeaders, *m_evenHeaders, *m_oddFooters, *m_evenFooters;
    KWTextFrameSet *m_firstHeader, *m_firstFooter, *m_maintext;

    bool m_setup;
};

#endif
