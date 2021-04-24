/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "StyleManagerDialog.h"
#include "StyleManager.h"

#include <KoCharacterStyle.h>
#include <KoParagraphStyle.h>
#include <QMessageBox>

StyleManagerDialog::StyleManagerDialog(QWidget *parent)
        : KoDialog(parent)
{
    setButtons(Ok | Cancel | Apply);
    m_styleManagerWidget = new StyleManager(this);
    setMainWidget(m_styleManagerWidget);
    setWindowTitle(i18n("Style Manager"));

    connect(this, &StyleManagerDialog::applyClicked, this, &StyleManagerDialog::slotApplyClicked);
}

StyleManagerDialog::~StyleManagerDialog()
{
}

void StyleManagerDialog::slotApplyClicked()
{
    if (m_styleManagerWidget->checkUniqueStyleName()) {
        m_styleManagerWidget->save();
    }
}

void StyleManagerDialog::accept()
{
    if (!m_styleManagerWidget->checkUniqueStyleName()) {
        return;
    }
    m_styleManagerWidget->save();
    KoDialog::accept();
    deleteLater();
}

void StyleManagerDialog::reject()
{
    if (m_styleManagerWidget->unappliedStyleChanges()){
        int ans = QMessageBox::warning(this, i18n("Save Changes"), i18n("You have changes that are not applied. "
        "What do you want to do with those changes?"), QMessageBox::Apply, QMessageBox::Discard, QMessageBox::Cancel);
        switch (ans) {
        case QMessageBox::Apply :
            if (m_styleManagerWidget->checkUniqueStyleName()) {
                m_styleManagerWidget->save();
                break;
            }
            return;
        case QMessageBox::Discard :
            break;
        case QMessageBox::Cancel :
            return;
        }
    }
    KoDialog::reject();
    deleteLater();
}

void StyleManagerDialog::closeEvent(QCloseEvent *e)
{
    e->ignore();
    reject();
}

void StyleManagerDialog::setStyleManager(KoStyleManager *sm)
{
    m_styleManagerWidget->setStyleManager(sm);
}

void StyleManagerDialog::setUnit(const KoUnit &unit)
{
    m_styleManagerWidget->setUnit(unit);
}

void StyleManagerDialog::setCharacterStyle(KoCharacterStyle *style, bool canDelete)
{
    m_styleManagerWidget->setCharacterStyle(style, canDelete);
}

void StyleManagerDialog::setParagraphStyle(KoParagraphStyle *style)
{
    m_styleManagerWidget->setParagraphStyle(style);
}
