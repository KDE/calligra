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

// #include <qprinter.h>

#include "kspread_undo.h"
#include "kspread_table.h"
#include "kspread_view.h"
#include "kspread_doc.h"
#include "kspread_map.h"

#include <qcstring.h>
#include <qtextstream.h>

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

QString KSpreadUndo::getRedoTitle()
{
    if ( m_stckRedo.isEmpty() )
	return QString("");
    return  m_stckRedo.current()->text();

}

QString KSpreadUndo::getUndoTitle()
{
    if ( m_stckUndo.isEmpty() )
	return QString("");
    return  m_stckUndo.current()->text();
}

/****************************************************************************
 *
 * KSpreadUndoRemoveColumn
 *
 ***************************************************************************/

KSpreadUndoRemoveColumn::KSpreadUndoRemoveColumn( KSpreadDoc *_doc, KSpreadTable *_table, int _column,int _nbCol ) :
    KSpreadUndoAction( _doc )
{
    title=i18n("Remove column(s)");
    m_tableName = _table->tableName();
    m_iColumn= _column;
    m_iNbCol=_nbCol;
    QRect selection;
    selection.setCoords( _column, 0, _column+m_iNbCol, 0x7fff );
    QDomDocument doc = _table->saveCellRect( selection );

    // Save to buffer
    QString buffer;
    QTextStream str( &buffer, IO_WriteOnly );
    str << doc;

    // This is a terrible hack to store unicode
    // data in a QCString in a way that
    // QCString::length() == QCString().size().
    // This allows us to treat the QCString like a QByteArray later on.
    m_data = buffer.utf8();
    int len = m_data.length();
    char tmp = m_data[ len - 1 ];
    m_data.resize( len );
    *( m_data.data() + len - 1 ) = tmp;
}

KSpreadUndoRemoveColumn::~KSpreadUndoRemoveColumn()
{
}

void KSpreadUndoRemoveColumn::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();

    table->insertColumn( m_iColumn,m_iNbCol);

    table->paste( m_data, QPoint( m_iColumn, 1 ) );
    if(table->getAutoCalc()) table->recalc(true);

    doc()->undoBuffer()->unlock();
}

void KSpreadUndoRemoveColumn::redo()
{
    doc()->undoBuffer()->lock();

    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    table->removeColumn( m_iColumn,m_iNbCol );

    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoInsertColumn
 *
 ***************************************************************************/

KSpreadUndoInsertColumn::KSpreadUndoInsertColumn( KSpreadDoc *_doc, KSpreadTable *_table, int _column, int _nbCol ) :
    KSpreadUndoAction( _doc )
{
    title=i18n("Insert column(s)");
    m_tableName = _table->tableName();
    m_iColumn= _column;
    m_iNbCol=_nbCol;
}

KSpreadUndoInsertColumn::~KSpreadUndoInsertColumn()
{
}

void KSpreadUndoInsertColumn::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();
    table->removeColumn( m_iColumn,m_iNbCol );
    doc()->undoBuffer()->unlock();
}

void KSpreadUndoInsertColumn::redo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();
    table->insertColumn( m_iColumn,m_iNbCol);
    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoRemoveRow
 *
 ***************************************************************************/

KSpreadUndoRemoveRow::KSpreadUndoRemoveRow( KSpreadDoc *_doc, KSpreadTable *_table, int _row,int _nbRow) :
    KSpreadUndoAction( _doc )
{
    title=i18n("Remove row(s)");
    m_tableName = _table->tableName();
    m_iRow = _row;
    m_iNbRow=  _nbRow;
    QRect selection;
    selection.setCoords( 0, _row, 0x7fff, _row+m_iNbRow );
    QDomDocument doc = _table->saveCellRect( selection );

    // Save to buffer
    QString buffer;
    QTextStream str( &buffer, IO_WriteOnly );
    str << doc;

    // This is a terrible hack to store unicode
    // data in a QCString in a way that
    // QCString::length() == QCString().size().
    // This allows us to treat the QCString like a QByteArray later on.
    m_data = buffer.utf8();
    int len = m_data.length();
    char tmp = m_data[ len - 1 ];
    m_data.resize( len );
    *( m_data.data() + len - 1 ) = tmp;

    // printf("UNDO {{{%s}}}\n", buffer.latin1() );
    // printf("UNDO2 %i bytes, length %i {{{%s}}}\n", m_data.length(), m_data.size(), (const char*)m_data );
    // printf("length=%i, size=%i", m_data.length(), m_data.size() );
    // printf("Last characters are %i %i %i\n", (int)m_data[ m_data.size() - 3 ],
    // (int)m_data[ m_data.size() - 2 ], (int)m_data[ m_data.size() - 1 ] );
}

KSpreadUndoRemoveRow::~KSpreadUndoRemoveRow()
{
}

void KSpreadUndoRemoveRow::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();

    table->insertRow( m_iRow,m_iNbRow );

    table->paste( m_data, QPoint( 1, m_iRow ) );
    if(table->getAutoCalc()) table->recalc(true);

    doc()->undoBuffer()->unlock();
}

void KSpreadUndoRemoveRow::redo()
{
    doc()->undoBuffer()->lock();

    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    table->removeRow( m_iRow,m_iNbRow );

    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoInsertRow
 *
 ***************************************************************************/

KSpreadUndoInsertRow::KSpreadUndoInsertRow( KSpreadDoc *_doc, KSpreadTable *_table, int _row,int _nbRow ) :
    KSpreadUndoAction( _doc )
{
    title=i18n("Insert row(s)");
    m_tableName = _table->tableName();
    m_iRow = _row;
    m_iNbRow=_nbRow;
}

KSpreadUndoInsertRow::~KSpreadUndoInsertRow()
{
}

void KSpreadUndoInsertRow::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();
    table->removeRow( m_iRow,m_iNbRow );
    doc()->undoBuffer()->unlock();
}

