// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2002 David Faure <faure@kde.org>
   2002 Laurent Montel <lmontel@mandrakesoft.com>

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


#include "KPrBgSpellCheck.h"
#include "KPrDocument.h"
#include "KPrTextObject.h"

#include "KoTextIterator.h"
#include "KoTextObject.h"

#include <kspell2/broker.h>
//Added by qt3to4:
#include <Q3ValueList>
using namespace KSpell2;

#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>

KPrBgSpellCheck::KPrBgSpellCheck(KPrDocument *_doc)
    : KoBgSpellCheck(Broker::openBroker( KSharedConfig::openConfig( "kpresenterrc" ) ), _doc )
{
    m_doc=_doc;
    m_currentObj=0L;
}

KPrBgSpellCheck::~KPrBgSpellCheck()
{
}


KoTextIterator *KPrBgSpellCheck::createWholeDocIterator() const
{
    Q3ValueList<KoTextObject *> objects = m_doc->visibleTextObjects( );

    kDebug()<<"Number of visible text objects = "<< objects.count() << endl;

    if ( objects.isEmpty() )
        return 0;

    return new KoTextIterator( objects, 0, 0 );
}

