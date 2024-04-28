/* This file is part of the KDE project
SPDX-FileCopyrightText: 2002, 2003 Laurent Montel <lmontel@mandrakesoft.com>
SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>

SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KarbonConfigureDialog.h"

#include "KarbonConfigInterfacePage.h"
#include "KarbonDocument.h"
#include "KarbonPart.h"
#include "KarbonView.h"

#include <KoIcon.h>

#include <KoConfigAuthorPage.h>
#include <KoConfigDocumentPage.h>
#include <KoConfigGridPage.h>
#include <KoConfigMiscPage.h>

#include <KConfigGroup>
#include <KLocalizedString>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

KarbonConfigureDialog::KarbonConfigureDialog(KarbonView *parent)
    : KPageDialog(parent)
{
    setFaceType(List);
    setWindowTitle(i18n("Configure"));
    setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::RestoreDefaults | QDialogButtonBox::Apply);

    QPushButton *okButton = button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);

    m_interfacePage = new KarbonConfigInterfacePage(parent);
    KPageWidgetItem *item = addPage(m_interfacePage, i18n("Interface"));
    item->setHeader(i18n("Interface"));
    item->setIcon(koIcon("preferences-desktop-theme"));

    m_miscPage = new KoConfigMiscPage(parent->part(), parent->part()->resourceManager());
    item = addPage(m_miscPage, i18n("Misc"));
    item->setHeader(i18n("Misc"));
    item->setIcon(koIcon("preferences-other"));

    m_gridPage = new KoConfigGridPage(parent->part());
    item = addPage(m_gridPage, i18n("Grid"));
    item->setHeader(i18n("Grid"));
    item->setIcon(koIcon("view-grid"));

    connect(m_miscPage, &KoConfigMiscPage::unitChanged, m_gridPage, &KoConfigGridPage::slotUnitChanged);

    m_defaultDocPage = new KoConfigDocumentPage(parent->part());
    item = addPage(m_defaultDocPage, i18nc("@title:tab Document settings page", "Document"));
    item->setHeader(i18n("Document Settings"));
    item->setIcon(koIcon("document-properties"));

    m_authorPage = new KoConfigAuthorPage();
    item = addPage(m_authorPage, i18nc("@title:tab Author page", "Author"));
    item->setHeader(i18n("Author"));
    item->setIcon(koIcon("user-identity"));

    connect(okButton, &QAbstractButton::clicked, this, &KarbonConfigureDialog::slotApply);
    connect(button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this, &KarbonConfigureDialog::slotApply);
    connect(button(QDialogButtonBox::RestoreDefaults), &QAbstractButton::clicked, this, &KarbonConfigureDialog::slotDefault);
}

void KarbonConfigureDialog::slotApply()
{
    m_interfacePage->apply();
    m_miscPage->apply();
    m_defaultDocPage->apply();
    m_gridPage->apply();
    m_authorPage->apply();
}

void KarbonConfigureDialog::slotDefault()
{
    QWidget *curr = currentPage()->widget();

    if (curr == m_interfacePage)
        m_interfacePage->slotDefault();
    else if (curr == m_miscPage)
        m_miscPage->slotDefault();
    else if (curr == m_gridPage)
        m_gridPage->slotDefault();
    else if (curr == m_defaultDocPage)
        m_defaultDocPage->slotDefault();
}
