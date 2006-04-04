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


#include "kspread_cell.h"
#include "kspread_doc.h"
#include "kspread_global.h"
#include "kspread_locale.h"
#include "kspread_map.h"
#include "kspread_sheet.h"
#include "kspread_sheetprint.h"
#include "kspread_style.h"
#include "kspread_style_manager.h"
#include "kspread_util.h"

#include "kspread_undo.h"

using namespace KSpread;

bool operator < (const QPoint& pointA , const QPoint& pointB)
{
	if (pointA.y() == pointB.y())
		return ( pointA.x() < pointB.x() );
	else
		return ( pointA.y() < pointB.y() );
}

/****************************************************************************
 *
 * Undo
 *
 ***************************************************************************/

Undo::Undo( Doc *_doc )
{
    m_pDoc = _doc;

    m_stckUndo.setAutoDelete( false );
    m_stckRedo.setAutoDelete( false );
}

Undo::~Undo()
{
    clear();
}

void Undo::appendUndo( UndoAction *_action )
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

void Undo::clear()
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

void Undo::undo()
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

    UndoAction *a = m_stckUndo.pop();
    a->undo();
    m_stckRedo.push( a );

    if ( m_pDoc )
    {
        m_pDoc->setShowMessageError( origErrorMessages  );
        m_pDoc->enableUndo( hasUndoActions() );
        m_pDoc->enableRedo( hasRedoActions() );
    }
}

void Undo::redo()
{
    if ( m_stckRedo.isEmpty() )
	return;
    UndoAction *a = m_stckRedo.pop();
    a->redo();
    m_stckUndo.push( a );

    if ( m_pDoc )
    {
	m_pDoc->enableUndo( hasUndoActions() );
	m_pDoc->enableRedo( hasRedoActions() );
    }
}

void Undo::lock()
{
  m_pDoc->undoLock();
}

void Undo::unlock()
{
  m_pDoc->undoUnlock();
}

bool Undo::isLocked() const
{
  return m_pDoc->undoLocked();
}

QString Undo::getRedoName()
{
    if ( m_stckRedo.isEmpty() )
	return QString("");
    return  m_stckRedo.current()->getName();

}

QString Undo::getUndoName()
{
    if ( m_stckUndo.isEmpty() )
	return QString("");
    return  m_stckUndo.current()->getName();
}

/****************************************************************************
 *
 * MacroUndoAction
 *
 ***************************************************************************/
MacroUndoAction::MacroUndoAction( Doc *_doc, const QString& _name ):
 UndoAction( _doc )
{
    name=_name;
}

MacroUndoAction::~MacroUndoAction()
{
    m_commands.setAutoDelete( true );
}

void MacroUndoAction::addCommand(UndoAction *command)
{
    m_commands.append(command);
}

void MacroUndoAction::undo()
{
    QPtrListIterator<UndoAction> it(m_commands);
    for ( ; it.current() ; ++it )
        it.current()->undo();
}

void MacroUndoAction::redo()
{
    QPtrListIterator<UndoAction> it(m_commands);
    for ( ; it.current() ; ++it )
        it.current()->redo();
}

/****************************************************************************
 *
 * UndoInsertRemoveAction
 *
 ***************************************************************************/

UndoInsertRemoveAction::UndoInsertRemoveAction( Doc * _doc ) :
    UndoAction( _doc )
{
}

UndoInsertRemoveAction::~UndoInsertRemoveAction()
{

}

void UndoInsertRemoveAction::saveFormulaReference( Sheet *_sheet,
                                             int col, int row, QString & formula )
{
    if ( _sheet == 0 )
        return;
    QString sheetName = _sheet->sheetName();

    m_lstFormulaCells.append( FormulaOfCell( sheetName, col, row, formula ) );
}

void UndoInsertRemoveAction::undoFormulaReference()
{
    QValueList<FormulaOfCell>::iterator it;
    for ( it = m_lstFormulaCells.begin(); it != m_lstFormulaCells.end(); ++it )
    {
        Sheet* sheet = doc()->map()->findSheet( (*it).sheetName() );
        if ( sheet )
        {
            Cell * cell = sheet->cellAt( (*it).col(), (*it).row() );
            if ( cell && !cell->isDefault() )
            {
                cell->setCellText( (*it).formula() );
            }
        }
    }
}

/****************************************************************************
 *
 * UndoRemoveColumn
 *
 ***************************************************************************/

UndoRemoveColumn::UndoRemoveColumn( Doc *_doc, Sheet *_sheet, int _column,int _nbCol ) :
    UndoInsertRemoveAction( _doc )
{
    name=i18n("Remove Columns");
    m_sheetName = _sheet->sheetName();
    m_iColumn= _column;
    m_iNbCol = _nbCol;
    m_printRange = _sheet->print()->printRange();
    m_printRepeatColumns = _sheet->print()->printRepeatColumns();
    QRect selection;
    selection.setCoords( _column, 1, _column+m_iNbCol, KS_rowMax );
    QDomDocument doc = _sheet->saveCellRegion( selection );

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

UndoRemoveColumn::~UndoRemoveColumn()
{
}

void UndoRemoveColumn::undo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
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

void UndoRemoveColumn::redo()
{
    doc()->undoLock();

    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    sheet->removeColumn( m_iColumn,m_iNbCol );

    doc()->undoUnlock();
}

/****************************************************************************
 *
 * UndoInsertColumn
 *
 ***************************************************************************/

UndoInsertColumn::UndoInsertColumn( Doc *_doc, Sheet *_sheet, int _column, int _nbCol ) :
    UndoInsertRemoveAction( _doc )
{
    name=i18n("Insert Columns");
    m_sheetName = _sheet->sheetName();
    m_iColumn= _column;
    m_iNbCol=_nbCol;
}

UndoInsertColumn::~UndoInsertColumn()
{
}

void UndoInsertColumn::undo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->removeColumn( m_iColumn,m_iNbCol );
    doc()->undoUnlock();

    undoFormulaReference();
}

void UndoInsertColumn::redo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->insertColumn( m_iColumn,m_iNbCol);
    doc()->undoUnlock();
}

/****************************************************************************
 *
 * UndoRemoveRow
 *
 ***************************************************************************/

