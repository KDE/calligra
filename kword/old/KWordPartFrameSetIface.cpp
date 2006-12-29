/* This file is part of the KDE project
   Copyright (C) 2002 Laurent MONTEL <lmontel@mandrakesoft.com>

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

#include "KWordPartFrameSetIface.h"
#include "KWDocument.h"
#include "KWPartFrameSet.h"
#include "KWView.h"
#include "KWCanvas.h"

#include <kapplication.h>
#include <kparts/partmanager.h>
#include <dcopclient.h>

KWordPartFrameSetIface::KWordPartFrameSetIface( KWPartFrameSet *fs )
    : KWordFrameSetIface( fs )
{
   m_partFrameSet = fs;
}

void KWordPartFrameSetIface::startEditing()
{
    if ( m_partFrameSet->isDeleted() )
        return; // DCOPRef();
    KWDocument *doc = m_partFrameSet->kWordDocument();
    KWView* view = doc->getAllViews().first();
    KoDocument* part = m_partFrameSet->getChild()->document();
    if ( !part || !view )
        return;
    view->partManager()->addPart( part, false );
    view->partManager()->setActivePart( part, view );
    /* return DCOPRef( kapp->dcopClient()->appId(),
       (static_cast<KWPartFrameSetEdit *>( canvas->currentFrameSetEdit()))->dcopObject()->objId() ); */
}
