/* This file is part of the KDE project
   Copyright (C) 2002-2003 Ariya Hidayat <ariya@kde.org>
             (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002-2003 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 Laurent Montel <montel@kde.org>

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
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qhgroupbox.h>

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
    QVBoxLayout *vbox = new QVBoxLayout( tab, KDialog::marginHint(), KDialog::spacingHint() );

    pApplyToAll = new QCheckBox ( i18n( "&Apply to all sheets" ), tab );
    pApplyToAll->setChecked( print->printGrid() );
    vbox->addWidget( pApplyToAll );

    //Range properties
    initGeneralOptions( tab, vbox );

    //Range properties
    initRanges( tab, vbox );

    //Scale properties
    initScaleOptions( tab, vbox );

    vbox->addStretch( 1 );
}

void KSpreadPaperLayout::initGeneralOptions( QWidget * tab, QVBoxLayout * vbox )
{
    KSpreadSheetPrint* print = m_pSheet->print();

    QHGroupBox *group = new QHGroupBox( i18n("General Options"), tab );
    vbox->addWidget( group );

    pPrintGrid = new QCheckBox ( i18n("Print &grid"), group );
    pPrintGrid->setChecked( print->printGrid() );

    pPrintCommentIndicator = new QCheckBox ( i18n("Print &comment indicator"), group );
    pPrintCommentIndicator->setChecked( print->printCommentIndicator() );

    pPrintFormulaIndicator = new QCheckBox ( i18n("Print &formula indicator"), group );
    pPrintFormulaIndicator->setChecked( print->printFormulaIndicator() );
}

void KSpreadPaperLayout::initRanges( QWidget * tab, QVBoxLayout * vbox )
{
    KSpreadSheetPrint* print = m_pSheet->print();

    QGroupBox *rangeGroup = new QGroupBox( i18n("Ranges"), tab );
    rangeGroup->setColumnLayout( 0, Qt::Vertical );
    rangeGroup->setMargin( KDialog::marginHint() );
    vbox->addWidget( rangeGroup );

    QGridLayout *grid = new QGridLayout( rangeGroup->layout(), 3, 2, KDialog::spacingHint() );

    QLabel *pPrintRange = new QLabel ( i18n("Print range:"), rangeGroup );
    grid->addWidget( pPrintRange, 0, 0 );

    ePrintRange = new QLineEdit( rangeGroup );
    ePrintRange->setText( util_rangeName( print->printRange() ) );
    grid->addWidget( ePrintRange, 0, 1 );

    QLabel *pRepeatCols = new QLabel ( i18n("Repeat columns on each page:"), rangeGroup );
    grid->addWidget( pRepeatCols, 1, 0 );

    eRepeatCols = new QLineEdit( rangeGroup );
    if ( print->printRepeatColumns().first != 0 )
        eRepeatCols->setText( KSpreadCell::columnName( print->printRepeatColumns().first ) +  ":" +
                              KSpreadCell::columnName( print->printRepeatColumns().second ) );
    grid->addWidget( eRepeatCols, 1, 1 );

    QLabel *pRepeatRows = new QLabel ( i18n("Repeat rows on each page:"), rangeGroup );
    grid->addWidget( pRepeatRows, 2, 0 );

    eRepeatRows = new QLineEdit( rangeGroup );
    if ( print->printRepeatRows().first != 0 )
        eRepeatRows->setText( QString().setNum( print->printRepeatRows().first ) +
                              ":" +
                              QString().setNum( print->printRepeatRows().second ) );
    grid->addWidget( eRepeatRows, 2, 1 );

    grid->addColSpacing( 0, pPrintRange->width() );
    grid->addColSpacing( 0, pRepeatRows->width() );
    grid->addColSpacing( 0, pRepeatCols->width() );
    grid->addColSpacing( 1, ePrintRange->width() );
    grid->addColSpacing( 1, eRepeatRows->width() );
    grid->addColSpacing( 1, eRepeatCols->width() );

    grid->addRowSpacing( 0, pPrintRange->height() );
    grid->addRowSpacing( 0, ePrintRange->height() );
    grid->addRowSpacing( 1, pRepeatRows->height() );
    grid->addRowSpacing( 1, eRepeatRows->height() );
    grid->addRowSpacing( 2, pRepeatCols->height() );
    grid->addRowSpacing( 2, eRepeatCols->height() );
}

void KSpreadPaperLayout::initScaleOptions( QWidget * tab, QVBoxLayout * vbox )
{
    KSpreadSheetPrint* print = m_pSheet->print();

    QButtonGroup *zoomGroup = new QButtonGroup( i18n("Scale Printout"), tab );
    zoomGroup->setColumnLayout( 0, Qt::Vertical );
    zoomGroup->setMargin( KDialog::marginHint() );
    vbox->addWidget( zoomGroup );

    QGridLayout *grid = new QGridLayout( zoomGroup->layout(), 2, 6,
                                         KDialog::spacingHint() );

    m_rScalingZoom = new QRadioButton ( i18n("Zoom:"), zoomGroup );
    grid->addWidget( m_rScalingZoom, 0, 0 );

    m_cZoom = new QComboBox( true, zoomGroup, "Zoom" );
    grid->addMultiCellWidget( m_cZoom, 0, 0, 1, 5, Qt::AlignLeft );

    QStringList lst;
    for( int i = 5; i < 500; i += 5 )
    {
        lst.append( QString( i18n( "%1%" ) ).arg( i ) );
        if( qRound( print->zoom() * 100 ) > i &&
            qRound( print->zoom() * 100 ) < i + 5 )
        {
            lst.append( QString( i18n( "%1%" ) ).arg( qRound( print->zoom() * 100 ) ) );
        }
    }
    m_cZoom->insertStringList( lst );

    int number_of_entries = m_cZoom->count();
    QString string = QString( i18n( "%1%" ) ).arg( qRound( print->zoom() * 100 ) );
    for (int i = 0; i < number_of_entries ; i++)
    {
        if ( string == (QString) m_cZoom->text(i) )
        {
            m_cZoom->setCurrentItem( i );
            break;
        }
    }

    m_rScalingLimitPages = new QRadioButton ( i18n("Limit pages:"), zoomGroup );
    grid->addWidget( m_rScalingLimitPages, 1, 0 );

    QLabel *pLimitPagesX = new QLabel ( i18n("X:"), zoomGroup );
    grid->addWidget( pLimitPagesX, 1, 1 );

    m_cLimitPagesX = new QComboBox( true, zoomGroup, "pagesX" );
    grid->addWidget( m_cLimitPagesX, 1, 2 );

    QStringList lstX;
    lstX.append( i18n( "No Limit" ) );
    for( int i = 1; i <= 20; i += 1 )
    {
        lstX.append( QString( "%1" ).arg( i ) );
        if( print->pageLimitX() > 20 )
        {
            lstX.append( QString( "%1" ).arg( print->pageLimitX() ) );
        }
    }
    m_cLimitPagesX->insertStringList( lstX );

    if ( print->pageLimitX() <= 20 )
        m_cLimitPagesX->setCurrentItem( print->pageLimitX() );
    else
        m_cLimitPagesX->setCurrentItem( 21 );

    QLabel *pLimitPagesY = new QLabel ( i18n("Y:"), zoomGroup );
    grid->addWidget( pLimitPagesY, 1, 3 );

    m_cLimitPagesY = new QComboBox( true, zoomGroup, "pagesY" );
    grid->addWidget( m_cLimitPagesY, 1, 4 );

    QStringList lstY;
    lstY.append( i18n( "No Limit" ) );
    for( int i = 1; i <= 20; i += 1 )
    {
        lstY.append( QString( "%1" ).arg( i ) );
        if( print->pageLimitY() > 20 )
        {
            lstY.append( QString( "%1" ).arg( print->pageLimitY() ) );
        }
    }
    m_cLimitPagesY->insertStringList( lstY );

    if ( print->pageLimitY() <= 20 )
        m_cLimitPagesY->setCurrentItem( print->pageLimitY() );
    else
        m_cLimitPagesY->setCurrentItem( 21 );

    if ( print->pageLimitY() != 0 || print->pageLimitX() != 0 )
    {
        m_rScalingLimitPages->setChecked( true );
    }
    else
    {
        m_rScalingZoom->setChecked( true );
    }

    connect( m_cZoom, SIGNAL( activated( int ) ), this, SLOT( slotChooseZoom( int ) ) );
    connect( m_cLimitPagesX, SIGNAL( activated( int ) ), this, SLOT( slotChoosePageLimit( int ) ) );
    connect( m_cLimitPagesY, SIGNAL( activated( int ) ), this, SLOT( slotChoosePageLimit( int ) ) );
}

void KSpreadPaperLayout::slotChooseZoom( int /*index*/ )
{
    m_rScalingZoom->setChecked( true );
}

