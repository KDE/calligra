/* This file is part of the KDE project
   Copyright (C) 2002 Montel Laurent <lmontel@mandrakesoft.com>

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

#include "kspread_dlg_paperlayout.h"
#include <kspread_doc.h>
#include "kspread_map.h"
#include "kspread_table.h"
#include <kspread_undo.h>
#include <kspread_util.h>
#include <kspread_view.h>

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>

#include <kapplication.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>

KSpreadPaperLayout::KSpreadPaperLayout( QWidget * parent, const char * name,
                                        const KoPageLayout & layout,
                                        const KoHeadFoot & headfoot,
                                        int tabs, KoUnit::Unit unit,
                                        KSpreadSheet * sheet, KSpreadView * view)
  : KoPageLayoutDia( parent, name, layout, headfoot, tabs, unit, false /*no modal*/),
    m_pSheet( sheet ),
    m_pView( view )
{
    initTab();
    connect( view, SIGNAL( sig_selectionChanged( KSpreadSheet *, const QRect & ) ),
             this, SLOT( slotSelectionChanged( KSpreadSheet *, const QRect & ) ) );
    qApp->installEventFilter( this );
    m_focus= 0L;
}

void KSpreadPaperLayout::initTab()
{
    QWidget *tab = addPage(i18n( "Options" ));
    QGridLayout *grid = new QGridLayout( tab, 8, 2, KDialog::marginHint(), KDialog::spacingHint() );

    pApplyToAll = new QCheckBox ( i18n( "&Apply to all sheets" ), tab );
    pApplyToAll->setChecked( m_pSheet->getPrintGrid() );
    grid->addWidget( pApplyToAll, 0, 0 );

    pPrintGrid = new QCheckBox ( i18n("Print &grid"), tab );
    pPrintGrid->setChecked( m_pSheet->getPrintGrid() );
    grid->addWidget( pPrintGrid, 1, 0 );

    pPrintCommentIndicator = new QCheckBox ( i18n("Print &comment indicator"), tab );
    pPrintCommentIndicator->setChecked( m_pSheet->getPrintCommentIndicator() );
    grid->addWidget( pPrintCommentIndicator, 2, 0 );

    pPrintFormulaIndicator = new QCheckBox ( i18n("Print &formula indicator"), tab );
    pPrintFormulaIndicator->setChecked( m_pSheet->getPrintFormulaIndicator() );
    grid->addWidget( pPrintFormulaIndicator, 3, 0 );

    QLabel *pPrintRange = new QLabel ( i18n("Print range:"), tab );
    grid->addWidget( pPrintRange, 4, 0 );

    ePrintRange = new QLineEdit( tab );
    grid->addWidget( ePrintRange, 4, 1 );
    ePrintRange->setText( util_rangeName( m_pSheet->printRange() ) );

    QLabel *pRepeatCols = new QLabel ( i18n("Repeat columns on each page:"), tab );
    grid->addWidget( pRepeatCols, 5, 0 );

    eRepeatCols = new QLineEdit( tab );
    grid->addWidget( eRepeatCols, 5, 1 );
    if ( m_pSheet->printRepeatColumns().first != 0 )
        eRepeatCols->setText( util_encodeColumnLabelText( m_pSheet->printRepeatColumns().first ) +
                              ":" +
                              util_encodeColumnLabelText( m_pSheet->printRepeatColumns().second ) );

    QLabel *pRepeatRows = new QLabel ( i18n("Repeat rows on each page:"), tab );
    grid->addWidget( pRepeatRows, 6, 0 );

    eRepeatRows = new QLineEdit( tab );
    grid->addWidget( eRepeatRows, 6, 1 );
    if ( m_pSheet->printRepeatRows().first != 0 )
        eRepeatRows->setText( QString().setNum( m_pSheet->printRepeatRows().first ) +
                              ":" +
                              QString().setNum( m_pSheet->printRepeatRows().second ) );

    // --------------- main grid ------------------
    grid->addColSpacing( 0, pApplyToAll->width() );
    grid->addColSpacing( 0, pPrintGrid->width() );
    grid->addColSpacing( 0, pPrintCommentIndicator->width() );
    grid->addColSpacing( 0, pPrintFormulaIndicator->width() );
    grid->addColSpacing( 0, pPrintRange->width() );
    grid->addColSpacing( 0, pRepeatRows->width() );
    grid->addColSpacing( 0, pRepeatCols->width() );
    grid->addColSpacing( 1, ePrintRange->width() );
    grid->addColSpacing( 1, eRepeatRows->width() );
    grid->addColSpacing( 1, eRepeatCols->width() );

    grid->addRowSpacing( 0, pApplyToAll->height() );
    grid->addRowSpacing( 1, pPrintGrid->height() );
    grid->addRowSpacing( 2, pPrintCommentIndicator->height() );
    grid->addRowSpacing( 3, pPrintFormulaIndicator->height() );
    grid->addRowSpacing( 4, pPrintRange->height() );
    grid->addRowSpacing( 4, ePrintRange->height() );
    grid->addRowSpacing( 5, pRepeatRows->height() );
    grid->addRowSpacing( 5, eRepeatRows->height() );
    grid->addRowSpacing( 6, pRepeatCols->height() );
    grid->addRowSpacing( 6, eRepeatCols->height() );
    grid->setRowStretch( 7, 1 );

}

