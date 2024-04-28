/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007-2008 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2011 Paul Mendez <paulestebanms@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrDeleteSlidesCommand.h"

#include <KLocalizedString>

#include "KPrDocument.h"
#include "KoPAPageBase.h"

KPrDeleteSlidesCommand::KPrDeleteSlidesCommand(KPrDocument *document, KoPAPageBase *page, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_document(document)
    , m_deletePages(false)
{
    Q_ASSERT(m_document);
    Q_ASSERT(page);
    int index = m_document->pageIndex(page);
    Q_ASSERT(index != -1);
    m_pages.insert(index, page);
    setText(kundo2_i18nc("Delete one slide", "Delete slide"));
}

KPrDeleteSlidesCommand::KPrDeleteSlidesCommand(KPrDocument *document, const QList<KoPAPageBase *> &pages, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_document(document)
    , m_deletePages(false)
{
    Q_ASSERT(m_document);
    Q_ASSERT(m_document->pages().count() > pages.count());
    int index = -1;

    foreach (KoPAPageBase *page, pages) {
        Q_ASSERT(page);
        index = m_document->pageIndex(page);
        Q_ASSERT(index != -1);
        m_pages.insert(index, page);
    }

    setText(kundo2_i18np("Delete slide", "Delete slides", m_pages.count()));
}

KPrDeleteSlidesCommand::~KPrDeleteSlidesCommand()
{
    if (!m_deletePages) {
        return;
    }

    m_customSlideShows.clear();
    qDeleteAll(m_pages);
}

void KPrDeleteSlidesCommand::redo()
{
    KUndo2Command::redo();
    int index = -1;
    QStringList customShows;

    foreach (KoPAPageBase *page, m_pages) {
        customShows.append(m_document->customSlideShows()->namesByPage(page));
        index = m_document->takePage(page);
        Q_ASSERT(index != -1);
    }
    Q_UNUSED(index); // to build with unused-but-set-variable
    customShows.removeDuplicates();
    // Save custom slides shows
    foreach (const QString &name, customShows) {
        m_customSlideShows.insert(name, m_document->customSlideShows()->getByName(name));
    }
    // Remove slide from all custom slides shows
    foreach (KoPAPageBase *page, m_pages) {
        m_document->customSlideShows()->removeSlideFromAll(page);
    }
    m_deletePages = true;
}

void KPrDeleteSlidesCommand::undo()
{
    KUndo2Command::undo();
    QMapIterator<int, KoPAPageBase *> i(m_pages);

    while (i.hasNext()) {
        i.next();
        m_document->insertPage(i.value(), i.key());
    }

    // Restore custom shows
    QMapIterator<QString, QList<KoPAPageBase *>> j(m_customSlideShows);

    while (j.hasNext()) {
        j.next();
        m_document->customSlideShows()->update(j.key(), j.value());
    }

    m_deletePages = false;
}
