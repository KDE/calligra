/* This file is part of the KDE project
   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002 Ariya Hidayat <ariya@kde.org>
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

#include "kspread_cell.h"
#include "kspread_dlg_database.h"
#include "kspread_doc.h"
#include "kspread_sheet.h"
#include "kspread_util.h"
#include "kspread_undo.h"
#include "kspread_view.h"

#include <kdebug.h>
#include <k3listview.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <knumvalidator.h>
#include <kpushbutton.h>

#include <qcheckbox.h>
#include <qcombobox.h>
#include <q3frame.h>
#include <q3header.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qsqldatabase.h>
#include <qsqlerror.h>
#include <qsqlfield.h>
#include <qsqlquery.h>
#include <qsqlrecord.h>
#include <q3textedit.h>
#include <qtooltip.h>
#include <qvariant.h>
#include <q3whatsthis.h>
#include <qwidget.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

using namespace KSpread;

#ifndef QT_NO_SQL

/********************************************************
 *                 Database wizard                      *
 ********************************************************/

DatabaseDialog::DatabaseDialog( View * parent, QRect const & rect, const char * name, bool modal, Qt::WFlags fl )
  : K3Wizard( (QWidget *) parent, name, modal, fl ),
    m_currentPage( eDatabase ),
    m_pView( parent ),
    m_targetRect( rect ),
    m_dbConnection( 0L )
{
  if ( !name )
    setName( "DatabaseDialog" );

  setCaption( i18n( "Insert Data From Database" ) );

  // database page

  m_database = new QWidget( this, "m_database" );
  m_databaseLayout = new QGridLayout( m_database, 1, 1, -1, -1, "m_databaseLayout");

  QFrame * Frame5 = new QFrame( m_database, "Frame5" );
// ###  Frame5->setFrameShape( QFrame::MShape );
// ###  Frame5->setFrameShadow( QFrame::MShadow );
  QVBoxLayout * Frame5Layout = new QVBoxLayout( Frame5 );
  Frame5Layout->setMargin(11);
  Frame5Layout->setSpacing(6);

  QFrame * Frame16 = new QFrame( Frame5, "Frame16" );
  Frame16->setFrameShape( QFrame::NoFrame );
  Frame16->setFrameShadow( QFrame::Plain );
  QGridLayout * Frame16Layout = new QGridLayout( Frame16, 1, 1, 11, 7, "Frame16Layout");

  m_Type = new QLabel( Frame16, "m_Type" );
  m_Type->setText( i18n( "Type:" ) );

  Frame16Layout->addWidget( m_Type, 0, 0 );

  QLabel * TextLabel4 = new QLabel( Frame16, "TextLabel4" );
  TextLabel4->setText( i18n( "User name:\n"
                               "(if necessary)" ) );
  Frame16Layout->addWidget( TextLabel4, 4, 0 );

  QLabel * TextLabel2 = new QLabel( Frame16, "TextLabel2" );
  TextLabel2->setText( i18n( "Host:" ) );
  Frame16Layout->addWidget( TextLabel2, 2, 0 );

  m_driver = new QComboBox( FALSE, Frame16, "m_driver" );
  Frame16Layout->addWidget( m_driver, 0, 1 );

  m_username = new QLineEdit( Frame16, "m_username" );
  Frame16Layout->addWidget( m_username, 4, 1 );

  m_host = new QLineEdit( Frame16, "m_host" );
  m_host->setText("localhost");
  Frame16Layout->addWidget( m_host, 2, 1 );

  QLabel * TextLabel3 = new QLabel( Frame16, "TextLabel3" );
  TextLabel3->setText( i18n( "Port:\n(if necessary)") );
  Frame16Layout->addWidget( TextLabel3, 3, 0 );

  m_password = new QLineEdit( Frame16, "m_password" );
  m_password->setEchoMode( QLineEdit::Password );
  Frame16Layout->addWidget( m_password, 5, 1 );

  m_port = new QLineEdit( Frame16, "m_port" );
  m_port->setValidator( new KIntValidator( m_port ) );
  Frame16Layout->addWidget( m_port, 3, 1 );

  QLabel * dbName = new QLabel( Frame16, "dbName" );
  dbName->setText( i18n( "Database name: ") );
  Frame16Layout->addWidget( dbName, 1, 0 );

  m_databaseName = new QLineEdit( Frame16, "m_databaseName" );
  Frame16Layout->addWidget( m_databaseName, 1, 1 );

  QLabel * TextLabel5 = new QLabel( Frame16, "TextLabel5" );
  TextLabel5->setText( i18n( "Password:\n"
                               "(if necessary)" ) );
  Frame16Layout->addWidget( TextLabel5, 5, 0 );
  Frame5Layout->addWidget( Frame16 );

  m_databaseStatus = new QLabel( Frame5, "m_databaseStatus" );
  m_databaseStatus->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)5, 0, 0, m_databaseStatus->sizePolicy().hasHeightForWidth() ) );
  m_databaseStatus->setMaximumSize( QSize( 32767, 30 ) );
  m_databaseStatus->setText( " " );
  Frame5Layout->addWidget( m_databaseStatus );

  m_databaseLayout->addWidget( Frame5, 0, 1 );

  QFrame * Frame17 = new QFrame( m_database, "Frame17" );
  Frame17->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)7, 0, 0, Frame17->sizePolicy().hasHeightForWidth() ) );
  Frame17->setMinimumSize( QSize( 111, 0 ) );
  Frame17->setFrameShape( QFrame::NoFrame );
  Frame17->setFrameShadow( QFrame::Plain );

  m_databaseLayout->addWidget( Frame17, 0, 0 );
  addPage( m_database, i18n( "Database" ) );

  // new page

  m_sheet = new QWidget( this, "m_table" );
  m_sheetLayout = new QGridLayout( m_sheet, 1, 1, 11, 6, "m_tableLayout");

  QFrame * Frame5_2 = new QFrame( m_sheet, "Frame5_2" );
