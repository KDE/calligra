/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 
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

#include <qprinter.h>
#include "kspread_undo.h"

#include "kspread_table.h"

#include <qbuffer.h>

/****************************************************************************
 *
 * KSpreadUndo
 *
 ***************************************************************************/

KSpreadUndo::KSpreadUndo( KSpreadDoc *_doc )
{
    m_pDoc = _doc;
    
    m_stckUndo.setAutoDelete( FALSE );
    m_stckRedo.setAutoDelete( FALSE );
    
    m_bLocked = FALSE;
}

KSpreadUndo::~KSpreadUndo()
{
    clear();
}

void KSpreadUndo::appendUndo( KSpreadUndoAction *_action )
{
    if ( m_bLocked )
	return;
    
    m_stckRedo.setAutoDelete( TRUE );
    m_stckRedo.clear();
    m_stckRedo.setAutoDelete( FALSE );

    m_stckUndo.push( _action );

    if ( m_pDoc )
    {
	m_pDoc->enableUndo( hasUndoActions() );
	m_pDoc->enableRedo( hasRedoActions() );
    }
}

void KSpreadUndo::clear()
{
    if ( m_bLocked )
	return;
    
    m_stckUndo.setAutoDelete( TRUE );
    m_stckRedo.setAutoDelete( TRUE );

    m_stckUndo.clear();
    m_stckRedo.clear();
    
    m_stckUndo.setAutoDelete( FALSE );
    m_stckRedo.setAutoDelete( FALSE );
}

void KSpreadUndo::undo()
{
    if ( m_stckUndo.isEmpty() )
	return;
    KSpreadUndoAction *a = m_stckUndo.pop();
    a->undo();
    m_stckRedo.push( a );

    if ( m_pDoc )
    {
	m_pDoc->enableUndo( hasUndoActions() );
	m_pDoc->enableRedo( hasRedoActions() );
    }
}

void KSpreadUndo::redo()
{
    if ( m_stckRedo.isEmpty() )
	return;
    KSpreadUndoAction *a = m_stckRedo.pop();
    a->redo();
    m_stckUndo.push( a );

    if ( m_pDoc )
    {
	m_pDoc->enableUndo( hasUndoActions() );
	m_pDoc->enableRedo( hasRedoActions() );
    }
}

/****************************************************************************
 *
 * KSpreadUndoDeleteColumn
 *
 ***************************************************************************/


KSpreadUndoDeleteColumn::KSpreadUndoDeleteColumn( KSpreadDoc *_doc, KSpreadTable *_table, int _column ) :
    KSpreadUndoAction( _doc )
{
  m_pTable = _table;
  m_iColumn= _column;
  m_lstCells.setAutoDelete( TRUE );
  m_pColumnLayout = 0L; 
}
    
KSpreadUndoDeleteColumn::~KSpreadUndoDeleteColumn()
{
  if ( m_pColumnLayout )
    delete m_pColumnLayout;
}

void KSpreadUndoDeleteColumn::undo()
{
    m_pDoc->undoBuffer()->lock();

    m_pTable->insertColumn( m_iColumn);

    KSpreadCell *o;
    for ( o = m_lstCells.first(); o != 0L; o = m_lstCells.next() )
	m_pTable->insertCell( o );
    if ( m_pColumnLayout )
	m_pTable->insertColumnLayout( m_pColumnLayout );

    // TODO
    /*
    if ( m_pTable->gui() )
    {
	m_pTable->drawVisibleObjects( TRUE );
	m_pTable->gui()->hBorderWidget()->repaint();
	m_pTable->gui()->vBorderWidget()->repaint();    
    }
    */
    m_pDoc->undoBuffer()->unlock();
}

void KSpreadUndoDeleteColumn::redo()
{
    m_pDoc->undoBuffer()->lock();
    m_pTable->deleteColumn( m_iColumn);
    m_pDoc->undoBuffer()->unlock();
}

void KSpreadUndoDeleteColumn::appendCell( KSpreadCell *_cell )
{
    m_lstCells.append( _cell );
}

/****************************************************************************
 *
 * KSpreadUndoInsertColumn
 *
 ***************************************************************************/

KSpreadUndoInsertColumn::KSpreadUndoInsertColumn( KSpreadDoc *_doc, KSpreadTable *_table, int _column ) :
    KSpreadUndoAction( _doc )
{
    m_pTable = _table;
    m_iColumn= _column;
}
    
