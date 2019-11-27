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

#ifndef CALLIGRA_SHEETS_LINK_COMMAND
#define CALLIGRA_SHEETS_LINK_COMMAND

#include <QString>
#include <kundo2command.h>

#include <Cell.h>

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Commands
 * \brief Adds/Removes a hyperlink.
 */
class LinkCommand : public KUndo2Command
{
public:
    LinkCommand(const Cell& cell, const QString& text, const QString& link);

    void redo() override;
    void undo() override;

protected:
    Cell cell;
    QString oldText;
    QString oldLink;
    QString newText;
    QString newLink;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_LINK_COMMAND
