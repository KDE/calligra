/* This file is part of the KDE project
Copyright (C) 2002, 2003 Laurent Montel <lmontel@mandrakesoft.com>
Copyright (C) 2006-2007 Jan Hambrecht <jaham@gmx.net>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/

#include "KarbonConfigureDialog.h"

#include "KarbonView.h"
#include "KarbonPart.h"
#include "KarbonDocument.h"
#include "KarbonConfigInterfacePage.h"

#include <KoIcon.h>

#include <KoConfigGridPage.h>
#include <KoConfigDocumentPage.h>
#include <KoConfigMiscPage.h>
#include <KoConfigAuthorPage.h>

#include <klocalizedstring.h>
#include <KConfigGroup>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

KarbonConfigureDialog::KarbonConfigureDialog(KarbonView* parent)
    : KPageDialog(parent)
{
    setFaceType(List);
    setWindowTitle(i18n("Configure"));
    setStandardButtons( QDialogButtonBox::Ok |
                        QDialogButtonBox::Cancel |
                        QDialogButtonBox::RestoreDefaults |
                        QDialogButtonBox::Apply);

    QPushButton *okButton = button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);

    m_interfacePage = new KarbonConfigInterfacePage(parent);
    KPageWidgetItem* item = addPage(m_interfacePage, i18n("Interface"));
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

    connect(m_miscPage, SIGNAL(unitChanged(KoUnit)), m_gridPage, SLOT(slotUnitChanged(KoUnit)));

    m_defaultDocPage = new KoConfigDocumentPage(parent->part());
    item = addPage(m_defaultDocPage, i18nc("@title:tab Document settings page", "Document"));
    item->setHeader(i18n("Document Settings"));
    item->setIcon(koIcon("document-properties"));

    m_authorPage = new KoConfigAuthorPage();
    item = addPage(m_authorPage, i18nc("@title:tab Author page", "Author" ));
    item->setHeader(i18n("Author"));
    item->setIcon(koIcon("user-identity"));

    connect(okButton, SIGNAL(clicked()), this, SLOT(slotApply()));
    connect(button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(slotApply()));
    connect(button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()), this, SLOT(slotDefault()));
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
    QWidget* curr = currentPage()->widget();

    if (curr == m_interfacePage)
        m_interfacePage->slotDefault();
    else if (curr == m_miscPage)
        m_miscPage->slotDefault();
    else if (curr == m_gridPage)
        m_gridPage->slotDefault();
    else if (curr == m_defaultDocPage)
        m_defaultDocPage->slotDefault();
}