UndoRemoveRow::UndoRemoveRow( Doc *_doc, Sheet *_sheet, int _row,int _nbRow) :
    UndoInsertRemoveAction( _doc )
{
    name=i18n("Remove Rows");

    m_sheetName = _sheet->sheetName();
    m_iRow = _row;
    m_iNbRow=  _nbRow;
    m_printRange=_sheet->print()->printRange();
    m_printRepeatRows = _sheet->print()->printRepeatRows();

    QRect selection;
    selection.setCoords( 1, _row, KS_colMax, _row+m_iNbRow );
    QDomDocument doc = _sheet->saveCellRegion( selection );

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

UndoRemoveRow::~UndoRemoveRow()
{
}

void UndoRemoveRow::undo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
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

void UndoRemoveRow::redo()
{
    doc()->undoLock();

    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    sheet->removeRow( m_iRow,m_iNbRow );

    doc()->undoUnlock();
}

/****************************************************************************
 *
 * UndoInsertRow
 *
 ***************************************************************************/

UndoInsertRow::UndoInsertRow( Doc *_doc, Sheet *_sheet, int _row,int _nbRow ) :
    UndoInsertRemoveAction( _doc )
{
    name=i18n("Insert Rows");
    m_sheetName = _sheet->sheetName();
    m_iRow = _row;
    m_iNbRow=_nbRow;
}

UndoInsertRow::~UndoInsertRow()
{
}

void UndoInsertRow::undo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->removeRow( m_iRow,m_iNbRow );
    doc()->undoUnlock();

    undoFormulaReference();
}

void UndoInsertRow::redo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->insertRow( m_iRow,m_iNbRow );
    doc()->undoUnlock();
}


/****************************************************************************
 *
 * UndoHideRow
 *
 ***************************************************************************/

/*UndoHideRow::UndoHideRow( Doc *_doc, Sheet *_sheet, int _row, int _nbRow , QValueList<int>_listRow) :
    UndoAction( _doc )
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

UndoHideRow::~UndoHideRow()
{
}

void UndoHideRow::createList( QValueList<int>&list,Sheet *tab )
{
RowFormat *rl;
for(int i=m_iRow;i<=(m_iRow+m_iNbRow);i++)
        {
        rl= tab->nonDefaultRowFormat( i );
        if(!rl->isHide())
                list.append(rl->row());
        }
}

void UndoHideRow::undo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->showRow( 0,-1,listRow );
    doc()->undoUnlock();
}

void UndoHideRow::redo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->hideRow(0,-1, listRow );
    doc()->undoUnlock();
}*/

/****************************************************************************
 *
 * UndoHideColumn
 *
 ***************************************************************************/

/*UndoHideColumn::UndoHideColumn( Doc *_doc, Sheet *_sheet, int _column, int _nbCol, QValueList<int>_listCol ) :
    UndoAction( _doc )
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

UndoHideColumn::~UndoHideColumn()
{
}

void UndoHideColumn::createList( QValueList<int>&list,Sheet *tab )
{
ColumnFormat *cl;
for(int i=m_iColumn;i<=(m_iColumn+m_iNbCol);i++)
  {
    cl= tab->nonDefaultColumnFormat( i );
    if(!cl->isHide())
      list.append(cl->column());
  }
}

void UndoHideColumn::undo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->showColumn(0,-1,listCol);
    doc()->undoUnlock();
}

void UndoHideColumn::redo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->hideColumn(0,-1,listCol);
    doc()->undoUnlock();
}*/

/****************************************************************************
 *
 * UndoShowRow
 *
 ***************************************************************************/

/*UndoShowRow::UndoShowRow( Doc *_doc, Sheet *_sheet, int _row, int _nbRow, QValueList<int>_listRow ) :
    UndoAction( _doc )
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

UndoShowRow::~UndoShowRow()
{
}

void UndoShowRow::createList( QValueList<int>&list,Sheet *tab )
{
RowFormat *rl;
for(int i=m_iRow;i<=(m_iRow+m_iNbRow);i++)
        {
        rl= tab->nonDefaultRowFormat( i );
        if(rl->isHide())
                list.append(rl->row());
        }
}

void UndoShowRow::undo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->hideRow(0,-1,listRow);
    doc()->undoUnlock();
}

void UndoShowRow::redo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->showRow(0,-1,listRow);
    doc()->undoUnlock();
}*/

/****************************************************************************
 *
 * UndoShowColumn
 *
 ***************************************************************************/

/*UndoShowColumn::UndoShowColumn( Doc *_doc, Sheet *_sheet, int _column, int _nbCol,QValueList<int>_listCol ) :
    UndoAction( _doc )
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

UndoShowColumn::~UndoShowColumn()
{
}

void UndoShowColumn::createList( QValueList<int>&list,Sheet *tab )
{
ColumnFormat *cl;
for(int i=m_iColumn;i<=(m_iColumn+m_iNbCol);i++)
  {
    cl= tab->nonDefaultColumnFormat( i );
    if(cl->isHide())
      list.append(cl->column());
  }

}

void UndoShowColumn::undo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->hideColumn( 0,-1,listCol );
    doc()->undoUnlock();
}

void UndoShowColumn::redo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->showColumn(0,-1,listCol);
    doc()->undoUnlock();
}*/


/****************************************************************************
 *
 * UndoPaperLayout
 *
 ***************************************************************************/

UndoPaperLayout::UndoPaperLayout( Doc *_doc, Sheet *_sheet )
    : UndoAction( _doc )
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

UndoPaperLayout::~UndoPaperLayout()
{
}

void UndoPaperLayout::undo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
        return;
    SheetPrint* print = sheet->print();

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

void UndoPaperLayout::redo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
        return;
    SheetPrint* print = sheet->print();

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
 * UndoSetText
 *
 ***************************************************************************/

UndoSetText::UndoSetText( Doc *_doc, Sheet *_sheet, const QString& _text, int _column, int _row,FormatType _formatType ) :
    UndoAction( _doc )
{
    name=i18n("Change Text");

    m_strText = _text;
    m_iColumn= _column;
    m_iRow = _row;
    m_sheetName = _sheet->sheetName();
    m_eFormatType=_formatType;
}

UndoSetText::~UndoSetText()
{
}

void UndoSetText::undo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    doc()->emitBeginOperation();
    Cell *cell = sheet->nonDefaultCell( m_iColumn, m_iRow );
    m_strRedoText = cell->text();
    m_eFormatTypeRedo=cell->format()->getFormatType( m_iColumn, m_iRow );
    cell->format()->setFormatType(m_eFormatType);

    if ( m_strText.isNull() )
	cell->setCellText( "" );
    else
	cell->setCellText( m_strText );
    sheet->updateView( QRect( m_iColumn, m_iRow, 1, 1 ) );
    doc()->undoUnlock();
}

void UndoSetText::redo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    doc()->emitBeginOperation();
    Cell *cell = sheet->nonDefaultCell( m_iColumn, m_iRow );
    m_strText = cell->text();
    m_eFormatType=cell->format()->getFormatType( m_iColumn, m_iRow );
    if ( m_strRedoText.isNull() )
	cell->setCellText( "" );
    else
	cell->setCellText( m_strRedoText );
    cell->format()->setFormatType(m_eFormatTypeRedo);
    sheet->updateView( QRect( m_iColumn, m_iRow, 1, 1 ) );
    doc()->undoUnlock();
}

