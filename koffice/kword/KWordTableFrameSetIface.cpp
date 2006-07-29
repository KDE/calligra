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

#include "KWordTableFrameSetIface.h"
#include "KWordTextFrameSetIface.h"
#include "KWordTextFrameSetEditIface.h"

#include "KWFrame.h"
#include "KWTableFrameSet.h"
#include <kapplication.h>
#include <dcopclient.h>
#include "KWView.h"
#include "KWDocument.h"
#include "KWCanvas.h"

KWordTableFrameSetIface::KWordTableFrameSetIface( KWTableFrameSet *_frame )
    : KWordFrameSetIface( _frame)
{
   m_table = _frame;
}

uint KWordTableFrameSetIface::nbRows()
{
  return m_table->getRows();
}

uint KWordTableFrameSetIface::numCols()
{
  return m_table->getColumns();
}
uint KWordTableFrameSetIface::numCells()
{
  return m_table->getNumCells();
}

DCOPRef KWordTableFrameSetIface::cell( int pos )
{
    // This method now sucks
    KWTableFrameSet::TableIter i(m_table);
    int p = 0;
    for(; i && p <= pos; ++i,++p)
    if( ! i.current() )
        return DCOPRef();

    return DCOPRef( kapp->dcopClient()->appId(),
            i->dcopObject()->objId() );
}

DCOPRef KWordTableFrameSetIface::cell( uint row, uint col )
{
  if( row>=m_table->getRows() || col>= m_table->getColumns())
        return DCOPRef();
    return DCOPRef( kapp->dcopClient()->appId(),
            m_table->cell(row,col)->dcopObject()->objId() );
}

DCOPRef KWordTableFrameSetIface::startEditingCell(uint row, uint col )
{
    if( row>=m_table->getRows() || col>= m_table->getColumns())
        return DCOPRef();

    KWDocument *doc=m_table->kWordDocument();
    KWView* view = doc->getAllViews().first();
    KWCanvas* canvas = view->getGUI()->canvasWidget();
    KWTextFrameSet *m_frametext=m_table->cell(row,col);
    if( !m_frametext || m_frametext->isDeleted())
        return DCOPRef();
    canvas->checkCurrentEdit(m_frametext, true);
    return DCOPRef( kapp->dcopClient()->appId(),
            (static_cast<KWTextFrameSetEdit *>( canvas->currentFrameSetEdit()))->dcopObject()->objId() );
}
