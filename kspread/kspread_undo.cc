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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#include "kspread_global.h"
#include "kspread_undo.h"
#include "kspread_cell.h"
#include "kspread_doc.h"
#include "kspread_locale.h"
#include "kspread_map.h"
#include "kspread_util.h"
#include "kspread_sheetprint.h"
#include "kspread_style.h"
#include "kspread_style_manager.h"

/****************************************************************************
 *
 * KSpreadUndo
 *
 ***************************************************************************/

KSpreadUndo::KSpreadUndo( KSpreadDoc *_doc )
{
    m_pDoc = _doc;

    m_stckUndo.setAutoDelete( false );
    m_stckRedo.setAutoDelete( false );
}

KSpreadUndo::~KSpreadUndo()
{
    clear();
}

void KSpreadUndo::appendUndo( KSpreadUndoAction *_action )
{
    if ( isLocked() )
	return;

    m_stckRedo.setAutoDelete( true );
    m_stckRedo.clear();
    m_stckRedo.setAutoDelete( false );

    m_stckUndo.push( _action );

    if ( m_pDoc )
    {
	m_pDoc->enableUndo( hasUndoActions() );
	m_pDoc->enableRedo( hasRedoActions() );
        m_pDoc->setModified( true );
    }
}

void KSpreadUndo::clear()
{
    if ( isLocked() )
	return;

    m_stckUndo.setAutoDelete( true );
    m_stckRedo.setAutoDelete( true );

    m_stckUndo.clear();
    m_stckRedo.clear();

    m_stckUndo.setAutoDelete( false );
    m_stckRedo.setAutoDelete( false );
}

void KSpreadUndo::undo()
{
    if ( m_stckUndo.isEmpty() )
        return;

    //Don't show error dialogs on undo
    bool origErrorMessages = true;
    if ( m_pDoc )
    {
        origErrorMessages = m_pDoc->getShowMessageError();
        m_pDoc->setShowMessageError( false );
    }

    KSpreadUndoAction *a = m_stckUndo.pop();
    a->undo();
    m_stckRedo.push( a );

    if ( m_pDoc )
    {
        m_pDoc->setShowMessageError( origErrorMessages  );
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

void KSpreadUndo::lock()
{
  m_pDoc->undoLock();
}

void KSpreadUndo::unlock()
{
  m_pDoc->undoUnlock();
}

bool KSpreadUndo::isLocked() const
{
  return m_pDoc->undoLocked();
}

QString KSpreadUndo::getRedoName()
{
    if ( m_stckRedo.isEmpty() )
	return QString("");
    return  m_stckRedo.current()->getName();

}

QString KSpreadUndo::getUndoName()
{
    if ( m_stckUndo.isEmpty() )
	return QString("");
    return  m_stckUndo.current()->getName();
}

/****************************************************************************
 *
 * KSpreadMacroUndoAction
 *
 ***************************************************************************/
KSpreadMacroUndoAction::KSpreadMacroUndoAction( KSpreadDoc *_doc, const QString& _name ):
 KSpreadUndoAction( _doc )
{
    name=_name;
}

KSpreadMacroUndoAction::~KSpreadMacroUndoAction()
{
    m_commands.setAutoDelete( true );
}

void KSpreadMacroUndoAction::addCommand(KSpreadUndoAction *command)
{
    m_commands.append(command);
}

void KSpreadMacroUndoAction::undo()
{
    QPtrListIterator<KSpreadUndoAction> it(m_commands);
    for ( ; it.current() ; ++it )
        it.current()->undo();
}

void KSpreadMacroUndoAction::redo()
{
    QPtrListIterator<KSpreadUndoAction> it(m_commands);
    for ( ; it.current() ; ++it )
        it.current()->redo();
}

/****************************************************************************
 *
 * KSpreadUndoInsertRemoveAction
 *
 ***************************************************************************/

KSpreadUndoInsertRemoveAction::KSpreadUndoInsertRemoveAction( KSpreadDoc * _doc ) :
    KSpreadUndoAction( _doc )
{
}

KSpreadUndoInsertRemoveAction::~KSpreadUndoInsertRemoveAction()
{

}

void KSpreadUndoInsertRemoveAction::saveFormulaReference( KSpreadSheet *_sheet,
                                             int col, int row, QString & formula )
{
    if ( _sheet == 0 )
        return;
    QString sheetName = _sheet->sheetName();

    m_lstFormulaCells.append( FormulaOfCell( sheetName, col, row, formula ) );
}

void KSpreadUndoInsertRemoveAction::undoFormulaReference()
{
    QValueList<FormulaOfCell>::iterator it;
    for ( it = m_lstFormulaCells.begin(); it != m_lstFormulaCells.end(); ++it )
    {
        KSpreadSheet* sheet = doc()->map()->findSheet( (*it).sheetName() );
        if ( sheet )
        {
            KSpreadCell * cell = sheet->cellAt( (*it).col(), (*it).row() );
            if ( cell && !cell->isDefault() )
            {
                cell->setCellText( (*it).formula() );
            }
        }
    }
}

/****************************************************************************
 *
 * KSpreadUndoRemoveColumn
 *
 ***************************************************************************/

KSpreadUndoRemoveColumn::KSpreadUndoRemoveColumn( KSpreadDoc *_doc, KSpreadSheet *_sheet, int _column,int _nbCol ) :
    KSpreadUndoInsertRemoveAction( _doc )
{
    name=i18n("Remove Columns");
    m_sheetName = _sheet->sheetName();
    m_iColumn= _column;
    m_iNbCol = _nbCol;
    m_printRange = _sheet->print()->printRange();
    m_printRepeatColumns = _sheet->print()->printRepeatColumns();
    QRect selection;
    selection.setCoords( _column, 1, _column+m_iNbCol, KS_rowMax );
    QDomDocument doc = _sheet->saveCellRect( selection );

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
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();

    sheet->insertColumn( m_iColumn,m_iNbCol);

    QPoint pastePoint( m_iColumn, 1 );
    sheet->paste( m_data, QRect( pastePoint, pastePoint ) );
    if(sheet->getAutoCalc()) sheet->recalc();

    sheet->print()->setPrintRange( m_printRange );
    sheet->print()->setPrintRepeatColumns( m_printRepeatColumns );

    doc()->undoUnlock();

    undoFormulaReference();
}

void KSpreadUndoRemoveColumn::redo()
{
    doc()->undoLock();

    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    sheet->removeColumn( m_iColumn,m_iNbCol );

    doc()->undoUnlock();
}

/****************************************************************************
 *
 * KSpreadUndoInsertColumn
 *
 ***************************************************************************/

KSpreadUndoInsertColumn::KSpreadUndoInsertColumn( KSpreadDoc *_doc, KSpreadSheet *_sheet, int _column, int _nbCol ) :
    KSpreadUndoInsertRemoveAction( _doc )
{
    name=i18n("Insert Columns");
    m_sheetName = _sheet->sheetName();
    m_iColumn= _column;
    m_iNbCol=_nbCol;
}

KSpreadUndoInsertColumn::~KSpreadUndoInsertColumn()
{
}

void KSpreadUndoInsertColumn::undo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->removeColumn( m_iColumn,m_iNbCol );
    doc()->undoUnlock();

    undoFormulaReference();
}

void KSpreadUndoInsertColumn::redo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->insertColumn( m_iColumn,m_iNbCol);
    doc()->undoUnlock();
}

/****************************************************************************
 *
 * KSpreadUndoRemoveRow
 *
 ***************************************************************************/

KSpreadUndoRemoveRow::KSpreadUndoRemoveRow( KSpreadDoc *_doc, KSpreadSheet *_sheet, int _row,int _nbRow) :
    KSpreadUndoInsertRemoveAction( _doc )
{
    name=i18n("Remove Rows");

    m_sheetName = _sheet->sheetName();
    m_iRow = _row;
    m_iNbRow=  _nbRow;
    m_printRange=_sheet->print()->printRange();
    m_printRepeatRows = _sheet->print()->printRepeatRows();

    QRect selection;
    selection.setCoords( 1, _row, KS_colMax, _row+m_iNbRow );
    QDomDocument doc = _sheet->saveCellRect( selection );

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
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();

    sheet->insertRow( m_iRow,m_iNbRow );

    QPoint pastePoint( 1, m_iRow );
    sheet->paste( m_data, QRect(pastePoint, pastePoint) );

    sheet->print()->setPrintRange( m_printRange );
    sheet->print()->setPrintRepeatRows( m_printRepeatRows );

    if(sheet->getAutoCalc()) sheet->recalc();

    doc()->undoUnlock();

    undoFormulaReference();
}

void KSpreadUndoRemoveRow::redo()
{
    doc()->undoLock();

    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    sheet->removeRow( m_iRow,m_iNbRow );

    doc()->undoUnlock();
}

/****************************************************************************
 *
 * KSpreadUndoInsertRow
 *
 ***************************************************************************/

KSpreadUndoInsertRow::KSpreadUndoInsertRow( KSpreadDoc *_doc, KSpreadSheet *_sheet, int _row,int _nbRow ) :
    KSpreadUndoInsertRemoveAction( _doc )
{
    name=i18n("Insert Rows");
    m_sheetName = _sheet->sheetName();
    m_iRow = _row;
    m_iNbRow=_nbRow;
}

KSpreadUndoInsertRow::~KSpreadUndoInsertRow()
{
}

