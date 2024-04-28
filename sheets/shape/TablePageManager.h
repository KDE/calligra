/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TABLE_PAGE_MANAGER
