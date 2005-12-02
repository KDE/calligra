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


#include "KWBgSpellCheck.h"

#include "KWDocument.h"
#include "KWTextFrameSet.h"

#include "KoTextIterator.h"
#include "KoTextObject.h"

#include <kspell2/broker.h>
using namespace KSpell2;

#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>


KWBgSpellCheck::KWBgSpellCheck(KWDocument *_doc)
    : KoBgSpellCheck( Broker::openBroker( KSharedConfig::openConfig( "kwordrc" ) ),
                      _doc )
{
    m_doc=_doc;
    m_currentFrame=0L;
}

KWBgSpellCheck::~KWBgSpellCheck()
{
}

KoTextIterator *KWBgSpellCheck::createWholeDocIterator() const
{
    QValueList<KoTextObject *> objects = m_doc->visibleTextObjects( 0 );

    kdDebug()<<"Number of visible text objects = "<< objects.count() << endl;

    if ( objects.isEmpty() )
        return 0;

    return new KoTextIterator( objects, 0, 0 );
}