void KSpreadUndoInsertRow::undo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->removeRow( m_iRow,m_iNbRow );
    doc()->undoUnlock();

    undoFormulaReference();
}

void KSpreadUndoInsertRow::redo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->insertRow( m_iRow,m_iNbRow );
    doc()->undoUnlock();
}


/****************************************************************************
 *
 * KSpreadUndoHideRow
 *
 ***************************************************************************/

KSpreadUndoHideRow::KSpreadUndoHideRow( KSpreadDoc *_doc, KSpreadSheet *_sheet, int _row, int _nbRow , QValueList<int>_listRow) :
    KSpreadUndoAction( _doc )
{
    name=i18n("Hide Rows");
    m_sheetName = _sheet->sheetName();
    m_iRow= _row;
    m_iNbRow=_nbRow;
    if(m_iNbRow!=-1)
      createList( listRow ,_sheet );
    else
      listRow=QValueList<int>(_listRow);
}

KSpreadUndoHideRow::~KSpreadUndoHideRow()
{
}

void KSpreadUndoHideRow::createList( QValueList<int>&list,KSpreadSheet *tab )
{
RowFormat *rl;
for(int i=m_iRow;i<=(m_iRow+m_iNbRow);i++)
        {
        rl= tab->nonDefaultRowFormat( i );
        if(!rl->isHide())
                list.append(rl->row());
        }
}

void KSpreadUndoHideRow::undo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->showRow( 0,-1,listRow );
    doc()->undoUnlock();
}

void KSpreadUndoHideRow::redo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->hideRow(0,-1, listRow );
    doc()->undoUnlock();
}

/****************************************************************************
 *
 * KSpreadUndoHideColumn
 *
 ***************************************************************************/

KSpreadUndoHideColumn::KSpreadUndoHideColumn( KSpreadDoc *_doc, KSpreadSheet *_sheet, int _column, int _nbCol, QValueList<int>_listCol ) :
    KSpreadUndoAction( _doc )
{
    name=i18n("Hide Columns");

    m_sheetName = _sheet->sheetName();
    m_iColumn= _column;
    m_iNbCol=_nbCol;
    if(m_iNbCol!=-1)
      createList( listCol ,_sheet );
    else
      listCol=QValueList<int>(_listCol);
}

KSpreadUndoHideColumn::~KSpreadUndoHideColumn()
{
}

void KSpreadUndoHideColumn::createList( QValueList<int>&list,KSpreadSheet *tab )
{
ColumnFormat *cl;
for(int i=m_iColumn;i<=(m_iColumn+m_iNbCol);i++)
  {
    cl= tab->nonDefaultColumnFormat( i );
    if(!cl->isHide())
      list.append(cl->column());
  }
}

void KSpreadUndoHideColumn::undo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->showColumn(0,-1,listCol);
    doc()->undoUnlock();
}

void KSpreadUndoHideColumn::redo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->hideColumn(0,-1,listCol);
    doc()->undoUnlock();
}

/****************************************************************************
 *
 * KSpreadUndoShowRow
 *
 ***************************************************************************/

KSpreadUndoShowRow::KSpreadUndoShowRow( KSpreadDoc *_doc, KSpreadSheet *_sheet, int _row, int _nbRow, QValueList<int>_listRow ) :
    KSpreadUndoAction( _doc )
{
    name=i18n("Show Rows");

    m_sheetName = _sheet->sheetName();
    m_iRow= _row;
    m_iNbRow=_nbRow;
    if(m_iNbRow!=-1)
      createList( listRow ,_sheet );
    else
      listRow=QValueList<int>(_listRow);
}

KSpreadUndoShowRow::~KSpreadUndoShowRow()
{
}

void KSpreadUndoShowRow::createList( QValueList<int>&list,KSpreadSheet *tab )
{
RowFormat *rl;
for(int i=m_iRow;i<=(m_iRow+m_iNbRow);i++)
        {
        rl= tab->nonDefaultRowFormat( i );
        if(rl->isHide())
                list.append(rl->row());
        }
}

void KSpreadUndoShowRow::undo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->hideRow(0,-1,listRow);
    doc()->undoUnlock();
}

void KSpreadUndoShowRow::redo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->showRow(0,-1,listRow);
    doc()->undoUnlock();
}

/****************************************************************************
 *
 * KSpreadUndoShowColumn
 *
 ***************************************************************************/

KSpreadUndoShowColumn::KSpreadUndoShowColumn( KSpreadDoc *_doc, KSpreadSheet *_sheet, int _column, int _nbCol,QValueList<int>_listCol ) :
    KSpreadUndoAction( _doc )
{
    name=i18n("Show Columns");

    m_sheetName = _sheet->sheetName();
    m_iColumn= _column;
    m_iNbCol=_nbCol;
    if(m_iNbCol!=-1)
      createList( listCol ,_sheet );
    else
      listCol=QValueList<int>(_listCol);
}

KSpreadUndoShowColumn::~KSpreadUndoShowColumn()
{
}

void KSpreadUndoShowColumn::createList( QValueList<int>&list,KSpreadSheet *tab )
{
ColumnFormat *cl;
for(int i=m_iColumn;i<=(m_iColumn+m_iNbCol);i++)
  {
    cl= tab->nonDefaultColumnFormat( i );
    if(cl->isHide())
      list.append(cl->column());
  }

}

void KSpreadUndoShowColumn::undo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->hideColumn( 0,-1,listCol );
    doc()->undoUnlock();
}

void KSpreadUndoShowColumn::redo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->showColumn(0,-1,listCol);
    doc()->undoUnlock();
}


/****************************************************************************
 *
 * KSpreadUndoPaperLayout
 *
 ***************************************************************************/

KSpreadUndoPaperLayout::KSpreadUndoPaperLayout( KSpreadDoc *_doc, KSpreadSheet *_sheet )
    : KSpreadUndoAction( _doc )
{
    name=i18n("Set Page Layout");
    m_sheetName = _sheet->sheetName();

    m_pl = _sheet->print()->paperLayout();
    m_hf = _sheet->print()->headFootLine();
    m_unit = doc()->unit();
    m_printGrid = _sheet->print()->printGrid();
    m_printCommentIndicator = _sheet->print()->printCommentIndicator();
    m_printFormulaIndicator = _sheet->print()->printFormulaIndicator();
    m_printRange = _sheet->print()->printRange();
    m_printRepeatColumns = _sheet->print()->printRepeatColumns();
    m_printRepeatRows = _sheet->print()->printRepeatRows();
    m_dZoom = _sheet->print()->zoom();
    m_iPageLimitX = _sheet->print()->pageLimitX();
    m_iPageLimitY = _sheet->print()->pageLimitY();
}

KSpreadUndoPaperLayout::~KSpreadUndoPaperLayout()
{
}

void KSpreadUndoPaperLayout::undo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
        return;
    KSpreadSheetPrint* print = sheet->print();

    doc()->undoLock();

    m_plRedo = print->paperLayout();
    print->setPaperLayout( m_pl.ptLeft,  m_pl.ptTop,
                           m_pl.ptRight, m_pl.ptBottom,
                           m_pl.format,  m_pl.orientation );

    m_hfRedo = print->headFootLine();
    print->setHeadFootLine( m_hf.headLeft, m_hf.headMid, m_hf.headRight,
                            m_hf.footLeft, m_hf.footMid, m_hf.footRight );

    m_unitRedo = doc()->unit();
    doc()->setUnit( m_unit );

    m_printGridRedo = print->printGrid();
    print->setPrintGrid( m_printGrid );

    m_printCommentIndicatorRedo = print->printCommentIndicator();
    print->setPrintCommentIndicator( m_printCommentIndicator );

    m_printFormulaIndicatorRedo = print->printFormulaIndicator();
    print->setPrintFormulaIndicator( m_printFormulaIndicator );

    m_printRangeRedo = print->printRange();
    print->setPrintRange( m_printRange );

    m_printRepeatColumnsRedo = print->printRepeatColumns();
    print->setPrintRepeatColumns( m_printRepeatColumns );

    m_printRepeatRowsRedo = print->printRepeatRows();
    print->setPrintRepeatRows( m_printRepeatRows );

    m_dZoomRedo = print->zoom();
    print->setZoom( m_dZoom );

    m_iPageLimitXRedo = print->pageLimitX();
    print->setPageLimitX( m_iPageLimitX );

    m_iPageLimitYRedo = print->pageLimitY();
    print->setPageLimitY( m_iPageLimitY );

    doc()->undoUnlock();
}

void KSpreadUndoPaperLayout::redo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
        return;
    KSpreadSheetPrint* print = sheet->print();

    doc()->undoLock();
    print->setPaperLayout( m_plRedo.ptLeft,  m_plRedo.ptTop,
                           m_plRedo.ptRight, m_plRedo.ptBottom,
                           m_plRedo.format, m_plRedo.orientation );

    print->setHeadFootLine( m_hfRedo.headLeft, m_hfRedo.headMid, m_hfRedo.headRight,
                            m_hfRedo.footLeft, m_hfRedo.footMid, m_hfRedo.footRight );

    doc()->setUnit( m_unitRedo );

    print->setPrintGrid( m_printGridRedo );
    print->setPrintCommentIndicator( m_printCommentIndicatorRedo );
    print->setPrintFormulaIndicator( m_printFormulaIndicatorRedo );

    print->setPrintRange( m_printRangeRedo );
    print->setPrintRepeatColumns( m_printRepeatColumnsRedo );
    print->setPrintRepeatRows( m_printRepeatRowsRedo );

    print->setZoom( m_dZoomRedo );

    print->setPageLimitX( m_iPageLimitX );
    print->setPageLimitY( m_iPageLimitY );

    doc()->undoUnlock();
}