void KSpreadUndoInsertRow::redo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();
    table->insertRow( m_iRow,m_iNbRow );
    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoSetText
 *
 ***************************************************************************/

KSpreadUndoSetText::KSpreadUndoSetText( KSpreadDoc *_doc, KSpreadTable *_table, const QString& _text, int _column, int _row,KSpreadCell::formatNumber _formatNumber ) :
    KSpreadUndoAction( _doc )
{
    title=i18n("Change text");
    m_strText = _text;
    m_iColumn= _column;
    m_iRow = _row;
    m_tableName = _table->tableName();
    m_eFormatNumber=_formatNumber;
}

KSpreadUndoSetText::~KSpreadUndoSetText()
{
}

void KSpreadUndoSetText::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();

    KSpreadCell *cell = table->nonDefaultCell( m_iColumn, m_iRow );
    m_strRedoText = cell->text();
    m_eFormatNumberRedo=cell->getFormatNumber( m_iColumn, m_iRow );
    cell->setFormatNumber(m_eFormatNumber);
    if ( m_strText.isNull() )
	cell->setCellText( "" );
    else
	cell->setCellText( m_strText );

    doc()->undoBuffer()->unlock();
}

void KSpreadUndoSetText::redo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();

    KSpreadCell *cell = table->nonDefaultCell( m_iColumn, m_iRow );
    m_strText = cell->text();
    m_eFormatNumber=cell->getFormatNumber( m_iColumn, m_iRow );
    if ( m_strRedoText.isNull() )
	cell->setCellText( "" );
    else
	cell->setCellText( m_strRedoText );
    cell->setFormatNumber(m_eFormatNumberRedo);
    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoSetTableName
 *
 ***************************************************************************/

KSpreadUndoSetTableName::KSpreadUndoSetTableName( KSpreadDoc *doc, KSpreadTable *table, const QString& name ) :
    KSpreadUndoAction( doc )
{
    title=i18n("Change table name");
    m_name = name;
    m_tableName = table->tableName();
}

KSpreadUndoSetTableName::~KSpreadUndoSetTableName()
{
}

void KSpreadUndoSetTableName::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();

    m_redoName = table->tableName();

    table->setTableName( m_name,false,false );

    doc()->undoBuffer()->unlock();
}

void KSpreadUndoSetTableName::redo()
{
    KSpreadTable* table = doc()->map()->findTable( m_name );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();

    table->setTableName( m_redoName,false,false );

    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoCellLayout
 *
 ***************************************************************************/

KSpreadUndoCellLayout::KSpreadUndoCellLayout( KSpreadDoc *_doc, KSpreadTable *_table, QRect &_selection, QString &_title ) :
    KSpreadUndoAction( _doc )
{
  if( _title.isEmpty())
        title=i18n("Change layout");
  else
        title=_title;
  m_rctRect = _selection;
  m_tableName = _table->tableName();
  copyLayout( m_lstLayouts, m_lstColLayouts,m_lstRowLayouts,_table );
}

void KSpreadUndoCellLayout::copyLayout(QValueList<layoutCell> &list,QValueList<layoutColumn> &listCol,QValueList<layoutRow> &listRow, KSpreadTable* table )
{
    list.clear();

    if( m_rctRect.bottom()==0x7FFF)
    {
    for(int i=m_rctRect.left();i<=m_rctRect.right();i++)
        {
         layoutColumn tmplayout;
         tmplayout.col=i;
         tmplayout.l=new ColumnLayout( table,i );
         tmplayout.l->copy( *(table->columnLayout( i )) );
         listCol.append(tmplayout);
        }
    KSpreadCell* c = table->firstCell();
    for( ; c; c = c->nextCell() )
        {
            int col = c->column();
            if ( m_rctRect.left() <= col && m_rctRect.right() >= col
            &&!c->isObscuringForced())
            {
            layoutCell tmplayout;
            tmplayout.col=c->column();
            tmplayout.row=c->row();
            tmplayout.l=new KSpreadLayout( table );
            tmplayout.l->copy( *(table->cellAt( tmplayout.col, tmplayout.row )) );
            list.append(tmplayout);
            }
        }

    }
    else if(m_rctRect.right()==0x7FFF)
    {
    for(int i=m_rctRect.top();i<=m_rctRect.bottom();i++)
        {
         layoutRow tmplayout;
         tmplayout.row=i;
         tmplayout.l=new RowLayout( table,i );
         tmplayout.l->copy( *(table->rowLayout( i )) );
         listRow.append(tmplayout);
        }
    KSpreadCell* c = table->firstCell();
    for( ; c; c = c->nextCell() )
        {
            int row = c->row();
            if ( m_rctRect.top() <= row && m_rctRect.bottom() >= row
            &&!c->isObscuringForced())
            {
            layoutCell tmplayout;
            tmplayout.col=c->column();
            tmplayout.row=c->row();
            tmplayout.l=new KSpreadLayout( table );
            tmplayout.l->copy( *(table->cellAt( tmplayout.col, tmplayout.row )) );
            list.append(tmplayout);
            }
        }
    }
    else
    {
        for ( int y = m_rctRect.top(); y <= m_rctRect.bottom(); y++ )
	        for ( int x = m_rctRect.left(); x <= m_rctRect.right(); x++ )
                {
                KSpreadCell *cell = table->nonDefaultCell( x, y );
                if(!cell->isObscured())
                        {
                        layoutCell tmplayout;
                        tmplayout.col=x;
                        tmplayout.row=y;
                        tmplayout.l=new KSpreadLayout( table );
                        tmplayout.l->copy( *(table->cellAt( x, y )) );
                        list.append(tmplayout);
                        }
                }
     }
}

KSpreadUndoCellLayout::~KSpreadUndoCellLayout()
{
}

void KSpreadUndoCellLayout::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();

    copyLayout( m_lstRedoLayouts,m_lstRedoColLayouts,m_lstRedoRowLayouts, table );
    if( m_rctRect.bottom()==0x7FFF)
    {
    QValueList<layoutColumn>::Iterator it2;
    for ( it2 = m_lstColLayouts.begin(); it2 != m_lstColLayouts.end(); ++it2 )
        {
            ColumnLayout *col= table->nonDefaultColumnLayout( (*it2).col );
	    col->copy( *(*it2).l );

        }
    }
    else if( m_rctRect.right()==0x7FFF)
    {
    QValueList<layoutRow>::Iterator it2;
    for ( it2 = m_lstRowLayouts.begin(); it2 != m_lstRowLayouts.end(); ++it2 )
        {
            RowLayout *row= table->nonDefaultRowLayout( (*it2).row );
	    row->copy( *(*it2).l );
        }
    }

    QValueList<layoutCell>::Iterator it2;
    for ( it2 = m_lstLayouts.begin(); it2 != m_lstLayouts.end(); ++it2 )
        {
            KSpreadCell *cell = table->nonDefaultCell( (*it2).col,(*it2).row );
	    cell->copy( *(*it2).l );
	    cell->setLayoutDirtyFlag();
	    cell->setDisplayDirtyFlag();
	    table->updateCell( cell, (*it2).col, (*it2).row );
        }

    table->updateView(m_rctRect);
    doc()->undoBuffer()->unlock();
}

