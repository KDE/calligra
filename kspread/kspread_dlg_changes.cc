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

FilterMain::FilterMain( FilterSettings * settings, QWidget * parent, 
                        const char * name, WFlags fl )
  : QWidget( parent, name, fl ),
    m_filterSettings( settings )
{
  setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, 
                              0, 0, sizePolicy().hasHeightForWidth() ) );
  QGridLayout * FilterMainLayout = new QGridLayout( this, 1, 1, 11, 6, "FilterMainLayout"); 

  m_dateBox = new QCheckBox( this, "m_dateBox" );
  m_dateBox->setText( i18n( "&Date" ) );
  connect( m_dateBox, SIGNAL( toggled( bool ) ), this, SLOT( slotDateStateChanged( bool ) ) );
  FilterMainLayout->addWidget( m_dateBox, 0, 0 );

  m_authorBox = new QCheckBox( this, "m_authorBox" );
  m_authorBox->setText( i18n( "&Author" ) );
  connect( m_authorBox, SIGNAL( toggled( bool ) ), this, SLOT( slotAuthorStateChanged( bool ) ) );
  FilterMainLayout->addWidget( m_authorBox, 2, 0 );

  m_rangeBox = new QCheckBox( this, "m_rangeBox" );
  m_rangeBox->setText( i18n( "&Range" ) );
  connect( m_rangeBox, SIGNAL( toggled( bool ) ), this, SLOT( slotRangeStateChanged( bool ) ) );
  FilterMainLayout->addWidget( m_rangeBox, 3, 0 );

  m_commentBox = new QCheckBox( this, "m_commentBox" );
  m_commentBox->setText( i18n( "&Comment" ) );
  connect( m_commentBox, SIGNAL( toggled( bool ) ), this, SLOT( slotCommentStateChanged( bool ) ) );
  FilterMainLayout->addWidget( m_commentBox, 4, 0 );
    
  m_authorEdit = new KLineEdit( this, "m_authorEdit" );
  connect( m_authorEdit, SIGNAL( textChanged( QString const & ) ),
           this, SLOT( slotAuthorChanged( QString const & ) ) );
  FilterMainLayout->addMultiCellWidget( m_authorEdit, 2, 2, 1, 2 );

  m_rangeEdit = new KLineEdit( this, "m_rangeEdit" );
  connect( m_rangeEdit, SIGNAL( textChanged( QString const & ) ),
           this, SLOT( slotRangeChanged( QString const & ) ) );
  FilterMainLayout->addMultiCellWidget( m_rangeEdit, 3, 3, 1, 2 );

  QSpacerItem * spacer = new QSpacerItem( 20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding );
  FilterMainLayout->addItem( spacer, 5, 1 );

  m_commentEdit = new KLineEdit( this, "m_commentEdit" );
  connect( m_commentEdit, SIGNAL( textChanged( QString const & ) ),
           this, SLOT( slotCommentChanged( QString const & ) ) );
  FilterMainLayout->addMultiCellWidget( m_commentEdit, 4, 4, 1, 2 );

  m_dateUsage = new KComboBox( FALSE, this, "m_dateUsage" );
  m_dateUsage->insertItem( i18n( "earlier than" ) );
  m_dateUsage->insertItem( i18n( "since" ) );
  m_dateUsage->insertItem( i18n( "equal to" ) );
  m_dateUsage->insertItem( i18n( "not equal to" ) );
  m_dateUsage->insertItem( i18n( "between" ) );
  FilterMainLayout->addWidget( m_dateUsage, 0, 1 );
  connect( m_dateUsage, SIGNAL( activated( int ) ),
           this, SLOT( slotDateUsageChanged( int ) ) );

  QLabel * textLabel = new QLabel( this, "textLabel" );
  textLabel->setText( i18n( "<p align=\"right\">and</p>" ) );
  FilterMainLayout->addWidget( textLabel, 1, 1 );

  QVBoxLayout * layout = new QVBoxLayout( 0, 0, 6, "layout"); 

  m_timeFirst = new QDateTimeEdit( this, "m_timeFirst" );
  m_timeFirst->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)5, 
                                           0, 0, m_timeFirst->sizePolicy().hasHeightForWidth() ) );
  connect( m_timeFirst, SIGNAL( valueChanged( QDateTime ) ),
           this, SLOT( slotFirstTimeChanged( QDateTime const & ) ) );
  layout->addWidget( m_timeFirst );

  m_timeSecond = new QDateTimeEdit( this, "m_timeSecond" );
  m_timeSecond->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)5, 
                                            0, 0, m_timeSecond->sizePolicy().hasHeightForWidth() ) );
  connect( m_timeSecond, SIGNAL( valueChanged( QDateTime ) ),
           this, SLOT( slotSecondTimeChanged( QDateTime const & ) ) );
  layout->addWidget( m_timeSecond );
  FilterMainLayout->addMultiCellLayout( layout, 0, 1, 2, 2 );

  m_dateBox->setChecked( settings->dateSet() );
  slotDateStateChanged( settings->dateSet() );

  m_authorBox->setChecked( settings->authorSet() );
  slotAuthorStateChanged( settings->authorSet() );

  m_commentBox->setChecked( settings->commentSet() );
  slotCommentStateChanged( settings->commentSet() );

  m_rangeBox->setChecked( settings->rangeSet() );
  slotRangeStateChanged( settings->rangeSet() );

  resize( QSize(549, 196).expandedTo(minimumSizeHint()) );
}

