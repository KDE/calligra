/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_STYLE
#define CALLIGRA_SHEETS_ACTION_STYLE

#include "DialogCellAction.h"

#include "ui/commands/AbstractRegionCommand.h"

class KoColor;
class KoColorPopupAction;

namespace Calligra
{
namespace Sheets
{

class CellStyle : public DialogCellAction
{
    Q_OBJECT
public:
    CellStyle(Actions *actions);
    virtual ~CellStyle();

protected Q_SLOTS:
    virtual void applyStyle();

protected:
    virtual ActionDialog *createDialog(QWidget *canvasWidget) override;
    virtual void onSelectionChanged() override;

    virtual QAction *createAction() override;
};

class Bold : public ToggleableCellAction
{
    Q_OBJECT
public:
    Bold(Actions *actions);
    virtual ~Bold();

protected:
    virtual QAction *createAction() override;
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class Italic : public ToggleableCellAction
{
    Q_OBJECT
public:
    Italic(Actions *actions);
    virtual ~Italic();

protected:
    virtual QAction *createAction() override;
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class Underline : public ToggleableCellAction
{
    Q_OBJECT
public:
    Underline(Actions *actions);
    virtual ~Underline();

protected:
    virtual QAction *createAction() override;
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class Strikeout : public ToggleableCellAction
{
    Q_OBJECT
public:
    Strikeout(Actions *actions);
    virtual ~Strikeout();

protected:
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class WrapText : public ToggleableCellAction
{
    Q_OBJECT
public:
    WrapText(Actions *actions);
    virtual ~WrapText();

protected:
    virtual QAction *createAction() override;
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class VerticalText : public ToggleableCellAction
{
    Q_OBJECT
public:
    VerticalText(Actions *actions);
    virtual ~VerticalText();

protected:
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class NumberFormat : public ToggleableCellAction
{
    Q_OBJECT
public:
    NumberFormat(Actions *actions);
    virtual ~NumberFormat();

protected:
    virtual QAction *createAction() override;
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class ScientificFormat : public ToggleableCellAction
{
    Q_OBJECT
public:
    ScientificFormat(Actions *actions);
    virtual ~ScientificFormat();

protected:
    virtual QAction *createAction() override;
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class PercentFormat : public ToggleableCellAction
{
    Q_OBJECT
public:
    PercentFormat(Actions *actions);
    virtual ~PercentFormat();

protected:
    virtual QAction *createAction() override;
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class MoneyFormat : public ToggleableCellAction
{
    Q_OBJECT
public:
    MoneyFormat(Actions *actions);
    virtual ~MoneyFormat();

protected:
    virtual QAction *createAction() override;
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class DateFormat : public ToggleableCellAction
{
    Q_OBJECT
public:
    DateFormat(Actions *actions);
    virtual ~DateFormat();

protected:
    virtual QAction *createAction() override;
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class TimeFormat : public ToggleableCellAction
{
    Q_OBJECT
public:
    TimeFormat(Actions *actions);
    virtual ~TimeFormat();

protected:
    virtual QAction *createAction() override;
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class IncreaseFontSize : public CellAction
{
    Q_OBJECT
public:
    IncreaseFontSize(Actions *actions);
    virtual ~IncreaseFontSize();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
};

class DecreaseFontSize : public CellAction
{
    Q_OBJECT
public:
    DecreaseFontSize(Actions *actions);
    virtual ~DecreaseFontSize();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
};

class IncreasePrecision : public CellAction
{
    Q_OBJECT
public:
    IncreasePrecision(Actions *actions);
    virtual ~IncreasePrecision();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
};

class DecreasePrecision : public CellAction
{
    Q_OBJECT
public:
    DecreasePrecision(Actions *actions);
    virtual ~DecreasePrecision();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
};

class DefaultStyle : public CellAction
{
    Q_OBJECT
public:
    DefaultStyle(Actions *actions);
    virtual ~DefaultStyle();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
};

/**
 * \ingroup Commands
 * \brief Sets the decimal precision level.
 */
class PrecisionCommand : public AbstractRegionCommand
{
public:
    PrecisionCommand();

    void setDecrease(bool decrease);

protected:
    bool performCommands() override;
    bool m_decrease : 1;
};

class FillColor : public CellAction
{
    Q_OBJECT
public:
    FillColor(Actions *actions);
    virtual ~FillColor();

protected Q_SLOTS:
    void triggeredFillColor(const KoColor &color);

protected:
    virtual void execute(Selection *, Sheet *, QWidget *) override
    {
    } // never called
    QAction *createAction() override;

    KoColorPopupAction *m_colorAction;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_STYLE
