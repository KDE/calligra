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

#include "kspread_canvas.h"
#include "kspread_changes.h"
#include "kspread_dlg_changes.h"
#include "kspread_doc.h"
#include "kspread_sheet.h"
#include "kspread_util.h"
#include "kspread_view.h"

#include <kcombobox.h>
#include <kdebug.h>
#include <klistview.h>
#include <klocale.h>
#include <kpushbutton.h>

#include <qcheckbox.h>
#include <qdatetime.h>
#include <qdatetimeedit.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qtabwidget.h>
#include <qtextedit.h>

CommentDlg::CommentDlg( QWidget * parent, const char * name, WFlags fl )
  : QWidget( parent, name, fl )
{
  QGridLayout * dlgLayout = new QGridLayout( this, 1, 1, 11, 6, "dlgLayout"); 

  m_comment = new QTextEdit( this, "m_comment" );

  dlgLayout->addMultiCellWidget( m_comment, 1, 1, 0, 4 );
  QSpacerItem * spacer = new QSpacerItem( 91, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  dlgLayout->addItem( spacer, 2, 4 );

  QVBoxLayout * layout2 = new QVBoxLayout( 0, 0, 6, "layout2"); 

  QLabel * textLabel1 = new QLabel( this, "textLabel1" );
  textLabel1->setText( i18n( "Author:" ) );
  layout2->addWidget( textLabel1 );
  QSpacerItem * spacer_2 = new QSpacerItem( 20, 21, QSizePolicy::Minimum, QSizePolicy::Expanding );
  layout2->addItem( spacer_2 );

  QLabel * textLabel2 = new QLabel( this, "textLabel2" );
  textLabel2->setText( i18n( "Subject:" ) );
  layout2->addWidget( textLabel2 );
  QSpacerItem * spacer_3 = new QSpacerItem( 20, 21, QSizePolicy::Minimum, QSizePolicy::Expanding );
  layout2->addItem( spacer_3 );

  QLabel * textLabel3 = new QLabel( this, "textLabel3" );
  textLabel3->setText( i18n( "Comment:" ) );
  layout2->addWidget( textLabel3 );

  dlgLayout->addMultiCellLayout( layout2, 0, 0, 0, 1 );
  QSpacerItem * spacer_4 = new QSpacerItem( 110, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  dlgLayout->addItem( spacer_4, 2, 0 );

  QVBoxLayout * layout1 = new QVBoxLayout( 0, 0, 6, "layout1"); 

  m_author = new QLabel( this, "m_author" );
  m_author->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
  layout1->addWidget( m_author );

  m_subject = new QLabel( this, "m_subject" );
  m_subject->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
  layout1->addWidget( m_subject );
  QSpacerItem* spacer_5 = new QSpacerItem( 221, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  layout1->addItem( spacer_5 );

  dlgLayout->addMultiCellLayout( layout1, 0, 0, 2, 4 );

  m_nextButton = new KPushButton( this, "m_nextButton" );
  m_nextButton->setMinimumSize( QSize( 100, 0 ) );
  m_nextButton->setText( i18n( "&Next" ) );

  dlgLayout->addWidget( m_nextButton, 2, 3 );

  m_previousButton = new KPushButton( this, "m_previousButton" );
  m_previousButton->setMinimumSize( QSize( 100, 0 ) );
  m_previousButton->setText( i18n( "&Previous" ) );

  dlgLayout->addMultiCellWidget( m_previousButton, 2, 2, 1, 2 );
  resize( QSize(600, 362).expandedTo(minimumSizeHint()) );
}

CommentDlg::~CommentDlg()
{
}

FilterMain::FilterMain( FilterSettings * settings, QWidget * parent, 
                        const char * name, WFlags fl )
  : QWidget( parent, name, fl ),
    m_filterSettings( settings )
{
  setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, 
                              0, 0, sizePolicy().hasHeightForWidth() ) );
  QGridLayout * FilterMainLayout = new QGridLayout( this, 1, 1, 11, 6, "FilterMainLayout"); 

  m_dateBox = new QCheckBox( this, "m_dateBox" );
  m_dateBox->setText( i18n( "&Date:" ) );
  connect( m_dateBox, SIGNAL( toggled( bool ) ), this, SLOT( slotDateStateChanged( bool ) ) );
  FilterMainLayout->addWidget( m_dateBox, 0, 0 );

  m_authorBox = new QCheckBox( this, "m_authorBox" );
  m_authorBox->setText( i18n( "&Author:" ) );
  connect( m_authorBox, SIGNAL( toggled( bool ) ), this, SLOT( slotAuthorStateChanged( bool ) ) );
  FilterMainLayout->addWidget( m_authorBox, 2, 0 );

  m_rangeBox = new QCheckBox( this, "m_rangeBox" );
  m_rangeBox->setText( i18n( "&Range:" ) );
  connect( m_rangeBox, SIGNAL( toggled( bool ) ), this, SLOT( slotRangeStateChanged( bool ) ) );
  FilterMainLayout->addWidget( m_rangeBox, 3, 0 );

  m_commentBox = new QCheckBox( this, "m_commentBox" );
  m_commentBox->setText( i18n( "&Comment:" ) );
  connect( m_commentBox, SIGNAL( toggled( bool ) ), this, SLOT( slotCommentStateChanged( bool ) ) );
  FilterMainLayout->addWidget( m_commentBox, 4, 0 );
    
  m_authorEdit = new QLineEdit( this, "m_authorEdit" );
  connect( m_authorEdit, SIGNAL( textChanged( const QString & ) ),
           this, SLOT( slotAuthorChanged( const QString & ) ) );
  FilterMainLayout->addMultiCellWidget( m_authorEdit, 2, 2, 1, 2 );

  m_rangeEdit = new QLineEdit( this, "m_rangeEdit" );
  connect( m_rangeEdit, SIGNAL( textChanged( const QString & ) ),
           this, SLOT( slotRangeChanged( const QString & ) ) );
  FilterMainLayout->addMultiCellWidget( m_rangeEdit, 3, 3, 1, 2 );

  QSpacerItem * spacer = new QSpacerItem( 20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding );
  FilterMainLayout->addItem( spacer, 5, 1 );

  m_commentEdit = new QLineEdit( this, "m_commentEdit" );
  connect( m_commentEdit, SIGNAL( textChanged( const QString & ) ),
           this, SLOT( slotCommentChanged( const QString & ) ) );
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
  m_timeFirst->setDateTime( QDateTime::currentDateTime() );
  connect( m_timeFirst, SIGNAL( valueChanged ( const QDateTime & ) ),
           this, SLOT( slotFirstTimeChanged( const QDateTime & ) ) );
  layout->addWidget( m_timeFirst );

  m_timeSecond = new QDateTimeEdit( this, "m_timeSecond" );
  m_timeSecond->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)5, 
                                            0, 0, m_timeSecond->sizePolicy().hasHeightForWidth() ) );
  m_timeSecond->setDateTime( m_timeFirst->dateTime() );
  connect( m_timeSecond, SIGNAL( valueChanged( const QDateTime & ) ),
           this, SLOT( slotSecondTimeChanged( const QDateTime & ) ) );
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
  m_listView->setRootIsDecorated( true );
  
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


