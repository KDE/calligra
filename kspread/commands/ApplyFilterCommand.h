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

#ifndef KSPREAD_APPLYFILTER_COMMAND
#define KSPREAD_APPLYFILTER_COMMAND

#include "AbstractRegionCommand.h"

#include "database/Database.h"

namespace KSpread
{
class Filter;

/**
 * \ingroup Commands
 * \brief Filtering a cell range.
 */
class ApplyFilterCommand : public AbstractRegionCommand
{
public:
    /**
     * Constructor.
     */
    ApplyFilterCommand();

    /**
     * Destructor.
     */
    virtual ~ApplyFilterCommand();

    virtual void redo();
    virtual void undo();

    void setDatabase(const Database& database);
    void setOldFilter(const Filter& filter);

private:
    Database m_database;
    Filter* m_oldFilter;
    QHash<int, bool> m_undoData;
};

} // namespace KSpread

#endif // KSPREAD_APPLYFILTER_COMMAND