/****************************************************************************
 *
 * KSpreadUndoSetText
 *
 ***************************************************************************/

KSpreadUndoSetText::KSpreadUndoSetText( KSpreadDoc *_doc, KSpreadSheet *_sheet, const QString& _text, int _column, int _row,FormatType _formatType ) :
    KSpreadUndoAction( _doc )
{
    name=i18n("Change Text");

    m_strText = _text;
    m_iColumn= _column;
    m_iRow = _row;
    m_sheetName = _sheet->sheetName();
    m_eFormatType=_formatType;
}

KSpreadUndoSetText::~KSpreadUndoSetText()
{
}

void KSpreadUndoSetText::undo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    doc()->emitBeginOperation();
    KSpreadCell *cell = sheet->nonDefaultCell( m_iColumn, m_iRow );
    m_strRedoText = cell->text();
    m_eFormatTypeRedo=cell->getFormatType( m_iColumn, m_iRow );
    cell->setFormatType(m_eFormatType);

    if ( m_strText.isNull() )
	cell->setCellText( "" );
    else
	cell->setCellText( m_strText );
    sheet->updateView( QRect( m_iColumn, m_iRow, 1, 1 ) );
    doc()->undoUnlock();
}

void KSpreadUndoSetText::redo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    doc()->emitBeginOperation();
    KSpreadCell *cell = sheet->nonDefaultCell( m_iColumn, m_iRow );
    m_strText = cell->text();
    m_eFormatType=cell->getFormatType( m_iColumn, m_iRow );
    if ( m_strRedoText.isNull() )
	cell->setCellText( "" );
    else
	cell->setCellText( m_strRedoText );
    cell->setFormatType(m_eFormatTypeRedo);
    sheet->updateView( QRect( m_iColumn, m_iRow, 1, 1 ) );
    doc()->undoUnlock();
}

/****************************************************************************
 *
 * KSpreadUndoCellFormat
 *
 ***************************************************************************/

KSpreadUndoCellFormat::KSpreadUndoCellFormat( KSpreadDoc * _doc,
                                              KSpreadSheet * _sheet,
                                              const QRect & _selection,
                                              const QString & _name ) :
  KSpreadUndoAction( _doc )
{
  if ( _name.isEmpty())
    name = i18n("Change Format");
  else
    name = _name;

  m_rctRect   = _selection;
  m_sheetName = _sheet->sheetName();
  copyFormat( m_lstFormats, m_lstColFormats, m_lstRowFormats, _sheet );
}

void KSpreadUndoCellFormat::copyFormat(QValueList<layoutCell> & list,
                                       QValueList<layoutColumn> & listCol,
                                       QValueList<layoutRow> & listRow,
                                       KSpreadSheet * sheet )
{
    QValueList<layoutCell>::Iterator it2;
  for ( it2 = list.begin(); it2 != list.end(); ++it2 )
  {
      delete (*it2).l;
  }
  list.clear();

  KSpreadCell * cell;
  int bottom = m_rctRect.bottom();
  int right  = m_rctRect.right();

  if ( util_isColumnSelected( m_rctRect ) )
  {
    /* Don't need to go through the loop twice...
      for (int i = m_rctRect.left(); i <= right; ++i)
      {
      layoutColumn tmplayout;
      tmplayout.col = i;
      tmplayout.l = new ColumnFormat( sheet, i );
      tmplayout.l->copy( *(sheet->columnFormat( i )) );
      listCol.append(tmplayout);
      }
    */
    for ( int c = m_rctRect.left(); c <= right; ++c )
    {
      layoutColumn tmplayout;
      tmplayout.col = c;
      tmplayout.l = new ColumnFormat( sheet, c );
      tmplayout.l->copy( *(sheet->columnFormat( c )) );
      listCol.append(tmplayout);

      cell = sheet->getFirstCellColumn( c );
      while ( cell )
      {
        if ( cell->isObscuringForced() )
        {
          cell = sheet->getNextCellDown( c, cell->row() );
          continue;
        }

        layoutCell tmplayout;
        tmplayout.col = c;
        tmplayout.row = cell->row();
        tmplayout.l = new KSpreadFormat( sheet, 0 );
        tmplayout.l->copy( *(sheet->cellAt( tmplayout.col, tmplayout.row )) );
        list.append(tmplayout);

        cell = sheet->getNextCellDown( c, cell->row() );
      }
    }
    /*
      KSpreadCell * c = sheet->firstCell();
      for( ; c; c = c->nextCell() )
      {
      int col = c->column();
      if ( m_rctRect.left() <= col && right >= col
          && !c->isObscuringForced())
      {
        layoutCell tmplayout;
        tmplayout.col = c->column();
        tmplayout.row = c->row();
        tmplayout.l = new KSpreadFormat( sheet, 0 );
        tmplayout.l->copy( *(sheet->cellAt( tmplayout.col, tmplayout.row )) );
        list.append(tmplayout);
      }
      }
    */
  }
  else if (util_isRowSelected( m_rctRect ) )
  {
    for ( int row = m_rctRect.top(); row <= bottom; ++row )
    {
      layoutRow tmplayout;
      tmplayout.row = row;
      tmplayout.l = new RowFormat( sheet, row );
      tmplayout.l->copy( *(sheet->rowFormat( row )) );
      listRow.append(tmplayout);

      cell = sheet->getFirstCellRow( row );
      while ( cell )
      {
        if ( cell->isObscuringForced() )
        {
          cell = sheet->getNextCellRight( cell->column(), row );
          continue;
        }
        layoutCell tmplayout;
        tmplayout.col = cell->column();
        tmplayout.row = row;
        tmplayout.l = new KSpreadFormat( sheet, 0 );
        tmplayout.l->copy( *(sheet->cellAt( cell->column(), row )) );
        list.append(tmplayout);

        cell = sheet->getNextCellRight( cell->column(), row );
      }
    }
    /*
      KSpreadCell * c = sheet->firstCell();
      for( ; c; c = c->nextCell() )
      {
      int row = c->row();
      if ( m_rctRect.top() <= row && bottom >= row
           && !c->isObscuringForced())
      {
        layoutCell tmplayout;
        tmplayout.col = c->column();
        tmplayout.row = c->row();
        tmplayout.l = new KSpreadFormat( sheet, 0 );
        tmplayout.l->copy( *(sheet->cellAt( tmplayout.col, tmplayout.row )) );
        list.append(tmplayout);
      }
      }
    */
  }
  else
  {
    for ( int y = m_rctRect.top(); y <= bottom; ++y )
      for ( int x = m_rctRect.left(); x <= right; ++x )
      {
        KSpreadCell * cell = sheet->nonDefaultCell( x, y );
        if ( !cell->isObscuringForced() )
        {
          layoutCell tmplayout;
          tmplayout.col = x;
          tmplayout.row = y;
          tmplayout.l = new KSpreadFormat( sheet, 0 );
          tmplayout.l->copy( *(sheet->cellAt( x, y )) );
          list.append(tmplayout);
        }
      }
  }
}

KSpreadUndoCellFormat::~KSpreadUndoCellFormat()
{
    QValueList<layoutCell>::Iterator it2;
    for ( it2 = m_lstFormats.begin(); it2 != m_lstFormats.end(); ++it2 )
    {
        delete (*it2).l;
    }
    m_lstFormats.clear();

    for ( it2 = m_lstRedoFormats.begin(); it2 != m_lstRedoFormats.end(); ++it2 )
    {
        delete (*it2).l;
    }
    m_lstRedoFormats.clear();

    QValueList<layoutColumn>::Iterator it3;
    for ( it3 = m_lstColFormats.begin(); it3 != m_lstColFormats.end(); ++it3 )
    {
        delete (*it3).l;
    }
    m_lstColFormats.clear();

    for ( it3 = m_lstRedoColFormats.begin(); it3 != m_lstRedoColFormats.end(); ++it3 )
    {
        delete (*it3).l;
    }
    m_lstRedoColFormats.clear();

    QValueList<layoutRow>::Iterator it4;
    for ( it4 = m_lstRowFormats.begin(); it4 != m_lstRowFormats.end(); ++it4 )
    {
        delete (*it4).l;
    }
    m_lstRowFormats.clear();

    for ( it4 = m_lstRedoRowFormats.begin(); it4 != m_lstRedoRowFormats.end(); ++it4 )
    {
        delete (*it4).l;
    }
    m_lstRedoRowFormats.clear();


}