// ###  Frame5_2->setFrameShape( QFrame::MShape );
// ###  Frame5_2->setFrameShadow( QFrame::MShadow );
  QGridLayout * Frame5_2Layout = new QGridLayout( Frame5_2, 1, 1, 11, 6, "Frame5_2Layout");

  QHBoxLayout * Layout21 = new QHBoxLayout();
  Layout21->setMargin(0);
  Layout21->setSpacing(6);

  //  QLabel * TextLabel12_2 = new QLabel( Frame5_2, "TextLabel12_2" );
  //  TextLabel12_2->setText( i18n( "Database:" ) );
  //  Layout21->addWidget( TextLabel12_2 );

  //  m_databaseList = new QComboBox( FALSE, Frame5_2, "m_databaseList" );
  //  Layout21->addWidget( m_databaseList );

  //  m_connectButton = new KPushButton( Frame5_2, "m_connectButton" );
  //  m_connectButton->setText( i18n( "&Connect" ) );
  //  Layout21->addWidget( m_connectButton );

  Frame5_2Layout->addLayout( Layout21, 0, 0 );

  m_sheetStatus = new QLabel( Frame5_2, "m_tableStatus" );
  m_sheetStatus->setText( " " );
  Frame5_2Layout->addWidget( m_sheetStatus, 3, 0 );

  m_SelectSheetLabel = new QLabel( Frame5_2, "m_SelectSheetLabel" );
  m_SelectSheetLabel->setText( i18n( "Select tables:" ) );
  Frame5_2Layout->addWidget( m_SelectSheetLabel, 1, 0 );

  m_sheetView = new K3ListView( Frame5_2 );
  m_sheetView->addColumn( i18n( "Sheet" ) );
  m_sheetView->setRootIsDecorated( FALSE );

  Frame5_2Layout->addWidget( m_sheetView, 2, 0 );

  m_sheetLayout->addWidget( Frame5_2, 0, 1 );

  QFrame * Frame17_2 = new QFrame( m_sheet, "Frame17_2" );
  Frame17_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)7, 0, 0, Frame17_2->sizePolicy().hasHeightForWidth() ) );
  Frame17_2->setMinimumSize( QSize( 111, 0 ) );
  Frame17_2->setFrameShape( QFrame::NoFrame );
  Frame17_2->setFrameShadow( QFrame::Plain );

  m_sheetLayout->addWidget( Frame17_2, 0, 0 );
  addPage( m_sheet, i18n( "Sheets" ) );

  m_columns = new QWidget( this, "m_columns" );
  m_columnsLayout = new QGridLayout( m_columns, 1, 1, 11, 6, "m_columnsLayout");

  QFrame * Frame5_2_2 = new QFrame( m_columns, "Frame5_2_2" );
// ###  Frame5_2_2->setFrameShape( QFrame::MShape );
// ###  Frame5_2_2->setFrameShadow( QFrame::MShadow );
  QGridLayout * Frame5_2_2Layout = new QGridLayout( Frame5_2_2, 1, 1, 11, 6, "Frame5_2_2Layout");

  QLabel * TextLabel11_2 = new QLabel( Frame5_2_2, "TextLabel11_2" );
  TextLabel11_2->setText( i18n( "Select columns:" ) );

  Frame5_2_2Layout->addWidget( TextLabel11_2, 0, 0 );

  m_columnView = new K3ListView( Frame5_2_2 );
  m_columnView->addColumn( i18n( "Column" ) );
  m_columnView->addColumn( i18n( "Sheet" ) );
  m_columnView->addColumn( i18n( "Data Type" ) );
  m_columnView->setRootIsDecorated( FALSE );

  Frame5_2_2Layout->addWidget( m_columnView, 1, 0 );

  m_columnsStatus = new QLabel( Frame5_2_2, "m_columnsStatus" );
  m_columnsStatus->setText( " " );
  Frame5_2_2Layout->addWidget( m_columnsStatus, 2, 0 );

  m_columnsLayout->addWidget( Frame5_2_2, 0, 1 );

  QFrame * Frame17_3 = new QFrame( m_columns, "Frame17_3" );
  Frame17_3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)7, 0, 0, Frame17_3->sizePolicy().hasHeightForWidth() ) );
  Frame17_3->setMinimumSize( QSize( 111, 0 ) );
  Frame17_3->setFrameShape( QFrame::NoFrame );
  Frame17_3->setFrameShadow( QFrame::Plain );

  m_columnsLayout->addWidget( Frame17_3, 0, 0 );
  addPage( m_columns, i18n( "Columns" ) );

  // options page

  m_options = new QWidget( this, "m_options" );
  m_optionsLayout = new QGridLayout( m_options, 1, 1, 11, 6, "m_optionsLayout");

  QFrame * optionsFrame = new QFrame( m_options );
