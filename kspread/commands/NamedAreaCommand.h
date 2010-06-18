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

#ifndef KSPREAD_NAMED_AREA_COMMAND
#define KSPREAD_NAMED_AREA_COMMAND

#include "AbstractRegionCommand.h"
#include "Region.h"

namespace KSpread
{

/**
 * \ingroup Commands
 * \brief Attaches a name to a cell range.
 */
class NamedAreaCommand : public AbstractRegionCommand
{
public:
    /**
     * Constructor.
     */
    NamedAreaCommand(QUndoCommand* parent = 0);

    /**
     * Destructor.
     */
    virtual ~NamedAreaCommand();

    void setAreaName(const QString& name);

    virtual void setReverse(bool reverse);

protected:
    virtual bool preProcessing();
    virtual bool mainProcessing();
    virtual bool postProcessing();

private:
    QString m_areaName;
    Region m_oldArea;
};

} // namespace KSpread

#endif // KSPREAD_NAMED_AREA_COMMAND