/****************************************************************************
 *
 * UndoCellFormat
 *
 ***************************************************************************/

UndoCellFormat::UndoCellFormat( Doc * _doc,
                                Sheet * _sheet,
                                const Region & _selection,
                                const QString & _name ) :
  UndoAction( _doc )
{
  if ( _name.isEmpty())
    name = i18n("Change Format");
  else
    name = _name;

  m_region   = _selection;
  m_sheetName = _sheet->sheetName();
  copyFormat( m_lstFormats, m_lstColFormats, m_lstRowFormats, _sheet );
}

void UndoCellFormat::copyFormat(QValueList<layoutCell> & list,
                                       QValueList<layoutColumn> & listCol,
                                       QValueList<layoutRow> & listRow,
                                       Sheet * sheet )
{
    QValueList<layoutCell>::Iterator it2;
  for ( it2 = list.begin(); it2 != list.end(); ++it2 )
  {
      delete (*it2).l;
  }
  list.clear();

  Cell * cell;
  Region::ConstIterator endOfList(m_region.constEnd());
  for (Region::ConstIterator it = m_region.constBegin(); it != endOfList; ++it)
  {
    QRect range = (*it)->rect().normalize();
  int bottom = range.bottom();
  int right  = range.right();

  if ( util_isColumnSelected( range ) )
  {
    /* Don't need to go through the loop twice...
      for (int i = range.left(); i <= right; ++i)
      {
      layoutColumn tmplayout;
      tmplayout.col = i;
      tmplayout.l = new ColumnFormat( sheet, i );
      tmplayout.l->copy( *(sheet->columnFormat( i )) );
      listCol.append(tmplayout);
      }
    */
    for ( int c = range.left(); c <= right; ++c )
    {
      layoutColumn tmplayout;
      tmplayout.col = c;
      tmplayout.l = new ColumnFormat( sheet, c );
      tmplayout.l->copy( *(sheet->columnFormat( c )) );
      listCol.append(tmplayout);

      cell = sheet->getFirstCellColumn( c );
      while ( cell )
      {
        if ( cell->isPartOfMerged() )
        {
          cell = sheet->getNextCellDown( c, cell->row() );
          continue;
        }

        layoutCell tmplayout;
        tmplayout.col = c;
        tmplayout.row = cell->row();
        tmplayout.l = new Format( sheet, 0 );
        tmplayout.l->copy( *(sheet->cellAt( tmplayout.col, tmplayout.row )->format()) );
        list.append(tmplayout);

        cell = sheet->getNextCellDown( c, cell->row() );
      }
    }
    /*
      Cell * c = sheet->firstCell();
      for( ; c; c = c->nextCell() )
      {
      int col = c->column();
      if ( range.left() <= col && right >= col
          && !c->isPartOfMerged())
      {
        layoutCell tmplayout;
        tmplayout.col = c->column();
        tmplayout.row = c->row();
        tmplayout.l = new Format( sheet, 0 );
        tmplayout.l->copy( *(sheet->cellAt( tmplayout.col, tmplayout.row )) );
        list.append(tmplayout);
      }
      }
    */
  }
  else if (util_isRowSelected( range ) )
  {
    for ( int row = range.top(); row <= bottom; ++row )
    {
      layoutRow tmplayout;
      tmplayout.row = row;
      tmplayout.l = new RowFormat( sheet, row );
      tmplayout.l->copy( *(sheet->rowFormat( row )) );
      listRow.append(tmplayout);

      cell = sheet->getFirstCellRow( row );
      while ( cell )
      {
        if ( cell->isPartOfMerged() )
        {
          cell = sheet->getNextCellRight( cell->column(), row );
          continue;
        }
        layoutCell tmplayout;
        tmplayout.col = cell->column();
        tmplayout.row = row;
        tmplayout.l = new Format( sheet, 0 );
        tmplayout.l->copy( *(sheet->cellAt( cell->column(), row )->format()) );
        list.append(tmplayout);

        cell = sheet->getNextCellRight( cell->column(), row );
      }
    }
    /*
      Cell * c = sheet->firstCell();
      for( ; c; c = c->nextCell() )
      {
      int row = c->row();
      if ( range.top() <= row && bottom >= row
           && !c->isPartOfMerged())
      {
        layoutCell tmplayout;
        tmplayout.col = c->column();
        tmplayout.row = c->row();
        tmplayout.l = new Format( sheet, 0 );
        tmplayout.l->copy( *(sheet->cellAt( tmplayout.col, tmplayout.row )) );
        list.append(tmplayout);
      }
      }
    */
  }
  else
  {
    for ( int y = range.top(); y <= bottom; ++y )
      for ( int x = range.left(); x <= right; ++x )
      {
        Cell * cell = sheet->nonDefaultCell( x, y );
        if ( !cell->isPartOfMerged() )
        {
          layoutCell tmplayout;
          tmplayout.col = x;
          tmplayout.row = y;
          tmplayout.l = new Format( sheet, 0 );
          tmplayout.l->copy( *(sheet->cellAt( x, y )->format()) );
          list.append(tmplayout);
        }
      }
  }
  }
}

UndoCellFormat::~UndoCellFormat()
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

void UndoCellFormat::undo()
{
  Sheet * sheet = doc()->map()->findSheet( m_sheetName );
  if ( !sheet )
    return;

  doc()->undoLock();
  doc()->emitBeginOperation();
  copyFormat( m_lstRedoFormats, m_lstRedoColFormats, m_lstRedoRowFormats, sheet );
  Region::ConstIterator endOfList(m_region.constEnd());
  for (Region::ConstIterator it = m_region.constBegin(); it != endOfList; ++it)
  {
    QRect range = (*it)->rect().normalize();
  if( util_isColumnSelected( range ) )
  {
    QValueList<layoutColumn>::Iterator it2;
    for ( it2 = m_lstColFormats.begin(); it2 != m_lstColFormats.end(); ++it2 )
    {
      ColumnFormat * col = sheet->nonDefaultColumnFormat( (*it2).col );
      col->copy( *(*it2).l );
    }
  }
  else if( util_isRowSelected( range ) )
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
    Cell *cell = sheet->nonDefaultCell( (*it2).col,(*it2).row );
    cell->format()->copy( *(*it2).l );
    cell->setLayoutDirtyFlag();
    cell->setDisplayDirtyFlag();
    sheet->updateCell( cell, (*it2).col, (*it2).row );
  }
  }
  sheet->setRegionPaintDirty( m_region );
  sheet->updateView( &m_region );

  doc()->undoUnlock();
}

