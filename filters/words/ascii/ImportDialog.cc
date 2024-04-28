/*
   This file is part of the KDE project
   SPDX-FileCopyrightText: 2001, 2002, 2003, 2004 Nicolas GOUTTE <goutte@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ImportDialog.h"

#include <QRadioButton>
#include <QApplication>

#include <KLocalizedString>
#include <KComboBox>
#include <KMessageBox>

#include "AsciiImportDebug.h"
#include <ui_ImportDialogUI.h>

AsciiImportDialog::AsciiImportDialog(QWidget* parent)
: KoDialog(parent)
{
    QWidget *widget = new QWidget();
    m_ui.setupUi(widget);
    m_radioGroup.addButton(m_ui.radioParagraphAsIs);
    m_radioGroup.addButton(m_ui.radioParagraphSentence);
    m_radioGroup.addButton(m_ui.radioParagraphEmptyLine);

    setButtons(Ok | Cancel);
    setCaption(i18n("Words's Plain Text Import Filter"));
    QApplication::restoreOverrideCursor();

    setMainWidget(widget);
}

AsciiImportDialog::~AsciiImportDialog()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
}

AsciiImportDialog::ParagraphStrategy AsciiImportDialog::getParagraphStrategy() const
{
    QAbstractButton* checkedButton = m_radioGroup.checkedButton();
    if (m_ui.radioParagraphAsIs == checkedButton) {
        return EndOfLine;
    }
    else if (m_ui.radioParagraphSentence == checkedButton) {
        return EndOfSentence;
    }
    else if (m_ui.radioParagraphEmptyLine == checkedButton) {
        return EmptyLine;
    }
    return EndOfLine;
}