void KSpreadUndoCellLayout::redo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();

    if( m_rctRect.bottom()==0x7FFF)
    {
    QValueList<layoutColumn>::Iterator it2;
    for ( it2 = m_lstRedoColLayouts.begin(); it2 != m_lstRedoColLayouts.end(); ++it2 )
        {
            ColumnLayout *col= table->nonDefaultColumnLayout( (*it2).col );
	    col->copy( *(*it2).l );
        }
    }
    else if( m_rctRect.right()==0x7FFF)
    {
    QValueList<layoutRow>::Iterator it2;
    for ( it2 = m_lstRedoRowLayouts.begin(); it2 != m_lstRedoRowLayouts.end(); ++it2 )
        {
            RowLayout *row= table->nonDefaultRowLayout( (*it2).row );
	    row->copy( *(*it2).l );
        }
    }

    QValueList<layoutCell>::Iterator it2;
    for ( it2 = m_lstRedoLayouts.begin(); it2 != m_lstRedoLayouts.end(); ++it2 )
        {
            KSpreadCell *cell = table->nonDefaultCell( (*it2).col,(*it2).row );
	    cell->copy( *(*it2).l );
	    cell->setLayoutDirtyFlag();
	    cell->setDisplayDirtyFlag();
	    table->updateCell( cell, (*it2).col, (*it2).row );
        }

    table->updateView(m_rctRect);
    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoDelete
 *
 ***************************************************************************/

KSpreadUndoDelete::KSpreadUndoDelete( KSpreadDoc *_doc, KSpreadTable* table, QRect & _selection)
    : KSpreadUndoAction( _doc )
{
    title=i18n("Delete");
    m_tableName = table->tableName();
    m_selection = _selection;
    createListCell( m_data, m_lstColumn,m_lstRow,table );

}

KSpreadUndoDelete::~KSpreadUndoDelete()
{
}

void KSpreadUndoDelete::createListCell( QCString &listCell,QValueList<columnSize> &listCol,QValueList<rowSize> &listRow, KSpreadTable* table )
{
    listRow.clear();
    listCol.clear();
    //copy a column(s)
    if(m_selection.bottom()==0x7FFF)
    {
        for( int y =m_selection.left() ; y <=m_selection.right() ; ++y )
        {
           ColumnLayout *cl=table->columnLayout(y);
           if(!cl->isDefault())
                {
                columnSize tmpSize;
                tmpSize.columnNumber=y;
                tmpSize.columnWidth=cl->width();
                listCol.append(tmpSize);
                }
        }
    }
    //copy a row(s)
    else if(m_selection.right()==0x7FFF)
    {
        //save size of row(s)
        for( int y =m_selection.top() ; y <=m_selection.bottom() ; ++y )
        {
           RowLayout *rw=table->rowLayout(y);
           if(!rw->isDefault())
                {
                rowSize tmpSize;
                tmpSize.rowNumber=y;
                tmpSize.rowHeight=rw->height();
                listRow.append(tmpSize);
                }
        }

    }

    //save all cells in area
    QDomDocument doc = table->saveCellRect( m_selection );
    // Save to buffer
    QString buffer;
    QTextStream str( &buffer, IO_WriteOnly );
    str << doc;

    // This is a terrible hack to store unicode
    // data in a QCString in a way that
    // QCString::length() == QCString().size().
    // This allows us to treat the QCString like a QByteArray later on.
    listCell = buffer.utf8();
    int len = listCell.length();
    char tmp = listCell[ len - 1 ];
    listCell.resize( len );
    *( listCell.data() + len - 1 ) = tmp;

}


