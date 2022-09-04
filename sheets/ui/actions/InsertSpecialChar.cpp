/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "InsertSpecialChar.h"
#include "Actions.h"
#include "./dialogs/CharacterSelectDialog.h"

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
    : CellAction(actions, "insertSpecialChar", i18n("S&pecial Character..."), koIcon("character-set"), i18n("Insert one or more symbols or letters not found on the keyboard"))
    , m_dlg(nullptr)
{

}

InsertSpecialChar::~InsertSpecialChar()
{
    if (m_dlg) delete m_dlg;
}


void InsertSpecialChar::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    m_selection = selection;

    QString fontFamily = Cell(sheet, selection->marker()).style().fontFamily();
    QChar c = ' ';

    if (!m_dlg) {
        m_dlg = new CharacterSelectDialog(canvasWidget, "SpecialCharDialog", fontFamily, c, false);
        connect(m_dlg, &CharacterSelectDialog::insertChar,
                this, &InsertSpecialChar::specialChar);
        connect(m_dlg, &CharacterSelectDialog::finished,
                   this, &InsertSpecialChar::specialCharDialogClosed);
    }
    m_dlg->show();
}

void InsertSpecialChar::specialCharDialogClosed()
{
    if (m_dlg) {
        disconnect(m_dlg, &CharacterSelectDialog::insertChar,
                   this, &InsertSpecialChar::specialChar);
        disconnect(m_dlg, &CharacterSelectDialog::finished,
                   this, &InsertSpecialChar::specialCharDialogClosed);
        m_dlg->deleteLater();
        m_dlg = nullptr;
    }
}

void InsertSpecialChar::specialChar(QChar character, const QString& fontName)
{
    const Style style = Cell(m_selection->activeSheet(), m_selection->marker()).style();
    if (style.fontFamily() != fontName) {
        Style newStyle;
        newStyle.setFontFamily(fontName);
        m_selection->activeSheet()->fullCellStorage()->setStyle(Region(m_selection->marker()), newStyle);
    }
    QKeyEvent keyEvent(QEvent::KeyPress, 0, Qt::NoModifier, QString(character));
    CellToolBase *tool = m_actions->tool();
    if (!tool->editor()) {
        tool->createEditor();
    }
    QApplication::sendEvent(tool->editor()->widget(), &keyEvent);
}