// ###  optionsFrame->setFrameShape( QFrame::MShape );
// ###  optionsFrame->setFrameShadow( QFrame::MShadow );
  QGridLayout * optionsFrameLayout = new QGridLayout( optionsFrame, 1, 1, 11, 6, "optionsFrameLayout");

  m_columns_1 = new QComboBox( false, optionsFrame, "m_columns_1" );
  optionsFrameLayout->addWidget( m_columns_1, 2, 0 );

  m_operatorValue_2 = new QLineEdit( optionsFrame, "m_operatorValue_2" );
  optionsFrameLayout->addWidget( m_operatorValue_2, 3, 2 );

  m_andBox = new QRadioButton( optionsFrame, "m_andBox" );
  m_andBox->setText( i18n( "Match all of the following (AND)" ) );
  m_andBox->setChecked( true );

  optionsFrameLayout->addWidget( m_andBox, 0, 0, 0, 2 );

  m_orBox = new QRadioButton( optionsFrame, "m_orBox" );
  m_orBox->setText( i18n( "Match any of the following (OR)" ) );
  optionsFrameLayout->addWidget( m_orBox, 1, 1, 0, 2 );

  m_operatorValue_1 = new QLineEdit( optionsFrame, "m_operatorValue" );
  optionsFrameLayout->addWidget( m_operatorValue_1, 2, 2 );

  m_columns_2 = new QComboBox(optionsFrame);
  m_columns_2->setEditable(false);
  optionsFrameLayout->addWidget( m_columns_2, 3, 0 );

  m_operatorValue_3 = new QLineEdit( optionsFrame, "m_operatorValue_3" );
  optionsFrameLayout->addWidget( m_operatorValue_3, 4, 2 );

  m_operator_1 = new QComboBox( FALSE, optionsFrame, "m_operator_1" );
  m_operator_1->insertItem( 0, i18n( "equals" ) );
  m_operator_1->insertItem( 1, i18n( "not equal" ) );
  m_operator_1->insertItem( 2, i18n( "in" ) );
  m_operator_1->insertItem( 3, i18n( "not in" ) );
  m_operator_1->insertItem( 4, i18n( "like" ) );
  m_operator_1->insertItem( 5, i18n( "greater" ) );
  m_operator_1->insertItem( 6, i18n( "lesser" ) );
  m_operator_1->insertItem( 7, i18n( "greater or equal" ) );
  m_operator_1->insertItem( 8, i18n( "less or equal" ) );

  optionsFrameLayout->addWidget( m_operator_1, 2, 1 );

  m_operator_2 = new QComboBox( FALSE, optionsFrame, "m_operator_2" );
  m_operator_2->insertItem( 0, i18n( "equals" ) );
  m_operator_2->insertItem( 1, i18n( "not equal" ) );
  m_operator_2->insertItem( 2, i18n( "in" ) );
  m_operator_2->insertItem( 3, i18n( "not in" ) );
  m_operator_2->insertItem( 4, i18n( "like" ) );
  m_operator_2->insertItem( 5, i18n( "greater" ) );
  m_operator_2->insertItem( 6, i18n( "lesser" ) );

  optionsFrameLayout->addWidget( m_operator_2, 3, 1 );

  m_operator_3 = new QComboBox( FALSE, optionsFrame, "m_operator_3" );
  m_operator_3->insertItem( 0, i18n( "equals" ) );
  m_operator_3->insertItem( 1, i18n( "not equal" ) );
  m_operator_3->insertItem( 2, i18n( "in" ) );
  m_operator_3->insertItem( 3, i18n( "not in" ) );
  m_operator_3->insertItem( 4, i18n( "like" ) );
  m_operator_3->insertItem( 5, i18n( "greater" ) );
  m_operator_3->insertItem( 6, i18n( "lesser" ) );

  optionsFrameLayout->addWidget( m_operator_3, 4, 1 );

  m_columns_3 = new QComboBox( false, optionsFrame, "m_columns_3" );

  optionsFrameLayout->addWidget( m_columns_3, 4, 0 );

  m_distinct = new QCheckBox( optionsFrame, "m_distinct" );
  m_distinct->setText( i18n( "Distinct" ) );

  optionsFrameLayout->addWidget( m_distinct, 7, 2 );

  QLabel * TextLabel19 = new QLabel( optionsFrame, "TextLabel19" );
  TextLabel19->setText( i18n( "Sorted by" ) );
  optionsFrameLayout->addWidget( TextLabel19, 5, 0 );

  m_columnsSort_1 = new QComboBox( false, optionsFrame, "m_columnsSort_1" );
  optionsFrameLayout->addWidget( m_columnsSort_1, 5, 1 );

  m_sortMode_1 = new QComboBox( false, optionsFrame, "m_sortMode_1" );
  m_sortMode_1->insertItem( 0, i18n( "Ascending" ) );
  m_sortMode_1->insertItem( 1, i18n( "Descending" ) );
  optionsFrameLayout->addWidget( m_sortMode_1, 5, 2 );

  QLabel * TextLabel19_2 = new QLabel( optionsFrame, "TextLabel19_2" );
  TextLabel19_2->setText( i18n( "Sorted by" ) );
  optionsFrameLayout->addWidget( TextLabel19_2, 6, 0 );

  m_columnsSort_2 = new QComboBox( false, optionsFrame, "m_columnsSort_2" );
  optionsFrameLayout->addWidget( m_columnsSort_2, 6, 1 );

  m_sortMode_2 = new QComboBox( false, optionsFrame, "m_sortMode_2" );
  m_sortMode_2->insertItem( 0, i18n( "Ascending" ) );
  m_sortMode_2->insertItem( 1, i18n( "Descending" ) );

  optionsFrameLayout->addWidget( m_sortMode_2, 6, 2 );
  QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  optionsFrameLayout->addItem( spacer, 7, 1 );
  QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  optionsFrameLayout->addItem( spacer_2, 7, 0 );

  m_optionsLayout->addWidget( optionsFrame, 0, 1 );

  QFrame * Frame17_4 = new QFrame( m_options, "Frame17_4" );
  Frame17_4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)7, 0, 0, Frame17_4->sizePolicy().hasHeightForWidth() ) );
  Frame17_4->setMinimumSize( QSize( 111, 0 ) );
  Frame17_4->setFrameShape( QFrame::NoFrame );
  Frame17_4->setFrameShadow( QFrame::Plain );

  m_optionsLayout->addWidget( Frame17_4, 0, 0 );
  addPage( m_options, i18n( "Query Options" ) );

  // result page

  m_result = new QWidget( this, "m_result" );
  m_resultLayout = new QGridLayout( m_result, 1, 1, 11, 6, "m_resultLayout");

  QFrame * Frame5_2_2_3 = new QFrame( m_result );
