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

#include <qptrlist.h>
#include <kdebug.h>
#include <koRect.h>
#include <qmap.h>

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
        int frameNumberForPage( int page /* 0-based! */ ) const
            {
                if ( page < m_startAtPage || ( m_endAtPage != -1 && page > m_endAtPage ) )
                    return -1;
                int pg = page - m_startAtPage; // always >=0
                // Note that 'page' is 0-based. This is why "Odd" looks for even numbers, and "Even" looks for odd numbers :}
                switch (m_oddEvenAll) {
                case Odd:
                    // we test page, not bg: even/odd is for the absolute page number, too confusing otherwise
                    if ( page % 2 == 0 )
                        return pg / 2; // page 0[+start] -> frame 0, page 2[+start] -> frame 1
                    else
                        return -1;
                case Even:
                    if ( page % 2 )
                        return pg / 2; // page 1 -> 0, page 3 -> 1
                    else
                        return -1;
                case All:
                    return pg; // page 0[+start] -> frame 0, etc.
                default:
                    return -1;
                }
            }

        // the last frame we need, layout() will delete any frame after that
        int lastFrameNumber( int lastPage ) const
            {
                if ( lastPage < m_startAtPage )
                    return -1; // we need none
                int pg = lastPage;
                if ( m_endAtPage > -1 )
                    pg = QMIN( m_endAtPage, pg );
                pg -= m_startAtPage; // always >=0
                Q_ASSERT( pg >= 0 );
                switch (m_oddEvenAll) {
                case Odd:
                case Even:
                    return pg / 2; // page 0 and 1 -> 0. page 2 and 3 -> 1.
                case All:
                    return pg; // page 0 -> 0 etc. ;)
                default:
                    return -1;
                }
            }

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
    KWFrameLayout( KWDocument* doc, QPtrList<HeaderFooterFrameset>& headersFooters,
                   QPtrList<HeaderFooterFrameset>& footnotes, QPtrList<HeaderFooterFrameset>& endnotes )
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
    QPtrList<HeaderFooterFrameset>& m_headersFooters;
    QPtrList<HeaderFooterFrameset>& m_footnotes;
    QPtrList<HeaderFooterFrameset>& m_endnotes;
    QMap<KWFrameSet *, bool> m_framesetsToUpdate;
    KWDocument* m_doc;
    int m_lastMainFramePage;
};

#endif
