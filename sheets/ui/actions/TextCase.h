/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_TEXTCASE
#define CALLIGRA_SHEETS_ACTION_TEXTCASE

#include "CellAction.h"

#include "ui/commands/DataManipulators.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Commands
 * \brief Converts string values to upper-/lowercase.
 */
class CALLIGRA_SHEETS_UI_EXPORT CaseManipulator : public AbstractDataManipulator
{
public:
    CaseManipulator();
    ~CaseManipulator() override;

    enum CaseMode { Upper = 0, Lower, FirstUpper };
    void changeMode(CaseMode mode)
    {
        m_mode = mode;
    }
    void changeLowerCase();
    void changeFirstUpper();

protected:
    Value newValue(Element *element, int col, int row, bool *parse, Format::Type *fmtType) override;

    /** do we want to change this cell ? */
    bool wantChange(Element *element, int col, int row) override;

    CaseMode m_mode;
};

class TextCase : public CellAction
{
    Q_OBJECT
public:
    TextCase(Actions *actions, CaseManipulator::CaseMode mode);
    virtual ~TextCase();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual QAction *createAction() override;

    static QString actionName(CaseManipulator::CaseMode dir);
    static QString actionCaption(CaseManipulator::CaseMode dir);
    static QString actionToolTip(CaseManipulator::CaseMode dir);
    static QIcon actionIcon(CaseManipulator::CaseMode dir);
    CaseManipulator::CaseMode m_mode;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_TEXTCASE
