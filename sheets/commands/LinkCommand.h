/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2004 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
