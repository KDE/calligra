/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999,2000,2001 Montel Laurent <lmontel@mandrakesoft.com>

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

#ifndef __kspread_dlg_goto__
#define __kspread_dlg_goto__

#include <qdialog.h>

class KSpreadView;
class KSpreadTable;
class KSpreadCell;
class QPushButton;
class QLineEdit;

class KSpreadGotoDlg : public QDialog
{
  Q_OBJECT
public:
  KSpreadGotoDlg( KSpreadView* parent, const char* name);

public slots:
  void slotOk();
  void slotClose();
  void textChanged ( const QString &_text );


protected:
  KSpreadView* m_pView;
  
  QLineEdit* m_nameCell;
  QPushButton* m_pOk;
  QPushButton* m_pClose;
};

#endif
