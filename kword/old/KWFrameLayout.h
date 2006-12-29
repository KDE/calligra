/* This file is part of the KOffice project
 * Copyright (C) 2002 David Faure <faure@kde.org>
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
#ifndef KWFRAMELAYOUT_H
#define KWFRAMELAYOUT_H

#include <q3ptrlist.h>
#include <kdebug.h>
#include <KoRect.h>
#include <QMap>

class KWDocument;
class KWFrameSet;
class KWTextFrameSet;
class KoRect;

class KWFrameLayout
{
public:
    // Maybe all that data should go into a KWHeaderFooterFrameSet
    // (or rather a base class shared by KWFootNoteFrameSet....)
    struct HeaderFooterFrameset {
        enum OddEvenAll { Even, Odd, All };

        HeaderFooterFrameset( KWTextFrameSet* fs, int start, int end,
                              double spacing, OddEvenAll oea = All );

        // Frameset. Also gives the type (header, footer, footnote).
        KWTextFrameSet* m_frameset;

        // Future features - but already used for "first page" stuff
        int m_startAtPage;
        int m_endAtPage; // (-1 for no end)

        // Odd/even/all
        OddEvenAll m_oddEvenAll;

        // Height in pt
        double m_height;

        // Space between this frame and the next one
        // (the one at bottom for headers, the one on top for footers/footnotes).
        // e.g. ptHeaderBodySpacing for headers/footers
        double m_spacing;

        // Minimum Y value - for footnotes
        double m_minY;

        // True once the footnote has been correctly positionned and
        // shouldn't be moved by checkFootNotes anymore.
        bool m_positioned;

        // frame number for the given page.... -1 if no frame on that page
        // The first frame is number 0.
        int frameNumberForPage( int page ) const;
        // The last frame we need (0-based), layout() will delete any frame after that
        int lastFrameNumber( int lastPage ) const;

        void debug();
        bool deleteFramesAfterLast( int lastPage );
    };

    /**
     * Constructor
     * @param doc the KWDocument we're layouting
     * @param headersFooters list of header and footer HFFs (see definition of HeaderFooterFrameset)
     * @param footnotes list of footnotes framesets HFFs
     * @param endnotes list of endnotes framesets HFFs
     */
    KWFrameLayout( KWDocument* doc, Q3PtrList<HeaderFooterFrameset>& headersFooters,
                   Q3PtrList<HeaderFooterFrameset>& footnotes, Q3PtrList<HeaderFooterFrameset>& endnotes )
        : m_headersFooters( headersFooters ), m_footnotes( footnotes ), m_endnotes( endnotes ), m_doc( doc )
        {}

    enum { DontRemovePages = 1 };
    /**
     * The main method of this file. Do the frame layout.
     * @param mainTextFrameSet if set, its frames will be resized. Usually: set in WP mode, not set in DTP mode.
     * @param numColumns number of columns to create for the main textframeset. Only relevant if mainTextFrameSet!=0.
     * @param fromPage first page to layout ( 0-based )
     * @param toPage last page to layout ( 0-based )
     * @param flags see enum above
     */
    void layout( KWFrameSet* mainTextFrameSet, int numColumns,
                 int fromPage, int toPage, uint flags );

protected:
    void resizeOrCreateHeaderFooter( KWTextFrameSet* headerFooter, uint frameNumber, const KoRect& rect );
    KoRect firstColumnRect( KWFrameSet* mainTextFrameSet, int pageNum, int numColumns ) const;
    enum HasFootNotes { NoFootNote, WithFootNotes, NoChange };
    bool resizeMainTextFrame( KWFrameSet* mainTextFrameSet, int pageNum, int numColumns, double ptColumnWidth, double ptColumnSpacing, double left, double top, double bottom, HasFootNotes hasFootNotes );
    void checkFootNotes();

private:
    // A _ref_ to a list. Must remain alive as long as this object.
    Q3PtrList<HeaderFooterFrameset>& m_headersFooters;
    Q3PtrList<HeaderFooterFrameset>& m_footnotes;
    Q3PtrList<HeaderFooterFrameset>& m_endnotes;
    QMap<KWFrameSet *, bool> m_framesetsToUpdate;
    KWDocument* m_doc;
    int m_lastMainFramePage;
};

#endif
