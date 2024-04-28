/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_FILL
#define CALLIGRA_SHEETS_ACTION_FILL

#include "CellAction.h"

#include "ui/commands/DataManipulators.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Commands
 * \brief Fills values into a cell range.
 */
class CALLIGRA_SHEETS_UI_EXPORT FillManipulator : public AbstractDFManipulator
{
public:
    FillManipulator();
    ~FillManipulator() override;

    enum Direction { Up = 0, Down, Left, Right };

    void setDirection(Direction d)
    {
        m_dir = d;
    }

protected:
    Value newValue(Element *element, int col, int row, bool *parse, Format::Type *fmtType) override;
    Style newFormat(Element *element, int col, int row) override;
    Direction m_dir;
};

class Fill : public CellAction
{
    Q_OBJECT
public:
    Fill(Actions *actions, FillManipulator::Direction dir);
    virtual ~Fill();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool enabledForSelection(Selection *selection, const Cell &) override;

    static QString actionName(FillManipulator::Direction dir);
    static QString actionCaption(FillManipulator::Direction dir);
    FillManipulator::Direction m_dir;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_FILL