FilterMain::~FilterMain()
{
}

void FilterMain::slotDateUsageChanged( int index )
{
  if ( index == 4 )
    m_timeSecond->setEnabled( true );
  else
    m_timeSecond->setEnabled( false );

  m_filterSettings->m_dateUsage = index;
}

void FilterMain::slotDateStateChanged( bool on )
{
  if ( on )
  {
    m_dateUsage->setEnabled( true );
    m_timeFirst->setEnabled( true );
    if ( m_dateUsage->currentItem() == 4 )
      m_timeSecond->setEnabled( true );
  }
  else
  {
    m_dateUsage->setEnabled( false );
    m_timeFirst->setEnabled( false );
    m_timeSecond->setEnabled( false );
  }
  m_filterSettings->m_dateSet = on;
}

void FilterMain::slotAuthorStateChanged( bool on )
{
  if ( on )
    m_authorEdit->setEnabled( true );
  else
    m_authorEdit->setEnabled( false );

  m_filterSettings->m_authorSet = on;
}

void FilterMain::slotCommentStateChanged( bool on )
{
  if ( on )
    m_commentEdit->setEnabled( true );
  else
    m_commentEdit->setEnabled( false );

  m_filterSettings->m_commentSet = on;
}

void FilterMain::slotRangeStateChanged( bool on )
{
  if ( on )
    m_rangeEdit->setEnabled( true );
  else
    m_rangeEdit->setEnabled( false );

  m_filterSettings->m_rangeSet = on;
}

void FilterMain::slotCommentChanged( QString const & text )
{
  m_filterSettings->m_comment = text;
}

void FilterMain::slotAuthorChanged( QString const & text )
{
  m_filterSettings->m_author = text;
}

void FilterMain::slotRangeChanged( QString const & text )
{
  m_filterSettings->m_range = text;
}

void FilterMain::slotFirstTimeChanged( QDateTime const & dt )
{
  m_filterSettings->m_firstTime = dt;
}

void FilterMain::slotSecondTimeChanged( QDateTime const & dt )
{
  m_filterSettings->m_secondTime = dt;
}


AcceptRejectWidget::AcceptRejectWidget( FilterSettings * settings, QWidget * parent, 
                                        const char * name, WFlags fl )
  : QWidget( parent, name, fl ),
    m_filterSettings( settings )
{
  QHBoxLayout * layout = new QHBoxLayout( this, 11, 6, "layout"); 
  QTabWidget  * listTab  = new QTabWidget( this, "listTab" );

  m_listTab         = new QWidget( listTab, "m_listTab" );
  QGridLayout * tabLayout   = new QGridLayout( m_listTab, 1, 1, 11, 6, "tabLayout"); 

  m_acceptButton = new KPushButton( m_listTab, "m_acceptButton" );
  m_acceptButton->setText( i18n( "&Accept" ) );
  m_acceptButton->setEnabled( false );
  tabLayout->addWidget( m_acceptButton, 1, 0 );

  m_rejectButton = new KPushButton( m_listTab, "m_rejectButton" );
  m_rejectButton->setText( i18n( "&Reject" ) );
  m_rejectButton->setEnabled( false );
  tabLayout->addWidget( m_rejectButton, 1, 1 );

  m_acceptAllButton = new KPushButton( m_listTab, "m_AcceptAllButton" );
  m_acceptAllButton->setText( i18n( "&Accept All" ) );
  m_acceptAllButton->setEnabled( false );
  tabLayout->addWidget( m_acceptAllButton, 1, 2 );

  m_rejectAllButton = new KPushButton( m_listTab, "m_rejectAllButton" );
  m_rejectAllButton->setText( i18n( "&Reject All" ) );
  m_rejectAllButton->setEnabled( false );
  tabLayout->addWidget( m_rejectAllButton, 1, 3 );

  m_listView = new KListView( m_listTab, "m_listView" );
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
  listTab->insertTab( m_listTab, i18n( "&List" ) );

  QWidget * tabFilter = new QWidget( listTab, "tabFilter" );
  QVBoxLayout * filterLayout = new QVBoxLayout( tabFilter, 1, 1, "fLayout"); 

  m_filter = new FilterMain( settings, tabFilter );
  m_filter->m_timeSecond->setEnabled( false );
  filterLayout->addWidget( m_filter, 0, 0 );

  listTab->insertTab( tabFilter, i18n( "&Filter" ) );
  layout->addWidget( listTab );

  connect( listTab, SIGNAL( currentChanged( QWidget * ) ),
           this, SLOT( slotTabChanged( QWidget * ) ) );

  resize( QSize(682, 480).expandedTo(minimumSizeHint()) );
}