KSpreadCommentDlg::KSpreadCommentDlg( KSpreadView * parent, KSpreadChanges * changes, 
                                      const char * name )
  : KDialogBase( parent, name, true, "",
                 KDialogBase::Ok | KDialogBase::Cancel, KDialogBase::Ok, false ),
    m_view( parent ),
    m_changes( changes ),
    m_dlg( new CommentDlg( this ) )
{
  setCaption( i18n( "Edit Comments" ) );
  setButtonBoxOrientation( Vertical );
  setMainWidget( m_dlg );

  connect( m_dlg->m_nextButton, SIGNAL( clicked() ), this, SLOT( slotNext() ) );
  connect( m_dlg->m_previousButton, SIGNAL( clicked() ), this, SLOT( slotPrevious() ) );

  m_dlg->m_nextButton->setEnabled( false );
  m_dlg->m_previousButton->setEnabled( false );
  m_dlg->m_comment->setTextFormat( Qt::PlainText );

  m_begin   = m_changes->m_changeRecords.begin();
  m_current = m_changes->m_changeRecords.begin();
  m_end     = m_changes->m_changeRecords.end();
  
  while ( m_current != m_end )
  {
    KSpreadChanges::ChangeRecord * record = m_current.data();
    if ( record->m_state == KSpreadChanges::ChangeRecord::PENDING )
    {
      addData( record );

      break;
    }

    ++m_current;
  }

  KSpreadChanges::RecordMap::iterator i = m_current;
  if ( ++i != m_end )
    m_dlg->m_nextButton->setEnabled( true );

  if ( m_current != m_begin )
    m_dlg->m_previousButton->setEnabled( true );
}

