/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef KSPREAD_TABLE_SHAPE
#define KSPREAD_TABLE_SHAPE

#include <KoShape.h>

#define TableShapeId "TableShape"

namespace KSpread
{
class Sheet;

class TableShape : public KoShape
{
public:
    TableShape( int columns = 2, int rows = 8 );
    virtual ~TableShape();

    void setColumns( int columns );
    void setRows( int rows );

    virtual void paint( QPainter& painter, const KoViewConverter& converter );

protected:
    /**
     * \return the sheet containing the data for this shape
     */
    Sheet* sheet() const;

private:
    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_TABLE_SHAPE
