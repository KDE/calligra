/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2009 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPAPageMoveCommand.h"

#include <KLocalizedString>

#include "KoPADocument.h"
#include "KoPAPageBase.h"

KoPAPageMoveCommand::KoPAPageMoveCommand(KoPADocument *document, KoPAPageBase *page, KoPAPageBase *after, KUndo2Command *parent)
    : KoPAPageMoveCommand(document, QList<KoPAPageBase *>{page}, after, parent)
{
}

KoPAPageMoveCommand::KoPAPageMoveCommand(KoPADocument *document, const QList<KoPAPageBase *> &pages, KoPAPageBase *after, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_document(document)
    , m_after(after)
{
    Q_ASSERT(document);
    Q_ASSERT(pages.size() > 0);

    for (KoPAPageBase *page : pages) {
        int index = m_document->pageIndex(page);
        m_pageIndices.insert(index, page);
    }

    if (pages.at(0)->pageType() == KoPageApp::Slide) {
        setText(kundo2_i18np("Move slide", "Move slides", pages.size()));
    } else {
        setText(kundo2_i18np("Move page", "Move pages", pages.size()));
    }
}

KoPAPageMoveCommand::~KoPAPageMoveCommand() = default;

void KoPAPageMoveCommand::redo()
{
    KoPAPageBase *after = m_after;
    const auto pages = m_pageIndices.values();

    for (KoPAPageBase *page : pages) {
        m_document->takePage(page);
        m_document->insertPage(page, after);
        after = page;
    }
}

void KoPAPageMoveCommand::undo()
{
    const auto pages = m_pageIndices.values();
    for (KoPAPageBase *page : pages) {
        m_document->takePage(page);
    }

    for (auto it = m_pageIndices.constBegin(); it != m_pageIndices.constEnd(); ++it) {
        m_document->insertPage(it.value(), it.key());
    }
}
