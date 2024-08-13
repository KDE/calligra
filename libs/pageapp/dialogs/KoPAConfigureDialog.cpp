/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoPAConfigureDialog.h"

#include "KoPAView.h"
#include <KoConfigAuthorPage.h>
#include <KoConfigDocumentPage.h>
#include <KoConfigGridPage.h>
#include <KoConfigMiscPage.h>
#include <KoPACanvasBase.h>
#include <KoShapeController.h>

#include <KoIcon.h>

#include <KLocalizedString>

#include <QPushButton>

KoPAConfigureDialog::KoPAConfigureDialog(KoPAView *parent)
    : KPageDialog(parent)
{
    setFaceType(FlatList);
    setMinimumWidth(1100);
    setWindowTitle(i18n("Configure"));
    setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::RestoreDefaults);
    button(QDialogButtonBox::Ok)->setDefault(true);

    m_miscPage = new KoConfigMiscPage(parent->koDocument(), parent->kopaCanvas()->shapeController()->resourceManager());
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

    connect(this, &QDialog::accepted, this, &KoPAConfigureDialog::slotApply);
    connect(button(QDialogButtonBox::RestoreDefaults), &QAbstractButton::clicked, this, &KoPAConfigureDialog::slotDefault);
    connect(button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this, &KoPAConfigureDialog::slotApply);
    connect(this, &KoPAConfigureDialog::changed, parent, &KoView::slotUpdateAuthorProfileActions);
}

void KoPAConfigureDialog::slotApply()
{
    m_docPage->apply();
    m_gridPage->apply();
    m_miscPage->apply();
    m_authorPage->apply();

    Q_EMIT changed();
}

void KoPAConfigureDialog::slotDefault()
{
    QWidget *curr = currentPage()->widget();

    if (curr == m_gridPage) {
        m_gridPage->slotDefault();
    } else if (curr == m_docPage) {
        m_docPage->slotDefault();
    }
}