void UndoCellFormat::redo()
{
  Sheet* sheet = doc()->map()->findSheet( m_sheetName );
  if ( !sheet )
    return;

  doc()->undoLock();
  doc()->emitBeginOperation();

  Region::ConstIterator endOfList(m_region.constEnd());
  for (Region::ConstIterator it = m_region.constBegin(); it != endOfList; ++it)
  {
    QRect range = (*it)->rect().normalize();
  if ( util_isColumnSelected( range ) )
  {
    QValueList<layoutColumn>::Iterator it2;
    for ( it2 = m_lstRedoColFormats.begin(); it2 != m_lstRedoColFormats.end(); ++it2 )
    {
      ColumnFormat * col = sheet->nonDefaultColumnFormat( (*it2).col );
      col->copy( *(*it2).l );
    }
  }
  else if( util_isRowSelected( range ) )
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
    Cell * cell = sheet->nonDefaultCell( (*it2).col,(*it2).row );
    cell->format()->copy( *(*it2).l );
    cell->setLayoutDirtyFlag();
    cell->setDisplayDirtyFlag();
    sheet->updateCell( cell, (*it2).col, (*it2).row );
  }
  }

  sheet->setRegionPaintDirty( m_region );
  sheet->updateView( &m_region );
  doc()->undoUnlock();
}

/****************************************************************************
 *
 * UndoChangeAngle
 *
 ***************************************************************************/

UndoChangeAngle::UndoChangeAngle( Doc * _doc,
                                              Sheet * _sheet,
                                              const Region & _selection ) :
  UndoAction( _doc )
{
  name = i18n("Change Angle");
  m_layoutUndo = new UndoCellFormat( _doc, _sheet, _selection, QString::null );
  m_resizeUndo = new UndoResizeColRow( _doc, _sheet, _selection );
}

UndoChangeAngle::~UndoChangeAngle()
{
  delete m_resizeUndo;
  delete m_layoutUndo;
}

void UndoChangeAngle::undo()
{
  m_layoutUndo->undo();
  m_resizeUndo->undo();
}

void UndoChangeAngle::redo()
{
  m_layoutUndo->redo();
  m_resizeUndo->redo();
}

/****************************************************************************
 *
 * UndoSort
 *
 ***************************************************************************/

UndoSort::UndoSort( Doc * _doc, Sheet * _sheet, const QRect & _selection ) :
    UndoAction( _doc )
{
  name        = i18n("Sort");

  m_rctRect   = _selection;
  m_sheetName = _sheet->sheetName();
  copyAll( m_lstFormats, m_lstColFormats, m_lstRowFormats, _sheet );
}

void UndoSort::copyAll(QValueList<layoutTextCell> & list, QValueList<layoutColumn> & listCol,
                              QValueList<layoutRow> & listRow, Sheet * sheet )
{
  QValueList<layoutTextCell>::Iterator it2;
  for ( it2 = list.begin(); it2 != list.end(); ++it2 )
  {
      delete (*it2).l;
  }
  list.clear();

  if ( util_isColumnSelected( m_rctRect ) )
  {
    Cell * c;
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
        if ( !c->isPartOfMerged() )
        {
          layoutTextCell tmplayout;
          tmplayout.col = col;
          tmplayout.row = c->row();
          tmplayout.l = new Format( sheet, 0 );
          tmplayout.l->copy( *(sheet->cellAt( tmplayout.col, tmplayout.row )->format()) );
          tmplayout.text = c->text();
          list.append(tmplayout);
        }

        c = sheet->getNextCellDown( col, c->row() );
      }
    }
  }
  else if ( util_isRowSelected( m_rctRect ) )
  {
    Cell * c;
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
        if ( !c->isPartOfMerged() )
        {
          layoutTextCell tmplayout;
          tmplayout.col = c->column();
          tmplayout.row = row;
          tmplayout.l   = new Format( sheet, 0 );
          tmplayout.l->copy( *(sheet->cellAt( tmplayout.col, tmplayout.row )->format()) );
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
    Cell * cell;
    for ( int y = m_rctRect.top(); y <= bottom; ++y )
      for ( int x = m_rctRect.left(); x <= right; ++x )
      {
        cell = sheet->nonDefaultCell( x, y );
        if (!cell->isPartOfMerged())
        {
          layoutTextCell tmplayout;
          tmplayout.col = x;
          tmplayout.row = y;
          tmplayout.l   = new Format( sheet, 0 );
          tmplayout.l->copy( *(sheet->cellAt( x, y )->format()) );
          tmplayout.text = cell->text();
          list.append(tmplayout);
        }
      }
  }
}

UndoSort::~UndoSort()
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

void UndoSort::undo()
{
  Sheet * sheet = doc()->map()->findSheet( m_sheetName );
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
    Cell *cell = sheet->nonDefaultCell( (*it2).col,(*it2).row );
    if ( (*it2).text.isEmpty() )
    {
      if(!cell->text().isEmpty())
        cell->setCellText( "" );
    }
    else
      cell->setCellText( (*it2).text );

    cell->format()->copy( *(*it2).l );
    cell->setLayoutDirtyFlag();
    cell->setDisplayDirtyFlag();
    sheet->updateCell( cell, (*it2).col, (*it2).row );
  }

  sheet->setRegionPaintDirty(m_rctRect);
  sheet->updateView( m_rctRect );

  doc()->undoUnlock();
}

void UndoSort::redo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
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
      Cell *cell = sheet->nonDefaultCell( (*it2).col,(*it2).row );

      if ( (*it2).text.isEmpty() )
      {
        if(!cell->text().isEmpty())
          cell->setCellText( "" );
      }
      else
        cell->setCellText( (*it2).text );

      cell->format()->copy( *(*it2).l );
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
 * UndoDelete
 *
 ***************************************************************************/

UndoDelete::UndoDelete( Doc *_doc, Sheet* sheet, const Region& region)
    : UndoAction( _doc )
{
    name=i18n("Delete");
    m_sheetName = sheet->sheetName();
    m_region = region;
    createListCell(m_data, m_lstColumn, m_lstRow, sheet);
}

UndoDelete::~UndoDelete()
{
}

