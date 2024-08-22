/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Paul Mendez <paulestebanms@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrRenameCustomSlideShowCommand.h"

#include "KPrCustomSlideShows.h"
#include "KPrCustomSlideShowsModel.h"
#include "KPrDocument.h"

KPrRenameCustomSlideShowCommand::KPrRenameCustomSlideShowCommand(KPrDocument *doc,
                                                                 KPrCustomSlideShowsModel *model,
                                                                 const QString &oldName,
                                                                 const QString &newName,
                                                                 KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_doc(doc)
    , m_model(model)
    , m_oldName(oldName)
    , m_newName(newName)
{
    setText(kundo2_i18n("Rename custom slide show"));
}

KPrRenameCustomSlideShowCommand::~KPrRenameCustomSlideShowCommand() = default;

void KPrRenameCustomSlideShowCommand::redo()
{
    m_doc->customSlideShows()->rename(m_oldName, m_newName);
    m_model->updateCustomSlideShowsList(m_newName);
}

void KPrRenameCustomSlideShowCommand::undo()
{
    m_doc->customSlideShows()->rename(m_newName, m_oldName);
    m_model->updateCustomSlideShowsList(m_oldName);
}
