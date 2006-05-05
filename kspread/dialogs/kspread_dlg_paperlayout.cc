/* This file is part of the KDE project
   Copyright (C) 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
             (C) 2002-2003 Ariya Hidayat <ariya@kde.org>
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kspread_dlg_paperlayout.h"
#include <kspread_doc.h>
#include "kspread_map.h"
#include "kspread_sheet.h"
#include "kspread_sheetprint.h"
#include <kspread_undo.h>
#include <kspread_util.h>
#include <kspread_view.h>
#include <selection.h>

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

using namespace KSpread;

PaperLayout::PaperLayout( QWidget * parent, const char * name,
                                        const KoPageLayout & layout,
                                        const KoHeadFoot & headfoot,
                                        int tabs, KoUnit::Unit unit,
                                        Sheet * sheet, View * view)
  : KoPageLayoutDia( parent, name, layout, headfoot, tabs, unit, false /*no modal*/),
    m_pSheet( sheet ),
    m_pView( view )
{
    initTab();
    connect(view->selectionInfo(), SIGNAL(changed(const Region&)),
            this, SLOT(slotSelectionChanged()));
    qApp->installEventFilter( this );
    m_focus= 0L;
}

void PaperLayout::initTab()
{
    SheetPrint* print = m_pSheet->print();

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

void PaperLayout::initGeneralOptions( QWidget * tab, QVBoxLayout * vbox )
{
    SheetPrint* print = m_pSheet->print();

    QGroupBox *group = new QGroupBox( i18n("General Options"), tab );
    QGridLayout *l = new QGridLayout( group, 2, 3 );

    pPrintGrid = new QCheckBox ( i18n("Print &grid"), group );
    pPrintGrid->setChecked( print->printGrid() );
    l->addWidget( pPrintGrid, 0, 0 );

    pPrintCommentIndicator = new QCheckBox ( i18n("Print &comment indicator"), group );
    pPrintCommentIndicator->setChecked( print->printCommentIndicator() );
    l->addWidget( pPrintCommentIndicator, 0, 1 );

    pPrintFormulaIndicator = new QCheckBox ( i18n("Print &formula indicator"), group );
    pPrintFormulaIndicator->setChecked( print->printFormulaIndicator() );
    l->addWidget( pPrintFormulaIndicator, 0, 2 );

    pPrintObjects = new QCheckBox ( i18n("Print &objects"), group );
    pPrintObjects->setChecked( print->printObjects() );
    l->addWidget( pPrintObjects, 1, 0 );

    pPrintCharts = new QCheckBox ( i18n("Print &charts"), group );
    pPrintCharts->setChecked( print->printCharts() );
    l->addWidget( pPrintCharts, 1, 1 );

//     Not used yet unless someone implement some kind of drawing object
//     pPrintGraphics = new QCheckBox ( i18n("Print &drawings"), group );
//     pPrintGraphics->setChecked( print->printGraphics() );
//     l->addWidget( pPrintGraphics, 1, 2 );

    vbox->addWidget( group );
}

void PaperLayout::initRanges( QWidget * tab, QVBoxLayout * vbox )
{
    SheetPrint* print = m_pSheet->print();

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
        eRepeatCols->setText( Cell::columnName( print->printRepeatColumns().first ) +  ":" +
                              Cell::columnName( print->printRepeatColumns().second ) );
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

void PaperLayout::initScaleOptions( QWidget * tab, QVBoxLayout * vbox )
{
    SheetPrint* print = m_pSheet->print();

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
        lst.append( i18n( "%1%" ).arg( i ) );
        if( qRound( print->zoom() * 100 ) > i &&
            qRound( print->zoom() * 100 ) < i + 5 )
        {
            lst.append( i18n( "%1%" ).arg( qRound( print->zoom() * 100 ) ) );
        }
    }
    m_cZoom->insertStringList( lst );

    int number_of_entries = m_cZoom->count();
    QString string = i18n( "%1%" ).arg( qRound( print->zoom() * 100 ) );
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
    }
    if( print->pageLimitX() > 20 )
    {
      lstX.append( QString( "%1" ).arg( print->pageLimitX() ) );
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
    }
    if( print->pageLimitY() > 20 )
    {
      lstY.append( QString( "%1" ).arg( print->pageLimitY() ) );
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

void PaperLayout::slotChooseZoom( int /*index*/ )
{
    m_rScalingZoom->setChecked( true );
}

void PaperLayout::slotChoosePageLimit( int /*index*/ )
{
    m_rScalingLimitPages->setChecked( true );
}

void PaperLayout::slotOk()
{
    if ( !m_pSheet->doc()->undoLocked() )
    {
        UndoAction* undo = new UndoPaperLayout( m_pSheet->doc(), m_pSheet );
        m_pSheet->doc()->addCommand( undo );
    }

    // get new values for borders
    Map   * map   = 0;
    Sheet * sheet = 0;

    if ( pApplyToAll->isChecked() )
      map = m_pSheet->doc()->map();

    if ( map )
      sheet = map->firstSheet();
    else
      sheet = m_pSheet;

    m_pView->doc()->emitBeginOperation( false );
    while ( sheet )
    {
      SheetPrint *print = sheet->print();

      KoPageLayout pl = layout();
      KoHeadFoot hf = headFoot();
      KoUnit::Unit unit = sheet->doc()->unit();
      print->setPrintGrid( pPrintGrid->isChecked() );
      print->setPrintCommentIndicator( pPrintCommentIndicator->isChecked() );
      print->setPrintFormulaIndicator( pPrintFormulaIndicator->isChecked() );
      print->setPrintObjects( pPrintObjects->isChecked() );
      print->setPrintCharts( pPrintCharts->isChecked() );
//       print->setPrintGraphics( pPrintGraphics->isChecked() );
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
            Point point1 ( tmpPrintRange.left( first ) );
            if ( point1.isValid() )
            {
                Point point2 ( tmpPrintRange.mid( first+1 ) );
                if ( point2.isValid() )
                {
                    error = false;
                    print->setPrintRange ( QRect( QPoint( QMIN( point1.pos().x(), point2.pos().x() ),
                                                          QMIN( point1.pos().y(), point2.pos().y() ) ),
                                                  QPoint( QMAX( point1.pos().x(), point2.pos().x() ),
                                                          QMAX( point1.pos().y(), point2.pos().y() ) ) ) );
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
        kdDebug() << "Zoom is selected" << endl;
        if( QString( "%1%" ).arg( qRound( print->zoom() * 100 ) ) != m_cZoom->currentText() )
        {
          kdDebug() << "New zoom is different than original: " << m_cZoom->currentText() << endl;
          QString zoomtext = m_cZoom->currentText();
          zoomtext.replace("%","");
          bool convertok = false;
          double zoomvalue = zoomtext.toDouble(&convertok);
          if (!convertok)
          {
            kdWarning() << "Could not convert zoom text to double value!!!" << endl;
          }
          else if( zoomvalue != 0.0 )
          {
            //reset page limits
            print->setPageLimitX(0);
            print->setPageLimitY(0);
            kdDebug() << "Setting print zoom: " << zoomvalue*0.01 << endl;
            print->setZoom( 0.01 * zoomvalue );
            kdDebug() << "New print zoom: " << print->zoom() << endl;
          }
          else
            kdDebug() << "Did not set print zoom" << endl;
        }
        else
          kdDebug() << "new zoom is same as original: " << m_cZoom->currentText() << endl;
      }
      else if (m_rScalingLimitPages->isChecked())
      {
        kdDebug() << "Limit pages is selected" << endl;
        kdDebug() << "Current zoom: " << print->zoom() << endl;
        
        //reset first, otherwise setting the first limit
        //would still check against the second limit and
        //possibly result in a smaller total zoom
        print->setPageLimitX( 0 );
        print->setPageLimitY( 0 );
        
        //start with at least 100%
        
        if (print->zoom() < 1.0)
        {
          kdDebug() << "Resetting zoom to 1.0" << endl;
          print->setZoom(1.0,false); //don't check page limits here
          kdDebug() << "Zoom is now: " << print->zoom() << endl;
        }
        
        bool convertok = false;
        
        int limitX = m_cLimitPagesX->currentText().toInt(&convertok);
        if (!convertok)  //THIS IS THE CASE WITH "No Limit"
          limitX = 0;  //0 means no limit
        
        convertok=false;
        int limitY = m_cLimitPagesY->currentText().toInt(&convertok);
        if (!convertok)  //THIS IS THE CASE WITH "No Limit"
          limitY=0;  //0 means no limit

        kdDebug() << "Zoom before setting limits: " << print->zoom() << endl;
        kdDebug() << "Chosen Limits: x: " << limitX << "; y: " << limitY << endl;
        print->setPageLimitX( limitX );
        kdDebug() << "Zoom after setting x limit: " << print->zoom() << endl;
        print->setPageLimitY( limitY );
        kdDebug() << "Zoom after setting y limit: " << print->zoom() << endl;
      }
      else
        kdWarning() << "ERROR: unknown zoom option selected" << endl;

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

void PaperLayout::closeEvent ( QCloseEvent * )
{
    delete this;
}

void PaperLayout::slotSelectionChanged()
{
  // TODO Stefan: check for improvement
  if (!m_pView->selectionInfo()->isValid())
    return;

  QString area = util_rangeName( m_pView->selectionInfo()->lastRange() );
  if ( m_focus )
  {
      if ( m_focus == ePrintRange )
        area = util_rangeName( m_pView->selectionInfo()->lastRange() );
      else if ( m_focus == eRepeatRows )
        area = util_rangeRowName( m_pView->selectionInfo()->lastRange() );
      else if ( m_focus == eRepeatCols )
        area = util_rangeColumnName( m_pView->selectionInfo()->lastRange() );
      else
          return;
      m_focus->setText( area );
  }
}

void PaperLayout::slotCancel()
{
  reject();
}

bool PaperLayout::eventFilter( QObject* obj, QEvent* ev )
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
