/* This file is part of the KOffice project
 * Copyright (C) 2005-2006 Thomas Zander <zander@kde.org>
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


/**
 * The Page Manager manages all the pages a document contains and separates all the frames
 * the document contains into actual printed pages.
 */
class KWORD_EXPORT KWPageManager : public QObject {
    Q_OBJECT
public:
    explicit KWPageManager(KWDocument* document);
    ~KWPageManager();

    /**
     * return pageNumber of @p point, pagenumbers for a normal document start at 0.
     */
    Q_SCRIPTABLE int pageNumber(const QPointF &point) const;

    /**
     * return pageNumber of the argument shape, pagenumbers for a normal document start at 0.
     */
    int pageNumber(const KoShape *shape) const;

    /**
     * return pageNumber of page with document-offset (in the Y direction) of @p ptY,
     * pagenumbers for a normal document start at 0.
     */
    Q_SCRIPTABLE int pageNumber(qreal ptY) const;

    /**
     * return total number of pages in this document.
     */
    Q_SCRIPTABLE int pageCount() const;

    /**
     * return the KWPage of a specific page number. Returns 0 if page does not exist.
     */
    Q_SCRIPTABLE KWPage* page(int pageNumber) const;

    /**
     * return the KWPage instance where the rect is on. Returns 0 if page does not exist.
     */
    KWPage* page(const KoShape *shape) const;

    /**
     * return the KWPage instance where the point is on. Returns 0 if page does not exist.
     */
    Q_SCRIPTABLE KWPage* page(const QPointF &point) const;

    /**
     * return the KWPage instance of the y-coordinate in the document. Returns 0 if
     * page does not exist.
     */
    KWPage* page(qreal ptY) const;

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
     * if onlyAllowAppend is set to true the pagenumber will be ignored and the new page
     * will always be appended.
     * @param pageNumber page number of the new page
     * @param pageStyle the page style to use for the new page
     */
    Q_SCRIPTABLE KWPage* insertPage(int pageNumber, KWPageStyle *pageStyle = 0);

    /**
     * Insert the page instance at the specified position in the document. Note that it is preferred
     * to use the insertPage(int) method which creates a new page.
     * @param page the page that will be inserted.
     */
    KWPage* insertPage(KWPage *page);

    /**
     * Append a new page at the end of the document
     * @param pageStyle the page style to use for the new page
     */
    Q_SCRIPTABLE KWPage* appendPage(KWPageStyle *pageStyle = 0);

    /// Remove the page with @p pageNumber renumbering all pages after pages already added
    Q_SCRIPTABLE void removePage(int pageNumber);

    /// Remove @p page renumbering all pages after pages already added
    void removePage(KWPage *page);

    /**
     * Returns the argument point, with altered coordinats if the point happens to be
     * outside all pages.
     * The resulting point is the shortest distance from the argument inside the document.
     * @param point the point to test
     */
    QPointF clipToDocument(const QPointF &point);

    /**
     * Return an ordered list of all pages.
     */
    QList<KWPage*> pages() const;

    /**
     * Return the padding used for this document. This is used to have some space around each
     * page outside of the printable area for page bleed.
     */
    const KoInsets &padding() const { return m_padding; }
    /**
     * Return the padding used for this document. This is used to have some space around each
     * page outside of the printable area for page bleed.
     */
    KoInsets &padding() { return m_padding; }
    /**
     * Set a new padding used for this document. This is used to have some space around each
     * page outside of the printable area for page bleed.
     */
    void setPadding(const KoInsets &padding) { m_padding = padding; }

//TODO move following both functions to KWPageStyle
    /**
     * This property can be set to register that new pages created should be made to be a pageSpread when aproriate.
     * Note that the pageManager itself will not use this variable since it doesn't have a factory method for pages.
     */
    KDE_DEPRECATED bool preferPageSpread() const { return m_preferPageSpread; }
    /**
     * Set the property to register that new pages created should be made to be a pageSpread when aproriate.
     * Note that the pageManager itself will not use this variable since it doesn't have a factory method for pages.
     * @param on If true, it is requested that new, even numbered pages are set to be page spreads.
     */
    KDE_DEPRECATED void setPreferPageSpread(bool on) { m_preferPageSpread = on; }

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
    void addPageStyle(KWPageStyle *pageStyle);

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
    Q_SCRIPTABLE KWPageStyle* addPageStyle(const QString &name);

    /**
     * Returns all pagestyles.
     */
    QHash<QString, KWPageStyle *> pageStyles() const;

    /**
     * Returns the \a KWPageStyle known under the name \p name or NULL if the
     * document has no such page style.
     */
    Q_SCRIPTABLE KWPageStyle *pageStyle(const QString &name) const;

    /**
     * Return the default page style. This equals to pageStyle("Standard").
     */
    Q_SCRIPTABLE KWPageStyle* defaultPageStyle() const;

    /**
     * Remove all page style and clears the default one.
     */
    Q_SCRIPTABLE void clearPageStyle();

private:
    /// helper method for the topOfPage and bottomOfPage
    qreal pageOffset(int pageNumber, bool bottom) const;
    friend class KWPage;

private:
    KWDocument* m_document;
    QList<KWPage*> m_pageList;
    bool m_preferPageSpread;

    QHash <QString, KWPageStyle *> m_pageStyle;
    KoInsets m_padding;

    /// disable copy constructor and assignment operator
    Q_DISABLE_COPY(KWPageManager)
};

#endif