void KSpreadUndoCellFormat::undo()
{
  KSpreadSheet * sheet = doc()->map()->findSheet( m_sheetName );
  if ( !sheet )
    return;

  doc()->undoLock();
  doc()->emitBeginOperation();
  copyFormat( m_lstRedoFormats, m_lstRedoColFormats, m_lstRedoRowFormats, sheet );
  if( util_isColumnSelected( m_rctRect ) )
  {
    QValueList<layoutColumn>::Iterator it2;
    for ( it2 = m_lstColFormats.begin(); it2 != m_lstColFormats.end(); ++it2 )
    {
      ColumnFormat * col = sheet->nonDefaultColumnFormat( (*it2).col );
      col->copy( *(*it2).l );
    }
  }
  else if( util_isRowSelected( m_rctRect ) )
  {
    QValueList<layoutRow>::Iterator it2;
    for ( it2 = m_lstRowFormats.begin(); it2 != m_lstRowFormats.end(); ++it2 )
    {
      RowFormat * row = sheet->nonDefaultRowFormat( (*it2).row );
      row->copy( *(*it2).l );
    }
  }

  QValueList<layoutCell>::Iterator it2;
  for ( it2 = m_lstFormats.begin(); it2 != m_lstFormats.end(); ++it2 )
  {
    KSpreadCell *cell = sheet->nonDefaultCell( (*it2).col,(*it2).row );
    cell->copy( *(*it2).l );
    cell->setLayoutDirtyFlag();
    cell->setDisplayDirtyFlag();
    sheet->updateCell( cell, (*it2).col, (*it2).row );
  }

  sheet->setRegionPaintDirty( m_rctRect );
  sheet->updateView( m_rctRect );

  doc()->undoUnlock();
}

void KSpreadUndoCellFormat::redo()
{
  KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
  if ( !sheet )
    return;

  doc()->undoLock();
  doc()->emitBeginOperation();

  if ( util_isColumnSelected( m_rctRect ) )
  {
    QValueList<layoutColumn>::Iterator it2;
    for ( it2 = m_lstRedoColFormats.begin(); it2 != m_lstRedoColFormats.end(); ++it2 )
    {
      ColumnFormat * col = sheet->nonDefaultColumnFormat( (*it2).col );
      col->copy( *(*it2).l );
    }
  }
  else if( util_isRowSelected( m_rctRect ) )
  {
    QValueList<layoutRow>::Iterator it2;
    for ( it2 = m_lstRedoRowFormats.begin(); it2 != m_lstRedoRowFormats.end(); ++it2 )
    {
      RowFormat * row = sheet->nonDefaultRowFormat( (*it2).row );
      row->copy( *(*it2).l );
    }
  }

  QValueList<layoutCell>::Iterator it2;
  for ( it2 = m_lstRedoFormats.begin(); it2 != m_lstRedoFormats.end(); ++it2 )
  {
    KSpreadCell * cell = sheet->nonDefaultCell( (*it2).col,(*it2).row );
    cell->copy( *(*it2).l );
    cell->setLayoutDirtyFlag();
    cell->setDisplayDirtyFlag();
    sheet->updateCell( cell, (*it2).col, (*it2).row );
  }

  sheet->setRegionPaintDirty( m_rctRect );
  sheet->updateView( m_rctRect );
  doc()->undoUnlock();
}

/****************************************************************************
 *
 * KSpreadUndoChangeAngle
 *
 ***************************************************************************/

KSpreadUndoChangeAngle::KSpreadUndoChangeAngle( KSpreadDoc * _doc,
                                              KSpreadSheet * _sheet,
                                              const QRect & _selection ) :
  KSpreadUndoAction( _doc )
{
  name = i18n("Change Angle");
  m_layoutUndo = new KSpreadUndoCellFormat( _doc, _sheet, _selection, QString::null );
  m_resizeUndo = new KSpreadUndoResizeColRow( _doc, _sheet, _selection );
}

KSpreadUndoChangeAngle::~KSpreadUndoChangeAngle()
{
  delete m_resizeUndo;
  delete m_layoutUndo;
}

void KSpreadUndoChangeAngle::undo()
{
  m_layoutUndo->undo();
  m_resizeUndo->undo();
}

void KSpreadUndoChangeAngle::redo()
{
  m_layoutUndo->redo();
  m_resizeUndo->redo();
}

/****************************************************************************
 *
 * KSpreadUndoSort
 *
 ***************************************************************************/

KSpreadUndoSort::KSpreadUndoSort( KSpreadDoc * _doc, KSpreadSheet * _sheet, const QRect & _selection ) :
    KSpreadUndoAction( _doc )
{
  name        = i18n("Sort");

  m_rctRect   = _selection;
  m_sheetName = _sheet->sheetName();
  copyAll( m_lstFormats, m_lstColFormats, m_lstRowFormats, _sheet );
}

void KSpreadUndoSort::copyAll(QValueList<layoutTextCell> & list, QValueList<layoutColumn> & listCol,
                              QValueList<layoutRow> & listRow, KSpreadSheet * sheet )
{
  QValueList<layoutTextCell>::Iterator it2;
  for ( it2 = list.begin(); it2 != list.end(); ++it2 )
  {
      delete (*it2).l;
  }
  list.clear();

  if ( util_isColumnSelected( m_rctRect ) )
  {
    KSpreadCell * c;
    for (int col = m_rctRect.left(); col <= m_rctRect.right(); ++col)
    {
      layoutColumn tmplayout;
      tmplayout.col = col;
      tmplayout.l = new ColumnFormat( sheet, col );
      tmplayout.l->copy( *(sheet->columnFormat( col )) );
      listCol.append(tmplayout);

      c = sheet->getFirstCellColumn( col );
      while ( c )
      {
        if ( !c->isObscuringForced() )
        {
          layoutTextCell tmplayout;
          tmplayout.col = col;
          tmplayout.row = c->row();
          tmplayout.l = new KSpreadFormat( sheet, 0 );
          tmplayout.l->copy( *(sheet->cellAt( tmplayout.col, tmplayout.row )) );
          tmplayout.text = c->text();
          list.append(tmplayout);
        }

        c = sheet->getNextCellDown( col, c->row() );
      }
    }
  }
  else if ( util_isRowSelected( m_rctRect ) )
  {
    KSpreadCell * c;
    for ( int row = m_rctRect.top(); row <= m_rctRect.bottom(); ++row)
    {
      layoutRow tmplayout;
      tmplayout.row = row;
      tmplayout.l = new RowFormat( sheet, row );
      tmplayout.l->copy( *(sheet->rowFormat( row )) );
      listRow.append(tmplayout);

      c = sheet->getFirstCellRow( row );
      while ( c )
      {
        if ( !c->isObscuringForced() )
        {
          layoutTextCell tmplayout;
          tmplayout.col = c->column();
          tmplayout.row = row;
          tmplayout.l   = new KSpreadFormat( sheet, 0 );
          tmplayout.l->copy( *(sheet->cellAt( tmplayout.col, tmplayout.row )) );
          tmplayout.text = c->text();
          list.append(tmplayout);
        }
        c = sheet->getNextCellRight( c->column(), row );
      }
    }
  }
  else
  {
    int bottom = m_rctRect.bottom();
    int right  = m_rctRect.right();
    KSpreadCell * cell;
    for ( int y = m_rctRect.top(); y <= bottom; ++y )
      for ( int x = m_rctRect.left(); x <= right; ++x )
      {
        cell = sheet->nonDefaultCell( x, y );
        if (!cell->isObscuringForced())
        {
          layoutTextCell tmplayout;
          tmplayout.col = x;
          tmplayout.row = y;
          tmplayout.l   = new KSpreadFormat( sheet, 0 );
          tmplayout.l->copy( *(sheet->cellAt( x, y )) );
          tmplayout.text = cell->text();
          list.append(tmplayout);
        }
      }
  }
}

KSpreadUndoSort::~KSpreadUndoSort()
{
    QValueList<layoutTextCell>::Iterator it2;
    for ( it2 = m_lstFormats.begin(); it2 != m_lstFormats.end(); ++it2 )
    {
        delete (*it2).l;
    }
    m_lstFormats.clear();

    for ( it2 = m_lstRedoFormats.begin(); it2 != m_lstRedoFormats.end(); ++it2 )
    {
        delete (*it2).l;
    }
    m_lstRedoFormats.clear();

    QValueList<layoutColumn>::Iterator it3;
    for ( it3 = m_lstColFormats.begin(); it3 != m_lstColFormats.end(); ++it3 )
    {
        delete (*it3).l;
    }
    m_lstColFormats.clear();

    for ( it3 = m_lstRedoColFormats.begin(); it3 != m_lstRedoColFormats.end(); ++it3 )
    {
        delete (*it3).l;
    }
    m_lstRedoColFormats.clear();

    QValueList<layoutRow>::Iterator it4;
    for ( it4 = m_lstRowFormats.begin(); it4 != m_lstRowFormats.end(); ++it4 )
    {
        delete (*it4).l;
    }
    m_lstRowFormats.clear();

    for ( it4 = m_lstRedoRowFormats.begin(); it4 != m_lstRedoRowFormats.end(); ++it4 )
    {
        delete (*it4).l;
    }
    m_lstRedoRowFormats.clear();

}

