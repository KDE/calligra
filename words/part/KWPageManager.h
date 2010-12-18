/* This file is part of the KOffice project
 * Copyright (C) 2005-2010 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
 * Copyright (C) 2008 Sebastian Sauer <mail@dipe.org>
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
#ifndef KW_PAGEMANAGER_H
#define KW_PAGEMANAGER_H

#include "kword_export.h"
#include "KWPageStyle.h"

#include <KoPageLayout.h>
#include <KoInsets.h>

#include <QList>
#include <QPointF>

class KWPage;
class KoShape;
class KWPageManagerPrivate;


/**
 * The Page Manager manages all the pages a document contains and separates all the frames
 * the document contains into actual printed pages.
 */
class KWORD_EXPORT KWPageManager
{
public:
    explicit KWPageManager();
    ~KWPageManager();

    /**
     * return pageNumber of @p point, pagenumbers for a normal document start at 0.
     */
    int pageNumber(const QPointF &point) const;

    /**
     * return pageNumber of the argument shape, pagenumbers for a normal document start at 0.
     */
    int pageNumber(const KoShape *shape) const;

    /**
     * return pageNumber of page with document-offset (in the Y direction) of @p ptY,
     * pagenumbers for a normal document start at 0.
     */
    int pageNumber(qreal ptY) const;

    /**
     * return total number of pages in this document.
     */
    int pageCount() const;

    /**
     * return the KWPage of a specific page number. Returns 0 if page does not exist.
     */
    KWPage page(int pageNumber) const;

    /**
     * return the KWPage instance where the rect is on. Returns 0 if page does not exist.
     */
    KWPage page(const KoShape *shape) const;

    /**
     * return the KWPage instance where the point is on. Returns 0 if page does not exist.
     */
    KWPage page(const QPointF &point) const;

    /**
     * return the KWPage instance of the y-coordinate in the document. Returns 0 if
     * page does not exist.
     */
    KWPage page(qreal ptY) const;

    /**
     * Return the y-offset in this document of the top of page with @p pageNumber
     * Note that pageNumber is NOT an offset in the document, but the real number
     * of the page.
     * @see setStartPage(int)
     */
    qreal topOfPage(int pageNumber) const; // in pt

    /**
     * Return the y-offset in this document of the bottom of page with @p pageNumber
     * Note that pageNumber is NOT an offset in the document, but the real number
     * of the page.
     * @see setStartPage(int)
     */
    qreal bottomOfPage(int pageNumber) const; // in pt

    /**
     * Inserts a new page at the specified position in the document.
     * Shifts the page currently at that position (if any) and any subsequent pages after.
     * @param pageNumber page number of the new page, or -1 to append
     * @param pageStyle the page style to use for the new page
     */
    KWPage insertPage(int pageNumber, const KWPageStyle &pageStyle = KWPageStyle());

    /**
     * Append a new page at the end of the document
     * @param pageStyle the page style to use for the new page
     */
    KWPage appendPage(const KWPageStyle &pageStyle = KWPageStyle());

    /// Remove the page with @p pageNumber renumbering all pages after pages already added
    void removePage(int pageNumber);

    /// Remove @p page renumbering all pages after pages already added
    void removePage(const KWPage &page);

    /**
     * Returns the argument point, with altered coordinats if the point happens to be
     * outside all pages.
     * The resulting point is the shortest distance from the argument inside the document.
     * @param point the point to test
     */
    QPointF clipToDocument(const QPointF &point) const;

    /**
     * Return an ordered list of all pages.
     * @param pageStyle if non empty return only the pages that follow the page style.
     */
    QList<KWPage> pages(const QString &pageStyle = QString()) const;

    /**
     * Return the padding used for this document. This is used to have some space around each
     * page outside of the printable area for page bleed.
     */
    KoInsets padding() const;

    /**
     * Set a new padding used for this document. This is used to have some space around each
     * page outside of the printable area for page bleed.
     */
    void setPadding(const KoInsets &padding);

    /**
     * This property can be set to register that new pages created should be made to be a pageSpread when aproriate.
     * Note that the pageManager itself will not use this variable since it doesn't have a factory method for pages.
     */
    bool preferPageSpread() const;

    /**
     * Set the property to register that new pages created should be made to be a pageSpread when aproriate.
     * Note that the pageManager itself will not use this variable since it doesn't have a factory method for pages.
     * @param on If true, it is requested that new, even numbered pages are set to be page spreads.
     */
    void setPreferPageSpread(bool on);

    /**
     * Add a new \a KWPageStyle instance to this document.
     *
     * \note that you need to make sure that you only add pageStyle with a
     * masterpage-name that are NOT already registered cause those names need
     * to be unique.
     *
     * \param pageStyle The \a KWPageStyle instance that should be added. The
     * document will take over ownership and takes care of deleting the instance
     * one the document itself got deleted.
     */
    void addPageStyle(const KWPageStyle &pageStyle);

    /**
     * Create a new named KWPageStyle for this document.
     *
     * \note that you need to make sure that you only add pageStyle with a
     * masterpage-name that are NOT already registered because those names need
     * to be unique.
     *
     * \param name The \a KWPageStyle instance that should be added. The
     * document will take over ownership and takes care of deleting the instance
     * one the document itself got deleted.
     */
    KWPageStyle addPageStyle(const QString &name);

    void removePageStyle(const KWPageStyle &pageStyle);

    /**
     * Returns all pagestyles.
     * // TODO only return a qlist of styles, they have a their name in them alraedy
     */
    QHash<QString, KWPageStyle> pageStyles() const;

    /**
     * Returns the \a KWPageStyle known under the name \p name or NULL if the
     * document has no such page style.
     */
    KWPageStyle pageStyle(const QString &name) const;

    /**
     * Return the default page style. This equals to pageStyle("Standard").
     */
    KWPageStyle defaultPageStyle() const;

    /**
     * Remove all page style and clears the default one.
     */
    void clearPageStyles();

    /**
     * Return the first page in the page managers list of pages.
     * Will return 0 if there are no pages.
     */
    const KWPage begin() const;

    /**
     * Return the last page in the page managers list of pages.
     * Will return 0 if there are no pages.
     */
    const KWPage last() const;

    /**
     * Return the first page in the page managers list of pages.
     * Will return 0 if there are no pages.
     */
    KWPage begin();

    /**
     * Return the last page in the page managers list of pages.
     * Will return 0 if there are no pages.
     */
    KWPage last();

    KWPageManagerPrivate *priv() { return d; }

private:
    /// disable copy constructor and assignment operator
    Q_DISABLE_COPY(KWPageManager)

    KWPageManagerPrivate * const d;
};

#endif