void KSpreadPaperLayout::slotOk()
{
    if ( !m_pSheet->doc()->undoBuffer()->isLocked() )
    {
        KSpreadUndoAction* undo = new KSpreadUndoPaperLayout( m_pSheet->doc(), m_pSheet );
        m_pSheet->doc()->undoBuffer()->appendUndo( undo );
    }

    // get new values for borders
    leftChanged();
    rightChanged();
    topChanged();
    bottomChanged();

    KSpreadMap   * map   = 0;
    KSpreadSheet * sheet = 0;

    if ( pApplyToAll->isChecked() )
      map = m_pSheet->doc()->map();

    if ( map )
      sheet = map->firstTable();
    else
      sheet = m_pSheet;

    while ( sheet )
    {
      KoPageLayout pl = getLayout();
      KoHeadFoot hf = getHeadFoot();
      KoUnit::Unit unit = sheet->doc()->getUnit();
      sheet->setPrintGrid( pPrintGrid->isChecked() );
      sheet->setPrintCommentIndicator( pPrintCommentIndicator->isChecked() );
      sheet->setPrintFormulaIndicator( pPrintFormulaIndicator->isChecked() );
      QString tmpPrintRange = ePrintRange->text();
      QString tmpRepeatCols = eRepeatCols->text();
      QString tmpRepeatRows = eRepeatRows->text();
      if ( tmpPrintRange.isEmpty() )
      {
        sheet->setPrintRange( QRect( QPoint( 1, 1 ), QPoint( KS_colMax, KS_rowMax ) ) );
      }
      else
      {
        bool error = true;
        int first = tmpPrintRange.find(":");
        if ( ( first != -1 ) && ( (int)tmpPrintRange.length() > first ) )
        {
            KSpreadPoint point1 ( tmpPrintRange.left( first ) );
            if ( point1.isValid() )
            {
                KSpreadPoint point2 ( tmpPrintRange.mid( first+1 ) );
                if ( point2.isValid() )
                {
                    error = false;
                    sheet->setPrintRange ( QRect( QPoint( QMIN( point1.pos.x(), point2.pos.x() ),
                                                          QMIN( point1.pos.y(), point2.pos.y() ) ),
                                                  QPoint( QMAX( point1.pos.x(), point2.pos.x() ),
                                                          QMAX( point1.pos.y(), point2.pos.y() ) ) ) );
                }
            }
        }

        if ( error ) KMessageBox::information( 0, i18n( "Print range wrong, changes are ignored." ) );
      }
      
      if ( tmpRepeatCols.isEmpty() )
      {
        sheet->setPrintRepeatColumns( qMakePair( 0, 0 ) );
      }
      else
      {
        bool error = true;
        int first = tmpRepeatCols.find(":");
        if ( ( first != -1 ) && ( (int)tmpRepeatCols.length() > first ) )
        {
            int col1 = util_decodeColumnLabelText( tmpRepeatCols.left( first ) );
            if ( col1 > 0 && col1 <= KS_colMax )
            {
                int col2 = util_decodeColumnLabelText( tmpRepeatCols.mid( first+1 ) );
                if ( col2 > 0 && col2 <= KS_colMax )
                {
                    error = false;
                    sheet->setPrintRepeatColumns ( qMakePair( col1, col2 ) );
                }
            }
        }

        if ( error )
          KMessageBox::information( 0, i18n( "Repeated columns range wrong, changes are ignored.\nMust be in format column:column (eg. B:C)" ) );
      }
      
      if ( tmpRepeatRows.isEmpty() )
      {
        sheet->setPrintRepeatRows ( qMakePair( 0, 0 ) );
      }
      else
      {
        bool error = true;
        int first = tmpRepeatRows.find(":");
        if ( ( first != -1 ) && ( (int)tmpRepeatRows.length() > first ) )
        {
            int row1 = tmpRepeatRows.left( first ).toInt();
            if ( row1 > 0 && row1 <= KS_rowMax )
            {
                int row2 = tmpRepeatRows.mid( first+1 ).toInt();
                if ( row2 > 0 && row2 <= KS_rowMax )
                {
                    error = false;
                    sheet->setPrintRepeatRows ( qMakePair( row1, row2 ) );
                }
            }
        }
        
        if ( error )
          KMessageBox::information( 0, i18n( "Repeated rows range wrong, changes are ignored.\nMust be in format row:row (eg. 2:3)" ) );
      }
      sheet->doc()->setModified( true );
      
      if ( pl.format == PG_CUSTOM )
      {
        sheet->setPaperWidth( qRound( POINT_TO_MM( pl.ptWidth ) *1000 ) / 1000 );
        sheet->setPaperHeight( qRound( POINT_TO_MM( pl.ptHeight ) *1000 ) / 1000 );
      }
      
      sheet->setPaperLayout( POINT_TO_MM(pl.ptLeft), POINT_TO_MM(pl.ptTop), POINT_TO_MM(pl.ptRight), POINT_TO_MM(pl.ptBottom), pl.format, pl.orientation );
      
      sheet->setHeadFootLine( sheet->delocalizeHeadFootLine( hf.headLeft  ),
                              sheet->delocalizeHeadFootLine( hf.headMid   ),
                              sheet->delocalizeHeadFootLine( hf.headRight ),
                              sheet->delocalizeHeadFootLine( hf.footLeft  ),
                              sheet->delocalizeHeadFootLine( hf.footMid   ),
                              sheet->delocalizeHeadFootLine( hf.footRight ) );
      
      sheet->doc()->setUnit( unit );

      if ( map )
        sheet = map->nextTable();
      else
        sheet = 0;
    }

    accept();
    delete this;
}

