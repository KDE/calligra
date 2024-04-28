/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_PAGE_MANAGER
#define CALLIGRA_SHEETS_PAGE_MANAGER

#include <Qt>

class QRect;
class QSizeF;

namespace Calligra
{
namespace Sheets
{
class PrintSettings;
class Sheet;

/**
 * Manages the layouting of pages.
 * Contains shared functionality between PrintManager, which layouts pages for
 * printing, and TablePageManager, which does the same for the table shape in
 * page based hosting apps.
 */
class PageManager
{
public:
    /**
     * Constructor.
     */
    explicit PageManager(Sheet *sheet);

    /**
     * Destructor.
     */
    virtual ~PageManager();

    /**
     * Layouts the pages.
     * Splits the used cell range, so that it fits on several pages.
     */
    void layoutPages();

    /**
     * Sets the print settings.
     * If the settings differ from the existing ones, the pages are recreated.
     * \param settings the print settings
     * \param force forces a recreation of the pages, if \c true
     */
    void setPrintSettings(const PrintSettings &settings, bool force = false);

    /**
     * Number of pages.
     */
    int pageCount() const;

    /**
     * Return the cell range of the requested page.
     * \param page the page number
     * \return the page's cell range
     */
    QRect cellRange(int page) const;

    /**
     * Return the visible size, the page size decreased by the borders, of the requested page.
     * \param page the page number
     * \return the page's visible size
     */
    virtual QSizeF size(int page) const;

protected:
    Sheet *sheet() const;
    const PrintSettings &printSettings() const;
    virtual void clearPages();
    virtual bool pageNeedsPrinting(const QRect &cellRange) const;
    virtual void insertPage(int page);
    virtual void preparePage(int page);

private:
    Q_DISABLE_COPY(PageManager)

    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_PAGE_MANAGER
