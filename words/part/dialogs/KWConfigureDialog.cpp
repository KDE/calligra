/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KWConfigureDialog.h"

#include "KWCanvasBase.h"
#include "KWView.h"

#include <KoConfigAuthorPage.h>
#include <KoConfigDocumentPage.h>
#include <KoConfigGridPage.h>
#include <KoConfigMiscPage.h>
#include <KoDocument.h>
#include <KoIcon.h>
#include <KoShapeController.h>

#include <KLocalizedString>

#include <QPushButton>

KWConfigureDialog::KWConfigureDialog(KWView *parent)
    : KPageDialog(parent)
{
    setFaceType(FlatList);
    setMinimumWidth(1100);
    setWindowTitle(i18n("Configure"));
    setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::RestoreDefaults);

    m_miscPage = new KoConfigMiscPage(parent->koDocument(), parent->canvasBase()->shapeController()->resourceManager());
    KPageWidgetItem *item = addPage(m_miscPage, i18n("Misc"));
    item->setHeader(i18n("Misc"));
    item->setIcon(koIcon("preferences-other"));

    m_gridPage = new KoConfigGridPage(parent->koDocument());
    item = addPage(m_gridPage, i18n("Grid"));
    item->setHeader(i18n("Grid"));
    item->setIcon(koIcon("view-grid"));

    connect(m_miscPage, &KoConfigMiscPage::unitChanged, m_gridPage, &KoConfigGridPage::slotUnitChanged);

    m_docPage = new KoConfigDocumentPage(parent->koDocument());
    item = addPage(m_docPage, i18nc("@title:tab Document settings page", "Document"));
    item->setHeader(i18n("Document Settings"));
    item->setIcon(koIcon("document-properties"));

    m_authorPage = new KoConfigAuthorPage();
    item = addPage(m_authorPage, i18nc("@title:tab Author page", "Author"));
    item->setHeader(i18n("Author"));
    item->setIcon(koIcon("user-identity"));

    connect(buttonBox(), &QDialogButtonBox::accepted, this, &KWConfigureDialog::slotApply);
    connect(buttonBox(), &QDialogButtonBox::clicked, this, &KWConfigureDialog::handleButtonClicked);
    connect(this, &KWConfigureDialog::changed, parent, &KoView::slotUpdateAuthorProfileActions);
}

void KWConfigureDialog::slotApply()
{
    m_gridPage->apply();
    m_miscPage->apply();
    m_docPage->apply();
    m_authorPage->apply();

    Q_EMIT changed();
}

void KWConfigureDialog::slotDefault()
{
    QWidget *curr = currentPage()->widget();

    if (curr == m_gridPage) {
        m_gridPage->slotDefault();
    } else if (curr == m_docPage) {
        m_docPage->slotDefault();
    }
}

void KWConfigureDialog::handleButtonClicked(QAbstractButton *button)
{
    if (button == buttonBox()->button(QDialogButtonBox::RestoreDefaults)) {
        slotDefault();
    } else if (button == buttonBox()->button(QDialogButtonBox::Apply)) {
        slotApply();
    }
}