void KSpreadUndoDelete::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;
    createListCell( m_dataRedo, m_lstRedoColumn,m_lstRedoRow,table );

    doc()->undoBuffer()->lock();
    if(m_selection.bottom()==0x7FFF)
    {
        QValueList<columnSize>::Iterator it2;
        for ( it2 = m_lstColumn.begin(); it2 != m_lstColumn.end(); ++it2 )
        {
           ColumnLayout *cl=table->nonDefaultColumnLayout((*it2).columnNumber);
           cl->setWidth((*it2).columnWidth);
        }
    }
    else if(m_selection.right()==0x7FFF)
    {
        QValueList<rowSize>::Iterator it2;
        for ( it2 = m_lstRow.begin(); it2 != m_lstRow.end(); ++it2 )
        {
           RowLayout *rw=table->nonDefaultRowLayout((*it2).rowNumber);
           rw->setHeight((*it2).rowHeight);
        }
    }

    table->deleteCells( m_selection );
    table->paste( m_data, m_selection.topLeft() );
    if(table->getAutoCalc()) table->recalc(true);

    doc()->undoBuffer()->unlock();
}

void KSpreadUndoDelete::redo()
{

    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();
    if(m_selection.bottom()==0x7FFF)
    {
        QValueList<columnSize>::Iterator it2;
        for ( it2 = m_lstRedoColumn.begin(); it2 != m_lstRedoColumn.end(); ++it2 )
        {
           ColumnLayout *cl=table->nonDefaultColumnLayout((*it2).columnNumber);
           cl->setWidth((*it2).columnWidth);
        }
    }
    else if(m_selection.right()==0x7FFF)
    {
        QValueList<rowSize>::Iterator it2;
        for ( it2 = m_lstRedoRow.begin(); it2 != m_lstRedoRow.end(); ++it2 )
        {
           RowLayout *rw=table->nonDefaultRowLayout((*it2).rowNumber);
           rw->setHeight((*it2).rowHeight);
        }
    }

    //move next line to refreshView
    //because I must know what is the real rect
    //that I must refresh, when there is cell Merged

    table->paste( m_dataRedo, m_selection.topLeft() );
    //table->deleteCells( m_selection );
    table->refreshView( m_selection );
    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoResizeColRow
 *
 ***************************************************************************/


KSpreadUndoResizeColRow::KSpreadUndoResizeColRow( KSpreadDoc *_doc, KSpreadTable *_table, QRect &_selection ) :
    KSpreadUndoAction( _doc )
{
  title=i18n("Resize");
  m_rctRect = _selection;
  m_tableName = _table->tableName();

  createList( m_lstColumn,m_lstRow, _table );
}

void KSpreadUndoResizeColRow::createList( QValueList<columnSize> &listCol,QValueList<rowSize> &listRow, KSpreadTable* table )
{
    listCol.clear();
    listRow.clear();

    if( m_rctRect.bottom()==0x7FFF) // entire column(s)
    {
    for( int y = m_rctRect.left(); y <= m_rctRect.right(); y++ )
        {
           ColumnLayout *cl=table->columnLayout(y);
           columnSize tmpSize;
           tmpSize.columnNumber=y;
           tmpSize.columnWidth=cl->width();
           listCol.append(tmpSize);
        }
    }
    else if(m_rctRect.right()==0x7FFF) // entire row(s)
    {
    for( int y = m_rctRect.top(); y <= m_rctRect.bottom(); y++ )
        {
           RowLayout *rw=table->rowLayout(y);
           rowSize tmpSize;
           tmpSize.rowNumber=y;
           tmpSize.rowHeight=rw->height();
           listRow.append(tmpSize);
        }
    }
    else //row and column
    {
    for( int y = m_rctRect.left(); y <= m_rctRect.right(); y++ )
        {
           ColumnLayout *cl=table->columnLayout(y);
           columnSize tmpSize;
           tmpSize.columnNumber=y;
           tmpSize.columnWidth=cl->width();
           listCol.append(tmpSize);
        }
    for( int y = m_rctRect.top(); y <= m_rctRect.bottom(); y++ )
        {
           RowLayout *rw=table->rowLayout(y);
           rowSize tmpSize;
           tmpSize.rowNumber=y;
           tmpSize.rowHeight=rw->height();
           listRow.append(tmpSize);
        }

    }
}

KSpreadUndoResizeColRow::~KSpreadUndoResizeColRow()
{
}

void KSpreadUndoResizeColRow::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();

    createList( m_lstRedoColumn,m_lstRedoRow, table );

    if( m_rctRect.bottom()==0x7FFF) // complete column(s)
    {
    QValueList<columnSize>::Iterator it2;
    for ( it2 = m_lstColumn.begin(); it2 != m_lstColumn.end(); ++it2 )
        {
           ColumnLayout *cl=table->columnLayout((*it2).columnNumber);
           cl->setWidth((*it2).columnWidth);
        }
    }
    else if(m_rctRect.right()==0x7FFF) // complete row(s)
    {
    QValueList<rowSize>::Iterator it2;
    for ( it2 = m_lstRow.begin(); it2 != m_lstRow.end(); ++it2 )
        {
           RowLayout *rw=table->rowLayout((*it2).rowNumber);
           rw->setHeight((*it2).rowHeight);
        }
    }
    else // row and column
    {
    QValueList<columnSize>::Iterator it2;
    for ( it2 = m_lstColumn.begin(); it2 != m_lstColumn.end(); ++it2 )
        {
           ColumnLayout *cl=table->columnLayout((*it2).columnNumber);
           cl->setWidth((*it2).columnWidth);
        }
    QValueList<rowSize>::Iterator it1;
    for ( it1 = m_lstRow.begin(); it1 != m_lstRow.end(); ++it1 )
        {
           RowLayout *rw=table->rowLayout((*it1).rowNumber);
           rw->setHeight((*it1).rowHeight);
        }
    }

    doc()->undoBuffer()->unlock();
}

