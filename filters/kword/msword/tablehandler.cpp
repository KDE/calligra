/* This file is part of the KOffice project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "tablehandler.h"

#include <functor.h>
#include <functordata.h>
#include <word97_generated.h>

#include <kdebug.h>

#include <algorithm>

KWordTableHandler::KWordTableHandler()
{
    tableEnd();
}

// Called by Document before invoking the table-row-functors
void KWordTableHandler::tableStart( const QString& name )
{
    Q_ASSERT( !name.isEmpty() );
    m_currentTableName = name;
    m_row = -1;
}

void KWordTableHandler::tableEnd()
{
    m_currentTableName = QString::null;
    m_row = -2;
    m_column = -2;
    // Warning: if doing more here, check that it's still ok to call this from the ctor
}

void KWordTableHandler::tableRowStart( wvWare::SharedPtr<const wvWare::Word97::TAP> tap )
{
    kdDebug() << k_funcinfo << endl;
    if ( m_row == -2 )
    {
        kdWarning() << "tableRowStart: tableStart not called previously!" << endl;
        return;
    }
    Q_ASSERT( !m_currentTableName.isEmpty() );
    m_row++;
    m_column = -1;
    m_tap = tap;
}

void KWordTableHandler::tableRowEnd()
{
    kdDebug() << k_funcinfo << endl;
}

void KWordTableHandler::tableCellStart()
{
    Q_ASSERT( m_tap );
    if ( !m_tap )
        return;
    m_column++;
    int nbCells = m_tap->itcMac;
    Q_ASSERT( m_column < nbCells );
    if ( m_column >= nbCells )
        return;

    // Get table cell descriptor
    const wvWare::Word97::TC& tc = m_tap->rgtc[ m_column ];

    // Check for merged cells
    int colSize = 1;
    if ( tc.fFirstMerged )
    {
        // This cell is the first one of a series of merged cells ->
        // we want to find out its size.
        int i = m_column + 1;
        while ( i < nbCells && m_tap->rgtc[ i ].fMerged ) {
            ++colSize;
            ++i;
        }
    }
    if ( tc.fVertRestart )
    {
        // Vertical merging is much harder to implement....
        // ##### How do I find out the TAP of the rows below?
    }
    kdDebug() << k_funcinfo << " row=" << m_row << " column=" << m_column << " colSize=" << colSize << endl;
    emit sigTableCellStart( m_row, m_column, 1 /*TODO*/, colSize, m_currentTableName );
}

void KWordTableHandler::tableCellEnd()
{
    kdDebug() << k_funcinfo << endl;
    emit sigTableCellEnd();
}

#include "tablehandler.moc"
