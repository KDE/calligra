/* This file is part of the KDE project
   Copyright (C) 2002 Norbert Andres, nandres@web.de

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

#include "kspread_changes.h"
#include "kspread_dlg_changes.h"
#include "kspread_view.h"

#include <kcombobox.h>
#include <kdebug.h>
#include <klineedit.h>
#include <klistview.h>
#include <klocale.h>
#include <kpushbutton.h>

#include <qcheckbox.h>
#include <qdatetimeedit.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtabwidget.h>

FilterMain::FilterMain( QWidget * parent, const char * name, WFlags fl )
  : QWidget( parent, name, fl )
{
  setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, 
                              0, 0, sizePolicy().hasHeightForWidth() ) );
  QGridLayout * FilterMainLayout = new QGridLayout( this, 1, 1, 11, 6, "FilterMainLayout"); 

  m_dateBox = new QCheckBox( this, "m_dateBox" );
  m_dateBox->setText( i18n( "&Date" ) );
  FilterMainLayout->addWidget( m_dateBox, 0, 0 );

  m_authorBox = new QCheckBox( this, "m_authorBox" );
  m_authorBox->setText( i18n( "&Author" ) );
  FilterMainLayout->addWidget( m_authorBox, 2, 0 );

  m_rangeBox = new QCheckBox( this, "m_rangeBox" );
  m_rangeBox->setText( i18n( "&Range" ) );
  FilterMainLayout->addWidget( m_rangeBox, 3, 0 );

  m_commentBox = new QCheckBox( this, "m_commentBox" );
  m_commentBox->setText( i18n( "&Comment" ) );
  FilterMainLayout->addWidget( m_commentBox, 4, 0 );
    
  m_authorEdit = new KLineEdit( this, "m_authorEdit" );
  FilterMainLayout->addMultiCellWidget( m_authorEdit, 2, 2, 1, 2 );

  m_rangeEdit = new KLineEdit( this, "m_rangeEdit" );
  FilterMainLayout->addMultiCellWidget( m_rangeEdit, 3, 3, 1, 2 );

  QSpacerItem * spacer = new QSpacerItem( 20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding );
  FilterMainLayout->addItem( spacer, 5, 1 );

  m_commentEdit = new KLineEdit( this, "m_commentEdit" );
  FilterMainLayout->addMultiCellWidget( m_commentEdit, 4, 4, 1, 2 );

  m_dateUsage = new KComboBox( FALSE, this, "m_dateUsage" );
  FilterMainLayout->addWidget( m_dateUsage, 0, 1 );

  QLabel * textLabel = new QLabel( this, "textLabel" );
  textLabel->setText( i18n( "<p align=\"right\">and</p>" ) );
  FilterMainLayout->addWidget( textLabel, 1, 1 );

  QVBoxLayout * layout = new QVBoxLayout( 0, 0, 6, "layout"); 

  m_timeFirst = new QDateTimeEdit( this, "m_timeFirst" );
  m_timeFirst->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)5, 
                                           0, 0, m_timeFirst->sizePolicy().hasHeightForWidth() ) );
  layout->addWidget( m_timeFirst );

  m_timeSecond = new QDateTimeEdit( this, "m_timeSecond" );
  m_timeSecond->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)5, 
                                            0, 0, m_timeSecond->sizePolicy().hasHeightForWidth() ) );
  layout->addWidget( m_timeSecond );
  FilterMainLayout->addMultiCellLayout( layout, 0, 1, 2, 2 );


    resize( QSize(549, 196).expandedTo(minimumSizeHint()) );
}

FilterMain::~FilterMain()
{
}


AcceptRejectWidget::AcceptRejectWidget( QWidget * parent, const char * name, WFlags fl )
  : QWidget( parent, name, fl )
{
  QHBoxLayout * layout = new QHBoxLayout( this, 11, 6, "layout"); 
  QTabWidget  * listTab  = new QTabWidget( this, "listTab" );

  QWidget     * tab         = new QWidget( listTab, "tab" );
  QGridLayout * tabLayout   = new QGridLayout( tab, 1, 1, 11, 6, "tabLayout"); 

  m_acceptButton = new KPushButton( tab, "m_acceptButton" );
  m_acceptButton->setText( i18n( "&Accept" ) );
  m_acceptButton->setEnabled( false );
  tabLayout->addWidget( m_acceptButton, 1, 0 );

  m_rejectButton = new KPushButton( tab, "m_rejectButton" );
  m_rejectButton->setText( i18n( "&Reject" ) );
  m_rejectButton->setEnabled( false );
  tabLayout->addWidget( m_rejectButton, 1, 1 );

  m_acceptAllButton = new KPushButton( tab, "m_AcceptAllButton" );
  m_acceptAllButton->setText( i18n( "&Accept All" ) );
  m_acceptAllButton->setEnabled( false );
  tabLayout->addWidget( m_acceptAllButton, 1, 2 );

  m_rejectAllButton = new KPushButton( tab, "m_rejectAllButton" );
  m_rejectAllButton->setText( i18n( "&Reject All" ) );
  m_rejectAllButton->setEnabled( false );
  tabLayout->addWidget( m_rejectAllButton, 1, 3 );

  m_listView = new KListView( tab, "m_listView" );
  m_listView->addColumn( i18n( "Action" ) );
  m_listView->addColumn( i18n( "Position" ) );
  m_listView->addColumn( i18n( "Author" ) );
  m_listView->addColumn( i18n( "Date" ) );
  m_listView->addColumn( i18n( "Comment" ) );

  m_listView->header()->setLabel( 0, i18n( "Action" ) );
  m_listView->header()->setLabel( 1, i18n( "Position" ) );
  m_listView->header()->setLabel( 2, i18n( "Author" ) );
  m_listView->header()->setLabel( 3, i18n( "Date" ) );
  m_listView->header()->setLabel( 4, i18n( "Comment" ) );
  
  tabLayout->addMultiCellWidget( m_listView, 0, 0, 0, 3 );
  listTab->insertTab( tab, i18n( "&List" ) );

  QWidget * tabFilter = new QWidget( listTab, "tabFilter" );
  QVBoxLayout * filterLayout = new QVBoxLayout( tabFilter, 1, 1, "fLayout"); 

  m_filter = new FilterMain( tabFilter );
  filterLayout->addWidget( m_filter, 0, 0 );

  listTab->insertTab( tabFilter, i18n( "&Filter" ) );
  layout->addWidget( listTab );

  resize( QSize(682, 480).expandedTo(minimumSizeHint()) );
}

AcceptRejectWidget::~AcceptRejectWidget()
{
}



KSpreadAcceptDlg::KSpreadAcceptDlg( KSpreadView * parent, KSpreadChanges * changes,
                                    const char * name )
  : KDialogBase( parent, name, true, "",
                 KDialogBase::Close, KDialogBase::Close, false ),   
    m_view( parent ),
    m_changes( changes ),
    m_dialog( new AcceptRejectWidget( this ) )
{
  setCaption( i18n( "Accept or Reject Changes" ) );
  setButtonBoxOrientation( Vertical );
  setMainWidget( m_dialog );
}

KSpreadAcceptDlg::~KSpreadAcceptDlg()
{
}

#include "kspread_dlg_changes.moc"


