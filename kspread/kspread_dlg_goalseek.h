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

#ifndef __kspread_dlg_goalseek__
#define __kspread_dlg_goalseek__

#include <kdialog.h>
#include <qpoint.h>
#include <qrect.h>

class KSpreadCell;
class KSpreadPoint;
class KSpreadTable;
class KSpreadView;

class QFrame;
class QGridLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QVBoxLayout;

class KSpreadGoalSeekDlg : public KDialog
{
  Q_OBJECT

 public:
  KSpreadGoalSeekDlg( KSpreadView * parent, QPoint const & marker, const char * name = 0,
                      bool modal = FALSE, WFlags fl = 0 );
  ~KSpreadGoalSeekDlg();

  /**
   * Find out which widget got focus.
   */
  bool eventFilter( QObject* obj, QEvent* ev );

 public slots:
  void buttonOkClicked();
  void buttonCancelClicked();
  void slotSelectionChanged( KSpreadTable * _table, const QRect & _selection );

 protected:
  virtual void closeEvent ( QCloseEvent * );

  QGridLayout * KSpreadGoalSeekDlgLayout;
  QGridLayout * m_startFrameLayout;
  QGridLayout * m_resultFrameLayout;

 private:
  KSpreadView * m_pView;
  KSpreadCell * m_sourceCell;
  KSpreadCell * m_targetCell;
  double        m_result;
  int           m_maxIter;
  bool          m_restored;
  double        m_oldSource;

  QString       m_oldText;
  QString       m_tableName;

  QFrame      * m_startFrame;
  QLineEdit   * m_targetValueEdit;
  QLineEdit   * m_targetEdit;
  QLineEdit   * m_sourceEdit;
  QPushButton * m_buttonOk;
  QPushButton * m_buttonCancel;
  QFrame      * m_resultFrame;
  QLabel      * m_newValueDesc;
  QLabel      * m_currentValueLabel;
  QLabel      * m_newValue;
  QLabel      * m_currentValue;
  QLabel      * m_resultText;

  /**
   * Tells which of the lineedits has the logical focus currently.
   * It may happen that a lineedit does not have qt focus but
   * logical focus but not the other way round.
   */
  QLineEdit   * m_focus;

  QPoint        m_anchor;
  QPoint        m_marker;
  QRect         m_selection;

  void startCalc(double _start, double _goal);
  void chooseCleanup();
};

#endif
