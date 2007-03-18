/* This file is part of the KDE project
   Copyright 2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2004 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "Damages.h"
#include "Doc.h"
#include "Localization.h"
#include "Sheet.h"

#include "LinkCommand.h"

using namespace KSpread;

LinkCommand::LinkCommand( const Cell& c, const QString& text, const QString& link )
{
  cell = c;
  oldText = cell.inputText();
  oldLink = cell.link();
  newText = text;
  newLink = link;

  Sheet* s = cell.sheet();
  if( s ) doc = s->doc();
}

void LinkCommand::redo()
{
  if( !cell ) return;

  if( !newText.isEmpty() )
    cell.setCellText( newText );
  cell.setLink( newLink  );

  doc->addDamage( new CellDamage( cell, CellDamage::Appearance ) );
}

void LinkCommand::undo()
{
  if( !cell ) return;

  cell.setCellText( oldText );
  cell.setLink( oldLink );

  doc->addDamage( new CellDamage( cell, CellDamage::Appearance ) );
}

QString LinkCommand::name() const
{
  return newLink.isEmpty() ? i18n("Remove Link") : i18n("Set Link");
}
