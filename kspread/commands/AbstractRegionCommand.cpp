/* This file is part of the KDE project
   Copyright 2005,2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
   Boston, MA 02110-1301, USA.
*/

#include <klocale.h>
#include <kmessagebox.h>

#include "Cell.h"
#include "Doc.h"
#include "Map.h"
#include "Sheet.h"

#include "AbstractRegionCommand.h"

using namespace KSpread;

//BEGIN NOTE Stefan: some words on operations
//
// 1. SubTotal
// a) Makes no sense to extend to non-contiguous selections (NCS) as
//    it refers to a change in one column.
// b) No special undo command available yet.
//
// 2. AutoSum
// a) should insert cell at the end of the selection, if the last
//    is not empty
// b) opens an editor, if the user's intention is fuzzy -> hard to
//    convert to NCS
//END

/***************************************************************************
  class AbstractRegionCommand
****************************************************************************/

AbstractRegionCommand::AbstractRegionCommand()
  : Region(),
    QUndoCommand(),
    m_sheet(0),
    m_reverse(false),
    m_firstrun(true),
    m_register(true),
    m_success(true)
{
}

AbstractRegionCommand::~AbstractRegionCommand()
{
}

bool AbstractRegionCommand::execute()
{
    if ( !m_firstrun )
        return false;
    // registering in undo history?
    if ( m_register )
        m_sheet->doc()->addCommand( this );
    else
        redo();
    return m_success;
}

void AbstractRegionCommand::redo()
{
    if ( !m_sheet )
    {
        kWarning() << "AbstractRegionCommand::redo(): No explicit m_sheet is set. "
                   << "Manipulating all sheets of the region." << endl;
    }

    bool successfully = true;
    successfully = preProcessing();
    if ( !successfully )
    {
        m_success = false;
        return;   // do nothing if pre-processing fails
    }

    m_sheet->doc()->setUndoLocked( true );
    m_sheet->doc()->emitBeginOperation();
    m_sheet->setRegionPaintDirty( *this );

    successfully = mainProcessing();
    if ( !successfully )
    {
        m_success = false;
        kWarning() << "AbstractRegionCommand::redo(): processing was not successful!" << endl;
    }

    successfully = true;
    successfully = postProcessing();
    if ( !successfully )
    {
        m_success = false;
        kWarning() << "AbstractRegionCommand::redo(): postprocessing was not successful!" << endl;
    }

    m_sheet->doc()->emitEndOperation();
    m_sheet->doc()->setUndoLocked( false );

    m_firstrun = false;
}

void AbstractRegionCommand::undo()
{
    kDebug() << k_funcinfo << " " << m_name << endl;
  m_reverse = !m_reverse;
  redo();
  m_reverse = !m_reverse;
}

bool AbstractRegionCommand::preProcessing()
{
    // If the sheet's protection is not enabled, cell protection statue will not take effect.
    if ( !m_sheet->isProtected() )
        return true;

    bool notProtected = true;
    Region::ConstIterator endOfList( constEnd() );
    for ( Region::ConstIterator it = constBegin(); it != endOfList; ++it )
    {
        const QRect range = (*it)->rect();

        for ( int col = range.left(); col <= range.right(); ++col )
        {
            for ( int row = range.top(); row <= range.bottom(); ++row )
            {
                Cell cell( m_sheet, col, row );
                if ( !cell.style().notProtected() )
                {
                    notProtected = false;
                    break;
                }
            }
            if ( !notProtected )
            {
                KMessageBox::error( 0, i18n( "Processing is not possible, "
                                             "because some cells are protected." ) );
                break;
            }
        }
    }
    return notProtected;
}

bool AbstractRegionCommand::mainProcessing()
{
  bool successfully = true;
  Region::Iterator endOfList(cells().end());
  for (Region::Iterator it = cells().begin(); it != endOfList; ++it)
  {
    successfully = successfully && process(*it);
  }
  return successfully;
}
