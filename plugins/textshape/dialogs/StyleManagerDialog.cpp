/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "StyleManagerDialog.h"
#include "StyleManager.h"

#include <KoCharacterStyle.h>
#include <KoParagraphStyle.h>
#include <QCloseEvent>
#include <QDialogButtonBox>
#include <QMessageBox>

StyleManagerDialog::StyleManagerDialog(QWidget *parent)
    : QDialog(parent)
{
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins({});
    m_styleManagerWidget = new StyleManager(this);
    layout->addWidget(m_styleManagerWidget);
    setWindowTitle(i18n("Style Manager"));

    connect(m_styleManagerWidget->buttonBox(), &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_styleManagerWidget->buttonBox(), &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_styleManagerWidget->buttonBox()->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &StyleManagerDialog::slotApplyClicked);
}

StyleManagerDialog::~StyleManagerDialog() = default;

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
    QDialog::accept();
    deleteLater();
}

void StyleManagerDialog::reject()
{
    if (m_styleManagerWidget->unappliedStyleChanges()) {
        int ans = QMessageBox::warning(this,
                                       i18n("Save Changes"),
                                       i18n("You have changes that are not applied. "
                                            "What do you want to do with those changes?"),
                                       QMessageBox::Apply,
                                       QMessageBox::Discard,
                                       QMessageBox::Cancel);
        switch (ans) {
        case QMessageBox::Apply:
            if (m_styleManagerWidget->checkUniqueStyleName()) {
                m_styleManagerWidget->save();
                break;
            }
            return;
        case QMessageBox::Discard:
            break;
        case QMessageBox::Cancel:
            return;
        }
    }
    QDialog::reject();
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
