/* This file is part of the KOffice project
 * Copyright (C) 2005 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; version 2.

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
#ifndef kw_framelist_h
#define kw_framelist_h

#include "KWFrame.h"

#include <qptrlist.h>
#include <qvaluevector.h>

class KWDocument;

/**
 * This class calculates and caches a list of frames that are on top or below the KWFrame
 * instance this class is created for.
 */
class KWFrameList {
public:
    /**
     * Constructor.
     * @param doc the document that will reference all framesets.
     * @param theFrame the frame this framestack work for.
     */
    KWFrameList(KWDocument *doc, KWFrame *theFrame);

    /**
     * Fetches the frames below the frame this frameList is for.
     */
    QValueList<KWFrame *> framesBelow() const;

    /**
     * Fetches the frames on top of the frame this frameList is for.
     */
    QValueList<KWFrame *> framesOnTop() const;

    /**
     * Update the z-ordering around the frame we host the framelist for.
     * This is typically called after this frame has been moved.
     * This will make sure all frames on the same page will also be updated.
     * @param oldPageNum a frame that has moved naturally has a previous page number which
     *  also needs to be updated because this frame has gone missing.
     */
    void updateAfterMove(int oldPageNum);

    /**
     * Update the z-ordering around the frame we host the framelist for.
     * This will make sure all frames on the same page will also be updated.
     */
    void update();

    /**
     * Update the z-ordering for all frames of the whole document.
     * @param doc the document where we can find the frames to be updated
     */
    static void recalcAllFrames(KWDocument *doc);

    /**
     * Update the z-ordering for all frames in the page section of the whole document.
     * @param doc the document where we can find the frames to be updated
     * @param pageFrom the first page that will be updated
     * @param pageTo the last page (including) that will be updated
     */
    static void recalcFrames(KWDocument *doc, int pageFrom, int pageTo);

    /**
     * Factory method to create and attach a new instance of a framelist.
     * Note; when an instance of a frameList already exists, calling this will do nothing.
     * @param f the frame to add te new framelist to.
     * @param doc the document that the frame belongs to.
     */
    static void createFrameList(KWFrame *f, KWDocument *doc);

    /**
     * Factory method to create and attach a new instance of a framelist to each frame
     * on the frameset.
     * Note; when an instance of a frameList already exists on a frame, calling this will
     * not overwrite the one on that frame.
     * @param fs the frameset to add te new framelists to.
     * @param doc the document that the frame belongs to.
     * @param forceUpdate when true, even already existing frameList instances will be
     *   updated.
     * @see update();
     */
    static void createFrameList(KWFrameSet *fs, KWDocument *doc, bool forceUpdate=true);

private:
    static KWFrameList *getFirstFrameList(KWDocument *doc);
    void updateZOrderFor(const QPtrList<KWFrame> &frames);
    void setFrames(const QPtrList<KWFrame> &frames);

    QValueVector<KWFrame *> m_frames; // sorted by Z Order
    KWDocument *m_doc;
    KWFrame *m_frame;
};

#endif
