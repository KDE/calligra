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

#include "KPrAddCustomSlideShowCommand.h"
#include "KPrDocument.h"
#include "KPrCustomSlideShowsModel.h"
#include "KPrCustomSlideShows.h"
#include "KoPAPageBase.h"

KPrAddCustomSlideShowCommand::KPrAddCustomSlideShowCommand(KPrDocument *doc, KPrCustomSlideShowsModel *model, const QString &name, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_doc(doc)
    , m_model(model)
    , m_name(name)
{
    setText(kundo2_i18n("Add custom slide show"));
}

KPrAddCustomSlideShowCommand::~KPrAddCustomSlideShowCommand()
{
}

void KPrAddCustomSlideShowCommand::redo()
{
    m_doc->customSlideShows()->insert(m_name, QList<KoPAPageBase*>());
    m_model->updateCustomSlideShowsList(m_name);
}

void KPrAddCustomSlideShowCommand::undo()
{
    m_doc->customSlideShows()->remove(m_name);
    m_model->updateCustomSlideShowsList(m_name);
}
