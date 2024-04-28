/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2015 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SectionsSplitDialog.h"

#include <KoSection.h>
#include <KoSectionUtils.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>

#include <KLocalizedString>

SectionsSplitDialog::SectionsSplitDialog(QWidget *parent, KoTextEditor *editor)
    : KoDialog(parent)
    , m_editor(editor)
{
    setCaption(i18n("Configure sections"));
    setButtons(KoDialog::Ok | KoDialog::Cancel);
    enableButton(KoDialog::Ok, false);
    showButtonSeparator(true);
    QWidget *form = new QWidget;
    m_widget.setupUi(form);
    setMainWidget(form);

    QList<KoSection *> secStartings = KoSectionUtils::sectionStartings(editor->blockFormat());
    QList<KoSectionEnd *> secEndings = KoSectionUtils::sectionEndings(editor->blockFormat());
    foreach (KoSection *sec, secStartings) {
        m_widget.beforeList->addItem(sec->name());
    }
    foreach (KoSectionEnd *secEnd, secEndings) {
        m_widget.afterList->addItem(secEnd->name());
    }

    connect(m_widget.beforeList, &QListWidget::itemSelectionChanged, this, &SectionsSplitDialog::beforeListSelection);
    connect(m_widget.afterList, &QListWidget::itemSelectionChanged, this, &SectionsSplitDialog::afterListSelection);

    connect(this, &SectionsSplitDialog::okClicked, this, &SectionsSplitDialog::slotOkClicked);
}

void SectionsSplitDialog::afterListSelection()
{
    if (m_widget.afterList->selectedItems().size()) { // FIXME: more elegant way to check selection?
        enableButton(KoDialog::Ok, true);
        m_widget.beforeList->clearSelection();
    }
}

void SectionsSplitDialog::beforeListSelection()
{
    if (m_widget.beforeList->selectedItems().size()) {
        enableButton(KoDialog::Ok, true);
        m_widget.afterList->clearSelection();
    }
}

void SectionsSplitDialog::slotOkClicked()
{
    if (m_widget.beforeList->selectedItems().size()) {
        m_editor->splitSectionsStartings(m_widget.beforeList->currentRow());
    } else {
        m_editor->splitSectionsEndings(m_widget.afterList->currentRow());
    }
}