void KSpreadUndoSort::undo()
{
  KSpreadSheet * sheet = doc()->map()->findSheet( m_sheetName );
  if ( !sheet )
    return;

  doc()->undoLock();
  doc()->emitBeginOperation();

  copyAll( m_lstRedoFormats, m_lstRedoColFormats,
           m_lstRedoRowFormats, sheet );

  if ( util_isColumnSelected( m_rctRect ) )
  {
    QValueList<layoutColumn>::Iterator it2;
    for ( it2 = m_lstColFormats.begin(); it2 != m_lstColFormats.end(); ++it2 )
    {
      ColumnFormat * col = sheet->nonDefaultColumnFormat( (*it2).col );
      col->copy( *(*it2).l );
    }
  }
  else if( util_isRowSelected( m_rctRect ) )
  {
    QValueList<layoutRow>::Iterator it2;
    for ( it2 = m_lstRowFormats.begin(); it2 != m_lstRowFormats.end(); ++it2 )
    {
      RowFormat *row= sheet->nonDefaultRowFormat( (*it2).row );
      row->copy( *(*it2).l );
    }
  }

  QValueList<layoutTextCell>::Iterator it2;
  for ( it2 = m_lstFormats.begin(); it2 != m_lstFormats.end(); ++it2 )
  {
    KSpreadCell *cell = sheet->nonDefaultCell( (*it2).col,(*it2).row );
    if ( (*it2).text.isEmpty() )
    {
      if(!cell->text().isEmpty())
        cell->setCellText( "" );
    }
    else
      cell->setCellText( (*it2).text );

    cell->copy( *(*it2).l );
    cell->setLayoutDirtyFlag();
    cell->setDisplayDirtyFlag();
    sheet->updateCell( cell, (*it2).col, (*it2).row );
  }

  sheet->setRegionPaintDirty(m_rctRect);
  sheet->updateView( m_rctRect );

  doc()->undoUnlock();
}

void KSpreadUndoSort::redo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    doc()->emitBeginOperation();

    if( util_isColumnSelected( m_rctRect ) )
    {
      QValueList<layoutColumn>::Iterator it2;
      for ( it2 = m_lstRedoColFormats.begin(); it2 != m_lstRedoColFormats.end(); ++it2 )
      {
        ColumnFormat *col= sheet->nonDefaultColumnFormat( (*it2).col );
        col->copy( *(*it2).l );
      }
    }
    else if( util_isRowSelected( m_rctRect ) )
    {
      QValueList<layoutRow>::Iterator it2;
      for ( it2 = m_lstRedoRowFormats.begin(); it2 != m_lstRedoRowFormats.end(); ++it2 )
      {
        RowFormat *row= sheet->nonDefaultRowFormat( (*it2).row );
        row->copy( *(*it2).l );
      }
    }

    QValueList<layoutTextCell>::Iterator it2;
    for ( it2 = m_lstRedoFormats.begin(); it2 != m_lstRedoFormats.end(); ++it2 )
    {
      KSpreadCell *cell = sheet->nonDefaultCell( (*it2).col,(*it2).row );

      if ( (*it2).text.isEmpty() )
      {
        if(!cell->text().isEmpty())
          cell->setCellText( "" );
      }
      else
        cell->setCellText( (*it2).text );

      cell->copy( *(*it2).l );
      cell->setLayoutDirtyFlag();
      cell->setDisplayDirtyFlag();
      sheet->updateCell( cell, (*it2).col, (*it2).row );
    }
    sheet->setRegionPaintDirty(m_rctRect);
    sheet->updateView( m_rctRect );
    doc()->undoUnlock();
}

/****************************************************************************
 *
 * KSpreadUndoDelete
 *
 ***************************************************************************/

KSpreadUndoDelete::KSpreadUndoDelete( KSpreadDoc *_doc, KSpreadSheet* sheet, const QRect & _selection)
    : KSpreadUndoAction( _doc )
{
    name=i18n("Delete");
    m_sheetName = sheet->sheetName();
    m_selection = _selection;
    createListCell( m_data, m_lstColumn,m_lstRow,sheet );

}

KSpreadUndoDelete::~KSpreadUndoDelete()
{
}

void KSpreadUndoDelete::createListCell( QCString &listCell,QValueList<columnSize> &listCol,QValueList<rowSize> &listRow, KSpreadSheet* sheet )
{
    listRow.clear();
    listCol.clear();
    //copy a column(s)
    if( util_isColumnSelected( m_selection ) )
    {
        for( int y = m_selection.left() ; y <= m_selection.right() ; ++y )
        {
           ColumnFormat * cl = sheet->columnFormat( y );
           if ( !cl->isDefault() )
           {
                columnSize tmpSize;
                tmpSize.columnNumber=y;
                tmpSize.columnWidth=cl->dblWidth();
                listCol.append(tmpSize);
           }
        }
    }
    //copy a row(s)
    else if( util_isRowSelected( m_selection ) )
    {
        //save size of row(s)
        for( int y =m_selection.top() ; y <=m_selection.bottom() ; ++y )
        {
           RowFormat *rw=sheet->rowFormat(y);
           if(!rw->isDefault())
                {
                rowSize tmpSize;
                tmpSize.rowNumber=y;
                tmpSize.rowHeight=rw->dblHeight();
                listRow.append(tmpSize);
                }
        }

    }

    //save all cells in area
    QDomDocument doc = sheet->saveCellRect( m_selection );
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
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;
    createListCell( m_dataRedo, m_lstRedoColumn,m_lstRedoRow,sheet );

    doc()->undoLock();
    doc()->emitBeginOperation();

    if( util_isColumnSelected( m_selection ) )
    {
        QValueList<columnSize>::Iterator it2;
        for ( it2 = m_lstColumn.begin(); it2 != m_lstColumn.end(); ++it2 )
        {
           ColumnFormat *cl=sheet->nonDefaultColumnFormat((*it2).columnNumber);
           cl->setDblWidth((*it2).columnWidth);
        }
    }
    else if( util_isRowSelected( m_selection ) )
    {
        QValueList<rowSize>::Iterator it2;
        for ( it2 = m_lstRow.begin(); it2 != m_lstRow.end(); ++it2 )
        {
           RowFormat *rw=sheet->nonDefaultRowFormat((*it2).rowNumber);
           rw->setDblHeight((*it2).rowHeight);
        }
    }

    sheet->deleteCells( m_selection );
    sheet->paste( m_data, m_selection );
    sheet->updateView( );

    if(sheet->getAutoCalc()) sheet->recalc();

    doc()->undoUnlock();
}

void KSpreadUndoDelete::redo()
{

    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    doc()->emitBeginOperation();

    if( util_isColumnSelected( m_selection ) )
    {
        QValueList<columnSize>::Iterator it2;
        for ( it2 = m_lstRedoColumn.begin(); it2 != m_lstRedoColumn.end(); ++it2 )
        {
           ColumnFormat *cl=sheet->nonDefaultColumnFormat((*it2).columnNumber);
           cl->setDblWidth((*it2).columnWidth);
        }
    }
    else if( util_isRowSelected( m_selection ) )
    {
        QValueList<rowSize>::Iterator it2;
        for ( it2 = m_lstRedoRow.begin(); it2 != m_lstRedoRow.end(); ++it2 )
        {
           RowFormat *rw=sheet->nonDefaultRowFormat((*it2).rowNumber);
           rw->setDblHeight((*it2).rowHeight);
        }
    }

    //move next line to refreshView
    //because I must know what is the real rect
    //that I must refresh, when there is cell Merged

    sheet->paste( m_dataRedo, m_selection );
    //sheet->deleteCells( m_selection );
    sheet->updateView();
    sheet->refreshView( m_selection );
    doc()->undoUnlock();
}

/****************************************************************************
 *
 * KSpreadUndoDragDrop
 *
 ***************************************************************************/

KSpreadUndoDragDrop::KSpreadUndoDragDrop( KSpreadDoc * _doc, KSpreadSheet * _sheet, const QRect & _source,
                                          const QRect & _target )
  : KSpreadUndoAction( _doc ),
    m_selectionSource( _source ),
    m_selectionTarget( _target )
{
    name = i18n( "Drag & Drop" );

    m_sheetName = _sheet->sheetName();

    saveCellRect( m_dataTarget, _sheet, _target );
    if ( _source.left() > 0 )
      saveCellRect( m_dataSource, _sheet, _source );
}

KSpreadUndoDragDrop::~KSpreadUndoDragDrop()
{
}

void KSpreadUndoDragDrop::saveCellRect( QCString & cells, KSpreadSheet * sheet,
                                        QRect const & rect )
{
    QDomDocument doc = sheet->saveCellRect( rect );
    // Save to buffer
    QString buffer;
    QTextStream str( &buffer, IO_WriteOnly );
    str << doc;

    cells = buffer.utf8();
    int len = cells.length();
    char tmp = cells[ len - 1 ];
    cells.resize( len );
    *( cells.data() + len - 1 ) = tmp;
}

void KSpreadUndoDragDrop::undo()
{
    KSpreadSheet * sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    if ( m_selectionSource.left() > 0 )
      saveCellRect( m_dataRedoSource, sheet, m_selectionSource );
    saveCellRect( m_dataRedoTarget, sheet, m_selectionTarget );

    doc()->undoLock();
    doc()->emitBeginOperation();

    sheet->deleteCells( m_selectionTarget );
    sheet->paste( m_dataTarget, m_selectionTarget );

    if ( m_selectionSource.left() > 0 )
    {
      sheet->deleteCells( m_selectionSource );
      sheet->paste( m_dataSource, m_selectionSource );
    }

    sheet->updateView();

    if ( sheet->getAutoCalc() )
      sheet->recalc();

    doc()->undoUnlock();
}

void KSpreadUndoDragDrop::redo()
{
    KSpreadSheet * sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    doc()->emitBeginOperation();

    //move next line to refreshView
    //because I must know what is the real rect
    //that I must refresh, when there is cell Merged

    sheet->paste( m_dataRedoTarget, m_selectionTarget );
    if ( m_selectionSource.left() > 0 )
      sheet->paste( m_dataRedoSource, m_selectionSource );

    sheet->updateView();
    sheet->refreshView( m_selectionSource );
    sheet->refreshView( m_selectionTarget );
    doc()->undoUnlock();
}


