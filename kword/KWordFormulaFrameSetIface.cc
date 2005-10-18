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

#include "KWordFormulaFrameSetIface.h"
#include "KWordViewIface.h"
#include "KWFrame.h"
#include "KWFormulaFrameSet.h"
#include "KWView.h"
#include "KWCanvas.h"
#include "KWDocument.h"
#include <kapplication.h>
#include <dcopclient.h>
#include "KWordFrameSetIface.h"

KWordFormulaFrameSetIface::KWordFormulaFrameSetIface( KWFormulaFrameSet *_frame )
    : KWordFrameSetIface( _frame)
{
   m_formulaFrame = _frame;
}

DCOPRef KWordFormulaFrameSetIface::startEditing()
{
    if ( m_formulaFrame->isDeleted() )
        return DCOPRef();
    KWDocument *doc = m_formulaFrame->kWordDocument();
    QValueList<KWView *> views = doc->getAllViews();
    KWCanvas* canvas = views.first()->getGUI()->canvasWidget();
    canvas->editFrameSet( m_formulaFrame );
    return DCOPRef( kapp->dcopClient()->appId(),
            ( static_cast<KWFormulaFrameSetEdit *>( canvas->currentFrameSetEdit() ) )
             ->dcopObject()->objId() );
}
