/* This file is part of the Calligra project
 * SPDX-FileCopyrightText: 2008 Sebastian Sauer <mail@dipe.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTEXTPAGE_H
#define KOTEXTPAGE_H

#include "kotext_export.h"

#include <QMetaType>
#include <QRectF>

class QString;

/**
 * Interface for a single OpenDocumentText page.
 *
 * The Words KWPageTextInfo class does implement this interface to provide
 * application specific functionality for single pages.
 * @see KoTextShapeData::setPage();
 */
class KOTEXT_EXPORT KoTextPage
{
public:
    /// Constructor.
    explicit KoTextPage();
    /// Destructor.
    virtual ~KoTextPage();

    enum PageSelection { PreviousPage = -1, CurrentPage = 0, NextPage = 1 };

    /**
     * Returns the unique number of this page for internal purposes. All pages
     * are numbered consecutively starting by 1.
     *
     * This is used for example to anchor images to pages. The image then refers
     * to the unique page-number.
     */
    virtual int pageNumber() const = 0;

    /**
     * Returns the number of this page for display purposes.
     *
     * Example how the parameters are used within ODF to display the
     * current page number on all pages except the first page;
     * \code
     * <text:page-number text:select-page="previous" text:page-adjust="1" />
     * \endcode
     *
     * \param select Defines the offset of the page to select for the
     * resulting page number.  If such a page does not exist, then -1 will be
     * returned before the adjustment will be taken into account. This
     * implements the ODF text:select-page attribute.
     * \param adjustment The value of the page number will be adjusted by this
     * specified number and if there exist a page with the resulting value it's
     * page number gets returned, otherwise -1 will be returned. This implements the
     * ODF text:page-adjust attribute.
     * \return the user visible page number, or -1 if the page referenced does not
     * exist.
     */
    virtual int visiblePageNumber(PageSelection select = CurrentPage, int adjustment = 0) const = 0;

    /**
     * Returns the name of the master-page that should be used for this page or a null
     * QString if this page does not explicit define a master-page in which case the
     * default master-page will be used.
     *
     * Per default a null QString is returned.
     */
    virtual QString masterPageName() const;

    /**
     * Returns the rect of the page in document coords
     */
    virtual QRectF rect() const = 0;

    /**
     * Returns the (text) content rect of the page in document coords
     */
    virtual QRectF contentRect() const
    {
        return rect();
    }
};

Q_DECLARE_METATYPE(KoTextPage *)

#endif
