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

#ifndef __kspread_dlg_validity__
#define __kspread_dlg_validity__

#include <qcombobox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include "kspread_cell.h"
#include <kdialogbase.h>
#include <qmultilineedit.h>
class KSpreadView;
class KSpreadTable;



class KSpreadDlgValidity : public KDialogBase
{
  Q_OBJECT
public:
KSpreadDlgValidity(KSpreadView* parent, const char* name ,const QRect &_marker );
void init();

public slots:
  void OkPressed();
  void changeIndexCond(int);
  void changeIndexType(int);
protected:
  KSpreadView* m_pView;
  QRect  marker;
  QLineEdit *val_max;
  QLineEdit *val_min;
  QLabel *edit1;
  QLabel *edit2;
  QComboBox *choose;
  QComboBox *chooseAction;
  QComboBox *chooseType;
  QLineEdit * title;
  QMultiLineEdit *message;
  KSpreadValidity result;
};



#endif