void UndoDelete::createListCell( QCString &listCell,QValueList<columnSize> &listCol,QValueList<rowSize> &listRow, Sheet* sheet )
{
    listRow.clear();
    listCol.clear();
    Region::ConstIterator endOfList = m_region.constEnd();
    for (Region::ConstIterator it = m_region.constBegin(); it != endOfList; ++it)
    {
      QRect range = (*it)->rect().normalize();
      // copy column(s)
      if ((*it)->isColumn())
    {
        for( int y = range.left() ; y <= range.right() ; ++y )
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
    // copy row(s)
    else if ((*it)->isRow())
    {
        //save size of row(s)
        for( int y = range.top() ; y <= range.bottom() ; ++y )
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
    }

    //save all cells in area
    QDomDocument doc = sheet->saveCellRegion( m_region );
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


void UndoDelete::undo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;
    createListCell( m_dataRedo, m_lstRedoColumn, m_lstRedoRow, sheet );

    doc()->undoLock();
    doc()->emitBeginOperation();

    {
        QValueList<columnSize>::Iterator it2;
        for ( it2 = m_lstColumn.begin(); it2 != m_lstColumn.end(); ++it2 )
        {
           ColumnFormat *cl=sheet->nonDefaultColumnFormat((*it2).columnNumber);
           cl->setDblWidth((*it2).columnWidth);
        }
    }

    {
        QValueList<rowSize>::Iterator it2;
        for ( it2 = m_lstRow.begin(); it2 != m_lstRow.end(); ++it2 )
        {
           RowFormat *rw=sheet->nonDefaultRowFormat((*it2).rowNumber);
           rw->setDblHeight((*it2).rowHeight);
        }
    }

    sheet->deleteCells(m_region);
    sheet->paste( m_data, m_region.boundingRect() );
    sheet->updateView( );

    if(sheet->getAutoCalc()) sheet->recalc();

    doc()->undoUnlock();
}

void UndoDelete::redo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    doc()->emitBeginOperation();

    {
        QValueList<columnSize>::Iterator it2;
        for ( it2 = m_lstRedoColumn.begin(); it2 != m_lstRedoColumn.end(); ++it2 )
        {
           ColumnFormat *cl=sheet->nonDefaultColumnFormat((*it2).columnNumber);
           cl->setDblWidth((*it2).columnWidth);
        }
    }

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

    sheet->paste( m_dataRedo, m_region.boundingRect() );
    //sheet->deleteCells( m_selection );
    sheet->updateView();
    sheet->refreshView( m_region ); // deletes the cells in region!
    doc()->undoUnlock();
}

/****************************************************************************
 *
 * UndoDragDrop
 *
 ***************************************************************************/

UndoDragDrop::UndoDragDrop( Doc * _doc, Sheet * _sheet,
                            const Region& _source,
                            const Region& _target )
  : UndoAction( _doc ),
    m_selectionSource( _source ),
    m_selectionTarget( _target )
{
    name = i18n( "Drag & Drop" );

    m_sheetName = _sheet->sheetName();

    saveCellRect( m_dataTarget, _sheet, _target );
    saveCellRect( m_dataSource, _sheet, _source );
}

UndoDragDrop::~UndoDragDrop()
{
}

void UndoDragDrop::saveCellRect( QCString & cells, Sheet * sheet,
                                 const Region& region )
{
    QDomDocument doc = sheet->saveCellRegion(region);
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

void UndoDragDrop::undo()
{
    Sheet * sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    saveCellRect( m_dataRedoSource, sheet, m_selectionSource );
    saveCellRect( m_dataRedoTarget, sheet, m_selectionTarget );

    doc()->undoLock();
    doc()->emitBeginOperation();

    sheet->deleteCells( m_selectionTarget );
    sheet->paste( m_dataTarget, m_selectionTarget.boundingRect() );

    sheet->deleteCells( m_selectionSource );
    sheet->paste( m_dataSource, m_selectionSource.boundingRect() );

    sheet->updateView();

    if ( sheet->getAutoCalc() )
      sheet->recalc();

    doc()->undoUnlock();
}

void UndoDragDrop::redo()
{
    Sheet * sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    doc()->emitBeginOperation();

    //move next line to refreshView
    //because I must know what is the real rect
    //that I must refresh, when there is cell Merged

    sheet->deleteCells( m_selectionTarget );
    sheet->paste( m_dataRedoTarget, m_selectionTarget.boundingRect() );
    sheet->deleteCells( m_selectionSource );
    sheet->paste( m_dataRedoSource, m_selectionSource.boundingRect() );

    sheet->updateView();
    if ( sheet->getAutoCalc() )
          sheet->recalc();

    doc()->undoUnlock();
}


/****************************************************************************
 *
 * UndoResizeColRow
 *
 ***************************************************************************/


UndoResizeColRow::UndoResizeColRow( Doc *_doc, Sheet *_sheet, const Region &_selection ) :
    UndoAction( _doc )
{
  name=i18n("Resize");
  m_region = _selection;
  m_sheetName = _sheet->sheetName();

  createList( m_lstColumn,m_lstRow, _sheet );
}

void UndoResizeColRow::createList( QValueList<columnSize> &listCol,QValueList<rowSize> &listRow, Sheet* sheet )
{
    listCol.clear();
    listRow.clear();
    Region::ConstIterator endOfList(m_region.constEnd());
    for (Region::ConstIterator it = m_region.constBegin(); it != endOfList; ++it)
    {
      QRect m_rctRect = (*it)->rect().normalize();

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
}

UndoResizeColRow::~UndoResizeColRow()
{
}

void UndoResizeColRow::undo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();

    createList( m_lstRedoColumn,m_lstRedoRow, sheet );

    Region::ConstIterator endOfList(m_region.constEnd());
    for (Region::ConstIterator it = m_region.constBegin(); it != endOfList; ++it)
    {
      QRect m_rctRect = (*it)->rect().normalize();

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
    }

    doc()->undoUnlock();
}

void UndoResizeColRow::redo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();

    Region::ConstIterator endOfList(m_region.constEnd());
    for (Region::ConstIterator it = m_region.constBegin(); it != endOfList; ++it)
    {
      QRect m_rctRect = (*it)->rect().normalize();

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
    }

    doc()->undoUnlock();
}

/****************************************************************************
 *
 * UndoChangeAreaTextCell
 *
 ***************************************************************************/


UndoChangeAreaTextCell::UndoChangeAreaTextCell( Doc *_doc, Sheet *_sheet, const Region &_selection ) :
    UndoAction( _doc )
{
  name=i18n("Change Text");

  m_region = _selection;
  m_sheetName = _sheet->sheetName();

  createList( m_lstTextCell, _sheet );
}

void UndoChangeAreaTextCell::createList( QMap<QPoint,QString> &map, Sheet* sheet )
{
  map.clear();

  Region::ConstIterator endOfList(m_region.constEnd());
  for (Region::ConstIterator it = m_region.constBegin(); it != endOfList; ++it)
  {
    QRect m_rctRect = (*it)->rect().normalize();
    int bottom = m_rctRect.bottom();
    int right  = m_rctRect.right();

    if( util_isColumnSelected( m_rctRect ) )
    {
      Cell * c;
      for ( int col = m_rctRect.left(); col <= right; ++col )
      {
        c = sheet->getFirstCellColumn( col );
        while ( c )
        {
          if ( !c->isPartOfMerged() )
          {
            //textOfCell tmpText;
            //tmpText.col = col;
            //tmpText.row = c->row();
            //tmpText.text = c->text();
            map.insert( QPoint(col,c->row()) , c->text() );
          }
          c = sheet->getNextCellDown( col, c->row() );
        }
      }
    }
    else if ( util_isRowSelected( m_rctRect ) )
    {
      Cell * c;
      for ( int row = m_rctRect.top(); row <= bottom; ++row )
      {
        c = sheet->getFirstCellRow( row );
        while ( c )
        {
          if ( !c->isPartOfMerged() )
          {
            //textOfCell tmpText;
            //tmpText.col = c->column();
            //tmpText.row = row;
            //tmpText.text = c->text();
            map.insert( QPoint(c->column(),row) , c->text() );
          }
          c = sheet->getNextCellRight( c->column(), row );
        }
      }
    }
    else
    {
      Cell * cell;
      for ( int x = m_rctRect.left(); x <= right; ++x )
      {
        cell = sheet->getFirstCellColumn( x );
        if ( !cell )
          continue;
        while ( cell && cell->row() <= bottom )
        {
          if ( !cell->isObscured() )
          {
            //textOfCell tmpText;
            //tmpText.col  = x;
            //tmpText.row  = cell->row();
            //tmpText.text = cell->text();
            map.insert( QPoint(x,cell->row()) , cell->text());
          }
          cell = sheet->getNextCellDown( x, cell->row() );
        }
      }
    }
  }
}

UndoChangeAreaTextCell::~UndoChangeAreaTextCell()
{
}

void UndoChangeAreaTextCell::undo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    doc()->emitBeginOperation();
    sheet->setRegionPaintDirty(m_region);
    
    kdDebug() << "creating redo list..." << endl; 
    createList( m_lstRedoTextCell, sheet );
    kdDebug() << "created redo list..." << endl;

    Region::ConstIterator endOfList(m_region.constEnd());
    for (Region::ConstIterator it = m_region.constBegin(); it != endOfList; ++it)
    {
      QRect m_rctRect = (*it)->rect().normalize();

    if ( !util_isRowSelected( m_rctRect )
         && !util_isColumnSelected( m_rctRect ) )
    {
      for ( int x = m_rctRect.left(); x <= m_rctRect.right(); ++x )
        for ( int y = m_rctRect.top(); y <= m_rctRect.bottom(); ++y )
        {
          Cell* cell = sheet->nonDefaultCell( x, y );

	  const QPoint location(x,y);

	  if ( m_lstTextCell.contains(location) )
		cell->setCellText( m_lstTextCell[location] );
	  else
		cell->setCellText( "",true );
	  
          /*bool found = false;
          QValueList<textOfCell>::Iterator it;
          for( it = m_lstTextCell.begin(); it != m_lstTextCell.end(); ++it )
            if ( (*it).col == x && (*it).row == y && !found )
            {
              cell->setCellText( (*it).text );
              found = true;
            }
          if( !found )
            cell->setCellText( "", true );*/
        }

    }
    else
    {
      QMap<QPoint,QString>::Iterator it2;
      for ( it2 = m_lstTextCell.begin(); it2 != m_lstTextCell.end(); ++it2 )
      {
        Cell *cell = sheet->nonDefaultCell( it2.key().x(), it2.key().y() );
        if ( it2.data().isEmpty() )
        {
          if ( !cell->text().isEmpty() )
            cell->setCellText( "" );
        }
        else
          cell->setCellText( it2.data() );
      }
    }
    }

    //sheet->updateView();
    doc()->emitEndOperation();
    doc()->undoUnlock();
}

