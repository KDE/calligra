/* This file is part of the KDE project
   Copyright (C) 2002-2004 Ariya Hidayat <ariya@kde.org>
             (C) 2003 Norbert Andres <nandres@web.de>
             (C) 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2000 David Faure <faure@kde.org>
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __kspread_dlg_resize2__
#define __kspread_dlg_resize2__

#include <kdialogbase.h>

class KSpreadView;
class KDoubleNumInput;
class KoUnitDoubleSpinBox;

class KSpreadResizeRow: public KDialogBase
{
  Q_OBJECT

public:
  KSpreadResizeRow( KSpreadView* parent, const char* name = 0 );
  double rowHeight;

protected slots:
  virtual void slotOk();
  virtual void slotDefault();

protected:
  KSpreadView* m_pView;
  KoUnitDoubleSpinBox *m_pHeight;
};

class KSpreadResizeColumn: public KDialogBase
{
  Q_OBJECT

public:
  KSpreadResizeColumn( KSpreadView* parent, const char* name = 0 );
  double columnWidth;

protected slots:
  virtual void slotOk();
  virtual void slotDefault();

protected:
  KSpreadView* m_pView;
  KoUnitDoubleSpinBox *m_pWidth;
};

#endif