// ###  Frame5_2_2_3->setFrameShape( QFrame::MShape );
// ###  Frame5_2_2_3->setFrameShadow( QFrame::MShadow );
  QGridLayout * Frame5_2_2_3Layout = new QGridLayout( Frame5_2_2_3 );
  Frame5_2_2_3Layout->setMargin(11);
  Frame5_2_2_3Layout->setSpacing(6);

  QLabel * TextLabel17 = new QLabel( Frame5_2_2_3, "TextLabel17" );
  TextLabel17->setText( i18n( "SQL query:" ) );
  Frame5_2_2_3Layout->addWidget( TextLabel17, 0, 0 );

  m_sqlQuery = new Q3TextEdit( Frame5_2_2_3, "m_sqlQuery" );
  Frame5_2_2_3Layout->addWidget( m_sqlQuery, 1, 0 );

  QFrame * Frame12 = new QFrame( Frame5_2_2_3, "Frame12" );
  Frame12->setFrameShape( QFrame::StyledPanel );
  Frame12->setFrameShadow( QFrame::Raised );
  QGridLayout * Frame12Layout = new QGridLayout( Frame12 );
  Frame12Layout->setMargin(11);
  Frame12Layout->setSpacing(6);

  m_startingRegion = new QRadioButton( Frame12, "m_startingRegion" );
  m_startingRegion->setText( i18n( "Insert in region" ) );
  Frame12Layout->addWidget( m_startingRegion, 0, 0 );

  m_cell = new QLineEdit( Frame12, "m_cell" );
  Frame12Layout->addWidget( m_cell, 1, 1 );

  m_region = new QLineEdit( Frame12, "m_region" );
  Frame12Layout->addWidget( m_region, 0, 1 );

  m_startingCell = new QRadioButton( Frame12, "m_startingCell" );
  m_startingCell->setText( i18n( "Starting in cell" ) );
  m_startingCell->setChecked( TRUE );
  Frame12Layout->addWidget( m_startingCell, 1, 0 );

  Frame5_2_2_3Layout->addWidget( Frame12, 2, 0 );
  m_resultLayout->addWidget( Frame5_2_2_3, 0, 1 );

  QFrame * Frame17_5 = new QFrame( m_result, "Frame17_5" );
  Frame17_5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)7, 0, 0, Frame17_5->sizePolicy().hasHeightForWidth() ) );
  Frame17_5->setMinimumSize( QSize( 111, 0 ) );
  Frame17_5->setFrameShape( QFrame::NoFrame );
  Frame17_5->setFrameShadow( QFrame::Plain );

  m_resultLayout->addWidget( Frame17_5, 0, 0 );
  addPage( m_result, i18n( "Result" ) );

  finishButton()->setEnabled(false);

  // signals and slots connections
  connect( m_orBox, SIGNAL( clicked() ), this, SLOT( orBox_clicked() ) );
  connect( m_andBox, SIGNAL( clicked() ), this, SLOT( andBox_clicked() ) );
  connect( m_startingCell, SIGNAL( clicked() ), this, SLOT( startingCell_clicked() ) );
  connect( m_startingRegion, SIGNAL( clicked() ), this, SLOT( startingRegion_clicked() ) );
  connect( m_driver, SIGNAL( activated(int) ), this, SLOT( databaseDriverChanged(int) ) );
  connect( m_host, SIGNAL( textChanged(const QString &) ), this, SLOT( databaseHostChanged(const QString &) ) );
  connect( m_databaseName, SIGNAL( textChanged(const QString &) ), this, SLOT( databaseNameChanged(const QString &) ) );
  connect( m_sheetView, SIGNAL( contextMenuRequested( Q3ListViewItem *, const QPoint &, int ) ),
           this, SLOT( popupSheetViewMenu(Q3ListViewItem *, const QPoint &, int ) ) );
  connect( m_sheetView, SIGNAL( clicked( Q3ListViewItem * ) ), this, SLOT( sheetViewClicked( Q3ListViewItem * ) ) );

  QStringList str = QSqlDatabase::drivers();
  m_driver->insertItem("");
  m_driver->insertItems( 1, str );


  helpButton()->hide();
  setNextEnabled(m_database, false);
  setNextEnabled(m_sheet, false);
  setNextEnabled(m_columns, false);
  setNextEnabled(m_options, false);
  setNextEnabled(m_result, false);
}