KSpreadCommentDlg::~KSpreadCommentDlg()
{
  CommentList::iterator iter = m_comments.begin();
  CommentList::iterator end  = m_comments.end();

  while ( iter != end )
  {
    QString * s = iter.data();
    delete s;

    ++iter;
  }
  m_comments.clear();
}

void KSpreadCommentDlg::addData( KSpreadChanges::ChangeRecord * record )
{
  KSpreadChanges::CellChange * ch = 0;
  QString action1;
  QString newValue;
  QString cellName( record->m_table->tableName() + '!' + 
                    util_encodeColumnLabelText( record->m_cell.x() )
                    + QString::number( record->m_cell.y() ) );

  if ( record->m_dependants.first() )
  {
    KSpreadChanges::ChangeRecord * r = record->m_dependants.first();
    if ( r->m_type == KSpreadChanges::ChangeRecord::CELL )
      newValue = ((KSpreadChanges::CellChange *) (r->m_change))->oldValue;
  }
  else
    newValue = record->m_table->cellAt( record->m_cell.x(), 
                                        record->m_cell.y() )->text();

  switch( record->m_type )
  {
   case KSpreadChanges::ChangeRecord::CELL:
    ch = (KSpreadChanges::CellChange *) record->m_change;

    action1 += i18n( "(Cell %1 changed from '%2' to '%3')" )
      .arg( cellName )
      .arg( ch->oldValue.length() > 0 ? ch->oldValue : i18n( "<empty>" ) )
      .arg( newValue.length() > 0 ? newValue : i18n( "<empty>" ) );
    break;
    
   case KSpreadChanges::ChangeRecord::INSERTCOLUMN:
    action1 = i18n( "Inserted column" );
    break;
    
   case KSpreadChanges::ChangeRecord::INSERTROW:
    action1 = i18n( "Inserted row" );
    break;
    
   case KSpreadChanges::ChangeRecord::INSERTTABLE:
    action1 = i18n( "Inserted table" );
    break;
    
   case KSpreadChanges::ChangeRecord::DELETECOLUMN:
    action1 = i18n( "Deleted column" );
    break;
    
   case KSpreadChanges::ChangeRecord::DELETEROW:
    action1 = i18n( "Deleted row" );
    break;
    
   case KSpreadChanges::ChangeRecord::DELETETABLE:
    action1 = i18n( "Deleted table" );
    break;

   case KSpreadChanges::ChangeRecord::MOVE:
    action1 = i18n( "Moved content" );
    break;
  };

  m_dlg->m_author->setText( m_changes->getAuthor( record->m_change->authorID ) + ", " 
                            + m_view->doc()->locale()->formatDateTime( record->m_change->timestamp ) );
  m_dlg->m_subject->setText( action1 );

  QString comment;
  CommentList::const_iterator iter = m_comments.find( record );
  if ( iter != m_comments.end() )
    comment = *(iter.data());
  else if ( record->m_change->comment )
    comment = *(record->m_change->comment );
  
  m_dlg->m_comment->setText( comment );
  m_currentRecord = record;
  m_dlg->m_comment->setModified( false );
  m_dlg->m_comment->setFocus();
}