void KSpreadUndoResizeColRow::redo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();
    if( m_rctRect.bottom()==0x7FFF) // complete column(s)
    {
    QValueList<columnSize>::Iterator it2;
    for ( it2 = m_lstRedoColumn.begin(); it2 != m_lstRedoColumn.end(); ++it2 )
        {
           ColumnLayout *cl=table->columnLayout((*it2).columnNumber);
           cl->setWidth((*it2).columnWidth);
        }
    }
    else if(m_rctRect.right()==0x7FFF) // complete row(s)
    {
    QValueList<rowSize>::Iterator it2;
    for ( it2 = m_lstRedoRow.begin(); it2 != m_lstRedoRow.end(); ++it2 )
        {
           RowLayout *rw=table->rowLayout((*it2).rowNumber);
           rw->setHeight((*it2).rowHeight);
        }
    }
    else // row and column
    {
    QValueList<columnSize>::Iterator it2;
    for ( it2 = m_lstRedoColumn.begin(); it2 != m_lstRedoColumn.end(); ++it2 )
        {
           ColumnLayout *cl=table->columnLayout((*it2).columnNumber);
           cl->setWidth((*it2).columnWidth);
        }
    QValueList<rowSize>::Iterator it1;
    for ( it1 = m_lstRedoRow.begin(); it1 != m_lstRedoRow.end(); ++it1 )
        {
           RowLayout *rw=table->rowLayout((*it1).rowNumber);
           rw->setHeight((*it1).rowHeight);
        }
    }

    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoChangeAreaTextCell
 *
 ***************************************************************************/


KSpreadUndoChangeAreaTextCell::KSpreadUndoChangeAreaTextCell( KSpreadDoc *_doc, KSpreadTable *_table, QRect &_selection ) :
    KSpreadUndoAction( _doc )
{
  title=i18n("Change text");
  m_rctRect = _selection;
  m_tableName = _table->tableName();

  createList( m_lstTextCell, _table );
}

void KSpreadUndoChangeAreaTextCell::createList( QValueList<textOfCell> &list, KSpreadTable* table )
{
    list.clear();
    if( m_rctRect.bottom()==0x7FFF)
    {
        KSpreadCell* c = table->firstCell();
        for( ; c; c = c->nextCell() )
        {
            int col = c->column();
            if ( m_rctRect.left() <= col && m_rctRect.right() >= col
            &&!c->isObscuringForced())
            {
                textOfCell tmpText;
                tmpText.col=c->column();
                tmpText.row=c->row();
                tmpText.text=c->text();
                list.append(tmpText);
            }

        }
    }
    else if(m_rctRect.right()==0x7FFF)
    {

        KSpreadCell* c = table->firstCell();
        for( ; c; c = c->nextCell() )
        {
        int row = c->row();
        if ( m_rctRect.top() <= row && m_rctRect.bottom() >= row
        &&!c->isObscuringForced())
                {
                textOfCell tmpText;
                tmpText.col=c->column();
                tmpText.row=c->row();
                tmpText.text=c->text();
                list.append(tmpText);
                }
        }
    }
    else
    {
        for ( int y = m_rctRect.top(); y <= m_rctRect.bottom(); y++ )
	        for ( int x = m_rctRect.left(); x <= m_rctRect.right(); x++ )
                {
                KSpreadCell *cell = table->nonDefaultCell( x, y );
                if(!cell->isObscured())
                        {
                        textOfCell tmpText;
                        tmpText.col=x;
                        tmpText.row=y;
                        tmpText.text=cell->text();
                        list.append(tmpText);
                        }
                }
     }
}

KSpreadUndoChangeAreaTextCell::~KSpreadUndoChangeAreaTextCell()
{
}

void KSpreadUndoChangeAreaTextCell::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();

    createList( m_lstRedoTextCell, table );


    QValueList<textOfCell>::Iterator it2;
    for ( it2 = m_lstTextCell.begin(); it2 != m_lstTextCell.end(); ++it2 )
    {
        KSpreadCell *cell = table->nonDefaultCell( (*it2).col, (*it2).row );
        if ( (*it2).text.isEmpty() )
                {
                if(!cell->text().isEmpty())
	                cell->setCellText( "" );
                }
        else
	       cell->setCellText( (*it2).text );
    }

    doc()->undoBuffer()->unlock();
}

void KSpreadUndoChangeAreaTextCell::redo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();

    QValueList<textOfCell>::Iterator it2;
    for ( it2 = m_lstRedoTextCell.begin(); it2 != m_lstRedoTextCell.end(); ++it2 )
    {
        KSpreadCell *cell = table->nonDefaultCell( (*it2).col, (*it2).row );
        if ( (*it2).text.isEmpty() )
                {
                if(!cell->text().isEmpty())
	                cell->setCellText( "" );
                }
        else
	       cell->setCellText( (*it2).text );
    }

    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoMergedCell
 *
 ***************************************************************************/