DatabaseDialog::~DatabaseDialog()
{
  // no need to delete child widgets, Qt does it all for us
  if ( m_dbConnection )
    m_dbConnection->close();
}

void DatabaseDialog::switchPage( int id )
{
  if ( id > eResult )
    --m_currentPage;
  if ( id < eDatabase )
    ++m_currentPage;

  switch ( id )
  {
   case eDatabase:
    showPage(m_database);
    break;

   case eSheets:
    showPage(m_sheet);
    break;

   case eColumns:
    showPage(m_columns);
    break;

   case eOptions:
    showPage(m_options);
    break;

   case eResult:
    showPage(m_result);
    break;

   default:
    break;
  }
}

void DatabaseDialog::next()
{
  switch ( m_currentPage )
  {
   case eDatabase:
    if (!databaseDoNext())
      return;
    break;

   case eSheets:
    if (!sheetsDoNext())
      return;
    break;

   case eColumns:
    if (!columnsDoNext())
      return;
    break;

   case eOptions:
    if (!optionsDoNext())
      return;
    break;

   case eResult:
    // there is nothing to do here
    break;

   default:
    break;
  }

  ++m_currentPage;

  switchPage( m_currentPage );
}

void DatabaseDialog::back()
{
  --m_currentPage;

  switchPage( m_currentPage );
}

void DatabaseDialog::accept()
{
  Sheet * sheet = m_pView->activeSheet();
  int top;
  int left;
  int width  = -1;
  int height = -1;
  if ( m_startingRegion->isChecked() )
  {
    Range range( m_region->text() );
    if ( range.isSheetKnown() )
    {
      KMessageBox::error( this, i18n("You cannot specify a table here.") );
      m_region->setFocus();
      m_region->selectAll();
      return;
    }

    range.setSheet(sheet);

    if ( !range.isValid() )
    {
      KMessageBox::error( this, i18n("You have to specify a valid region.") );
      m_region->setFocus();
      m_region->selectAll();
      return;
    }

    top    = range.range().top();
    left   = range.range().left();
    width  = range.range().width();
    height = range.range().height();
  }
  else
  {
    Point point( m_cell->text() );
    if ( point.isSheetKnown() )
    {
      KMessageBox::error( this, i18n("You cannot specify a table here.") );
      m_cell->setFocus();
      m_cell->selectAll();
      return;
    }
    point.setSheet(sheet);
    //    if ( point.pos.x() < 1 || point.pos.y() < 1 )
    if ( !point.isValid() )
    {
      KMessageBox::error( this, i18n("You have to specify a valid cell.") );
      m_cell->setFocus();
      m_cell->selectAll();
      return;
    }
    top  = point.pos().y();
    left = point.pos().x();
  }

  int i;
  QString queryStr;
  QString tmp = m_sqlQuery->text();
  for ( i = 0; i < (int) tmp.length(); ++i )
  {
    if ( tmp[i] != '\n' )
      queryStr += tmp[i];
    else
      queryStr += " ";
  }

  Cell * cell;
  QSqlQuery query( *m_dbConnection );

  // Check the whole query for SQL that might modify database.
  // If there is an update command, then it must be at the start of the string,
  // or after an open bracket (e.g. nested update) or a space to be valid SQL.
  // An update command must also be followed by a space, or it would be parsed
  // as an identifier.
  // For sanity, also check that there is a SELECT
  QRegExp couldModifyDB( "(^|[( \\s])(UPDATE|DELETE|INSERT|CREATE) ", false /* cs */ );
  QRegExp couldQueryDB( "(^|[( \\s])(SELECT) ", false /* cs */ );

  if (couldModifyDB.indexIn( queryStr ) != -1 || couldQueryDB.indexIn( queryStr ) == -1 )
  {
    KMessageBox::error( this, i18n("You are not allowed to change data in the database.") );
    m_sqlQuery->setFocus();
    return;
  }

  if ( !query.exec( queryStr ) )
  {
    KMessageBox::error( this, i18n( "Executing query failed." ) );
    m_sqlQuery->setFocus();
    return;
  }

  if ( query.size() == 0 )
  {
    KMessageBox::error( this, i18n( "You did not get any results with this query." ) );
    m_sqlQuery->setFocus();
    return;
  }

  int y = 0;
  int count = m_columns_1->count();
  if ( width != -1 )
  {
    if ( count > width )
      count = width;
  }

  if ( height == -1 )
  {
    height = 0;
    if ( query.first() )
    {
      if ( query.isValid() )
        ++height;
    }
    while ( query.next() )
    {
      if ( query.isValid() )
        ++height;
    }
  }

  if ( !m_pView->doc()->undoLocked() )
  {
    QRect r(left, top, count, height);
    UndoInsertData * undo = new UndoInsertData( m_pView->doc(), sheet, r );
    m_pView->doc()->addCommand( undo );
  }

  m_pView->doc()->emitBeginOperation();

  if ( query.first() )
  {
    if ( query.isValid() )
    {
      for ( i = 0; i < count; ++i )
      {
        cell = sheet->nonDefaultCell( left + i, top + y );
        cell->setCellText( query.value( i ).toString() );
      }
      ++y;
    }
  }

  if ( y != height )
  {
    while ( query.next() )
    {
      if ( !query.isValid() )
        continue;

      for ( i = 0; i < count; ++i )
      {
        cell = sheet->nonDefaultCell( left + i, top + y );
        cell->setCellText( query.value( i ).toString() );
      }
      ++y;

      if ( y == height )
        break;
    }
  }

  m_pView->slotUpdateView( sheet );
  K3Wizard::accept();
}

