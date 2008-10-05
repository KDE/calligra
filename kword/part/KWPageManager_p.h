#ifndef KWPageManagerPrivate_H
#define KWPageManagerPrivate_H

#include "KWPageStyle.h"
#include "KWPage.h"

#include <QHash>
#include <QMap>

class KWPageManagerPrivate
{
public:
    KWPageManagerPrivate();

    qreal pageOffset(int pageNum, bool bottom) const;

    /**
     * Update the page number for the page related to the pageId and also update the
     * page number of all pages following the page.
     */
    void setPageNumberForId(int pageId, int newPageNumber);


    struct Page {
        Page()
            : pageSide(KWPage::Right),
            pageNumber(1),
            orientation(KoPageFormat::Landscape),
            textDirection(KoText::AutoDirection)
        {
        }
        KWPageStyle style;
        KWPage::PageSide pageSide;
        int pageNumber; // set by the append-page and overwritten by the text-layout
        KoPageFormat::Orientation orientation;
        KoText::Direction textDirection;
    };

    // use a sorted map to find page the identifier for page objects based on the page number.
    QMap<int, int> pageNumbers; // page number to pageId

    // use a fast access hash to store the page objects, sorted by their identifier
    QHash<int, Page> pages; // pageId to page struct

    int lastId; // pageIds are distributed serially,

    bool preferPageSpread;

    QHash <QString, KWPageStyle> pageStyles;
    KoInsets padding;
};

#endif
