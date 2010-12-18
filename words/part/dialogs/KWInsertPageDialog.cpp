/* This file is part of the KDE project
 * Copyright (C) 2010 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KWInsertPageDialog.h"
#include "../KWView.h"
#include "../KWDocument.h"

#include <KLocale>

KWInsertPageDialog::KWInsertPageDialog(KWDocument *document, KWView *parent)
    : KDialog(parent),
    m_document(document),
    m_currentPageNumber(parent->currentPage().pageNumber())
{
    m_widget.setupUi(mainWidget());
    setCaption(i18n("Insert Page"));
    setButtons(KDialog::Ok | KDialog::Cancel);
    setModal(true);
    m_widget.afterCurrent->setChecked(true);
    if (m_currentPageNumber < 1) {
        m_widget.beforeCurrent->setEnabled(false);
        m_widget.afterCurrent->setEnabled(false);
        m_widget.afterLast->setChecked(true);
    } else if (parent->currentPage() == m_document->pageManager()->begin()) {
        m_widget.beforeCurrent->setEnabled(false);
    }
    QHash<QString, KWPageStyle> styles = m_document->pageManager()->pageStyles();
    foreach (const QString &style, styles.keys()) {
        m_widget.pageStyle->addItem(style);
    }
    // TODO can we select the page style a bit smarter?

    connect(this, SIGNAL(okClicked()), this, SLOT(doIt()));
}

KWInsertPageDialog::~KWInsertPageDialog()
{
}

void KWInsertPageDialog::doIt()
{
    if (m_widget.beforeCurrent->isChecked())
        m_document->insertPage(m_currentPageNumber - 1, m_widget.pageStyle->currentText());
    else if (m_widget.afterCurrent->isChecked())
        m_document->insertPage(m_currentPageNumber, m_widget.pageStyle->currentText());
    else
        m_document->appendPage(m_widget.pageStyle->currentText());
}


