/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Paul Mendez <paulestebanms@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrDelCustomSlideShowCommand.h"
#include "KPrCustomSlideShows.h"
#include "KPrCustomSlideShowsModel.h"
#include "KPrDocument.h"
#include "KoPAPageBase.h"

KPrDelCustomSlideShowCommand::KPrDelCustomSlideShowCommand(KPrDocument *doc, KPrCustomSlideShowsModel *model, const QString &name, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_doc(doc)
    , m_model(model)
    , m_name(name)
    , m_oldCustomShow(doc->customSlideShows()->getByName(name))
{
    setText(kundo2_i18n("Delete custom slide show"));
}

KPrDelCustomSlideShowCommand::~KPrDelCustomSlideShowCommand() = default;

void KPrDelCustomSlideShowCommand::redo()
{
    m_doc->customSlideShows()->remove(m_name);
    m_model->updateCustomSlideShowsList(m_name);
}

void KPrDelCustomSlideShowCommand::undo()
{
    m_doc->customSlideShows()->insert(m_name, m_oldCustomShow);
    m_model->updateCustomSlideShowsList(m_name);
}