bool DatabaseDialog::databaseDoNext()
{
  m_dbConnection = &QSqlDatabase::addDatabase( m_driver->currentText() );

  if ( m_dbConnection )
  {
    m_dbConnection->setDatabaseName( m_databaseName->text() );
    m_dbConnection->setHostName( m_host->text() );

    if ( !m_username->text().isEmpty() )
      m_dbConnection->setUserName( m_username->text() );

    if ( !m_password->text().isEmpty() )
      m_dbConnection->setPassword( m_password->text() );

    if ( !m_port->text().isEmpty() )
    {
      bool ok = false;
      int port = m_port->text().toInt( &ok );
      if (!ok)
      {
        KMessageBox::error( this, i18n("The port must be a number") );
        return false;
      }
      m_dbConnection->setPort( port );
    }

    m_databaseStatus->setText( i18n("Connecting to database...") );
    if ( m_dbConnection->open() )
    {
      m_databaseStatus->setText( i18n("Connected. Retrieving table information...") );
      QStringList sheetList( m_dbConnection->tables() );

      if ( sheetList.isEmpty() )
      {
        KMessageBox::error( this, i18n("This database contains no tables") );
        return false;
      }

      m_sheetView->clear();

      for ( int i = 0; i < sheetList.size(); ++i )
      {
        Q3CheckListItem * item = new Q3CheckListItem( m_sheetView, sheetList[i],
                                                    Q3CheckListItem::CheckBox );
        item->setOn(false);
        m_sheetView->insertItem( item );
      }

      m_sheetView->setEnabled( true );
      m_databaseStatus->setText( " " );
    }
    else
    {
      QSqlError error = m_dbConnection->lastError();
      QString errorMsg;
      QString err1 = error.driverText();
      QString err2 = error.databaseText();
      if ( !err1.isEmpty() )
      {
        errorMsg.append( error.driverText() );
        errorMsg.append( "\n" );
      }
      if ( !err2.isEmpty() && err1 != err2)
      {
        errorMsg.append( error.databaseText() );
        errorMsg.append( "\n" );
      }
      m_databaseStatus->setText( " " );

      KMessageBox::error( this, errorMsg );
      return false;
    }
  }
  else
  {
    KMessageBox::error( this, i18n("Driver could not be loaded") );
    m_databaseStatus->setText( " " );
    return false;
  }
  setNextEnabled(m_sheet, true);

  return true;
}

bool DatabaseDialog::sheetsDoNext()
{
  m_databaseStatus->setText( i18n("Retrieving meta data of tables...") );
  QStringList sheets;

  for (Q3ListViewItem * item = (Q3CheckListItem *) m_sheetView->firstChild(); item; item = item->nextSibling())
  {
    if (((Q3CheckListItem * ) item)->isOn())
    {
      sheets.append(((Q3CheckListItem * ) item)->text());
    }
  }

  if (sheets.empty())
  {
    KMessageBox::error( this, i18n("You have to select at least one table.") );
    return false;
  }

  m_columnView->clear();
  QSqlRecord info;
  Q3CheckListItem * item;
  for (int i = 0; i < (int) sheets.size(); ++i)
  {
    info = m_dbConnection->record( sheets[i] );
    for (int j = 0; j < (int) info.count(); ++j)
    {
      QString name = info.fieldName(j);
      item = new Q3CheckListItem( m_columnView, name,
                                 Q3CheckListItem::CheckBox );
      item->setOn(false);
      m_columnView->insertItem( item );
      item->setText( 1, sheets[i] );
      QSqlField field = info.field(name);
      item->setText( 2, QVariant::typeToName(field.type()) );
    }
  }
  m_columnView->setSorting(1, true);
  m_columnView->sort();
  m_columnView->setSorting( -1 );

  setNextEnabled(m_columns, true);

  return true;
}