void KSpreadPaperLayout::closeEvent ( QCloseEvent * )
{
    delete this;
}

void KSpreadPaperLayout::slotSelectionChanged( KSpreadSheet* /*_table*/, const QRect& _selection )
{
  if ( _selection.left() == 0 || _selection.top() == 0 ||
       _selection.right() == 0 || _selection.bottom() == 0 )
    return;

  QString area = util_rangeName( _selection );
  if ( m_focus )
  {
      if ( m_focus == ePrintRange )
          area = util_rangeName( _selection );
      else if ( m_focus == eRepeatRows )
          area = util_rangeRowName( _selection );
      else if ( m_focus == eRepeatCols )
          area = util_rangeColumnName( _selection );
      else
          return;
      m_focus->setText( area );
  }
}

void KSpreadPaperLayout::slotCancel()
{
  reject();
  delete this;
}

bool KSpreadPaperLayout::eventFilter( QObject* obj, QEvent* ev )
{
    if ( obj == ePrintRange && ev->type() == QEvent::FocusIn )
        m_focus = ePrintRange;
    else if ( obj == eRepeatCols && ev->type() == QEvent::FocusIn )
        m_focus = eRepeatCols;
    else if ( obj == eRepeatRows && ev->type() == QEvent::FocusIn )
        m_focus = eRepeatRows;
    else
        return false;

    return false;
}


#include "kspread_dlg_paperlayout.moc"
