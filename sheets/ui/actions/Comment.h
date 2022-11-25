/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_COMMENT
#define CALLIGRA_SHEETS_ACTION_COMMENT


#include "CellAction.h"

#include "ui/commands/AbstractRegionCommand.h"


namespace Calligra
{
namespace Sheets
{
class CommentDialog;

class Comment : public CellAction {
Q_OBJECT
public:
    Comment(Actions *actions);
    virtual ~Comment();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;

    CommentDialog *m_dlg;
    Selection *m_selection;
};

class ClearComment : public CellAction {
Q_OBJECT
public:
    ClearComment(Actions *actions);
    virtual ~ClearComment();

protected:
    virtual QAction *createAction() override;
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
};


/**
 * \class CommentCommand
 * \ingroup Commands
 * \brief Adds/Removes comments to/of a cell region.
 */
class CALLIGRA_SHEETS_UI_EXPORT CommentCommand : public AbstractRegionCommand
{
public:
    explicit CommentCommand(KUndo2Command *parent = 0);
    void setComment(const QString& comment);

protected:
    bool process(Element* element) override;

private:
    QString m_comment;
};


} // namespace Sheets
} // namespace Calligra

#endif   // CALLIGRA_SHEETS_ACTION_COMMENT
