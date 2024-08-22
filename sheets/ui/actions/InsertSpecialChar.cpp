/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "InsertSpecialChar.h"
#include "./dialogs/CharacterSelectDialog.h"
#include "Actions.h"

#include "core/CellStorage.h"
#include "core/Sheet.h"
#include "ui/CellEditorBase.h"
#include "ui/CellToolBase.h"
#include "ui/Selection.h"

#include <QApplication>
#include <QKeyEvent>

#include <KLocalizedString>

#include <KoCanvasBase.h>

using namespace Calligra::Sheets;

InsertSpecialChar::InsertSpecialChar(Actions *actions)
    : DialogCellAction(actions,
                       "insertSpecialChar",
                       i18n("S&pecial Character..."),
                       koIcon("character-set"),
                       i18n("Insert one or more symbols or letters not found on the keyboard"))
{
}

InsertSpecialChar::~InsertSpecialChar() = default;

ActionDialog *InsertSpecialChar::createDialog(QWidget *canvasWidget)
{
    QString fontFamily = Cell(m_selection->activeSheet(), m_selection->cursor()).style().fontFamily();

    CharacterSelectDialog *dlg = new CharacterSelectDialog(canvasWidget, fontFamily);
    connect(dlg, &CharacterSelectDialog::insertChar, this, &InsertSpecialChar::specialChar);
    return dlg;
}

void InsertSpecialChar::specialChar(QChar character, const QString &fontName)
{
    Sheet *sheet = m_selection->activeSheet();
    const Style style = Cell(sheet, m_selection->cursor()).style();
    if (style.fontFamily() != fontName) {
        Style newStyle;
        newStyle.setFontFamily(fontName);
        sheet->fullCellStorage()->setStyle(Region(m_selection->cursor()), newStyle);
    }
    QKeyEvent keyEvent(QEvent::KeyPress, 0, Qt::NoModifier, QString(character));
    CellToolBase *tool = m_actions->tool();
    if (!tool->editor()) {
        tool->createEditor();
    }
    QApplication::sendEvent(tool->editor()->widget(), &keyEvent);
}
