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
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_DATABASE_DIALOG
#define KSPREAD_DATABASE_DIALOG

#include <QDialog>
#include <QRect>
//Added by qt3to4:
#include <QFrame>
#include <QLabel>
#include <QtSql/QSqlDatabase>

#include <kassistantdialog.h>

class QCheckBox;
class QComboBox;
class QFrame;
class QLabel;
class QLineEdit;
class Q3ListViewItem;
class QRadioButton;
class QTextEdit;

class K3ListView;
class KPageWidgetItem;
class KPushButton;

namespace KSpread
{
class View;

class DatabaseDialog : public KAssistantDialog
{
  Q_OBJECT

 public:
  enum PageId { eDatabase = 0, eTables = 1, eColumns = 2, eOptions = 3, eResult = 4 };

  DatabaseDialog( View * parent, QRect const & rect, const char * name = 0, bool modal = false, Qt::WFlags fl = 0 );
  virtual ~DatabaseDialog();

 private slots:
  void orBox_clicked();
  void andBox_clicked();
  void startingCell_clicked();
  void startingRegion_clicked();
  void connectButton_clicked();
  void databaseNameChanged( const QString & s );
  void databaseHostChanged( const QString & s );
  void databaseDriverChanged( int );
  void popupTableViewMenu( Q3ListViewItem *, const QPoint &, int );
  void tableViewClicked( Q3ListViewItem * );
  void accept();

 protected:
  void next();
  void back();

 private:
  int            m_currentPage;
  View  * m_pView;
  QRect          m_targetRect;
  QSqlDatabase   m_dbConnection;

  KPageWidgetItem * m_database;
  QLabel       * m_databaseStatus;
  QLineEdit    * m_username;
  QLineEdit    * m_port;
  QLineEdit    * m_databaseName;
  QComboBox    * m_driver;
  QLineEdit    * m_password;
  QLineEdit    * m_host;
  QLabel       * m_Type;
  KPageWidgetItem * m_table;
  QComboBox    * m_databaseList;
  KPushButton  * m_connectButton;
  QLabel       * m_tableStatus;
  QLabel       * m_SelectTableLabel;
  K3ListView    * m_tableView;
  KPageWidgetItem * m_columns;
  K3ListView    * m_columnView;
  QLabel       * m_columnsStatus;
  KPageWidgetItem * m_options;
  QComboBox    * m_columns_1;
  QComboBox    * m_columns_2;
  QComboBox    * m_columns_3;
  QComboBox    * m_operator_1;
  QComboBox    * m_operator_2;
  QComboBox    * m_operator_3;
  QLineEdit    * m_operatorValue_1;
  QLineEdit    * m_operatorValue_2;
  QLineEdit    * m_operatorValue_3;
  QRadioButton * m_andBox;
  QRadioButton * m_orBox;
  QComboBox    * m_columnsSort_1;
  QComboBox    * m_columnsSort_2;
  QComboBox    * m_sortMode_1;
  QComboBox    * m_sortMode_2;
  QCheckBox    * m_distinct;
  KPageWidgetItem * m_result;
  QTextEdit    * m_sqlQuery;
  QRadioButton * m_startingRegion;
  QLineEdit    * m_cell;
  QLineEdit    * m_region;
  QRadioButton * m_startingCell;

  void switchPage( int id );
  bool databaseDoNext();
  bool tablesDoNext();
  bool columnsDoNext();
  bool optionsDoNext();

  QString exchangeWildcards(QString const & value);
  QString getWhereCondition( QString const &, QString const &, int );
};

} // namespace KSpread

#endif // KSPREAD_DATABASE_DIALOG
