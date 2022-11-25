/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_STYLE
#define CALLIGRA_SHEETS_ACTION_STYLE


#include "CellAction.h"


namespace Calligra
{
namespace Sheets
{

class Bold : public ToggleableCellAction {
Q_OBJECT
public:
    Bold(Actions *actions);
    virtual ~Bold();

protected:
    virtual QAction *createAction() override;
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class Italic : public ToggleableCellAction {
Q_OBJECT
public:
    Italic(Actions *actions);
    virtual ~Italic();

protected:
    virtual QAction *createAction() override;
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class Underline : public ToggleableCellAction {
Q_OBJECT
public:
    Underline(Actions *actions);
    virtual ~Underline();

protected:
    virtual QAction *createAction() override;
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class Strikeout : public ToggleableCellAction {
Q_OBJECT
public:
    Strikeout(Actions *actions);
    virtual ~Strikeout();

protected:
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class WrapText : public ToggleableCellAction {
Q_OBJECT
public:
    WrapText(Actions *actions);
    virtual ~WrapText();

protected:
    virtual QAction *createAction() override;
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class VerticalText : public ToggleableCellAction {
Q_OBJECT
public:
    VerticalText(Actions *actions);
    virtual ~VerticalText();

protected:
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class PercentFormat : public ToggleableCellAction {
Q_OBJECT
public:
    PercentFormat(Actions *actions);
    virtual ~PercentFormat();

protected:
    virtual QAction *createAction() override;
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class MoneyFormat : public ToggleableCellAction {
Q_OBJECT
public:
    MoneyFormat(Actions *actions);
    virtual ~MoneyFormat();

protected:
    virtual QAction *createAction() override;
    virtual void executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool checkedForSelection(Selection *selection, const Cell &activeCell) override;
};

class IncreaseFontSize : public CellAction {
Q_OBJECT
public:
    IncreaseFontSize(Actions *actions);
    virtual ~IncreaseFontSize();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
};

class DecreaseFontSize : public CellAction {
Q_OBJECT
public:
    DecreaseFontSize(Actions *actions);
    virtual ~DecreaseFontSize();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
};

class DefaultStyle : public CellAction {
Q_OBJECT
public:
    DefaultStyle(Actions *actions);
    virtual ~DefaultStyle();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
};











} // namespace Sheets
} // namespace Calligra

#endif   // CALLIGRA_SHEETS_ACTION_STYLE