void UndoChangeAreaTextCell::redo()
{
    Sheet * sheet = doc()->map()->findSheet( m_sheetName );

    if ( !sheet )
	return;

    doc()->undoLock();
    doc()->emitBeginOperation();
    sheet->setRegionPaintDirty(m_region);

    Region::ConstIterator endOfList(m_region.constEnd());
    for (Region::ConstIterator it = m_region.constBegin(); it != endOfList; ++it)
    {
      QRect m_rctRect = (*it)->rect().normalize();

    if ( !util_isRowSelected( m_rctRect )
         && !util_isColumnSelected( m_rctRect ) )
    {
      for ( int x = m_rctRect.left(); x <= m_rctRect.right(); ++x )
        for ( int y = m_rctRect.top(); y <= m_rctRect.bottom(); ++y )
        {
          Cell* cell = sheet->nonDefaultCell( x, y );

	  const QPoint location(x,y);
	  
	  if (m_lstRedoTextCell.contains(location))
		  cell->setCellText( m_lstRedoTextCell[location] );
	  else
		  cell->setCellText( "" , true );
          /*bool found = false;
          QValueList<textOfCell>::Iterator it;
          for( it = m_lstRedoTextCell.begin(); it != m_lstRedoTextCell.end(); ++it )
            if ( (*it).col == x && (*it).row == y && !found )
            {
              cell->setCellText( (*it).text );
              found = true;
            }
          if( !found )
            cell->setCellText( "", true );*/
        }

    }
    else
    {
      QMap<QPoint,QString>::Iterator it2;
      for ( it2 = m_lstRedoTextCell.begin(); it2 != m_lstRedoTextCell.end(); ++it2 )
      {
        Cell *cell = sheet->nonDefaultCell( it2.key().x(), it2.key().y() );
        if ( it2.data().isEmpty() )
        {
          if ( !cell->text().isEmpty() )
            cell->setCellText( "" );
        }
        else
          cell->setCellText( it2.data() );
      }
    }
    }

    //sheet->updateView();
    doc()->emitEndOperation();
    doc()->undoUnlock();
}

/****************************************************************************
 *
 * UndoMergedCell
 *
 ***************************************************************************/


UndoMergedCell::UndoMergedCell( Doc *_doc, Sheet *_sheet, int _column, int _row , int _extraX,int _extraY) :
    UndoAction( _doc )
{
  name=i18n("Merge Cells");

  m_sheetName = _sheet->sheetName();
  m_iRow=_row;
  m_iCol=_column;
  m_iExtraX=_extraX;
  m_iExtraY=_extraY;

}

UndoMergedCell::~UndoMergedCell()
{
}

void UndoMergedCell::undo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();

    Cell *cell = sheet->nonDefaultCell( m_iCol, m_iRow );
    m_iExtraRedoX=cell->extraXCells();
    m_iExtraRedoY=cell->extraYCells();

    sheet->changeMergedCell( m_iCol, m_iRow, m_iExtraX,m_iExtraY);

    doc()->undoUnlock();
}

void UndoMergedCell::redo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();

    sheet->changeMergedCell( m_iCol, m_iRow, m_iExtraRedoX,m_iExtraRedoY);

    doc()->undoUnlock();
}

/****************************************************************************
 *
 * UndoAutofill
 *
 ***************************************************************************/

UndoAutofill::UndoAutofill( Doc *_doc, Sheet* sheet, const QRect & _selection)
    : UndoAction( _doc )
{
    name=i18n("Autofill");

    m_sheetName = sheet->sheetName();
    m_selection = _selection;
    createListCell( m_data, sheet );

}

