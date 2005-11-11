/* This file is part of the KDE project
   Copyright (C) 1999-2003 Laurent Montel <montel@kde.org>
             (C) 2003 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2003 Ariya Hidayat <ariya@kde.org>
             (C) 2003 Norbert Andres <nandres@web.de>
             (C) 1999 Stephan Kulow <coolo@kde.org>
             (C) 1998-2000 Torben Weis <weis@kde.org>

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

#ifndef __kspread_dlg_goto__
#define __kspread_dlg_goto__

#include <kdialogbase.h>

class KLineEdit;

namespace KSpread
{
class Sheet;
class View;

class GotoDialog : public KDialogBase
{
  Q_OBJECT
public:
  GotoDialog( View* parent, const char* name);

public slots:
  void slotOk();
  void textChanged ( const QString &_text );


protected:
  View* m_pView;
  KLineEdit* m_nameCell;
};

} //

#endif