KSpreadUndoMergedCell::KSpreadUndoMergedCell( KSpreadDoc *_doc, KSpreadTable *_table, int _column, int _row , int _extraX,int _extraY) :
    KSpreadUndoAction( _doc )
{
  title=i18n("Merge cells");
  m_tableName = _table->tableName();
  m_iRow=_row;
  m_iCol=_column;
  m_iExtraX=_extraX;
  m_iExtraY=_extraY;

}

KSpreadUndoMergedCell::~KSpreadUndoMergedCell()
{
}

void KSpreadUndoMergedCell::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();

    KSpreadCell *cell = table->nonDefaultCell( m_iCol, m_iRow );
    m_iExtraRedoX=cell->extraXCells();
    m_iExtraRedoY=cell->extraYCells();

    table->changeMergedCell( m_iCol, m_iRow, m_iExtraX,m_iExtraY);

    doc()->undoBuffer()->unlock();
}

void KSpreadUndoMergedCell::redo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();

    table->changeMergedCell( m_iCol, m_iRow, m_iExtraRedoX,m_iExtraRedoY);

    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoAutofill
 *
 ***************************************************************************/

KSpreadUndoAutofill::KSpreadUndoAutofill( KSpreadDoc *_doc, KSpreadTable* table, QRect & _selection)
    : KSpreadUndoAction( _doc )
{
    title=i18n("Autofill");
    m_tableName = table->tableName();
    m_selection = _selection;
    createListCell( m_data, table );

}

KSpreadUndoAutofill::~KSpreadUndoAutofill()
{
}

void KSpreadUndoAutofill::createListCell( QCString &list, KSpreadTable* table )
{
    QDomDocument doc = table->saveCellRect( m_selection );
    // Save to buffer
    QString buffer;
    QTextStream str( &buffer, IO_WriteOnly );
    str << doc;

    // This is a terrible hack to store unicode
    // data in a QCString in a way that
    // QCString::length() == QCString().size().
    // This allows us to treat the QCString like a QByteArray later on.
    list = buffer.utf8();
    int len = list.length();
    char tmp = list[ len - 1 ];
    list.resize( len );
    *( list.data() + len - 1 ) = tmp;
}

void KSpreadUndoAutofill::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    createListCell( m_dataRedo, table );

    doc()->undoBuffer()->lock();
    table->deleteCells( m_selection );
    table->paste( m_data, m_selection.topLeft() );
    if(table->getAutoCalc()) table->recalc(true);

    doc()->undoBuffer()->unlock();
}

void KSpreadUndoAutofill::redo()
{
    doc()->undoBuffer()->lock();

    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    table->deleteCells( m_selection );
    doc()->undoBuffer()->lock();
    table->paste( m_dataRedo, m_selection.topLeft() );
    if(table->getAutoCalc()) table->recalc(true);

    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoInsertCellRow
 *
 ***************************************************************************/

KSpreadUndoInsertCellRow::KSpreadUndoInsertCellRow( KSpreadDoc *_doc, KSpreadTable *_table, QRect _rect ) :
    KSpreadUndoAction( _doc )
{
    title=i18n("Insert cell");
    m_tableName = _table->tableName();
    m_rect=_rect;
}

KSpreadUndoInsertCellRow::~KSpreadUndoInsertCellRow()
{
}

void KSpreadUndoInsertCellRow::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();
    table->unshiftRow( m_rect);
    doc()->undoBuffer()->unlock();
}

void KSpreadUndoInsertCellRow::redo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();
    table->shiftRow( m_rect);
    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoInsertCellCol
 *
 ***************************************************************************/


KSpreadUndoInsertCellCol::KSpreadUndoInsertCellCol( KSpreadDoc *_doc, KSpreadTable *_table,QRect _rect ) :
    KSpreadUndoAction( _doc )
{
    title=i18n("Insert cell");
    m_tableName = _table->tableName();
    m_rect=_rect;
}

KSpreadUndoInsertCellCol::~KSpreadUndoInsertCellCol()
{
}

void KSpreadUndoInsertCellCol::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();
    table->unshiftColumn( m_rect);
    doc()->undoBuffer()->unlock();
}

void KSpreadUndoInsertCellCol::redo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();
    table->shiftColumn( m_rect );
    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoRemoveCellRow
 *
 ***************************************************************************/

KSpreadUndoRemoveCellRow::KSpreadUndoRemoveCellRow( KSpreadDoc *_doc, KSpreadTable *_table, QRect rect ) :
    KSpreadUndoAction( _doc )
{
    title=i18n("Remove cell");
    m_tableName = _table->tableName();
    m_rect=rect;
    QDomDocument doc = _table->saveCellRect( m_rect );
    // Save to buffer
    QString buffer;
    QTextStream str( &buffer, IO_WriteOnly );
    str << doc;

    // This is a terrible hack to store unicode
    // data in a QCString in a way that
    // QCString::length() == QCString().size().
    // This allows us to treat the QCString like a QByteArray later on.
    m_data = buffer.utf8();
    int len = m_data.length();
    char tmp = m_data[ len - 1 ];
    m_data.resize( len );
    *( m_data.data() + len - 1 ) = tmp;
}

KSpreadUndoRemoveCellRow::~KSpreadUndoRemoveCellRow()
{
}

void KSpreadUndoRemoveCellRow::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();
    table->shiftRow( m_rect );
    table->paste( m_data, QPoint(m_rect.left(),m_rect.top()) );
    doc()->undoBuffer()->unlock();
}