void KSpreadCommentDlg::slotNext()
{
  if ( m_dlg->m_comment->isModified() )
    m_comments[ m_currentRecord ] = new QString( m_dlg->m_comment->text() );

  ++m_current;
  while ( m_current != m_end )
  {
    KSpreadChanges::ChangeRecord * record = m_current.data();
    if ( record->m_state == KSpreadChanges::ChangeRecord::PENDING )
    {
      addData( record );

      break;
    }
    ++m_current;
  }

  KSpreadChanges::RecordMap::iterator i = m_current;
  if ( m_current == m_end || ( ++i == m_end ) )
    m_dlg->m_nextButton->setEnabled( false );
  if ( m_current == m_begin )
    m_dlg->m_previousButton->setEnabled( false );
  else
    m_dlg->m_previousButton->setEnabled( true );
}

void KSpreadCommentDlg::slotPrevious()
{
  if ( m_dlg->m_comment->isModified() )
    m_comments[ m_currentRecord ] = new QString( m_dlg->m_comment->text() );

  if ( m_current != m_begin )
    --m_current;
  while ( m_current != m_begin )
  {
    KSpreadChanges::ChangeRecord * record = m_current.data();
    if ( record->m_state == KSpreadChanges::ChangeRecord::PENDING )
    {
      addData( record );

      break;
    }
    --m_current;
  }

  if ( m_current == m_begin )
  {
    KSpreadChanges::ChangeRecord * record = m_current.data();
    if ( record->m_state == KSpreadChanges::ChangeRecord::PENDING )
      addData( record );

    m_dlg->m_previousButton->setEnabled( false );
  }
  KSpreadChanges::RecordMap::iterator i = m_current;
  if ( m_current == m_end || ( ++i == m_end ) )
    m_dlg->m_nextButton->setEnabled( false );
  else 
    m_dlg->m_nextButton->setEnabled( true );
}
  
