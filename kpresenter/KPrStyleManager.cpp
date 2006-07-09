// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>

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

#include "KPrDocument.h"
#include "KPrStyleManager.h"
#include "KPrStyleManager.moc"
#include <KoUnit.h>
#include <kdebug.h>
#include <KoStyleManager.h>
#include <KoStyleCollection.h>

KPrStyleManager::KPrStyleManager( QWidget *_parent, KoUnit::Unit unit,KPrDocument *_doc,
                                  const KoStyleCollection& styleCollection, const QString & activeStyleName)
    : KoStyleManager(_parent,unit,styleCollection,activeStyleName)
{
    m_doc = _doc;
}

KoParagStyle* KPrStyleManager::addStyleTemplate(KoParagStyle *style)
{
    m_doc->setModified( true );
    return m_doc->styleCollection()->addStyle(style);
}

void KPrStyleManager::applyStyleChange( KoStyleChangeDefMap changed )
{
    m_doc->applyStyleChange( changed );
}

void KPrStyleManager::removeStyleTemplate( KoParagStyle *style )
{
    m_doc->setModified( true );
    m_doc->styleCollection()->removeStyle(style);
}

void KPrStyleManager::updateAllStyleLists()
{
    m_doc->updateAllStyleLists();
}

void KPrStyleManager::updateStyleListOrder( const QStringList & list)
{
    m_doc->updateStyleListOrder( list );
}