/****************************************************************************
 *
 * KSpreadUndoResizeColRow
 *
 ***************************************************************************/


KSpreadUndoResizeColRow::KSpreadUndoResizeColRow( KSpreadDoc *_doc, KSpreadSheet *_sheet, const QRect &_selection ) :
    KSpreadUndoAction( _doc )
{
  name=i18n("Resize");
  m_rctRect = _selection;
  m_sheetName = _sheet->sheetName();

  createList( m_lstColumn,m_lstRow, _sheet );
}

void KSpreadUndoResizeColRow::createList( QValueList<columnSize> &listCol,QValueList<rowSize> &listRow, KSpreadSheet* sheet )
{
    listCol.clear();
    listRow.clear();

    if( util_isColumnSelected( m_rctRect ) ) // entire column(s)
    {
    for( int y = m_rctRect.left(); y <= m_rctRect.right(); y++ )
        {
           ColumnFormat *cl=sheet->columnFormat(y);
	   if(!cl->isHide())
	     {
	       columnSize tmpSize;
	       tmpSize.columnNumber=y;
	       tmpSize.columnWidth=cl->dblWidth();
	       listCol.append(tmpSize);
	     }
        }
    }
    else if( util_isRowSelected( m_rctRect ) ) // entire row(s)
    {
    for( int y = m_rctRect.top(); y <= m_rctRect.bottom(); y++ )
        {
           RowFormat *rw=sheet->rowFormat(y);
	   if(!rw->isHide())
	     {
	       rowSize tmpSize;
	       tmpSize.rowNumber=y;
	       tmpSize.rowHeight=rw->dblHeight();
	       listRow.append(tmpSize);
	     }
        }
    }
    else //row and column
    {
    for( int y = m_rctRect.left(); y <= m_rctRect.right(); y++ )
        {
           ColumnFormat *cl=sheet->columnFormat(y);
	   if(!cl->isHide())
	     {
	       columnSize tmpSize;
	       tmpSize.columnNumber=y;
	       tmpSize.columnWidth=cl->dblWidth();
	       listCol.append(tmpSize);
	     }
        }
    for( int y = m_rctRect.top(); y <= m_rctRect.bottom(); y++ )
        {
           RowFormat *rw=sheet->rowFormat(y);
	   if(!rw->isHide())
	     {
	       rowSize tmpSize;
	       tmpSize.rowNumber=y;
	       tmpSize.rowHeight=rw->dblHeight();
	       listRow.append(tmpSize);
	     }
        }

    }
}

KSpreadUndoResizeColRow::~KSpreadUndoResizeColRow()
{
}

void KSpreadUndoResizeColRow::undo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();

    createList( m_lstRedoColumn,m_lstRedoRow, sheet );

    if( util_isColumnSelected( m_rctRect ) ) // complete column(s)
    {
    QValueList<columnSize>::Iterator it2;
    for ( it2 = m_lstColumn.begin(); it2 != m_lstColumn.end(); ++it2 )
        {
           ColumnFormat *cl=sheet->columnFormat((*it2).columnNumber);
           cl->setDblWidth((*it2).columnWidth);
        }
    }
    else if( util_isRowSelected( m_rctRect ) ) // complete row(s)
    {
    QValueList<rowSize>::Iterator it2;
    for ( it2 = m_lstRow.begin(); it2 != m_lstRow.end(); ++it2 )
        {
           RowFormat *rw=sheet->rowFormat((*it2).rowNumber);
           rw->setDblHeight((*it2).rowHeight);
        }
    }
    else // row and column
    {
    QValueList<columnSize>::Iterator it2;
    for ( it2 = m_lstColumn.begin(); it2 != m_lstColumn.end(); ++it2 )
        {
           ColumnFormat *cl=sheet->columnFormat((*it2).columnNumber);
           cl->setDblWidth((*it2).columnWidth);
        }
    QValueList<rowSize>::Iterator it1;
    for ( it1 = m_lstRow.begin(); it1 != m_lstRow.end(); ++it1 )
        {
           RowFormat *rw=sheet->rowFormat((*it1).rowNumber);
           rw->setDblHeight((*it1).rowHeight);
        }
    }

    doc()->undoUnlock();
}

void KSpreadUndoResizeColRow::redo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    if( util_isColumnSelected( m_rctRect ) ) // complete column(s)
    {
    QValueList<columnSize>::Iterator it2;
    for ( it2 = m_lstRedoColumn.begin(); it2 != m_lstRedoColumn.end(); ++it2 )
        {
           ColumnFormat *cl=sheet->columnFormat((*it2).columnNumber);
           cl->setDblWidth((*it2).columnWidth);
        }
    }
    else if( util_isRowSelected( m_rctRect ) ) // complete row(s)
    {
    QValueList<rowSize>::Iterator it2;
    for ( it2 = m_lstRedoRow.begin(); it2 != m_lstRedoRow.end(); ++it2 )
        {
           RowFormat *rw=sheet->rowFormat((*it2).rowNumber);
           rw->setDblHeight((*it2).rowHeight);
        }
    }
    else // row and column
    {
    QValueList<columnSize>::Iterator it2;
    for ( it2 = m_lstRedoColumn.begin(); it2 != m_lstRedoColumn.end(); ++it2 )
        {
           ColumnFormat *cl=sheet->columnFormat((*it2).columnNumber);
           cl->setDblWidth((*it2).columnWidth);
        }
    QValueList<rowSize>::Iterator it1;
    for ( it1 = m_lstRedoRow.begin(); it1 != m_lstRedoRow.end(); ++it1 )
        {
           RowFormat *rw=sheet->rowFormat((*it1).rowNumber);
           rw->setDblHeight((*it1).rowHeight);
        }
    }

    doc()->undoUnlock();
}

/****************************************************************************
 *
 * KSpreadUndoChangeAreaTextCell
 *
 ***************************************************************************/


KSpreadUndoChangeAreaTextCell::KSpreadUndoChangeAreaTextCell( KSpreadDoc *_doc, KSpreadSheet *_sheet, const QRect &_selection ) :
    KSpreadUndoAction( _doc )
{
  name=i18n("Change Text");

  m_rctRect = _selection;
  m_sheetName = _sheet->sheetName();

  createList( m_lstTextCell, _sheet );
}

void KSpreadUndoChangeAreaTextCell::createList( QValueList<textOfCell> &list, KSpreadSheet* sheet )
{
    int bottom = m_rctRect.bottom();
    int right  = m_rctRect.right();
    list.clear();

    if( util_isColumnSelected( m_rctRect ) )
    {
      KSpreadCell * c;
      for ( int col = m_rctRect.left(); col <= right; ++col )
      {
        c = sheet->getFirstCellColumn( col );
        while ( c )
        {
          if ( !c->isObscuringForced() )
          {
            textOfCell tmpText;
            tmpText.col = col;
            tmpText.row = c->row();
            tmpText.text = c->text();
            list.append(tmpText);
          }
          c = sheet->getNextCellDown( col, c->row() );
        }
      }
    }
    else if ( util_isRowSelected( m_rctRect ) )
    {
      KSpreadCell * c;
      for ( int row = m_rctRect.top(); row <= bottom; ++row )
      {
        c = sheet->getFirstCellRow( row );
        while ( c )
        {
          if ( !c->isObscuringForced() )
          {
            textOfCell tmpText;
            tmpText.col = c->column();
            tmpText.row = row;
            tmpText.text = c->text();
            list.append(tmpText);
          }
          c = sheet->getNextCellRight( c->column(), row );
        }
      }
    }
    else
    {
      KSpreadCell * cell;
      for ( int x = m_rctRect.left(); x <= right; ++x )
      {
        cell = sheet->getFirstCellColumn( x );
        if ( !cell )
          continue;
        while ( cell && cell->row() <= bottom )
        {
          if ( !cell->isObscured() )
          {
            textOfCell tmpText;
            tmpText.col  = x;
            tmpText.row  = cell->row();
            tmpText.text = cell->text();
            list.append( tmpText );
          }
          cell = sheet->getNextCellDown( x, cell->row() );
        }
      }
    }
}

KSpreadUndoChangeAreaTextCell::~KSpreadUndoChangeAreaTextCell()
{
}

void KSpreadUndoChangeAreaTextCell::undo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    doc()->emitBeginOperation();
    createList( m_lstRedoTextCell, sheet );


    if ( !util_isRowSelected( m_rctRect )
         && !util_isColumnSelected( m_rctRect ) )
    {
      for ( int x = m_rctRect.left(); x <= m_rctRect.right(); ++x )
        for ( int y = m_rctRect.top(); y <= m_rctRect.bottom(); ++y )
        {
          KSpreadCell* cell = sheet->nonDefaultCell( x, y );
          bool found = false;
          QValueList<textOfCell>::Iterator it;
          for( it = m_lstTextCell.begin(); it != m_lstTextCell.end(); ++it )
            if ( (*it).col == x && (*it).row == y && !found )
            {
              cell->setCellText( (*it).text );
              found = true;
            }
          if( !found )
            cell->setCellText( "", true );
        }

    }
    else
    {
      QValueList<textOfCell>::Iterator it2;
      for ( it2 = m_lstTextCell.begin(); it2 != m_lstTextCell.end(); ++it2 )
      {
        KSpreadCell *cell = sheet->nonDefaultCell( (*it2).col, (*it2).row );
        if ( (*it2).text.isEmpty() )
        {
          if ( !cell->text().isEmpty() )
            cell->setCellText( "" );
        }
        else
          cell->setCellText( (*it2).text );
      }
    }

    sheet->updateView();
    doc()->undoUnlock();
}