void KSpreadUndoRemoveCellRow::redo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();
    table->unshiftRow( m_rect);
    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoRemoveCellCol
 *
 ***************************************************************************/

KSpreadUndoRemoveCellCol::KSpreadUndoRemoveCellCol( KSpreadDoc *_doc, KSpreadTable *_table, QRect _rect ) :
    KSpreadUndoAction( _doc )
{
    title=i18n("Remove cell");
    m_tableName = _table->tableName();
    m_rect=_rect;
    QDomDocument doc = _table->saveCellRect( m_rect );
    // Save to buffer
    QString buffer;
    QTextStream str( &buffer, IO_WriteOnly );
    str << doc;

    // This is a terrible hack to store unicode
    // data in a QCString in a way that
    // QCString::length() == QCString().size().
    // This allows us to treat the QCString like a QByteArray later on.
    m_data = buffer.utf8();
    int len = m_data.length();
    char tmp = m_data[ len - 1 ];
    m_data.resize( len );
    *( m_data.data() + len - 1 ) = tmp;
}

KSpreadUndoRemoveCellCol::~KSpreadUndoRemoveCellCol()
{
}

void KSpreadUndoRemoveCellCol::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();
    table->shiftColumn( m_rect );
    table->paste( m_data, QPoint(m_rect.left(),m_rect.top()) );
    doc()->undoBuffer()->unlock();
}

void KSpreadUndoRemoveCellCol::redo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();
    table->unshiftColumn( m_rect );
    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoConditional
 *
 ***************************************************************************/

KSpreadUndoConditional::KSpreadUndoConditional( KSpreadDoc *_doc, KSpreadTable* table, QRect & _selection)
    : KSpreadUndoAction( _doc )
{
    title=i18n("Conditional cell attribut");
    m_tableName = table->tableName();
    m_selection = _selection;
    createListCell( m_data, table );

}

KSpreadUndoConditional::~KSpreadUndoConditional()
{
}

void KSpreadUndoConditional::createListCell( QCString &list, KSpreadTable* table )
{
    QDomDocument doc = table->saveCellRect( m_selection );
    // Save to buffer
    QString buffer;
    QTextStream str( &buffer, IO_WriteOnly );
    str << doc;

    // This is a terrible hack to store unicode
    // data in a QCString in a way that
    // QCString::length() == QCString().size().
    // This allows us to treat the QCString like a QByteArray later on.
    list = buffer.utf8();
    int len = list.length();
    char tmp = list[ len - 1 ];
    list.resize( len );
    *( list.data() + len - 1 ) = tmp;
}

void KSpreadUndoConditional::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    createListCell( m_dataRedo, table );

    doc()->undoBuffer()->lock();
    table->paste( m_data, m_selection.topLeft() );
    if(table->getAutoCalc()) table->recalc(true);

    doc()->undoBuffer()->unlock();
}

void KSpreadUndoConditional::redo()
{
    doc()->undoBuffer()->lock();

    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();
    table->paste( m_dataRedo, m_selection.topLeft() );
    if(table->getAutoCalc()) table->recalc(true);

    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoHideTable
 *
 ***************************************************************************/

KSpreadUndoHideTable::KSpreadUndoHideTable( KSpreadDoc *_doc, KSpreadTable *_table) :
    KSpreadUndoAction( _doc )
{
    title=i18n("Hide table");
    m_tableName = _table->tableName();
}

KSpreadUndoHideTable::~KSpreadUndoHideTable()
{
}

void KSpreadUndoHideTable::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();
    table->hideTable(false);
    doc()->undoBuffer()->unlock();
}

void KSpreadUndoHideTable::redo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();
    table->hideTable(true);
    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoShowTable
 *
 ***************************************************************************/


KSpreadUndoShowTable::KSpreadUndoShowTable( KSpreadDoc *_doc, KSpreadTable *_table) :
    KSpreadUndoAction( _doc )
{
    title=i18n("Show table");
    m_tableName = _table->tableName();
}

KSpreadUndoShowTable::~KSpreadUndoShowTable()
{
}

void KSpreadUndoShowTable::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();
    table->hideTable(true);
    doc()->undoBuffer()->unlock();
}

void KSpreadUndoShowTable::redo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();
    table->hideTable(false);
    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoCellPaste
 *
 ***************************************************************************/

KSpreadUndoCellPaste::KSpreadUndoCellPaste( KSpreadDoc *_doc, KSpreadTable* table, int _nbCol,int _nbRow, int _xshift,int _yshift, QRect &_selection,bool insert )
    : KSpreadUndoAction( _doc )
{
    title=i18n("Paste");
    m_tableName = table->tableName();
    m_selection = _selection;
    nbCol=_nbCol;
    nbRow=_nbRow;
    xshift=_xshift;
    yshift=_yshift;
    b_insert=insert;
    createListCell( m_data, m_lstColumn,m_lstRow,table );

}

KSpreadUndoCellPaste::~KSpreadUndoCellPaste()
{
}

