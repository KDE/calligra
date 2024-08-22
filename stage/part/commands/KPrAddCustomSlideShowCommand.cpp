/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Paul Mendez <paulestebanms@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrAddCustomSlideShowCommand.h"
#include "KPrCustomSlideShows.h"
#include "KPrCustomSlideShowsModel.h"
#include "KPrDocument.h"
#include "KoPAPageBase.h"

KPrAddCustomSlideShowCommand::KPrAddCustomSlideShowCommand(KPrDocument *doc, KPrCustomSlideShowsModel *model, const QString &name, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_doc(doc)
    , m_model(model)
    , m_name(name)
{
    setText(kundo2_i18n("Add custom slide show"));
}

KPrAddCustomSlideShowCommand::~KPrAddCustomSlideShowCommand() = default;

void KPrAddCustomSlideShowCommand::redo()
{
    m_doc->customSlideShows()->insert(m_name, QList<KoPAPageBase *>());
    m_model->updateCustomSlideShowsList(m_name);
}

void KPrAddCustomSlideShowCommand::undo()
{
    m_doc->customSlideShows()->remove(m_name);
    m_model->updateCustomSlideShowsList(m_name);
}
