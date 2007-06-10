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

#ifndef KSPREAD_DATABASE_RANGE
#define KSPREAD_DATABASE_RANGE

#include <QSharedDataPointer>

namespace KSpread
{
class Region;

class DatabaseRange
{
public:
    /**
     * Constructor.
     * Creates an empty database range.
     */
    DatabaseRange();

    /**
     * Constructor.
     * Creates a database range named \p name.
     */
    DatabaseRange( const QString& name );

    /**
     * Copy Constructor.
     */
    DatabaseRange( const DatabaseRange& other );

    /**
     * Destructor.
     */
    virtual ~DatabaseRange();

    /**
     * \return \c true if this is the default/empty database range
     */
    bool isEmpty() const;

    bool horizontallyOriented() const;
    bool verticallyOriented() const;

    /**
     * \return \c true if filter buttons should be displayed
     */
    bool displayFilterButtons() const;

    /**
     * Sets wether filter buttons should be displayed.
     */
    void setDisplayFilterButtons( bool enable );

    /**
     * \return the actual database cell range
     */
    const Region& range() const;

    /**
     * Sets the actual database cell range.
     * \p region has to be contiguous.
     */
    void setRange( const Region& region );

    void operator=( const DatabaseRange& other );
    bool operator==( const DatabaseRange& other ) const;
    bool operator<( const DatabaseRange& other ) const;

private:
    class Private;
    QSharedDataPointer<Private> d;
};

} // namespace KSpread

#endif // KSPREAD_DATABASE_RANGE
