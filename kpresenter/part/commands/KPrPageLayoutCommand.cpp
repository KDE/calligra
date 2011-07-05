/* This file is part of the KDE project
 * Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or ( at your option ) any later version.
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

#include "KPrPageLayoutCommand.h"

#include "pagelayout/KPrPlaceholders.h"

KPrPageLayoutCommand::KPrPageLayoutCommand( KPrPlaceholders * placeholders, KPrPageLayout * layout, KUndo2Command *parent )
: KUndo2Command( parent )
, m_placeholders( placeholders )
, m_oldLayout( placeholders->layout() )
, m_newLayout( layout )
{
}

KPrPageLayoutCommand::~KPrPageLayoutCommand()
{
    // nothing needs to be deleted here as the layouts are only used but not owned
}

void KPrPageLayoutCommand::redo()
{
    KUndo2Command::redo();
    m_placeholders->setLayout( m_newLayout );
}

void KPrPageLayoutCommand::undo()
{
    KUndo2Command::undo();
    m_placeholders->setLayout( m_oldLayout );
}
