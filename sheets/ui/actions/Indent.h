/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_INDENT
#define CALLIGRA_SHEETS_ACTION_INDENT

#include "CellAction.h"

#include "ui/commands/AbstractRegionCommand.h"

namespace Calligra
{
namespace Sheets
{
class IndentDialog;

class Indent : public CellAction
{
    Q_OBJECT
public:
    Indent(Actions *actions, bool negative);
    virtual ~Indent();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool enabledForSelection(Selection *selection, const Cell &activeCell) override;
    virtual QAction *createAction() override;

    bool m_negative;
};

/**
 * \ingroup Commands
 * \brief Changes the indentation level.
 */
class IndentationCommand : public AbstractRegionCommand
{
public:
    IndentationCommand();

    void setIndent(double val);

protected:
    bool performCommands() override;

private:
    double m_indent;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_INDENT