void KSpreadUndoChangeAreaTextCell::redo()
{
    KSpreadSheet * sheet = doc()->map()->findSheet( m_sheetName );

    if ( !sheet )
	return;

    doc()->undoLock();
    doc()->emitBeginOperation();

    if ( !util_isRowSelected( m_rctRect )
         && !util_isColumnSelected( m_rctRect ) )
    {
      for ( int x = m_rctRect.left(); x <= m_rctRect.right(); ++x )
        for ( int y = m_rctRect.top(); y <= m_rctRect.bottom(); ++y )
        {
          KSpreadCell* cell = sheet->nonDefaultCell( x, y );
          bool found = false;
          QValueList<textOfCell>::Iterator it;
          for( it = m_lstRedoTextCell.begin(); it != m_lstRedoTextCell.end(); ++it )
            if ( (*it).col == x && (*it).row == y && !found )
            {
              cell->setCellText( (*it).text );
              found = true;
            }
          if( !found )
            cell->setCellText( "", true );
        }

    }
    else
    {
      QValueList<textOfCell>::Iterator it2;
      for ( it2 = m_lstRedoTextCell.begin(); it2 != m_lstRedoTextCell.end(); ++it2 )
      {
        KSpreadCell *cell = sheet->nonDefaultCell( (*it2).col, (*it2).row );
        if ( (*it2).text.isEmpty() )
        {
          if ( !cell->text().isEmpty() )
            cell->setCellText( "" );
        }
        else
          cell->setCellText( (*it2).text );
      }
    }

    sheet->updateView();
    doc()->undoUnlock();
}

/****************************************************************************
 *
 * KSpreadUndoMergedCell
 *
 ***************************************************************************/


KSpreadUndoMergedCell::KSpreadUndoMergedCell( KSpreadDoc *_doc, KSpreadSheet *_sheet, int _column, int _row , int _extraX,int _extraY) :
    KSpreadUndoAction( _doc )
{
  name=i18n("Merge Cells");

  m_sheetName = _sheet->sheetName();
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
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();

    KSpreadCell *cell = sheet->nonDefaultCell( m_iCol, m_iRow );
    m_iExtraRedoX=cell->extraXCells();
    m_iExtraRedoY=cell->extraYCells();

    sheet->changeMergedCell( m_iCol, m_iRow, m_iExtraX,m_iExtraY);

    doc()->undoUnlock();
}

void KSpreadUndoMergedCell::redo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();

    sheet->changeMergedCell( m_iCol, m_iRow, m_iExtraRedoX,m_iExtraRedoY);

    doc()->undoUnlock();
}

/****************************************************************************
 *
 * KSpreadUndoAutofill
 *
 ***************************************************************************/

KSpreadUndoAutofill::KSpreadUndoAutofill( KSpreadDoc *_doc, KSpreadSheet* sheet, const QRect & _selection)
    : KSpreadUndoAction( _doc )
{
    name=i18n("Autofill");

    m_sheetName = sheet->sheetName();
    m_selection = _selection;
    createListCell( m_data, sheet );

}

KSpreadUndoAutofill::~KSpreadUndoAutofill()
{
}

void KSpreadUndoAutofill::createListCell( QCString &list, KSpreadSheet* sheet )
{
    QDomDocument doc = sheet->saveCellRect( m_selection );
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
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    createListCell( m_dataRedo, sheet );

    doc()->undoLock();
    doc()->emitBeginOperation();

    sheet->deleteCells( m_selection );
    sheet->paste( m_data, m_selection );
    if(sheet->getAutoCalc()) sheet->recalc();

    sheet->updateView();

    doc()->undoUnlock();
}

void KSpreadUndoAutofill::redo()
{
    doc()->undoLock();

    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->emitBeginOperation();

    sheet->deleteCells( m_selection );
    doc()->undoLock();
    sheet->paste( m_dataRedo, m_selection );
    if ( sheet->getAutoCalc() )
      sheet->recalc();
    sheet->updateView();
    doc()->undoUnlock();
}

/****************************************************************************
 *
 * KSpreadUndoInsertCellRow
 *
 ***************************************************************************/

KSpreadUndoInsertCellRow::KSpreadUndoInsertCellRow( KSpreadDoc *_doc, KSpreadSheet *_sheet, const QRect &_rect ) :
    KSpreadUndoInsertRemoveAction( _doc )
{
    name=i18n("Insert Cell");

    m_sheetName = _sheet->sheetName();
    m_rect=_rect;
}

KSpreadUndoInsertCellRow::~KSpreadUndoInsertCellRow()
{
}

void KSpreadUndoInsertCellRow::undo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->unshiftRow( m_rect);
    doc()->undoUnlock();

    undoFormulaReference();
}

void KSpreadUndoInsertCellRow::redo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->shiftRow( m_rect);
    doc()->undoUnlock();
}

/****************************************************************************
 *
 * KSpreadUndoInsertCellCol
 *
 ***************************************************************************/


KSpreadUndoInsertCellCol::KSpreadUndoInsertCellCol( KSpreadDoc *_doc, KSpreadSheet *_sheet, const QRect &_rect ) :
    KSpreadUndoInsertRemoveAction( _doc )
{
    name=i18n("Insert Cell");

    m_sheetName = _sheet->sheetName();
    m_rect=_rect;
}

KSpreadUndoInsertCellCol::~KSpreadUndoInsertCellCol()
{
}

void KSpreadUndoInsertCellCol::undo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->unshiftColumn( m_rect);
    doc()->undoUnlock();

    undoFormulaReference();
}

void KSpreadUndoInsertCellCol::redo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->shiftColumn( m_rect );
    doc()->undoUnlock();
}

/****************************************************************************
 *
 * KSpreadUndoRemoveCellRow
 *
 ***************************************************************************/

KSpreadUndoRemoveCellRow::KSpreadUndoRemoveCellRow( KSpreadDoc *_doc, KSpreadSheet *_sheet, const QRect &rect ) :
    KSpreadUndoInsertRemoveAction( _doc )
{
    name=i18n("Remove Cell");

    m_sheetName = _sheet->sheetName();
    m_rect=rect;
    QDomDocument doc = _sheet->saveCellRect( m_rect );
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
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->shiftRow( m_rect );
    sheet->paste( m_data, m_rect );
    doc()->undoUnlock();

    undoFormulaReference();
}

void KSpreadUndoRemoveCellRow::redo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->unshiftRow( m_rect);
    doc()->undoUnlock();
}

/****************************************************************************
 *
 * KSpreadUndoRemoveCellCol
 *
 ***************************************************************************/

KSpreadUndoRemoveCellCol::KSpreadUndoRemoveCellCol( KSpreadDoc *_doc, KSpreadSheet *_sheet, const QRect &_rect ) :
    KSpreadUndoInsertRemoveAction( _doc )
{
    name=i18n("Remove Cell");

    m_sheetName = _sheet->sheetName();
    m_rect=_rect;
    QDomDocument doc = _sheet->saveCellRect( m_rect );
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
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->shiftColumn( m_rect );
    sheet->paste( m_data, m_rect );
    doc()->undoUnlock();

    undoFormulaReference();
}

void KSpreadUndoRemoveCellCol::redo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->unshiftColumn( m_rect );
    doc()->undoUnlock();
}

/****************************************************************************
 *
 * KSpreadUndoConditional
 *
 ***************************************************************************/

KSpreadUndoConditional::KSpreadUndoConditional( KSpreadDoc *_doc, KSpreadSheet* sheet, QRect const & _selection)
    : KSpreadUndoAction( _doc )
{
    name=i18n("Conditional Cell Attribute");

    m_sheetName = sheet->sheetName();
    m_selection = _selection;
    createListCell( m_data, sheet );

}

KSpreadUndoConditional::~KSpreadUndoConditional()
{
}

void KSpreadUndoConditional::createListCell( QCString &list, KSpreadSheet* sheet )
{
    QDomDocument doc = sheet->saveCellRect( m_selection );
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
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    createListCell( m_dataRedo, sheet );

    doc()->undoLock();
    sheet->paste( m_data, m_selection );
    if(sheet->getAutoCalc()) sheet->recalc();

    doc()->undoUnlock();
}

void KSpreadUndoConditional::redo()
{
    doc()->undoLock();

    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->paste( m_dataRedo, m_selection );
    if(sheet->getAutoCalc()) sheet->recalc();

    doc()->undoUnlock();
}


/****************************************************************************
 *
 * KSpreadUndoCellPaste
 *
 ***************************************************************************/

KSpreadUndoCellPaste::KSpreadUndoCellPaste( KSpreadDoc *_doc, KSpreadSheet* sheet, int _nbCol,int _nbRow, int _xshift,int _yshift, QRect &_selection,bool insert,int _insertTo )
    : KSpreadUndoAction( _doc )
{
    if(!insert)
        name=i18n("Paste");
    else
        name=i18n("Paste & Insert");

    m_sheetName = sheet->sheetName();
    m_selection = _selection;
    nbCol=_nbCol;
    nbRow=_nbRow;
    xshift=_xshift;
    yshift=_yshift;
    b_insert=insert;
    m_iInsertTo=_insertTo;
    if( !b_insert)
        createListCell( m_data, m_lstColumn,m_lstRow,sheet );

}

