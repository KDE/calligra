/* This file is part of the KDE project
   Copyright 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_PAGE_MANAGER
#define KSPREAD_PAGE_MANAGER

#include "kspread_export.h"

class QRect;
class QSizeF;

namespace KSpread
{
class PrintSettings;
class Sheet;

/**
 * Manages printing on the sheet level.
 */
class KSPREAD_EXPORT PageManager
{
public:
    /**
     * Constructor.
     */
    PageManager(Sheet* sheet);

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
    void setPrintSettings(const PrintSettings& settings, bool force = false);

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
    Sheet* sheet() const;
    const PrintSettings& printSettings() const;
    virtual void clearPages();
    virtual bool pageNeedsPrinting(const QRect& cellRange) const;
    virtual void insertPage(int page);
    virtual void preparePage(int page);

private:
    Q_DISABLE_COPY(PageManager)

    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_PAGE_MANAGER
