/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 Montel Laurent <montell@club-internet.fr>

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

#ifndef __kspread_dlg_formula2__
#define __kspread_dlg_formula2__

#include <qframe.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpushbutton.h>

class KSpreadView;
class KSpreadTable;
class KSpreadCell;

enum type_create {type_double,type_string,type_logic,type_int};

struct param
{
  int nb_param;
  QString firstElementLabel;
  type_create firstElementType;

  QString secondElementLabel;
  type_create secondElementType;

  QString thirdElementLabel;
  type_create thirdElementType;

  QString fourElementLabel;
  type_create fourElementType;

  QString fiveElementLabel;
  type_create fiveElementType;
  bool multiple;
  QString help;
};

class KSpreadDlgFormula2 : public QDialog
{
  Q_OBJECT
public:
  KSpreadDlgFormula2( KSpreadView* parent, const char* name,const QString& formulaName=0);
  void changeFunction();
  QString make_formula( const QString& _text,type_create elementType);
  QString create_formula(QString _text);
public slots:
  void slotOk();
  void slotClose();
  void slotselected(const QString &);
  void slotDoubleClicked(QListBoxItem *);
  void slotActivated(const QString &);
  void slotChangeText(const QString &);
  void slotSelectionChanged( KSpreadTable* _table, const QRect& _selection );
  void slotSelectButton();
private:
  bool eventFilter( QObject* obj, QEvent* ev );
protected:
  KSpreadView* m_pView;
  QPushButton* m_pOk;
  QPushButton* m_pClose;

  QLabel *label1;
  QLabel *label2;
  QLabel *label3;
  QLabel *label4;
  QLabel *label5;
  QLabel *help;
  QComboBox *typeFunction;
  QListBox *functions;
  QLineEdit *result;
  QLineEdit *firstElement;
  QLineEdit *secondElement;
  QLineEdit *thirdElement;
  QLineEdit *fourElement;
  QLineEdit *fiveElement;
  QLineEdit* m_focus;
  int m_column;
  int m_row;
  QString m_oldText;
  QString m_funcName;
  QString m_tableName;
  param funct;
  QPushButton *selectFunction;

  int m_oldLength;
  QString m_rightText;
  QString m_leftText;
  bool refresh_result;
};

#endif
