/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef KSPREAD_FILTER_POPUP
#define KSPREAD_FILTER_POPUP

#include <QFrame>

class QAbstractButton;

namespace KSpread
{
class Cell;
class Database;
class Filter;

class FilterPopup : public QFrame
{
    Q_OBJECT
public:
    /**
     * Constructor.
     */
    FilterPopup(QWidget* parent, const Cell& cell, Database* database);

    static void showPopup(QWidget* parent, const Cell& cell, const QRect& cellRect, Database* database);

    /**
     * Destructor.
     */
    virtual ~FilterPopup();

    void updateFilter(Filter* filter) const;

protected:
    void closeEvent(QCloseEvent*);

private Q_SLOTS:
    void buttonClicked(QAbstractButton* button);

private:
    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_FILTER_POPUP