AcceptRejectWidget::~AcceptRejectWidget()
{
}

void AcceptRejectWidget::slotTabChanged( QWidget * widget )
{
  if ( widget == m_listTab )
    applyFilterSettings();
}

void AcceptRejectWidget::applyFilterSettings()
{
  // TODO
}

FilterDlg::FilterDlg( FilterSettings * settings, QWidget * parent, 
                      const char * name, WFlags fl )
  : QWidget( parent, name, fl )
{
  setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, 
                              sizePolicy().hasHeightForWidth() ) );

  QGridLayout * layout = new QGridLayout( this, 1, 1, 11, 6, "Form1Layout"); 
  QSpacerItem * spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
  layout->addItem( spacer, 2, 0 );

  QFrame * frame = new QFrame( this, "frame4" );
  frame->setFrameShape( QFrame::NoFrame );
  frame->setFrameShadow( QFrame::Plain );

  QGridLayout * frameLayout = new QGridLayout( frame, 1, 1, 11, 6, "frame4Layout"); 

  m_showChanges = new QCheckBox( frame, "m_showChanges" );
  m_showChanges->setText( i18n( "&Show changes in document" ) );
  frameLayout->addWidget( m_showChanges, 0, 0 );

  m_showAccepted = new QCheckBox( frame, "m_showAccepted" );
  m_showAccepted->setText( i18n( "Show &accepted changes" ) );
  frameLayout->addWidget( m_showAccepted, 1, 0 );

  m_showRejected = new QCheckBox( frame, "m_showRejected" );
  m_showRejected->setText( i18n( "Show &rejected changes" ) );
  frameLayout->addWidget( m_showRejected, 2, 0 );

  layout->addWidget( frame, 0, 0 );

  m_filterMain = new FilterMain( settings, this );
  m_filterMain->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 
                                            0, 0, m_filterMain->sizePolicy().hasHeightForWidth() ) );
  layout->addWidget( m_filterMain, 1, 0 );

  resize( QSize(539, 500).expandedTo(minimumSizeHint()) );
}

FilterDlg::~FilterDlg()
{
}


KSpreadFilterDlg::KSpreadFilterDlg( KSpreadView * parent, KSpreadChanges * changes, 
                                    const char * name )
  : KDialogBase( parent, name, true, "",
                 KDialogBase::Ok | KDialogBase::Cancel, KDialogBase::Ok, false ),
    m_view( parent ),
    m_changes( changes ),
    m_dlg( new FilterDlg( &changes->m_filterSettings, this ) )
{
  setCaption( i18n( "Filter Changes" ) );
  setButtonBoxOrientation( Vertical );
  setMainWidget( m_dlg );
}

KSpreadFilterDlg::~KSpreadFilterDlg()
{
}
  
KSpreadAcceptDlg::KSpreadAcceptDlg( KSpreadView * parent, KSpreadChanges * changes,
                                    const char * name )
  : KDialogBase( parent, name, true, "",
                 KDialogBase::Close, KDialogBase::Close, false ),   
    m_view( parent ),
    m_changes( changes ),
    m_dialog( new AcceptRejectWidget( &changes->m_filterSettings, this ) )
{
  setCaption( i18n( "Accept or Reject Changes" ) );
  setButtonBoxOrientation( Vertical );
  setMainWidget( m_dialog );
}

KSpreadAcceptDlg::~KSpreadAcceptDlg()
{
}

#include "kspread_dlg_changes.moc"