KSpreadUndoCellPaste::~KSpreadUndoCellPaste()
{
}

void KSpreadUndoCellPaste::createListCell( QCString &listCell,QValueList<columnSize> &listCol,QValueList<rowSize> &listRow, KSpreadSheet* sheet )
{
    listCol.clear();
    listRow.clear();
    //copy a column(s)
    if(nbCol!=0)
    {
        //save all cells
        QRect rect;
        rect.setCoords( xshift, 1, xshift+nbCol, KS_rowMax );
        QDomDocument doc = sheet->saveCellRect( rect);
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
           ColumnFormat *cl=sheet->columnFormat(y);
           if(!cl->isDefault())
                {
                columnSize tmpSize;
                tmpSize.columnNumber=y;
                tmpSize.columnWidth=cl->dblWidth();
                listCol.append(tmpSize);
                }
        }
    }
    //copy a row(s)
    else if(nbRow!=0)
    {
        //save all cells
        QRect rect;
        rect.setCoords( 1, yshift, KS_colMax, yshift+nbRow );
        QDomDocument doc = sheet->saveCellRect( rect);
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
           RowFormat *rw=sheet->rowFormat(y);
           if(!rw->isDefault())
                {
                rowSize tmpSize;
                tmpSize.rowNumber=y;
                tmpSize.rowHeight=rw->dblHeight();
                listRow.append(tmpSize);
                }
        }

    }
    //copy just an area
    else
    {
        //save all cells in area
        QDomDocument doc = sheet->saveCellRect( m_selection );
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
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    createListCell( m_dataRedo, m_lstRedoColumn,m_lstRedoRow,sheet );

    doc()->undoLock();
    doc()->emitBeginOperation();

    if(nbCol!=0)
    {
        if(!b_insert)
                {
                QRect rect;
                rect.setCoords( xshift, 1, xshift+nbCol, KS_rowMax );
                sheet->deleteCells( rect );
                QPoint pastePoint(xshift, 1);
                sheet->paste( m_data, QRect(pastePoint, pastePoint) );
                QValueList<columnSize>::Iterator it2;
                for ( it2 = m_lstColumn.begin(); it2 != m_lstColumn.end(); ++it2 )
                        {
                        ColumnFormat *cl=sheet->nonDefaultColumnFormat((*it2).columnNumber);
                        cl->setDblWidth((*it2).columnWidth);
                        }
                }
        else
                {
                sheet->removeColumn( xshift+1,nbCol-1,false);
                }
    }
    else if(nbRow!=0)
    {
        if(!b_insert)
                {
                QRect rect;
                rect.setCoords( 1, yshift, KS_colMax, yshift+nbRow );
                sheet->deleteCells( rect );

                QPoint pastePoint(1, yshift);
                sheet->paste( m_data, QRect(pastePoint, pastePoint) );
                QValueList<rowSize>::Iterator it2;
                for ( it2 = m_lstRow.begin(); it2 != m_lstRow.end(); ++it2 )
                        {
                        RowFormat *rw=sheet->nonDefaultRowFormat((*it2).rowNumber);
                        rw->setDblHeight((*it2).rowHeight);
                        }
                }
        else
                {
                sheet->removeRow(  yshift+1,nbRow-1);
                }
    }
    else
    {
    if(!b_insert)
        {
        sheet->deleteCells( m_selection );
        sheet->paste( m_data, m_selection );
        }
    else
        {
        if(m_iInsertTo==-1)
                sheet->unshiftRow(m_selection);
        else if(m_iInsertTo==1)
                sheet->unshiftColumn(m_selection);
        }
    }

    if(sheet->getAutoCalc())
        sheet->recalc();
    sheet->updateView();
    doc()->undoUnlock();
}

void KSpreadUndoCellPaste::redo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    doc()->emitBeginOperation();

    if(nbCol!=0)
    {
        if( b_insert)
                {
                sheet->insertColumn(  xshift+1,nbCol-1,false);
                }
        QRect rect;
        rect.setCoords( xshift, 1, xshift+nbCol, KS_rowMax );
        sheet->deleteCells( rect );
        QPoint pastePoint(xshift, 1);
        sheet->paste( m_dataRedo, QRect(pastePoint, pastePoint) );
        QValueList<columnSize>::Iterator it2;
         for ( it2 = m_lstRedoColumn.begin(); it2 != m_lstRedoColumn.end(); ++it2 )
                {
                ColumnFormat *cl=sheet->nonDefaultColumnFormat((*it2).columnNumber);
                cl->setDblWidth((*it2).columnWidth);
                }

    }
    else if(nbRow!=0)
    {
        if( b_insert)
                {
                sheet->insertRow(  yshift+1,nbRow-1);
                }

        QRect rect;
        rect.setCoords( 1, yshift, KS_colMax, yshift+nbRow );
        sheet->deleteCells( rect );

        QPoint pastePoint(1, yshift);
        sheet->paste( m_dataRedo, QRect(pastePoint, pastePoint) );
        QValueList<rowSize>::Iterator it2;
        for ( it2 = m_lstRedoRow.begin(); it2 != m_lstRedoRow.end(); ++it2 )
                {
                RowFormat *rw=sheet->nonDefaultRowFormat((*it2).rowNumber);
                 rw->setDblHeight((*it2).rowHeight);
                 }
    }
    else
    {
      if (b_insert)
      {
        if (m_iInsertTo==-1)
                sheet->shiftRow(m_selection);
        else if(m_iInsertTo==1)
                sheet->shiftColumn(m_selection);

      }
      sheet->deleteCells( m_selection );
      sheet->paste( m_dataRedo, m_selection );
    }
    if (sheet->getAutoCalc())
        sheet->recalc();

    sheet->updateView();

    doc()->undoUnlock();
}


/****************************************************************************
 *
 * KSpreadUndoStyleCell
 *
 ***************************************************************************/

KSpreadUndoStyleCell::KSpreadUndoStyleCell( KSpreadDoc *_doc, KSpreadSheet* sheet, const QRect & _selection)
    : KSpreadUndoAction( _doc )
{
    name=i18n("Style of Cell");

    m_sheetName = sheet->sheetName();
    m_selection = _selection;
    createListCell( m_lstStyleCell, sheet );

}

KSpreadUndoStyleCell::~KSpreadUndoStyleCell()
{
}

void KSpreadUndoStyleCell::createListCell( QValueList<styleCell> &listCell, KSpreadSheet* sheet )
{
  int bottom = m_selection.bottom();
  int right  = m_selection.right();
  if ( util_isColumnSelected( m_selection ) )
  {
    KSpreadCell * c;
    for ( int col = m_selection.left(); col <= right; ++ col )
    {
      c = sheet->getFirstCellColumn( col );
      while ( c )
      {
        if ( !c->isObscuringForced() )
        {
	  styleCell tmpStyleCell;
	  tmpStyleCell.row = c->row();
	  tmpStyleCell.col = col;
	  listCell.append(tmpStyleCell);
        }
        c = sheet->getNextCellDown( col, c->row() );
      }
    }
  }
  else if ( util_isRowSelected( m_selection ) )
  {
    KSpreadCell * c;
    for ( int row = m_selection.top(); row <= bottom; ++row )
    {
      c = sheet->getFirstCellRow( row );
      while ( c )
      {
        if ( !c->isObscuringForced() )
        {
	  styleCell tmpStyleCell;
	  tmpStyleCell.row = row;
	  tmpStyleCell.col = c->column();
	  listCell.append(tmpStyleCell);
        }
        c = sheet->getNextCellRight( c->column(), row );
      }
    }
  }
  else
  {
    KSpreadCell * cell;
    for ( int i = m_selection.top(); i <= bottom; ++i)
	for ( int j = m_selection.left(); j <= right; ++j )
        {
          cell = sheet->nonDefaultCell( j, i);
          styleCell tmpStyleCell;
          tmpStyleCell.row = i;
          tmpStyleCell.col = j;
          listCell.append(tmpStyleCell);
        }
  }
}

void KSpreadUndoStyleCell::undo()
{
    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    createListCell( m_lstRedoStyleCell, sheet );

    doc()->undoLock();
    doc()->emitBeginOperation();


    QValueList<styleCell>::Iterator it2;
    for ( it2 = m_lstStyleCell.begin(); it2 != m_lstStyleCell.end(); ++it2 )
      {
	sheet->nonDefaultCell( (*it2).col, (*it2).row);
      }
    sheet->setRegionPaintDirty(m_selection);
    sheet->updateView( m_selection );
    doc()->undoUnlock();
}

void KSpreadUndoStyleCell::redo()
{
    doc()->undoLock();

    KSpreadSheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    doc()->emitBeginOperation();

    QValueList<styleCell>::Iterator it2;
    for ( it2 = m_lstRedoStyleCell.begin(); it2 != m_lstRedoStyleCell.end(); ++it2 )
      {
	sheet->nonDefaultCell( (*it2).col, (*it2).row);
      }
    sheet->setRegionPaintDirty(m_selection);
    sheet->updateView();

    doc()->undoUnlock();
}

KSpreadUndoInsertData::KSpreadUndoInsertData( KSpreadDoc * _doc, KSpreadSheet * _sheet, QRect & _selection )
    : KSpreadUndoChangeAreaTextCell( _doc, _sheet, _selection )
{
    name = i18n("Insert Data From Database");
}
