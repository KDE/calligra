/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWPageStylePropertiesCommand.h"
#include "KWDocument.h"
#include "KWPage.h"
#include "KWPageStyle_p.h"
#include "frames/KWFrame.h"

#include <KoShapeMoveCommand.h>

KWPageStylePropertiesCommand::KWPageStylePropertiesCommand(KWDocument *document,
                                                           const KWPageStyle &styleBefore,
                                                           const KWPageStyle &styleAfter,
                                                           KUndo2Command *parent)
    : KUndo2Command(kundo2_i18n("Page Properties"), parent)
    , m_document(document)
    , m_style(styleBefore)
    , m_styleBefore(styleBefore)
    , m_styleAfter(styleAfter)
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
