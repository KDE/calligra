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

#ifndef CALLIGRA_SHEETS_TABLE_PAGE_MANAGER
#define CALLIGRA_SHEETS_TABLE_PAGE_MANAGER

#include "PageManager.h"

namespace Calligra
{
namespace Sheets
{
class TableShape;

class TablePageManager : public PageManager
{
public:
    /**
     * Constructor.
     */
    explicit TablePageManager(TableShape *masterShape);

    /**
     * Destructor.
     */
    ~TablePageManager();

    QSizeF size(int page) const override;

protected:
    void clearPages() override;
    void insertPage(int page) override;
    void preparePage(int page) override;

private:
    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TABLE_PAGE_MANAGER
