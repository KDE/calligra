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

#ifndef __kspread_dlg_sort__
#define __kspread_dlg_sort__

#include <kdialogbase.h>
#include <qrect.h>
#include <qbuttongroup.h>
#include <qstringlist.h>

class KSpreadView;
class KSpreadTable;
class KSpreadCell;
class QComboBox;
class QRadioButton;
class QCheckBox;

class KSpreadSortDlg : public KDialogBase
{
  Q_OBJECT
public:
  KSpreadSortDlg( KSpreadView* parent, const char* name );

private slots:
  void slotOk();
  void slotpress(int id);

private:
  void init();

  KSpreadView  * m_pView;
  QButtonGroup * m_grp;
  QRadioButton * m_rb_row;
  QRadioButton * m_rb_column;
  QComboBox    * m_combo;
  QStringList    m_list_column;
  QStringList    m_list_row;
  QCheckBox    * m_decrease;
  QCheckBox    * m_cpLayout;
};

#endif
