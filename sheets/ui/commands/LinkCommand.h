/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2004 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_LINK_COMMAND
#define CALLIGRA_SHEETS_LINK_COMMAND

#include <QString>
#include "AbstractRegionCommand.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Commands
 * \brief Adds/Removes a hyperlink.
 */
class LinkCommand : public AbstractRegionCommand
{
public:
    LinkCommand(const QString& text, const QString& link);
protected:
    bool process(Element* element) override;

    QString newText;
    QString newLink;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_LINK_COMMAND
