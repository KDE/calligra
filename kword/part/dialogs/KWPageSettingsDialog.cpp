/* This file is part of the KDE project
 * Copyright (C) 2007, 2010 Thomas Zander <zander@kde.org>
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
#include "KWDocumentColumns.h"

#include <KWDocument.h>
#include <commands/KWPagePropertiesCommand.h>

#include <QTimer>

//#include <KDebug>

KWPageSettingsDialog::KWPageSettingsDialog(QWidget *parent, KWDocument *document, const KWPage &page)
        : KoPageLayoutDialog(parent, page.pageStyle().pageLayout()),
        m_document(document),
        m_page(page)
{
    Q_ASSERT(document);
    Q_ASSERT(page.isValid());

    setPageSpread(m_page.pageSide() == KWPage::PageSpread);
    setTextDirection(m_page.directionHint());

    m_columns = new KWDocumentColumns(this, m_page.pageStyle().columns());
    addPage(m_columns, i18n("Columns"));
}

void KWPageSettingsDialog::accept()
{
    if (applyToDocument()) { // rename to section
        // TODO
    } else {
        KoText::Direction newDir = textDirection();
        KoPageLayout lay = pageLayout();
        if (lay.pageEdge >= 0 || lay.bindingSide >= 0) {
            // asserts check if our super didn't somehow mess up
            Q_ASSERT(lay.pageEdge >= 0);
            Q_ASSERT(lay.bindingSide >= 0);
            Q_ASSERT(lay.leftMargin == -1);
            Q_ASSERT(lay.rightMargin == -1);

            // its a page spread, which kword can handle, so we can savely set the
            // normal page size and assume that the page object will do the right thing
            lay.width /= (qreal) 2;
        }
        KWPagePropertiesCommand *cmd = new KWPagePropertiesCommand(m_document, m_page,
                lay, newDir, m_columns->columns());
        m_document->addCommand(cmd);
    }

    KoPageLayoutDialog::accept();
}

void KWPageSettingsDialog::reject()
{
    KoPageLayoutDialog::reject();
}
