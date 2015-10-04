/* This file is part of the KDE project
 * Copyright (C) 2007-2008 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2011 Paul Mendez <paulestebanms@gmail.com>
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

#include "KPrDeleteSlidesCommand.h"

#include <klocalizedstring.h>

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
    setText(kundo2_i18n("Delete slide"));
}

KPrDeleteSlidesCommand::KPrDeleteSlidesCommand(KPrDocument *document, const QList<KoPAPageBase*> &pages, KUndo2Command *parent)
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
    //Save custom slides shows
    foreach (const QString &name, customShows) {
        m_customSlideShows.insert(name, m_document->customSlideShows()->getByName(name));
    }
    //Remove slide from all custom slides shows
    foreach (KoPAPageBase *page, m_pages) {
        m_document->customSlideShows()->removeSlideFromAll(page);
    }
    m_deletePages = true;
}

void KPrDeleteSlidesCommand::undo()
{
    KUndo2Command::undo();
    QMapIterator<int, KoPAPageBase*> i(m_pages);

    while (i.hasNext()) {
        i.next();
        m_document->insertPage(i.value(), i.key());
    }

    //Restore custom shows
    QMapIterator<QString, QList<KoPAPageBase*> > j(m_customSlideShows);

    while (j.hasNext()) {
        j.next();
        m_document->customSlideShows()->update(j.key(), j.value());
    }

    m_deletePages = false;
}
