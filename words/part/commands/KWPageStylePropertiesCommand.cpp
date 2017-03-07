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

#include "KWPageStylePropertiesCommand.h"
#include "KWDocument.h"
#include "KWPage.h"
#include "KWPageStyle_p.h"
#include "frames/KWFrame.h"

#include <KoShapeMoveCommand.h>

KWPageStylePropertiesCommand::KWPageStylePropertiesCommand(KWDocument *document, const KWPageStyle &styleBefore, const KWPageStyle &styleAfter, KUndo2Command *parent)
    : KUndo2Command(kundo2_i18n("Page Properties"), parent),
    m_document(document),
    m_style(styleBefore),
    m_styleBefore(styleBefore),
    m_styleAfter(styleAfter)
{
    Q_ASSERT(m_styleAfter != m_styleBefore); // would be kinda useless
    m_styleBefore.detach(m_styleBefore.name()); // all mine now!
    m_styleAfter.detach(m_styleAfter.name()); // all mine now!
}

void KWPageStylePropertiesCommand::redo()
{
    KUndo2Command::redo();
    m_style.priv()->copyProperties(m_styleAfter.priv());
    m_document->updatePagesForStyle(m_style);
}

void KWPageStylePropertiesCommand::undo()
{
    KUndo2Command::undo();
    m_style.priv()->copyProperties(m_styleBefore.priv());
    m_document->updatePagesForStyle(m_style);
}

#if 0
void KWPageStylePropertiesCommand::setLayout(const KoPageLayout &layout)
{
    KWPageStyle style = m_page.pageStyle();
    style.setPageLayout(layout);

    foreach (KWPage page, m_document->m_pageManager.pages(style.name())) {
        page.setPageSide(page.pageNumber() % 2 == 0 ? KWPage::Left : KWPage::Right);
    }
}
#endif
