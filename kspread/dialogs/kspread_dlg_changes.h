/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres, nandres@web.de

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

#ifndef __kspread_dlg_changes__
#define __kspread_dlg_changes__

#include <kdialogbase.h>
#include <qmap.h>
#include <qwidget.h>

class KSpreadChanges;
class ChangeRecord;
class KSpreadView;

class KComboBox;
class KListView;
class KListViewItem;
class KPushButton;

class QCheckBox;
class QDateTimeEdit;
class QLineEdit;
class QListViewItem;
class QTextEdit;

class FilterSettings;

class CommentDlg : public QWidget
{
  Q_OBJECT
  
 public:
  CommentDlg( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~CommentDlg();
  
  QTextEdit   * m_comment;
  QLabel      * m_author;
  QLabel      * m_subject;
  KPushButton * m_nextButton;
  KPushButton * m_previousButton;
};

class FilterMain : public QWidget
{
  Q_OBJECT
 public:
  FilterMain( FilterSettings * settings, QWidget * parent = 0, 
              const char * name = "FilterMain", WFlags fl = 0 );
  ~FilterMain();

  QCheckBox     * m_dateBox;
  QCheckBox     * m_authorBox;
  QCheckBox     * m_rangeBox;
  QCheckBox     * m_commentBox;
  QLineEdit     * m_authorEdit;
  QLineEdit     * m_rangeEdit;
  QLineEdit     * m_commentEdit;
  KComboBox     * m_dateUsage;
  QDateTimeEdit * m_timeFirst;
  QDateTimeEdit * m_timeSecond;

 protected slots:
  void slotDateUsageChanged( int );
  void slotDateStateChanged( bool );
  void slotAuthorStateChanged( bool );
  void slotCommentStateChanged( bool );
  void slotRangeStateChanged( bool );
  void slotCommentChanged( const QString & text );
  void slotAuthorChanged( const QString & text );
  void slotRangeChanged( const QString & text );
  void slotFirstTimeChanged( const QDateTime & );
  void slotSecondTimeChanged( const QDateTime & );

 private:
  FilterSettings * m_filterSettings;
};

class AcceptRejectWidget : public QWidget
{
  Q_OBJECT

 public:
  AcceptRejectWidget( FilterSettings * settings, QWidget * parent = 0, 
                      const char * name = 0, WFlags fl = 0 );
  ~AcceptRejectWidget();
  
  KPushButton    * m_acceptButton;
  KPushButton    * m_rejectButton;
  KPushButton    * m_acceptAllButton;
  KPushButton    * m_rejectAllButton;
  KListView      * m_listView;
  FilterMain     * m_filter;

 protected slots:
  void slotTabChanged( QWidget * );
  
 private:
  QWidget        * m_listTab;
  FilterSettings * m_filterSettings;

  void applyFilterSettings();
};


class FilterDlg : public QWidget
{
  Q_OBJECT

 public:
  FilterDlg( FilterSettings * settings, QWidget * parent = 0, 
             const char * name = 0, WFlags fl = 0 );
  ~FilterDlg();

 private:
  QCheckBox      * m_showChanges;
  QCheckBox      * m_showAccepted;
  QCheckBox      * m_showRejected;
  FilterMain     * m_filterMain;
};


class KSpreadCommentDlg : public KDialogBase
{
  Q_OBJECT  
 public:
  KSpreadCommentDlg( KSpreadView * parent, KSpreadChanges * changes, 
                     const char * name = "KSpreadCommentDlg" );
  ~KSpreadCommentDlg();
  
 protected slots:
  void slotOk();
  void slotNext();
  void slotPrevious();

 private:
  class CommentList : public QMap<KSpreadChanges::ChangeRecord *, QString *> {};

  KSpreadView    * m_view;
  KSpreadChanges * m_changes;
  CommentDlg     * m_dlg;

  CommentList      m_comments;

  KSpreadChanges::RecordMap::iterator m_begin;
  KSpreadChanges::RecordMap::iterator m_current;
  KSpreadChanges::RecordMap::iterator m_end;
  KSpreadChanges::ChangeRecord      * m_currentRecord;

  void addData( KSpreadChanges::ChangeRecord * record );
};

class KSpreadFilterDlg : public KDialogBase
{
  Q_OBJECT  
 public:
  KSpreadFilterDlg( KSpreadView * parent, KSpreadChanges * changes, 
                    const char * name = "KSpreadFilterDlg" );
  ~KSpreadFilterDlg();
  
 private:
  KSpreadView    * m_view;
  KSpreadChanges * m_changes;
  FilterDlg      * m_dlg;
};

class KSpreadAcceptDlg : public KDialogBase
{  
  Q_OBJECT
 public:
  KSpreadAcceptDlg( KSpreadView * parent, KSpreadChanges * changes,
                    const char * name = "KSpreadAcceptDlg" );
  ~KSpreadAcceptDlg();

 private slots:
  void acceptButtonClicked();
  void rejectButtonClicked();
  void listViewSelectionChanged( QListViewItem * );

 private:
  class ItemMap : public QMap<KListViewItem *, KSpreadChanges::ChangeRecord *> {};

  KSpreadView        * m_view;
  KSpreadChanges     * m_changes;  
  AcceptRejectWidget * m_dialog;
  KListViewItem      * m_acceptElement; 
  KListViewItem      * m_rejectElement;
  ItemMap              m_itemMap;

  void fillList();
  void addChangeRecord( KListViewItem * element, KSpreadChanges::ChangeRecord * record );
  void makeUnselectable( KListViewItem * item );
  void applyFlag( KListViewItem * item, KSpreadChanges::ChangeRecord::State state );
  void applyFlag( KSpreadChanges::ChangeRecord * record, KSpreadChanges::ChangeRecord::State state );
  void enableButtons( bool mode );
};


#endif