UndoAutofill::~UndoAutofill()
{
}

void UndoAutofill::createListCell( QCString &list, Sheet* sheet )
{
    QDomDocument doc = sheet->saveCellRegion( m_selection );
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

void UndoAutofill::undo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    createListCell( m_dataRedo, sheet );

    doc()->undoLock();
    doc()->emitBeginOperation();

    sheet->deleteCells( m_selection );
    sheet->paste( m_data, m_selection );
    //if(sheet->getAutoCalc()) sheet->recalc();

    doc()->emitEndOperation();
    //sheet->updateView();

    doc()->undoUnlock();
}

void UndoAutofill::redo()
{
    doc()->undoLock();

    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
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
 * UndoInsertCellRow
 *
 ***************************************************************************/

UndoInsertCellRow::UndoInsertCellRow( Doc *_doc, Sheet *_sheet, const QRect &_rect ) :
    UndoInsertRemoveAction( _doc )
{
    name=i18n("Insert Cell");

    m_sheetName = _sheet->sheetName();
    m_rect=_rect;
}

UndoInsertCellRow::~UndoInsertCellRow()
{
}

void UndoInsertCellRow::undo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->unshiftRow( m_rect);
    doc()->undoUnlock();

    undoFormulaReference();
}

void UndoInsertCellRow::redo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->shiftRow( m_rect);
    doc()->undoUnlock();
}

/****************************************************************************
 *
 * UndoInsertCellCol
 *
 ***************************************************************************/


UndoInsertCellCol::UndoInsertCellCol( Doc *_doc, Sheet *_sheet, const QRect &_rect ) :
    UndoInsertRemoveAction( _doc )
{
    name=i18n("Insert Cell");

    m_sheetName = _sheet->sheetName();
    m_rect=_rect;
}

UndoInsertCellCol::~UndoInsertCellCol()
{
}

void UndoInsertCellCol::undo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->unshiftColumn( m_rect);
    doc()->undoUnlock();

    undoFormulaReference();
}

void UndoInsertCellCol::redo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->shiftColumn( m_rect );
    doc()->undoUnlock();
}

/****************************************************************************
 *
 * UndoRemoveCellRow
 *
 ***************************************************************************/

UndoRemoveCellRow::UndoRemoveCellRow( Doc *_doc, Sheet *_sheet, const QRect &rect ) :
    UndoInsertRemoveAction( _doc )
{
    name=i18n("Remove Cell");

    m_sheetName = _sheet->sheetName();
    m_rect=rect;
    QDomDocument doc = _sheet->saveCellRegion( m_rect );
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

UndoRemoveCellRow::~UndoRemoveCellRow()
{
}

void UndoRemoveCellRow::undo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->shiftRow( m_rect );
    sheet->paste( m_data, m_rect );
    doc()->undoUnlock();

    undoFormulaReference();
}

void UndoRemoveCellRow::redo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->unshiftRow( m_rect);
    doc()->undoUnlock();
}

/****************************************************************************
 *
 * UndoRemoveCellCol
 *
 ***************************************************************************/

UndoRemoveCellCol::UndoRemoveCellCol( Doc *_doc, Sheet *_sheet, const QRect &_rect ) :
    UndoInsertRemoveAction( _doc )
{
    name=i18n("Remove Cell");

    m_sheetName = _sheet->sheetName();
    m_rect=_rect;
    QDomDocument doc = _sheet->saveCellRegion( m_rect );
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

UndoRemoveCellCol::~UndoRemoveCellCol()
{
}

void UndoRemoveCellCol::undo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->shiftColumn( m_rect );
    sheet->paste( m_data, m_rect );
    doc()->undoUnlock();

    undoFormulaReference();
}

void UndoRemoveCellCol::redo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->unshiftColumn( m_rect );
    doc()->undoUnlock();
}

/****************************************************************************
 *
 * UndoConditional
 *
 ***************************************************************************/

UndoConditional::UndoConditional( Doc *_doc, Sheet* sheet, const Region & _selection)
    : UndoAction( _doc )
{
    name=i18n("Conditional Cell Attribute");

    m_sheetName = sheet->sheetName();
    m_region = _selection;
    createListCell( m_data, sheet );

}

UndoConditional::~UndoConditional()
{
}

void UndoConditional::createListCell( QCString &list, Sheet* sheet )
{
    // Save to buffer
    QString buffer;
    QTextStream str( &buffer, IO_WriteOnly );

    QDomDocument doc = sheet->saveCellRegion( m_region );
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

void UndoConditional::undo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    createListCell( m_dataRedo, sheet );

    doc()->undoLock();
    sheet->paste(m_data, m_region.boundingRect());
    if (sheet->getAutoCalc())
    {
      sheet->recalc();
    }

    doc()->undoUnlock();
}

void UndoConditional::redo()
{
    doc()->undoLock();

    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
    if ( !sheet )
	return;

    doc()->undoLock();
    sheet->paste(m_dataRedo, m_region.boundingRect());
    if (sheet->getAutoCalc())
    {
      sheet->recalc();
    }

    doc()->undoUnlock();
}


/****************************************************************************
 *
 * UndoCellPaste
 *
 ***************************************************************************/

UndoCellPaste::UndoCellPaste(Doc *_doc, Sheet* sheet,
                             int _xshift, int _yshift,
                             const Region& region, bool insert, int _insertTo)
    : UndoAction( _doc )
{
    if(!insert)
        name=i18n("Paste");
    else
        name=i18n("Paste & Insert");

    m_sheetName = sheet->sheetName();
    m_region = region;
    xshift=_xshift;
    yshift=_yshift;
    b_insert=insert;
    m_iInsertTo=_insertTo;
    if( !b_insert)
        createListCell( m_data, m_lstColumn,m_lstRow,sheet );

}

UndoCellPaste::~UndoCellPaste()
{
}

