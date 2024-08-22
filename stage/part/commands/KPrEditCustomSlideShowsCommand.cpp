/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Paul Mendez <paulestebanms@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrEditCustomSlideShowsCommand.h"
#include "KPrDocument.h"
#include "KoPAPageBase.h"

KPrEditCustomSlideShowsCommand::KPrEditCustomSlideShowsCommand(KPrDocument *doc,
                                                               const QString &name,
                                                               QList<KoPAPageBase *> newCustomShow,
                                                               KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_doc(doc)
    , m_name(name)
    , m_newCustomShow(newCustomShow)
    , m_oldCustomShow(doc->customSlideShows()->getByName(name))
{
    setText(kundo2_i18n("Edit custom slide show"));
}

KPrEditCustomSlideShowsCommand::~KPrEditCustomSlideShowsCommand() = default;

void KPrEditCustomSlideShowsCommand::redo()
{
    m_doc->customSlideShows()->update(m_name, m_newCustomShow);
}

void KPrEditCustomSlideShowsCommand::undo()
{
    m_doc->customSlideShows()->update(m_name, m_oldCustomShow);
}
