/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005, 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