bool DatabaseDialog::columnsDoNext()
{
  QStringList columns;
  for (Q3ListViewItem * item = m_columnView->firstChild(); item; item = item->nextSibling())
  {
    if (((Q3CheckListItem * ) item)->isOn())
    {
      columns.append( item->text(1) + "." + ((Q3CheckListItem * ) item)->text());
    }
  }

  if (columns.empty())
  {
    KMessageBox::error( this, i18n("You have to select at least one column.") );
    return false;
  }

  m_columns_1->clear();
  m_columns_2->clear();
  m_columns_3->clear();
  m_columns_1->insertItems( 0,columns);
  m_columns_2->insertItems( 0,columns);
  m_columns_3->insertItems( 0,columns);
  m_columnsSort_1->clear();
  m_columnsSort_2->clear();
  m_columnsSort_1->insertItem( i18n("None") );
  m_columnsSort_2->insertItem( i18n("None") );
  m_columnsSort_1->insertItems( 0,columns);
  m_columnsSort_2->insertItems( 0,columns);

  setNextEnabled(m_options, true);

  return true;
}


QString DatabaseDialog::getWhereCondition(QString const & column,
                                              QString const & value,
                                              int op)
{
  QString wherePart;

  switch( op )
  {
   case 0:
    wherePart += column;
    wherePart += " = ";
    break;
   case 1:
    wherePart += "NOT ";
    wherePart += column;
    wherePart += " = ";
    break;
   case 2:
    wherePart += column;
    wherePart += " IN ";
    break;
   case 3:
    wherePart += "NOT ";
    wherePart += column;
    wherePart += " IN ";
    break;
   case 4:
    wherePart += column;
    wherePart += " LIKE ";
    break;
   case 5:
    wherePart += column;
    wherePart += " > ";
    break;
   case 6:
    wherePart += column;
    wherePart += " < ";
    break;
   case 7:
    wherePart += column;
    wherePart += " >= ";
    break;
   case 8:
    wherePart += column;
    wherePart += " <= ";
    break;
  }

  if ( op != 2 && op != 3 )
  {
    QString val;
    bool ok = false;
    value.toDouble(&ok);

    if ( !ok )
    {
      if (value[0] != '\'')
        val = "'";

      val += value;

      if (value[value.length() - 1] != '\'')
        val += "'";
    }
    else
      val = value;

    wherePart += val;
  }
  else  // "in" & "not in"
  {
    QString val;
    if (value[0] != '(')
      val = "(";
    val += value;
    if ( value[value.length() - 1] != ')' )
      val += ")";
    wherePart += val;
  }

  return wherePart;
}

QString DatabaseDialog::exchangeWildcards(QString const & value)
{
  QString str(value);
  int p = str.indexOf('*');
  while ( p > -1 )
  {
    str = str.replace( p, 1, "%" );
    p = str.indexOf('*');
  }

  p = str.indexOf('?');
  while ( p > -1 )
  {
    str = str.replace( p, 1, "_" );
    p = str.indexOf('?');
  }
  return str;
}

