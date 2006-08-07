/* This file is part of the KDE project
   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 1999-2005 Laurent Montel <montel@kde.org>
             (C) 1998-1999 Torben Weis <weis@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef __kspread_dlg_validity__
#define __kspread_dlg_validity__

#include <kpagedialog.h>

#include "Validity.h"

class QLabel;
class QLineEdit;
class QTextEdit;
class QComboBox;
class QPushButton;
class QCheckBox;

namespace KSpread
{
class Sheet;
class View;

class DlgValidity : public KPageDialog
{
  Q_OBJECT
public:
DlgValidity(View* parent, const char* name ,const QRect &_marker );
void init();

public slots:
 void OkPressed();
 void clearAllPressed();
 void changeIndexCond(int);
 void changeIndexType(int);
protected:
    void displayOrNotListOfValidity( bool _displayList);

  View* m_pView;
  QRect  marker;
  QLineEdit *val_max;
  QLineEdit *val_min;
  QLabel *edit1;
  QLabel *edit2;
  QComboBox *choose;
  QComboBox *chooseAction;
  QComboBox *chooseType;
  QLineEdit * title;
  QTextEdit *message;
  Validity result;
  QCheckBox *displayMessage;
  QCheckBox *allowEmptyCell;
    QCheckBox *displayHelp;
    QTextEdit *messageHelp;
    QLineEdit *titleHelp;
    QTextEdit *validityList;
    QLabel *validityLabelList;
};

} // namespace KSpread

#endif
