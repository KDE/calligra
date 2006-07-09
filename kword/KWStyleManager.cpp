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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KWDocument.h"
#include "KWStyleManager.h"
#include "KWStyleManager.moc"

#include <KoUnit.h>
#include <KoStyleCollection.h>
#include <kdebug.h>
/******************************************************************/
/* Class: KWStyleManager                                          */
/******************************************************************/

KWStyleManager::KWStyleManager( QWidget *_parent, KoUnit::Unit unit,
                                KWDocument *_doc, const KoStyleCollection & styles,
                                const QString & activeStyleName)
    : KoStyleManager( _parent, unit,
                      styles, activeStyleName,
                      ShowIncludeInToc )
{
    m_doc = _doc;
}

KoParagStyle* KWStyleManager::addStyleTemplate(KoParagStyle *style)
{
    m_doc->setModified( true);
    return m_doc->styleCollection()->addStyle(style);
}

void KWStyleManager::applyStyleChange( KoStyleChangeDefMap changed )
{
    m_doc->applyStyleChange( changed );
}

void KWStyleManager::removeStyleTemplate( KoParagStyle *style )
{
    m_doc->setModified( true);
    m_doc->styleCollection()->removeStyle(style);
}

void KWStyleManager::updateAllStyleLists()
{
    m_doc->updateAllStyleLists();
}

void KWStyleManager::updateStyleListOrder( const QStringList & list)
{
    m_doc->styleCollection()->updateStyleListOrder( list );
}


