/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 
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

#ifndef __kspread_dlg_cons__
#define __kspread_dlg_cons__

#include <qstringlist.h>
#include <kdialogbase.h>

class QLineEdit;
class QPushButton;
class QComboBox;
class QCheckBox;
class QListBox;

class KSpreadView;
class KSpreadSheet;

class KSpreadConsolidate : public KDialogBase
{
  Q_OBJECT
public:
  KSpreadConsolidate( KSpreadView* parent, const char* name );
  ~KSpreadConsolidate();
  QStringList refs();
  
public slots:
  virtual void slotOk();
  virtual void slotCancel();
  void slotAdd();
  void slotRemove();

  void slotSelectionChanged( KSpreadSheet* _table, const QRect& _selection );
  void slotReturnPressed();

protected:
  virtual void closeEvent ( QCloseEvent * ); 

  KSpreadView* m_pView;
  QLineEdit* m_pRef;
  QListBox* m_pRefs;
  QComboBox* m_pFunction;
  QPushButton* m_pAdd;
  QPushButton* m_pRemove;
  QCheckBox* m_pRow;
  QCheckBox* m_pCol;
  QCheckBox* m_pCopy;

  QString evaluate( const QString& formula, KSpreadSheet* table );

  enum { Sum = 0, Average, Count, Max, Min, Product, StdDev, Var };
};

#endif
