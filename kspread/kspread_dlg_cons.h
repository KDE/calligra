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

#include <qdialog.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qstrlist.h>
#include <qlistbox.h>
#include <qrect.h>
#include <qcheckbox.h>

class KSpreadView;
class KSpreadTable;

class KSpreadConsolidate : public QDialog
{
  Q_OBJECT
public:
  KSpreadConsolidate( KSpreadView* parent, const char* name );
  
  QStrList refs();
  
public slots:
  void slotOk();
  void slotClose();
  void slotAdd();
  void slotRemove();

  void slotSelectionChanged( KSpreadTable* _table, const QRect& _selection );
  void slotReturnPressed();
  
protected:
  KSpreadView* m_pView;
  
  QLineEdit* m_pRef;
  QListBox* m_pRefs;
  QComboBox* m_pFunction;
  QPushButton* m_pOk;
  QPushButton* m_pClose;
  QPushButton* m_pAdd;
  QPushButton* m_pRemove;
  QCheckBox* m_pRow;
  QCheckBox* m_pCol;
  QCheckBox* m_pCopy;
  
  int m_idSumme;
  int m_idAverage;
};

#endif