bool DatabaseDialog::optionsDoNext()
{
  if ( m_operator_1->currentIndex() == 4 )
  {
    if ( ( m_operatorValue_1->text().indexOf('*') != -1 )
         || ( m_operatorValue_1->text().indexOf('?') != -1 ) )
    {
      // xgettext: no-c-format
      int res = KMessageBox::warningYesNo( this, i18n("'*' or '?' are not valid wildcards in SQL. "
                                                      "The proper replacements are '%' or '_'. Do you want to replace them?") );

      if ( res == KMessageBox::Yes )
        m_operatorValue_1->setText(exchangeWildcards(m_operatorValue_1->text()));
    }
  }

  if ( m_operator_2->currentIndex() == 4 )
  {
    if ( ( m_operatorValue_2->text().indexOf('*') != -1 )
         || ( m_operatorValue_2->text().indexOf('?') != -1 ) )
    {
      // xgettext: no-c-format
      int res = KMessageBox::warningYesNo( this, i18n("'*' or '?' are not valid wildcards in SQL. "
                                                      "The proper replacements are '%' or '_'. Do you want to replace them?") );

      if ( res == KMessageBox::Yes )
        m_operatorValue_2->setText(exchangeWildcards(m_operatorValue_2->text()));
    }
  }

  if ( m_operator_3->currentIndex() == 4 )
  {
    if ( ( m_operatorValue_3->text().indexOf('*') != -1 )
         || ( m_operatorValue_3->text().indexOf('?') != -1 ) )
    {
      // xgettext: no-c-format
      int res = KMessageBox::warningYesNo( this, i18n("'*' or '?' are not valid wildcards in SQL. "
                                                      "The proper replacements are '%' or '_'. Do you want to replace them?") );

      if ( res == KMessageBox::Yes )
        m_operatorValue_3->setText(exchangeWildcards(m_operatorValue_3->text()));
    }
  }

  QString query("SELECT ");

  if (m_distinct->isChecked())
    query += "DISTINCT ";

  int i;
  int l = m_columns_1->count() - 1;
  for ( i = 0; i < l; ++i )
  {
    query += m_columns_1->text( i );
    query += ", ";
  }
  query += m_columns_1->text( l );

  query += "\nFROM ";

  Q3ListViewItem * item = (Q3CheckListItem *) m_sheetView->firstChild();
  bool b = false;
  while ( item )
  {
    if (((Q3CheckListItem * ) item)->isOn())
    {
      if ( b )
        query += ", ";
      b = true;
      query += ((Q3CheckListItem * ) item)->text();
    }
    item = item->nextSibling();
  }

  if ( ( !m_operatorValue_1->text().isEmpty() )
       || ( !m_operatorValue_2->text().isEmpty() )
       || ( !m_operatorValue_3->text().isEmpty() ) )
    query += "\nWHERE ";

  bool added = false;
  if ( !m_operatorValue_1->text().isEmpty() )
  {
    query += getWhereCondition(m_columns_1->currentText(),
                               m_operatorValue_1->text(),
                               m_operator_1->currentIndex());
    added = true;
  }

  if ( !m_operatorValue_2->text().isEmpty() )
  {
    if (added)
      query += ( m_andBox->isChecked() ? " AND " : " OR " );

    query += getWhereCondition(m_columns_2->currentText(),
                               m_operatorValue_2->text(),
                               m_operator_2->currentIndex());
    added = true;
  }

  if ( !m_operatorValue_3->text().isEmpty() )
  {
    if (added)
      query += ( m_andBox->isChecked() ? " AND " : " OR " );

    query += getWhereCondition(m_columns_3->currentText(),
                               m_operatorValue_3->text(),
                               m_operator_3->currentIndex());
  }

  if ( (m_columnsSort_1->currentIndex() != 0)
       || (m_columnsSort_2->currentIndex() != 0) )
  {
    query += "\nORDER BY ";
    bool added = false;
    if ( m_columnsSort_1->currentIndex() != 0 )
    {
      added = true;
      query += m_columnsSort_1->currentText();
      if ( m_sortMode_1->currentIndex() == 1 )
        query += " DESC ";
    }

    if ( m_columnsSort_2->currentIndex() != 0 )
    {
      if ( added )
        query += ", ";

      query += m_columnsSort_2->currentText();
      if ( m_sortMode_2->currentIndex() == 1 )
        query += " DESC ";
    }
  }

  m_sqlQuery->setText(query);
  m_cell->setText(Cell::name( m_targetRect.left(), m_targetRect.top() ) );
  m_region->setText(util_rangeName( m_targetRect ) );

  setFinishEnabled( m_result, true );

  return true;
}

void DatabaseDialog::orBox_clicked()
{
  m_andBox->setChecked( false );
  m_orBox->setChecked( true );
}

void DatabaseDialog::andBox_clicked()
{
  m_andBox->setChecked( true );
  m_orBox->setChecked( false );
}

void DatabaseDialog::startingCell_clicked()
{
  m_startingCell->setChecked( true );
  m_startingRegion->setChecked( false );
}

void DatabaseDialog::startingRegion_clicked()
{
  m_startingCell->setChecked( false );
  m_startingRegion->setChecked( true );
}

void DatabaseDialog::connectButton_clicked()
{
  qWarning( "DatabaseDialog::connectButton_clicked(): Not implemented yet!" );
}

void DatabaseDialog::databaseNameChanged(const QString & s)
{
  if ( !m_driver->currentText().isEmpty() && !s.isEmpty()
       && !m_host->text().isEmpty() )
    setNextEnabled(m_database, true);
  else
    setNextEnabled(m_database, false);
}

void DatabaseDialog::databaseHostChanged(const QString & s)
{
  if ( !m_driver->currentText().isEmpty() && !s.isEmpty()
       && !m_databaseName->text().isEmpty() )
    setNextEnabled(m_database, true);
  else
    setNextEnabled(m_database, false);
}

void DatabaseDialog::databaseDriverChanged(int index)
{
  if ( index > 0 && !m_host->text().isEmpty()
       && !m_databaseName->text().isEmpty() )
    setNextEnabled(m_database, true);
  else
    setNextEnabled(m_database, false);
}

void DatabaseDialog::popupSheetViewMenu( Q3ListViewItem *, const QPoint &, int )
{
  // TODO: popup menu with "Select All", "Inverse selection", "remove selection"
}

void DatabaseDialog::sheetViewClicked( Q3ListViewItem * )
{
//   if ( item )
//   {
//     QCheckListItem * i = (QCheckListItem *) item;
//     i->setChecked( !i->isChecked() );
//   }
//   kDebug() << "clicked" << endl;
}


#include "kspread_dlg_database.moc"

#endif // QT_NO_SQL
