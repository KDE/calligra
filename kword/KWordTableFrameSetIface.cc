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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "KWordTableFrameSetIface.h"
#include "KWordTextFrameSetIface.h"
#include "KWordTextFrameSetEditIface.h"

#include "kwframe.h"
#include "kwtableframeset.h"
#include <kapplication.h>
#include <dcopclient.h>
#include "kwview.h"
#include "kwdoc.h"
#include "kwcanvas.h"

KWordTableFrameSetIface::KWordTableFrameSetIface( KWTableFrameSet *_frame )
    : KWordFrameSetIface( _frame)
{
   m_table = _frame;
}

unsigned int KWordTableFrameSetIface::nbRows()
{
  return m_table->getRows();
}

unsigned int KWordTableFrameSetIface::numCols()
{
  return m_table->getCols();
}
unsigned int KWordTableFrameSetIface::numCells()
{
  return m_table->getNumCells();
}

bool KWordTableFrameSetIface::hasSelectedFrame()
{
  return m_table->hasSelectedFrame();
}

void KWordTableFrameSetIface::deselectAll()
{
  m_table->deselectAll();
}

DCOPRef KWordTableFrameSetIface::getCell( int pos )
{
    if( pos>=(int)m_table->getNumCells())
        return DCOPRef();
    return DCOPRef( kapp->dcopClient()->appId(),
		    m_table->getCell(pos)->dcopObject()->objId() );
}

DCOPRef KWordTableFrameSetIface::getCell( unsigned int row, unsigned int col )
{
  if( row>=m_table->getRows() || col>= m_table->getCols())
        return DCOPRef();
    return DCOPRef( kapp->dcopClient()->appId(),
		    m_table->getCell(row,col)->dcopObject()->objId() );
}

DCOPRef KWordTableFrameSetIface::startEditingCell(unsigned int row, unsigned int col )
{
    if( row>=m_table->getRows() || col>= m_table->getCols())
        return DCOPRef();

    KWDocument *doc=m_table->kWordDocument();
    QPtrList <KWView> lst=doc->getAllViews();
    KWTextFrameSet *m_frametext=m_table->getCell(row,col);
    if( !m_frametext)
        return DCOPRef();
    lst.at(0)->getGUI()->canvasWidget()->checkCurrentEdit(m_frametext, true);
    return DCOPRef( kapp->dcopClient()->appId(),
		    (static_cast<KWTextFrameSetEdit *>( lst.at(0)->getGUI()->canvasWidget()->currentFrameSetEdit()))->dcopObject()->objId() );
}