void KSpreadUndoCellPaste::createListCell( QCString &listCell,QValueList<columnSize> &listCol,QValueList<rowSize> &listRow, KSpreadTable* table )
{
    listCol.clear();
    listRow.clear();
    //copy a column(s)
    if(nbCol!=0)
    {
        //save all cells
        QRect rect;
        rect.setCoords(xshift,1,xshift+nbCol,0X7FFF);
        QDomDocument doc = table->saveCellRect( rect);
        // Save to buffer
        QString buffer;
        QTextStream str( &buffer, IO_WriteOnly );
        str << doc;

        // This is a terrible hack to store unicode
        // data in a QCString in a way that
        // QCString::length() == QCString().size().
        // This allows us to treat the QCString like a QByteArray later on.
        listCell = buffer.utf8();
        int len = listCell.length();
        char tmp = listCell[ len - 1 ];
        listCell.resize( len );
        *( listCell.data() + len - 1 ) = tmp;

        //save size of columns
        for( int y = 1; y <=nbCol ; ++y )
        {
           ColumnLayout *cl=table->columnLayout(y);
           if(!cl->isDefault())
                {
                columnSize tmpSize;
                tmpSize.columnNumber=y;
                tmpSize.columnWidth=cl->width();
                listCol.append(tmpSize);
                }
        }
    }
    //copy a row(s)
    else if(nbRow!=0)
    {
        //save all cells
        QRect rect;
        rect.setCoords(1,yshift,0x7FFF,yshift+nbRow);
        QDomDocument doc = table->saveCellRect( rect);
        // Save to buffer
        QString buffer;
        QTextStream str( &buffer, IO_WriteOnly );
        str << doc;

        // This is a terrible hack to store unicode
        // data in a QCString in a way that
        // QCString::length() == QCString().size().
        // This allows us to treat the QCString like a QByteArray later on.
        listCell = buffer.utf8();
        int len = listCell.length();
        char tmp = listCell[ len - 1 ];
        listCell.resize( len );
        *( listCell.data() + len - 1 ) = tmp;

        //save size of columns
        for( int y = 1; y <=nbRow ; ++y )
        {
           RowLayout *rw=table->rowLayout(y);
           if(!rw->isDefault())
                {
                rowSize tmpSize;
                tmpSize.rowNumber=y;
                tmpSize.rowHeight=rw->height();
                listRow.append(tmpSize);
                }
        }

    }
    //copy just an area
    else
    {
        //save all cells in area
        QDomDocument doc = table->saveCellRect( m_selection );
        // Save to buffer
        QString buffer;
        QTextStream str( &buffer, IO_WriteOnly );
        str << doc;

        // This is a terrible hack to store unicode
        // data in a QCString in a way that
        // QCString::length() == QCString().size().
        // This allows us to treat the QCString like a QByteArray later on.
        listCell = buffer.utf8();
        int len = listCell.length();
        char tmp = listCell[ len - 1 ];
        listCell.resize( len );
        *( listCell.data() + len - 1 ) = tmp;
    }
}

void KSpreadUndoCellPaste::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    createListCell( m_dataRedo, m_lstRedoColumn,m_lstRedoRow,table );

    doc()->undoBuffer()->lock();
    if(nbCol!=0)
    {
        QRect rect;
        rect.setCoords(xshift,1,xshift+nbCol,0x7FFF);
        table->deleteCells( rect );
        table->paste( m_data, QPoint(xshift,1) );
        QValueList<columnSize>::Iterator it2;
        for ( it2 = m_lstColumn.begin(); it2 != m_lstColumn.end(); ++it2 )
        {
           ColumnLayout *cl=table->nonDefaultColumnLayout((*it2).columnNumber);
           cl->setWidth((*it2).columnWidth);
        }
    }
    else if(nbRow!=0)
    {
        QRect rect;
        rect.setCoords(1,yshift,0x7FFF,yshift+nbRow);
        table->deleteCells( rect );
        table->paste( m_data, QPoint(1,yshift) );
        QValueList<rowSize>::Iterator it2;
        for ( it2 = m_lstRow.begin(); it2 != m_lstRow.end(); ++it2 )
        {
           RowLayout *rw=table->nonDefaultRowLayout((*it2).rowNumber);
           rw->setHeight((*it2).rowHeight);
        }
    }
    else
    {
    table->deleteCells( m_selection );
    table->paste( m_data,m_selection.topLeft());
    }
    if(table->getAutoCalc()) table->recalc(true);
    doc()->undoBuffer()->unlock();
}

void KSpreadUndoCellPaste::redo()
{
    doc()->undoBuffer()->lock();

    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();
    if(nbCol!=0)
    {
        QRect rect;
        rect.setCoords(xshift,1,xshift+nbCol,0X7FFF);
        table->deleteCells( rect );
        table->paste( m_dataRedo, QPoint(xshift,1) );
        QValueList<columnSize>::Iterator it2;
        for ( it2 = m_lstRedoColumn.begin(); it2 != m_lstRedoColumn.end(); ++it2 )
        {
           ColumnLayout *cl=table->nonDefaultColumnLayout((*it2).columnNumber);
           cl->setWidth((*it2).columnWidth);
        }
    }
    else if(nbRow!=0)
    {
        QRect rect;
        rect.setCoords(1,yshift,0x7FFF,yshift+nbRow);
        table->deleteCells( rect );
        table->paste( m_dataRedo, QPoint(1,yshift) );
        QValueList<rowSize>::Iterator it2;
        for ( it2 = m_lstRedoRow.begin(); it2 != m_lstRedoRow.end(); ++it2 )
        {
           RowLayout *rw=table->nonDefaultRowLayout((*it2).rowNumber);
           rw->setHeight((*it2).rowHeight);
        }
    }
    else
    {
    table->deleteCells( m_selection );
    table->paste( m_dataRedo,m_selection.topLeft());
    }
    if(table->getAutoCalc()) table->recalc(true);

    doc()->undoBuffer()->unlock();
}
