/* This file is part of the KDE project
   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 1999-2001 Laurent Montel <montel@kde.org>
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

#ifndef __kspread_dlg_area__
#define __kspread_dlg_area__

#include <kdialogbase.h>

class QPushButton;
class QLineEdit;
class QLabel;

namespace KSpread
{
class View;

class AreaDialog : public KDialogBase
{
  Q_OBJECT
public:
  AreaDialog( View * parent, const char * name, const QPoint & _marker );

public slots:
  void slotOk();
  void slotAreaNamechanged( const QString & text);

protected:
  View * m_pView;
  QLineEdit   * m_areaName;
  QPoint        m_marker;
};

} // namespace KSpread

#endif
