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
#include "kspread_sheet.h"
#include "kspread_sheetprint.h"
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
    KSpreadSheetPrint* print = m_pSheet->print();

    QWidget *tab = addPage(i18n( "Options" ));
    QGridLayout *grid = new QGridLayout( tab, 9, 2, KDialog::marginHint(), KDialog::spacingHint() );

    pApplyToAll = new QCheckBox ( i18n( "&Apply to all sheets" ), tab );
    pApplyToAll->setChecked( print->printGrid() );
    grid->addWidget( pApplyToAll, 0, 0 );

    pPrintGrid = new QCheckBox ( i18n("Print &grid"), tab );
    pPrintGrid->setChecked( print->printGrid() );
    grid->addWidget( pPrintGrid, 1, 0 );

    pPrintCommentIndicator = new QCheckBox ( i18n("Print &comment indicator"), tab );
    pPrintCommentIndicator->setChecked( print->printCommentIndicator() );
    grid->addWidget( pPrintCommentIndicator, 2, 0 );

    pPrintFormulaIndicator = new QCheckBox ( i18n("Print &formula indicator"), tab );
    pPrintFormulaIndicator->setChecked( print->printFormulaIndicator() );
    grid->addWidget( pPrintFormulaIndicator, 3, 0 );

    QLabel *pPrintRange = new QLabel ( i18n("Print range:"), tab );
    grid->addWidget( pPrintRange, 4, 0 );

    ePrintRange = new QLineEdit( tab );
    grid->addWidget( ePrintRange, 4, 1 );
    ePrintRange->setText( util_rangeName( print->printRange() ) );

    QLabel *pRepeatCols = new QLabel ( i18n("Repeat columns on each page:"), tab );
    grid->addWidget( pRepeatCols, 5, 0 );

    eRepeatCols = new QLineEdit( tab );
    grid->addWidget( eRepeatCols, 5, 1 );
    if ( print->printRepeatColumns().first != 0 )
        eRepeatCols->setText( util_encodeColumnLabelText( print->printRepeatColumns().first ) +
                              ":" +
                              util_encodeColumnLabelText( print->printRepeatColumns().second ) );

    QLabel *pRepeatRows = new QLabel ( i18n("Repeat rows on each page:"), tab );
    grid->addWidget( pRepeatRows, 6, 0 );

    eRepeatRows = new QLineEdit( tab );
    grid->addWidget( eRepeatRows, 6, 1 );
    if ( print->printRepeatRows().first != 0 )
        eRepeatRows->setText( QString().setNum( print->printRepeatRows().first ) +
                              ":" +
                              QString().setNum( print->printRepeatRows().second ) );

    QLabel *pZoom = new QLabel ( i18n("Zoom printout:"), tab );
    grid->addWidget( pZoom, 7, 0 );

    m_cZoom = new QComboBox( true, tab, "Zoom" );
    grid->addWidget( m_cZoom, 7, 1 );

    QStringList lst;
    lst.append("");
    for( int i = 5; i < 500; i += 5 )
    {
        lst.append( QString( "%1" ).arg( i ) );
        if( qRound( print->zoom() * 100 ) > i &&
            qRound( print->zoom() * 100 ) < i + 5 )
        {
            lst.append( QString( "%1" ).arg( print->zoom() * 100 ) );
        }
    }

    m_cZoom->insertStringList( lst );

    int number_of_entries = m_cZoom->count();
    QString string;
    string.setNum( qRound( print->zoom() * 100 ) );

    for (int i = 0; i < number_of_entries ; i++)
    {
        if ( string == (QString) m_cZoom->text(i) )
        {
            m_cZoom->setCurrentItem( i );
            break;
        }
    }


    // --------------- main grid ------------------
    grid->addColSpacing( 0, pApplyToAll->width() );
    grid->addColSpacing( 0, pPrintGrid->width() );
    grid->addColSpacing( 0, pPrintCommentIndicator->width() );
    grid->addColSpacing( 0, pPrintFormulaIndicator->width() );
    grid->addColSpacing( 0, pPrintRange->width() );
    grid->addColSpacing( 0, pRepeatRows->width() );
    grid->addColSpacing( 0, pRepeatCols->width() );
    grid->addColSpacing( 0, pZoom->width() );
    grid->addColSpacing( 1, ePrintRange->width() );
    grid->addColSpacing( 1, eRepeatRows->width() );
    grid->addColSpacing( 1, eRepeatCols->width() );
    grid->addColSpacing( 1, m_cZoom->width() );

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
    grid->addRowSpacing( 7, pZoom->height() );
    grid->addRowSpacing( 7, m_cZoom->height() );
    grid->setRowStretch( 8, 1 );

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

    m_pView->doc()->emitBeginOperation( false );
    while ( sheet )
    {
      KSpreadSheetPrint *print = sheet->print();

      KoPageLayout pl = getLayout();
      KoHeadFoot hf = getHeadFoot();
      KoUnit::Unit unit = sheet->doc()->getUnit();
      print->setPrintGrid( pPrintGrid->isChecked() );
      print->setPrintCommentIndicator( pPrintCommentIndicator->isChecked() );
      print->setPrintFormulaIndicator( pPrintFormulaIndicator->isChecked() );
      QString tmpPrintRange = ePrintRange->text();
      QString tmpRepeatCols = eRepeatCols->text();
      QString tmpRepeatRows = eRepeatRows->text();
      if ( tmpPrintRange.isEmpty() )
      {
        print->setPrintRange( QRect( QPoint( 1, 1 ), QPoint( KS_colMax, KS_rowMax ) ) );
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
                    print->setPrintRange ( QRect( QPoint( QMIN( point1.pos.x(), point2.pos.x() ),
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
        print->setPrintRepeatColumns( qMakePair( 0, 0 ) );
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
                    print->setPrintRepeatColumns ( qMakePair( col1, col2 ) );
                }
            }
        }

        if ( error )
          KMessageBox::information( 0, i18n( "Repeated columns range wrong, changes are ignored.\nMust be in format column:column (eg. B:C)" ) );
      }

      if ( tmpRepeatRows.isEmpty() )
      {
        print->setPrintRepeatRows ( qMakePair( 0, 0 ) );
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
                    print->setPrintRepeatRows ( qMakePair( row1, row2 ) );
                }
            }
        }

        if ( error )
          KMessageBox::information( 0, i18n( "Repeated rows range wrong, changes are ignored.\nMust be in format row:row (eg. 2:3)" ) );
      }
      print->setZoom( 0.01 * m_cZoom->currentText().toDouble() );

      sheet->doc()->setModified( true );

      if ( pl.format == PG_CUSTOM )
      {
        print->setPaperWidth( qRound( POINT_TO_MM( pl.ptWidth ) *1000 ) / 1000 );
        print->setPaperHeight( qRound( POINT_TO_MM( pl.ptHeight ) *1000 ) / 1000 );
      }

      print->setPaperLayout( POINT_TO_MM(pl.ptLeft), POINT_TO_MM(pl.ptTop),
                             POINT_TO_MM(pl.ptRight), POINT_TO_MM(pl.ptBottom),
                             pl.format, pl.orientation );

      print->setHeadFootLine( print->delocalizeHeadFootLine( hf.headLeft  ),
                              print->delocalizeHeadFootLine( hf.headMid   ),
                              print->delocalizeHeadFootLine( hf.headRight ),
                              print->delocalizeHeadFootLine( hf.footLeft  ),
                              print->delocalizeHeadFootLine( hf.footMid   ),
                              print->delocalizeHeadFootLine( hf.footRight ) );

      sheet->doc()->setUnit( unit );

      if ( map )
        sheet = map->nextTable();
      else
        sheet = 0;
    }

    m_pView->doc()->emitEndOperation();
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
