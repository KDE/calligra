/* This file is part of the KDE project
   Copyright 2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2004 Laurent Montel <montel@kde.org>

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

#ifndef KSPREAD_DEFINE_PRINT_RANGE_COMMAND
#define KSPREAD_DEFINE_PRINT_RANGE_COMMAND

#include <QRect>
#include <QString>

#include "commands/AbstractRegionCommand.h"

/**
 * The KSpread namespace.
 */
namespace KSpread
{
class Sheet;

/**
 * \ingroup Commands
 * Defines a print range.
 */
class DefinePrintRangeCommand : public AbstractRegionCommand
{
public:
    explicit DefinePrintRangeCommand();

    virtual void redo();
    virtual void undo();

private:
    Region m_oldPrintRegion;
};

} // namespace KSpread

#endif // KSPREAD_DEFINE_PRINT_RANGE_COMMAND
