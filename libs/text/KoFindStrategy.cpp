/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoFindStrategy.h"

#include <KLocalizedString>
#include <KMessageBox>
#include <kfind.h>
#include <kfinddialog.h>

#include "FindDirection_p.h"

class NonClosingFindDialog : public KFindDialog
{
    Q_OBJECT
public:
    NonClosingFindDialog(QWidget *parent)
        : KFindDialog(parent)
    {
    }

    void accept() override
    {
    }
};

KoFindStrategy::KoFindStrategy(QWidget *parent)
    : m_dialog(new NonClosingFindDialog(parent))
    , m_matches(0)
{
    m_dialog->setOptions(KFind::FromCursor);
}

KoFindStrategy::~KoFindStrategy()
{
    if (m_dialog->parent() == nullptr)
        delete m_dialog;
}

KFindDialog *KoFindStrategy::dialog() const
{
    return m_dialog;
}

void KoFindStrategy::reset()
{
    m_matches = 0;
}

void KoFindStrategy::displayFinalDialog()
{
    KMessageBox::information(m_dialog, m_matches ? i18np("Found 1 match", "Found %1 matches", m_matches) : i18n("Found no match"));
    reset();
}

bool KoFindStrategy::foundMatch(QTextCursor &cursor, FindDirection *findDirection)
{
    ++m_matches;
    findDirection->select(cursor);
    return false;
}
#include "KoFindStrategy.moc"