KSpreadUndoInsertColumn::~KSpreadUndoInsertColumn()
{
}

void KSpreadUndoInsertColumn::undo()
{
    m_pDoc->undoBuffer()->lock();
    m_pTable->deleteColumn( m_iColumn);
    m_pDoc->undoBuffer()->unlock();
}

void KSpreadUndoInsertColumn::redo()
{
    m_pDoc->undoBuffer()->lock();
    m_pTable->insertColumn( m_iColumn);
    m_pDoc->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoDeleteRow
 *
 ***************************************************************************/

KSpreadUndoDeleteRow::KSpreadUndoDeleteRow( KSpreadDoc *_doc, KSpreadTable *_table, int _row ) :
    KSpreadUndoAction( _doc )
{
    m_pTable = _table;
    m_iRow = _row;
    m_lstCells.setAutoDelete( TRUE );
    m_pRowLayout = 0L;
}
    
KSpreadUndoDeleteRow::~KSpreadUndoDeleteRow()
{
    if ( m_pRowLayout )
	delete m_pRowLayout;
}

void KSpreadUndoDeleteRow::undo()
{
    m_pDoc->undoBuffer()->lock();

    m_pTable->insertRow( m_iRow );

    KSpreadCell *o;
    for ( o = m_lstCells.first(); o != 0L; o = m_lstCells.next() )
	m_pTable->insertCell( o );

    if ( m_pRowLayout )
	m_pTable->insertRowLayout( m_pRowLayout );

    // TODO
    /*
    if ( m_pTable->gui() )
    {
	m_pTable->drawVisibleObjects( TRUE );
	m_pTable->gui()->hBorderWidget()->repaint();
	m_pTable->gui()->vBorderWidget()->repaint();    
    }
    */
    m_pDoc->undoBuffer()->unlock();
}

void KSpreadUndoDeleteRow::redo()
{
    m_pDoc->undoBuffer()->lock();
    m_pTable->deleteRow( m_iRow );
    m_pDoc->undoBuffer()->unlock();
}

void KSpreadUndoDeleteRow::appendCell( KSpreadCell *_cell )
{
    m_lstCells.append( _cell );
}

/****************************************************************************
 *
 * KSpreadUndoInsertRow
 *
 ***************************************************************************/

KSpreadUndoInsertRow::KSpreadUndoInsertRow( KSpreadDoc *_doc, KSpreadTable *_table, int _row ) :
    KSpreadUndoAction( _doc )
{
    m_pTable = _table;
    m_iRow = _row;
}
    
KSpreadUndoInsertRow::~KSpreadUndoInsertRow()
{
}

void KSpreadUndoInsertRow::undo()
{
    m_pDoc->undoBuffer()->lock();
    m_pTable->deleteRow( m_iRow );
    m_pDoc->undoBuffer()->unlock();
}

void KSpreadUndoInsertRow::redo()
{
    m_pDoc->undoBuffer()->lock();
    m_pTable->insertRow( m_iRow );
    m_pDoc->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoSetText
 *
 ***************************************************************************/

KSpreadUndoSetText::KSpreadUndoSetText( KSpreadDoc *_doc, KSpreadTable *_table, const char *_text, int _column, int _row ) :
    KSpreadUndoAction( _doc )
{
    m_strText = _text;
    m_iColumn= _column;
    m_iRow = _row;
    m_pTable = _table;
}

KSpreadUndoSetText::~KSpreadUndoSetText()
{
}

void KSpreadUndoSetText::undo()
{
    m_pDoc->undoBuffer()->lock();

    KSpreadCell *cell = m_pTable->nonDefaultCell( m_iColumn, m_iRow );
    m_strRedoText = cell->text();
    if ( m_strText.isNull() )
	cell->setText( "" );
    else
	cell->setText( m_strText.data() );

    // TODO
    /*
    if ( m_pTable->gui() )
	m_pTable->drawObject( obj, column, row, TRUE );
	*/

    m_pDoc->undoBuffer()->unlock();
}

void KSpreadUndoSetText::redo()
{
    m_pDoc->undoBuffer()->lock();

    KSpreadCell *cell = m_pTable->nonDefaultCell( m_iColumn, m_iRow );
    m_strText = cell->text();
    if ( m_strRedoText.isNull() )
	cell->setText( "" );
    else
	cell->setText( m_strRedoText.data() );

    // TODO
    /*
    if ( m_pTable->gui() )
	m_pTable->drawObject( obj, column, row, TRUE );
	*/

    m_pDoc->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoCellKSpreadLayout
 *
 ***************************************************************************/

KSpreadUndoCellLayout::KSpreadUndoCellLayout( KSpreadDoc *_doc, KSpreadTable *_table, QRect &_selection ) :
    KSpreadUndoAction( _doc )
{
  m_rctRect = _selection;
  m_pTable = _table;
  m_lstLayouts.setAutoDelete( TRUE );
    
  copyLayout( m_lstLayouts );
}

void KSpreadUndoCellLayout::copyLayout( QList<KSpreadLayout> &list)
{
    list.clear();
    
    for ( int y = m_rctRect.top(); y <= m_rctRect.bottom(); y++ )
	for ( int x = m_rctRect.left(); x <= m_rctRect.right(); x++ )
	{
	    KSpreadLayout *l = new KSpreadLayout( m_pTable );
	    l->copy( *(m_pTable->cellAt( x, y )) );
	    list.append( l );
	}   
}

KSpreadUndoCellLayout::~KSpreadUndoCellLayout()
{
}

void KSpreadUndoCellLayout::undo()
{
    m_pDoc->undoBuffer()->lock();

    copyLayout( m_lstRedoLayouts );
    
    KSpreadLayout *l;
    l = m_lstLayouts.first();
    
    for ( int y = m_rctRect.top(); y <= m_rctRect.bottom(); y++ )
	for ( int x = m_rctRect.left(); x <= m_rctRect.right(); x++ )
	{
	    KSpreadCell *cell = m_pTable->nonDefaultCell( x, y );
	    cell->copy( *l );
	    cell->setLayoutDirtyFlag();
	    l = m_lstLayouts.next();
	}

    // TODO
    /*
    if ( m_pTable->gui() )
	m_pTable->drawVisibleObjects( TRUE );
	*/

    m_pDoc->undoBuffer()->unlock();
}

void KSpreadUndoCellLayout::redo()
{
    m_pDoc->undoBuffer()->lock();

    KSpreadLayout *l;
    l = m_lstRedoLayouts.first();
    
    for ( int y = m_rctRect.top(); y <= m_rctRect.bottom(); y++ )
	for ( int x = m_rctRect.left(); x <= m_rctRect.right(); x++ )
	{
	    KSpreadCell *cell = m_pTable->nonDefaultCell( x, y );
	    cell->copy( *l );
	    cell->setLayoutDirtyFlag();
	    l = m_lstRedoLayouts.next();
	}
    
    // TODO
    /*
    if ( m_pTable->gui() )
	m_pTable->drawVisibleObjects( TRUE );
	*/

    m_pDoc->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoDelete
 *
 ***************************************************************************/

KSpreadUndoDelete::KSpreadUndoDelete( KSpreadDoc *_doc, KSpreadTable *, QRect &)
    : KSpreadUndoAction( _doc )
{
  /* rect = _rect;
    m_pTable = _table;
    
    QBuffer device( array );
    device.open( IO_WriteOnly );

    KorbSession *korb = new KorbSession( &device );
    KSpreadCell o_root;
    
    o_root = m_pTable->saveCells( korb, rect.left(), rect.top(), rect.right(), rect.bottom() );

    if ( o_root != 0 )
	korb->setRootObject( o_root );

    korb->release();
    delete korb;
    device.close(); */
}

KSpreadUndoDelete::~KSpreadUndoDelete()
{
}

void KSpreadUndoDelete::undo()
{
    m_pDoc->undoBuffer()->lock();
    // m_pTable->loadCells( m_array, m_rctRect.left(), m_rctRect.top() );
    m_pDoc->undoBuffer()->unlock();

    // TODO
    /*
    if ( m_pTable->gui() )
	m_pTable->drawVisibleObjects( TRUE ); */
}

void KSpreadUndoDelete::redo()
{
    m_pDoc->undoBuffer()->lock();
    m_pTable->deleteCells( m_rctRect.left(), m_rctRect.top(), m_rctRect.right(), m_rctRect.bottom() );
    m_pDoc->undoBuffer()->unlock();

    // TODO
    /*
    if ( m_pTable->gui() )
	m_pTable->drawVisibleObjects( TRUE ); */
}