void KSpreadCommentDlg::slotOk()
{
  if ( m_dlg->m_comment->isModified() )
    m_comments[ m_currentRecord ] = new QString( m_dlg->m_comment->text() );

  CommentList::const_iterator iter = m_comments.begin();
  CommentList::const_iterator end  = m_comments.end();

  while ( iter != end )
  {
    iter.key()->m_change->comment = iter.data();

    ++iter;
  }

  m_comments.clear();

  KDialogBase::slotOk();
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
    m_dialog( new AcceptRejectWidget( &changes->m_filterSettings, this ) ),
    m_acceptElement( 0 ),
    m_rejectElement( 0 )
{
  m_changes->m_locked = true;

  setCaption( i18n( "Accept or Reject Changes" ) );
  setButtonBoxOrientation( Vertical );
  setMainWidget( m_dialog );

  fillList();

  connect( m_dialog->m_acceptButton, SIGNAL( clicked() ), this, SLOT( acceptButtonClicked() ) );
  connect( m_dialog->m_rejectButton, SIGNAL( clicked() ), this, SLOT( rejectButtonClicked() ) );
  connect( m_dialog->m_listView, SIGNAL( selectionChanged( QListViewItem * ) ), 
           this, SLOT( listViewSelectionChanged( QListViewItem * ) ) );
}

KSpreadAcceptDlg::~KSpreadAcceptDlg()
{
  m_changes->m_locked = false;
}

void KSpreadAcceptDlg::addChangeRecord( KListViewItem * element, KSpreadChanges::ChangeRecord * record )
{
  QString action1;
  QString action2;
  QString author;
  QString timestamp;
  QString comment;
  QString newValue;
  bool    take2 = false;
  KListViewItem * parent = element;

  author    = m_changes->getAuthor( record->m_change->authorID );
  timestamp = m_view->doc()->locale()->formatDateTime( record->m_change->timestamp );
  comment   = ( record->m_change->comment ? *(record->m_change->comment) : "" );    
  
  QString cellName( record->m_table->tableName() + '!' + 
                    util_encodeColumnLabelText( record->m_cell.x() )
                    + QString::number( record->m_cell.y() ) );

  if ( record->m_state == KSpreadChanges::ChangeRecord::PENDING )
    take2 = true;

  if ( record->m_dependants.first() )
  {
    KSpreadChanges::ChangeRecord * r = record->m_dependants.first();
    if ( r->m_type == KSpreadChanges::ChangeRecord::CELL )
    {
      newValue = ((KSpreadChanges::CellChange *) (r->m_change))->oldValue;
    }
  }
  else
    newValue = record->m_table->cellAt( record->m_cell.x(), 
                                        record->m_cell.y() )->text();
  
  KSpreadChanges::CellChange * ch = 0;

  switch( record->m_type )
  {
   case KSpreadChanges::ChangeRecord::CELL:
    ch = (KSpreadChanges::CellChange *) record->m_change;
    action1 = i18n( "Changed content" );
    action2 = QString( "'%1' -> '%2'" )
      .arg( ch->oldValue.length() > 0 ? ch->oldValue : i18n( "<empty>" ) )
      .arg( newValue.length() > 0 ? newValue : i18n( "<empty>" ) );

    if ( record->m_state == KSpreadChanges::ChangeRecord::REJECTED )
      comment += i18n( "(Cell %1 changed from '%2' to '%3')" )
        .arg( cellName )
        .arg( newValue.length() > 0 ? newValue : i18n( "<empty>" ) )
        .arg( ch->oldValue.length() > 0 ? ch->oldValue : i18n( "<empty>" ) );
    else
      comment += i18n( "(Cell %1 changed from '%2' to '%3')" )
        .arg( cellName )
        .arg( ch->oldValue.length() > 0 ? ch->oldValue : i18n( "<empty>" ) )
        .arg( newValue.length() > 0 ? newValue : i18n( "<empty>" ) );
    break;
    
   case KSpreadChanges::ChangeRecord::INSERTCOLUMN:
    action1 = i18n( "Inserted column" );
    break;
    
   case KSpreadChanges::ChangeRecord::INSERTROW:
    action1 = i18n( "Inserted row" );
    break;
    
   case KSpreadChanges::ChangeRecord::INSERTTABLE:
    action1 = i18n( "Inserted table" );
    break;
    
   case KSpreadChanges::ChangeRecord::DELETECOLUMN:
    action1 = i18n( "Deleted column" );
    break;
    
   case KSpreadChanges::ChangeRecord::DELETEROW:
    action1 = i18n( "Deleted row" );
    break;
    
   case KSpreadChanges::ChangeRecord::DELETETABLE:
    action1 = i18n( "Deleted table" );
    break;

   case KSpreadChanges::ChangeRecord::MOVE:
    action1 = i18n( "Moved content" );
    break;
  };
  
  if ( parent == 0 && record->m_state == KSpreadChanges::ChangeRecord::ACCEPTED )
    parent = m_acceptElement;
  else
  if ( parent == 0 && record->m_state == KSpreadChanges::ChangeRecord::REJECTED )
    parent = m_rejectElement;
  else
  if ( parent != m_acceptElement && record->m_state == KSpreadChanges::ChangeRecord::ACCEPTED )
  {
    bool found = false;
    KListViewItem * i = (KListViewItem *) parent->parent();
    while ( i )
    {
      if ( i == m_acceptElement )
        found = true;

      i = (KListViewItem *) i->parent();
    }

    if ( !found )
      parent = m_acceptElement;
  }
  else
  if ( parent != m_rejectElement && record->m_state == KSpreadChanges::ChangeRecord::REJECTED )
  {
    bool found = false;
    KListViewItem * i = (KListViewItem *) parent->parent();
    while ( i )
    {
      if ( i == m_rejectElement )
        found = true;

      i = (KListViewItem *) i->parent();
    }

    if ( !found )
      parent = m_rejectElement;
  }
  
  
  KListViewItem * el = 0;

  if ( parent != 0 )
  {
    el = new KListViewItem( parent, ( take2 ? action2 : action1 ),
                            cellName , author, timestamp,
                            comment );    
    parent->setExpandable( true );
  }
  else
    el = new KListViewItem( m_dialog->m_listView, action1,
                            cellName , author, timestamp,
                            comment );    

  if ( ( record->state() == KSpreadChanges::ChangeRecord::ACCEPTED )
       || ( record->state() == KSpreadChanges::ChangeRecord::REJECTED ) )
    el->setSelectable( false );

  m_itemMap[ el ] = record;

  QPtrListIterator<KSpreadChanges::ChangeRecord> it( record->m_dependants );
  for ( ; it.current(); ++it )
  {
    kdDebug() << "Adding record: " << it.current()->m_id <<  endl;
    addChangeRecord( el, it.current() );
  }
}

void KSpreadAcceptDlg::fillList()
{
  if ( m_changes->m_dependancyList.isEmpty() )
    m_changes->fillDependancyList();

  kdDebug() << "Filling list" <<  endl;
  m_acceptElement = new KListViewItem( m_dialog->m_listView, i18n( "Accepted" ) );
  m_rejectElement = new KListViewItem( m_dialog->m_listView, i18n( "Rejected" ) );

  m_acceptElement->setSelectable( false );
  m_rejectElement->setSelectable( false );

  QPtrListIterator<KSpreadChanges::ChangeRecord> it( m_changes->m_dependancyList );
  for ( ; it.current(); ++it )
  {
    kdDebug() << "Adding record as top level: " << it.current()->m_id <<  endl;
    addChangeRecord( 0, it.current() );
  }
  kdDebug() << "Filling list done" <<  endl;
}

void KSpreadAcceptDlg::makeUnselectable( KListViewItem * item )
{
  item->setSelectable( false );
  KListViewItem * i = (KListViewItem *) item->firstChild();
  while( i )
  {
    makeUnselectable( i );
    i = (KListViewItem *) i->nextSibling();
  }
}

void KSpreadAcceptDlg::applyFlag( KSpreadChanges::ChangeRecord * record, KSpreadChanges::ChangeRecord::State state )
{
  record->setState( state );

  QPtrListIterator<KSpreadChanges::ChangeRecord> it( record->m_dependants );
  for ( ; it.current(); ++it )
  {
    if ( state == KSpreadChanges::ChangeRecord::ACCEPTED )
      state = KSpreadChanges::ChangeRecord::REJECTED;
    applyFlag( it.current(), state );
  }
}

void KSpreadAcceptDlg::applyFlag( KListViewItem * item, KSpreadChanges::ChangeRecord::State state )
{
  ItemMap::const_iterator iter = m_itemMap.find( item );
  if ( iter != m_itemMap.end() )
  {
    KSpreadChanges::ChangeRecord * record = iter.data();

    applyFlag( record, state );
  }
}

void KSpreadAcceptDlg::acceptButtonClicked()
{
  KListView * list     = (KListView *) m_acceptElement->listView();
  KListViewItem * item = (KListViewItem *) list->selectedItem();

  if ( !item )
    return;

  enableButtons( false );

  kdDebug() << "AcceptClicked: " << item->text( 0 ) << endl;

  KListViewItem * parent = (KListViewItem *) item->parent();

  if ( parent )
  {
    parent->takeItem( item );

    KListViewItem * i = parent;
    while ( i )
    {
      i = (KListViewItem *) i->parent();
      if ( i )
        parent = i;
    }
    list->takeItem( parent );
    m_rejectElement->insertItem ( parent );
  }
  else
    list->takeItem( item );

  m_acceptElement->insertItem( item );

  KListViewItem * child = (KListViewItem *) item->firstChild();

  if ( child )
  {
    ItemMap::const_iterator iter = m_itemMap.find( child );
    if ( iter != m_itemMap.end() )
    {
      KSpreadChanges::ChangeRecord * record = iter.data();

      switch( record->m_type )
      {
       case KSpreadChanges::ChangeRecord::CELL:
        ((KSpreadChanges::CellChange *) (record->m_change))->cell->setCellText( ((KSpreadChanges::CellChange *) (record->m_change))->oldValue );
        break;
        
       case KSpreadChanges::ChangeRecord::INSERTCOLUMN:
        break;
        
       case KSpreadChanges::ChangeRecord::INSERTROW:
        break;
        
       case KSpreadChanges::ChangeRecord::INSERTTABLE:
        break;
        
       case KSpreadChanges::ChangeRecord::DELETECOLUMN:
        break;
        
       case KSpreadChanges::ChangeRecord::DELETEROW:
        break;
        
       case KSpreadChanges::ChangeRecord::DELETETABLE:
        break;
        
       case KSpreadChanges::ChangeRecord::MOVE:
        break;
      }
    }  
  }
  // TODO:  findChilds( m_acceptElement, item );

  makeUnselectable( item );
  applyFlag( item, KSpreadChanges::ChangeRecord::ACCEPTED );  
}

void KSpreadAcceptDlg::rejectButtonClicked()
{
  KListView * list     = (KListView *) m_acceptElement->listView();
  KListViewItem * item = (KListViewItem *) list->selectedItem();

  if ( !item )
    return;

  enableButtons( false );

  if ( item->parent() )
  {
    KListViewItem * parent = (KListViewItem *) item->parent();
    parent->takeItem( item );
  }
  else
    list->takeItem( item );

  m_rejectElement->insertItem( item );

  // TODO:  findChilds( m_rejectElement, item );

  makeUnselectable( item );
  applyFlag( item, KSpreadChanges::ChangeRecord::REJECTED );

  ItemMap::const_iterator iter = m_itemMap.find( item );
  if ( iter != m_itemMap.end() )
  {
    KSpreadChanges::ChangeRecord * record = iter.data();

    switch( record->m_type )
    {
     case KSpreadChanges::ChangeRecord::CELL:
      ((KSpreadChanges::CellChange *) (record->m_change))->cell->setCellText( ((KSpreadChanges::CellChange *) (record->m_change))->oldValue );
      break;
    
     case KSpreadChanges::ChangeRecord::INSERTCOLUMN:
      break;
    
     case KSpreadChanges::ChangeRecord::INSERTROW:
      break;
    
     case KSpreadChanges::ChangeRecord::INSERTTABLE:
      break;
    
     case KSpreadChanges::ChangeRecord::DELETECOLUMN:
      break;
    
     case KSpreadChanges::ChangeRecord::DELETEROW:
      break;
    
     case KSpreadChanges::ChangeRecord::DELETETABLE:
      break;

     case KSpreadChanges::ChangeRecord::MOVE:
      break;
    }
  }  
}

void KSpreadAcceptDlg::listViewSelectionChanged( QListViewItem * item )
{
  if ( !item )
    return;

  ItemMap::const_iterator iter = m_itemMap.find( (KListViewItem *) item );
  if ( iter != m_itemMap.end() )
  {
    KSpreadChanges::ChangeRecord * record = iter.data();

    switch( record->m_type )
    {
     case KSpreadChanges::ChangeRecord::CELL:
      m_view->canvasWidget()->gotoLocation( record->m_cell, record->m_table );
      break;
    
     case KSpreadChanges::ChangeRecord::INSERTCOLUMN:
      break;
    
     case KSpreadChanges::ChangeRecord::INSERTROW:
      break;
    
     case KSpreadChanges::ChangeRecord::INSERTTABLE:
      break;
    
     case KSpreadChanges::ChangeRecord::DELETECOLUMN:
      break;
    
     case KSpreadChanges::ChangeRecord::DELETEROW:
      break;
    
     case KSpreadChanges::ChangeRecord::DELETETABLE:
      break;

     case KSpreadChanges::ChangeRecord::MOVE:
      break;
    }    
  }

  enableButtons( true );
}

void KSpreadAcceptDlg::enableButtons( bool mode )
{
  kdDebug() << "Enabled Buttons: " << mode << ", Protected: " << m_changes->isProtected() << endl;
  if ( mode && m_changes->isProtected() )
    return;

  m_dialog->m_acceptButton->setEnabled( mode );
  m_dialog->m_rejectButton->setEnabled( mode );
  kdDebug() << "Enabled Buttons: done " << endl;
}

#include "kspread_dlg_changes.moc"


