/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022-2023 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ActionDialog.h"

using namespace Calligra::Sheets;

/******************************************************************/
/* class CharacterSelectDialog                                           */
/******************************************************************/

ActionDialog::ActionDialog(QWidget *parent, ButtonCodes extraButtons)
    : KoDialog(parent)
{
    setModal(false);
    ButtonCodes buttons = Apply | Close;
    buttons |= extraButtons;
    setButtons(buttons);
    setDefaultButton(Apply);

    connect(this, &KoDialog::applyClicked, this, &ActionDialog::slotApply);
    connect(this, &KoDialog::closeClicked, this, &ActionDialog::slotClose);
}

ActionDialog::~ActionDialog() = default;

void ActionDialog::slotApply()
{
    onApply();
}

void ActionDialog::slotClose()
{
    onClose();
    accept();
}

void ActionDialog::showEvent(QShowEvent *event)
{
    QWidget *w = defaultWidget();
    if (w)
        w->setFocus();
    KoDialog::showEvent(event);
}