void UndoCellPaste::createListCell(QCString& listCell,
                                   QValueList<columnSize>& listCol,
                                   QValueList<rowSize>& listRow,
                                   Sheet* sheet)
{
  listCol.clear();
  listRow.clear();

  Region::ConstIterator endOfList = m_region.constEnd();
  for (Region::ConstIterator it = m_region.constBegin(); it != endOfList; ++it)
  {
    int nbCol = 0;
    int nbRow = 0;
    QRect range = (*it)->rect().normalize();
    if ((*it)->isColumn())
    {
      nbCol = range.width();
    }
    else if ((*it)->isRow())
    {
      nbRow = range.height();
    }

    // copy column(s)
    if (nbCol != 0)
    {
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
    else if (nbRow != 0)
    {
      //save size of columns
      for ( int y = 1; y <=nbRow ; ++y )
      {
        RowFormat *rw=sheet->rowFormat(y);
        if (!rw->isDefault())
        {
          rowSize tmpSize;
          tmpSize.rowNumber=y;
          tmpSize.rowHeight=rw->dblHeight();
          listRow.append(tmpSize);
        }
      }
    }
  }
  //save all cells in area
  QDomDocument doc = sheet->saveCellRegion(m_region);
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

void UndoCellPaste::undo()
{
  Sheet* sheet = doc()->map()->findSheet( m_sheetName );
  if ( !sheet )
      return;

  createListCell( m_dataRedo, m_lstRedoColumn, m_lstRedoRow, sheet );

  doc()->undoLock();
  doc()->emitBeginOperation();

  uint numCols = 0;
  uint numRows = 0;

  Region::ConstIterator endOfList = m_region.constEnd();
  for (Region::ConstIterator it = --m_region.constEnd(); it != endOfList; --it)
  {
    QRect range = (*it)->rect().normalize();

    if ((*it)->isColumn())
    {
      yshift = 0;
      uint col = range.left();
      uint width = range.width();
      if (!b_insert)
      {
        QValueList<columnSize>::Iterator it2;
        for ( it2 = m_lstColumn.begin(); it2 != m_lstColumn.end(); ++it2 )
        {
          ColumnFormat *cl=sheet->nonDefaultColumnFormat((*it2).columnNumber);
          cl->setDblWidth((*it2).columnWidth);
        }
      }
      else
      {
        numCols += width;
        sheet->removeColumn(col, width - 1, false);
      }
    }
    else if ((*it)->isRow())
    {
      xshift = 0;
      uint row = range.top();
      uint height = range.height();
      if (!b_insert)
      {
        QValueList<rowSize>::Iterator it2;
        for ( it2 = m_lstRow.begin(); it2 != m_lstRow.end(); ++it2 )
        {
          RowFormat *rw=sheet->nonDefaultRowFormat((*it2).rowNumber);
          rw->setDblHeight((*it2).rowHeight);
        }
      }
      else
      {
        numRows += height;
        sheet->removeRow(row, height - 1);
      }
    }

    if (!b_insert)
    {
      sheet->deleteCells(range);
    }
  } // for (Region::...

  if (b_insert) // with insertion
  {
    QRect rect = m_region.boundingRect();
    if (m_iInsertTo == -1 || (m_iInsertTo == 0 && numRows == 0))
    {
      // substract already removed columns
      rect.setWidth(rect.width() - numCols);
      sheet->unshiftRow(rect);
    }
    else if (m_iInsertTo == 1 || (m_iInsertTo == 0 && numCols == 0))
    {
      // substract already removed rows
      rect.setHeight(rect.height() - numRows);
      sheet->unshiftColumn(rect);
    }
  }
  else // without insertion
  {
    sheet->paste(m_data, m_region.boundingRect());
  }

  if (sheet->getAutoCalc())
  {
      sheet->recalc();
  }
  sheet->updateView();
  doc()->undoUnlock();
}

void UndoCellPaste::redo()
{
  Sheet* sheet = doc()->map()->findSheet( m_sheetName );
  if ( !sheet )
      return;

  doc()->undoLock();
  doc()->emitBeginOperation();

  uint numCols = 0;
  uint numRows = 0;

  Region::ConstIterator endOfList = m_region.constEnd();
  for (Region::ConstIterator it = m_region.constBegin(); it != endOfList; ++it)
  {
    QRect range = (*it)->rect().normalize();

    if ((*it)->isColumn())
    {
      int width = range.width();
      if (b_insert)
      {
        numCols += width;
        sheet->insertColumn(xshift + 1, width - 1, false);
      }
      QValueList<columnSize>::Iterator it2;
      for ( it2 = m_lstRedoColumn.begin(); it2 != m_lstRedoColumn.end(); ++it2 )
      {
        ColumnFormat *cl=sheet->nonDefaultColumnFormat((*it2).columnNumber);
        cl->setDblWidth((*it2).columnWidth);
      }
    }
    else if ((*it)->isRow())
    {
      int height = range.height();
      if (b_insert)
      {
        numRows += height;
        sheet->insertRow(yshift + 1, height - 1);
      }
      QValueList<rowSize>::Iterator it2;
      for ( it2 = m_lstRedoRow.begin(); it2 != m_lstRedoRow.end(); ++it2 )
      {
        RowFormat *rw=sheet->nonDefaultRowFormat((*it2).rowNumber);
        rw->setDblHeight((*it2).rowHeight);
      }
    }
    else
    {
      if (!b_insert)
      {
        sheet->deleteCells( range );
      }
    }
  } // for (Region::...

  if (b_insert)
  {
    QRect rect = m_region.boundingRect();
    if (m_iInsertTo == -1 || (m_iInsertTo == 0 && numRows == 0))
    {
      rect.setWidth(rect.width() - numCols);
      sheet->shiftRow(rect);
    }
    else if (m_iInsertTo == 1 || (m_iInsertTo == 0 && numCols == 0))
    {
      rect.setHeight(rect.height() - numRows);
      sheet->shiftColumn(rect);
    }
  }

  sheet->paste( m_dataRedo, m_region.boundingRect() );

  if (sheet->getAutoCalc())
  {
    sheet->recalc();
  }

  sheet->updateView();
  doc()->undoUnlock();
}


/****************************************************************************
 *
 * UndoStyleCell
 *
 ***************************************************************************/

UndoStyleCell::UndoStyleCell( Doc *_doc, Sheet* sheet, const QRect & _selection)
    : UndoAction( _doc )
{
    name=i18n("Style of Cell");

    m_sheetName = sheet->sheetName();
    m_selection = _selection;
    createListCell( m_lstStyleCell, sheet );

}

UndoStyleCell::~UndoStyleCell()
{
}

void UndoStyleCell::createListCell( QValueList<styleCell> &listCell, Sheet* sheet )
{
  int bottom = m_selection.bottom();
  int right  = m_selection.right();
  if ( util_isColumnSelected( m_selection ) )
  {
    Cell * c;
    for ( int col = m_selection.left(); col <= right; ++ col )
    {
      c = sheet->getFirstCellColumn( col );
      while ( c )
      {
        if ( !c->isPartOfMerged() )
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
    Cell * c;
    for ( int row = m_selection.top(); row <= bottom; ++row )
    {
      c = sheet->getFirstCellRow( row );
      while ( c )
      {
        if ( !c->isPartOfMerged() )
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
    Cell * cell;
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

void UndoStyleCell::undo()
{
    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
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

void UndoStyleCell::redo()
{
    doc()->undoLock();

    Sheet* sheet = doc()->map()->findSheet( m_sheetName );
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

UndoInsertData::UndoInsertData( Doc * _doc, Sheet * _sheet, QRect & _selection )
    : UndoChangeAreaTextCell( _doc, _sheet, _selection )
{
    name = i18n("Insert Data From Database");
}
