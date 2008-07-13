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

#ifndef KSPREAD_PRINT_MANAGER
#define KSPREAD_PRINT_MANAGER

#include "PageManager.h"

class QPainter;
class QPrinter;
class QRect;

namespace KSpread
{
class PrintSettings;
class Sheet;

/**
 * Manages printing on the sheet level.
 */
class PrintManager : public PageManager
{
public:
    /**
     * Constructor.
     */
    PrintManager(Sheet* sheet);

    /**
     * Destructor.
     */
    virtual ~PrintManager();

    void printPage(int page, QPainter& painter);

    double zoom() const;

    virtual QSizeF size(int page) const;

protected:
    virtual bool pageNeedsPrinting(const QRect& cellRange) const;

private:
    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_PRINT_MANAGER
