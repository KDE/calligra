/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Font.h"
#include "Actions.h"

#include "ui/commands/StyleCommand.h"

#include "core/Cell.h"
#include "core/Style.h"
#include "ui/CellEditorBase.h"
#include "ui/CellToolBase.h"
#include "ui/Selection.h"

#include <KLocalizedString>
#include <kfontaction.h>
#include <kfontsizeaction.h>

#include "KoCanvasBase.h"
#include "KoColor.h"
#include "KoColorPopupAction.h"

using namespace Calligra::Sheets;

Font::Font(Actions *actions)
    : CellAction(actions, "font", i18n("Select Font..."), QIcon(), i18n("Set the cell font"))
{
}

Font::~Font() = default;

QAction *Font::createAction()
{
    m_fontAction = new KFontAction(m_caption, m_actions->tool());
    m_fontAction->setToolTip(m_tooltip);
    m_fontAction->setIconText(i18n("Font"));
    connect(m_fontAction, &KFontAction::textTriggered, this, &Font::triggeredFont);
    return m_fontAction;
}

void Font::updateOnChange(Selection *, const Cell &activeCell)
{
    const Style style = activeCell.style();
    m_fontAction->setFont(style.fontFamily());
}

void Font::triggeredFont(const QString &name)
{
    CellToolBase *tool = m_actions->tool();
    Selection *selection = tool->selection();
    Sheet *sheet = selection->activeSheet();

    StyleCommand *command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Change Font"));
    Style s;
    s.setFontFamily(name);
    command->setStyle(s);
    command->add(*selection);
    KoCanvasBase *canvas = selection->canvas();
    command->execute(canvas);
    // Don't leave the focus in the toolbars combo box ...
    if (tool->editor()) {
        const Style style = Cell(sheet, selection->cursor()).style();
        tool->editor()->setEditorFont(style.font(), true, canvas->viewConverter());
        selection->emitRequestFocusEditor();
    } else {
        canvas->canvasWidget()->setFocus();
    }
}

FontSize::FontSize(Actions *actions)
    : CellAction(actions, "fontSize", i18n("Select Font Size"), QIcon(), i18n("Set the cell font size"))
{
}

FontSize::~FontSize() = default;

QAction *FontSize::createAction()
{
    m_fontAction = new KFontSizeAction(m_caption, m_actions->tool());
    m_fontAction->setIconText(i18n("Font Size"));
    m_fontAction->setToolTip(m_tooltip);
    connect(m_fontAction, &KFontSizeAction::fontSizeChanged, this, &FontSize::triggeredSize);
    return m_fontAction;
}

void FontSize::updateOnChange(Selection *, const Cell &activeCell)
{
    const Style style = activeCell.style();
    m_fontAction->setFontSize(style.fontSize());
}

void FontSize::triggeredSize(int size)
{
    CellToolBase *tool = m_actions->tool();
    Selection *selection = tool->selection();
    Sheet *sheet = selection->activeSheet();

    StyleCommand *command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Change Font"));
    Style s;
    s.setFontSize(size);
    command->setStyle(s);
    command->add(*selection);
    KoCanvasBase *canvas = selection->canvas();
    command->execute(canvas);

    // Don't leave the focus in the toolbars combo box ...
    if (tool->editor()) {
        const Cell cell(sheet, selection->cursor());
        tool->editor()->setEditorFont(cell.style().font(), true, canvas->viewConverter());
        selection->emitRequestFocusEditor();
    } else {
        canvas->canvasWidget()->setFocus();
    }
}

FontColor::FontColor(Actions *actions)
    : CellAction(actions, "textColor", i18n("Text Color"), koIcon("format-text-color"), i18n("Set the text color"))
{
}

FontColor::~FontColor() = default;

QAction *FontColor::createAction()
{
    m_fontAction = new KoColorPopupAction(m_actions->tool());

    m_fontAction->setText(m_caption);
    m_fontAction->setIcon(m_icon);
    m_fontAction->setToolTip(m_tooltip);
    connect(m_fontAction, &KoColorPopupAction::colorChanged, this, &FontColor::triggeredColor);
    return m_fontAction;
}

void FontColor::triggeredColor(const KoColor &color)
{
    CellToolBase *tool = m_actions->tool();
    Selection *selection = tool->selection();
    Sheet *sheet = selection->activeSheet();

    StyleCommand *command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Change Text Color"));
    Style s;
    s.setFontColor(color.toQColor());
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}

void FontColor::updateOnChange(Selection *, const Cell &activeCell)
{
    const Style style = activeCell.style();
    m_fontAction->setCurrentColor(style.fontColor());
}