void KSpreadPaperLayout::slotChoosePageLimit( int /*index*/ )
{
    m_rScalingLimitPages->setChecked( true );
}

void KSpreadPaperLayout::slotOk()
{
    if ( !m_pSheet->doc()->undoLocked() )
    {
        KSpreadUndoAction* undo = new KSpreadUndoPaperLayout( m_pSheet->doc(), m_pSheet );
        m_pSheet->doc()->addCommand( undo );
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
      sheet = map->firstSheet();
    else
      sheet = m_pSheet;

    m_pView->doc()->emitBeginOperation( false );
    while ( sheet )
    {
      KSpreadSheetPrint *print = sheet->print();

      KoPageLayout pl = layout();
      KoHeadFoot hf = headFoot();
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

      if ( m_rScalingZoom->isChecked() )
      {
        if( QString( "%1%" ).arg( qRound( print->zoom() * 100 ) ) != m_cZoom->currentText() )
        {
          if( m_cZoom->currentText().toDouble() != 0.0 )
            print->setZoom( 0.01 * m_cZoom->currentText().toDouble() );
        }
      }
      else
      {
        if( print->pageLimitX() != m_cLimitPagesX->currentText().toInt() )
          print->setPageLimitX( m_cLimitPagesX->currentText().toInt() );

        if( print->pageLimitY() != m_cLimitPagesY->currentText().toInt() )
          print->setPageLimitY( m_cLimitPagesY->currentText().toInt() );
      }

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
        sheet = map->nextSheet();
      else
        sheet = 0;
    }

    m_pView->slotUpdateView( m_pView->activeSheet() );
    accept();
}

void KSpreadPaperLayout::closeEvent ( QCloseEvent * )
{
    delete this;
}

void KSpreadPaperLayout::slotSelectionChanged( KSpreadSheet* /*_sheet*/, const QRect& _selection )
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
