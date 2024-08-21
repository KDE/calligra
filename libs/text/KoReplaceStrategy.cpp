/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoReplaceStrategy.h"

#include <KLocalizedString>
#include <KMessageBox>
#include <QTextCursor>
#include <kfind.h>
#include <kreplacedialog.h>

#include "FindDirection_p.h"

KoReplaceStrategy::KoReplaceStrategy(QWidget *parent)
    : m_dialog(new KReplaceDialog(parent))
    , m_replaced(0)
{
    m_dialog->setOptions(KFind::FromCursor);
}

KoReplaceStrategy::~KoReplaceStrategy()
{
    if (m_dialog->parent() == nullptr)
        delete m_dialog;
}

KFindDialog *KoReplaceStrategy::dialog() const
{
    return m_dialog;
}

void KoReplaceStrategy::reset()
{
    m_replaced = 0;
}

void KoReplaceStrategy::displayFinalDialog()
{
    if (m_replaced == 0) {
        KMessageBox::information(m_dialog->parentWidget(), i18n("Found no match\n\nNo text was replaced"));
    } else {
        KMessageBox::information(m_dialog->parentWidget(), i18np("1 replacement made", "%1 replacements made", m_replaced));
    }
    reset();
}

bool KoReplaceStrategy::foundMatch(QTextCursor &cursor, FindDirection *findDirection)
{
    bool replace = true;
    if ((m_dialog->options() & KReplaceDialog::PromptOnReplace) != 0) {
        findDirection->select(cursor);
        // TODO: not only Yes and No, but Yes, No, All and Cancel
        int value = KMessageBox::questionTwoActions(m_dialog->parentWidget(),
                                                    i18n("Replace %1 with %2?", m_dialog->pattern(), m_dialog->replacement()),
                                                    {},
                                                    KStandardGuiItem::ok(),
                                                    KStandardGuiItem::cancel());
        if (value != KMessageBox::PrimaryAction) {
            replace = false;
        }
    }

    if (replace) {
        cursor.insertText(m_dialog->replacement());
        ++m_replaced;
    }

    return true;
}
