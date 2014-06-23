/* This file is part of the KDE project
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

#include "KPrRenameCustomSlideShowCommand.h"

#include "KPrDocument.h"
#include "KPrCustomSlideShowsModel.h"
#include "KPrCustomSlideShows.h"

KPrRenameCustomSlideShowCommand::KPrRenameCustomSlideShowCommand(KPrDocument *doc, KPrCustomSlideShowsModel *model, const QString &oldName, const QString &newName, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_doc(doc)
    , m_model(model)
    , m_oldName(oldName)
    , m_newName(newName)
{
    setText(kundo2_i18n("Rename custom slide show"));
}

KPrRenameCustomSlideShowCommand::~KPrRenameCustomSlideShowCommand()
{
}

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
