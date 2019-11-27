/* This file is part of the KDE project
   Copyright 2005,2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef CALLIGRA_SHEETS_COMMENT_COMMAND
#define CALLIGRA_SHEETS_COMMENT_COMMAND

#include "AbstractRegionCommand.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \class CommentCommand
 * \ingroup Commands
 * \brief Adds/Removes comments to/of a cell region.
 */
class CommentCommand : public AbstractRegionCommand
{
public:
    explicit CommentCommand(KUndo2Command *parent = 0);
    void setComment(const QString& comment);

protected:
    bool process(Element* element) override;
    bool mainProcessing() override;

private:
    QString m_comment;
    QList< QPair<QRectF, QString> > m_undoData;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_COMMENT_COMMAND
