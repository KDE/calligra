/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kwdoc.h"
#include "stylist.h"
#include "stylist.moc"
#include <koUnit.h>
#include <kdebug.h>
/******************************************************************/
/* Class: KWStyleManager                                          */
/******************************************************************/

KWStyleManager::KWStyleManager( QWidget *_parent, KoUnit::Unit unit,KWDocument *_doc )
    : KoStyleManager(_parent,unit)
{
    m_doc = _doc;
}

const QPtrList<KoStyle> & KWStyleManager::styleList()
{
    kdDebug()<<"*********************************************************************************************************coucou\n";
    return m_doc->styleList();
}

KoStyle* KWStyleManager::addStyleTemplate(KoStyle *style)
{
    return m_doc->addStyleTemplate(style);
}

void KWStyleManager::applyStyleChange( KoStyle * changedStyle, int paragLayoutChanged, int formatChanged )
{
    m_doc->applyStyleChange( changedStyle, paragLayoutChanged,formatChanged );
}

void KWStyleManager::removeStyleTemplate( KoStyle *style )
{
    m_doc->removeStyleTemplate(style);
}

void KWStyleManager::updateAllStyleLists()
{
    m_doc->updateAllStyleLists();
}
