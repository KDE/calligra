#ifndef KWFRAMELAYOUT_H
#define KWFRAMELAYOUT_H

#include <qptrlist.h>

class KWDocument;
class KWFrameSet;
class KWTextFrameSet;
class KoRect;

namespace KWFrameLayout
{

    // All that data should go into a KWHeaderFooterFrameSet
    // (hmm, KWHeaderFooterFootNoteEndNoteFrameSet? TODO: find a shorter name ;)
    struct HeaderFooterFrameset {
        enum OddEvenAll { Odd, Even, All };

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

        // frame number for the given page.... -1 if no frame on that page
        int frameNumberForPage( int page ) const
            {
                if ( page < m_startAtPage || ( m_endAtPage != -1 && page > m_endAtPage ) )
                    return -1;
                int pg = page - m_startAtPage; // always >=0
                switch (m_oddEvenAll) {
                case Even:
                    if ( page % 2 == 0 ) // even/odd is for the absolute page number, too confusing otherwise
                        return pg / 2; // page 0[+start] -> frame 0, page 2[+start] -> frame 1
                    else
                        return -1;
                case Odd:
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
                int pg = lastPage - m_startAtPage; // always >=0
                switch (m_oddEvenAll) {
                case Even:
                case Odd:
                    return pg / 2; // page 0 and 1 -> 0. page 2 and 3 -> 1.
                case All:
                    return pg; // page 0 -> 0 etc. ;)
                default:
                    return -1;
                }
            }
    };

    void layout( KWDocument* doc, KWFrameSet* mainTextFrameSet,
                 QPtrList<HeaderFooterFrameset>& info,
                 int fromPage, int toPage );
    void resizeOrCreateHeaderFooter( KWTextFrameSet* headerFooter, uint frameNumber, const KoRect& rect );
};

#endif
