/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999, 2000 Montel Laurent <montell@club-internet.fr>

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

#ifndef __kspread_dlg_conditional__
#define __kspread_dlg_conditional__

#include <kdialogbase.h>
#include "kspread_cell.h"
class KSpreadView;
class KSpreadTable;
class QLabel;
class QLineEdit;
class QFont;
class KColorButton;
class QComboBox;

#define KSPREAD_NUM_CONDITIONALS 3

class KSpreadWidgetconditional : public QWidget
{
  Q_OBJECT
public:
  KSpreadWidgetconditional(QWidget *_parent, const QString &name);
  double getBackFirstValue()const;
  double getBackSecondValue()const;
  QFont getFont()const {return font;}
  QColor getColor()const;
  Conditional typeOfCondition()const;
  void init(KSpreadConditional tmp);
public slots:
  void changeLabelFont();
  void changeIndex(const QString &text);
  void refreshPreview();
  void disabled();
signals:
  void fontSelected();
protected:
  QComboBox *choose;
  QLineEdit *edit1;
  QLineEdit *edit2;
  KColorButton* color;
  QPushButton *fontButton;
  QFont font;
  KSpreadConditional tmpCond;
  QLineEdit *preview;
};

class KSpreadconditional : public KDialogBase
{
  Q_OBJECT
public:
  KSpreadconditional(KSpreadView* parent, const char* name,
		     const QRect &_marker );
  void init();
  public slots:
  void slotOk();

protected:
  KSpreadView* m_pView;
  QRect  marker;
  KSpreadWidgetconditional *conditionals[KSPREAD_NUM_CONDITIONALS];
  KSpreadConditional result;
};

#endif
