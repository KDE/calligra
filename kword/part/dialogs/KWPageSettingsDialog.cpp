/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
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

#include "KWPageSettingsDialog.h"

#include "KWDocument.h"
#include "commands/KWPagePropertiesCommand.h"

#include <QTimer>

//#include <KDebug>

KWPageSettingsDialog::KWPageSettingsDialog(QWidget *parent, KWDocument *document, KWPage *page)
    : KoPageLayoutDialog(parent, page->pageStyle()->pageLayout()),
    m_document(document),
    m_page(page)
{
    Q_ASSERT(document);
    Q_ASSERT(page);

    setPageSpread(m_page->pageSide() == KWPage::PageSpread);
    setTextDirection(m_page->directionHint());
}

void KWPageSettingsDialog::accept()
{
    if (applyToDocument()) {
        // TODO
    }
    else {
        KoText::Direction newDir = textDirection();
        KWPagePropertiesCommand *cmd = new KWPagePropertiesCommand(m_document, m_page, pageLayout(), newDir);
        m_document->addCommand(cmd);
    }

    KoPageLayoutDialog::accept();
}

void KWPageSettingsDialog::reject()
{
    KoPageLayoutDialog::reject();
}
