/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres <nandres@web.de>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2000-2002 Laurent Montel <montel@kde.org>

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

#ifndef __kspread_dlg_pasteinsert__
#define __kspread_dlg_pasteinsert__

#include <kdialog.h>

class QCheckBox;
class QRadioButton;

namespace KSpread
{
class Sheet;
class View;

class PasteInsertDialog : public KDialog
{
    Q_OBJECT
public:
    PasteInsertDialog( View* parent, const char* name, const QRect &_rect );

public slots:
    void slotOk();
private:
  View* m_pView;
  QRadioButton *rb1;
  QRadioButton *rb2;
  QRect  rect;
};

} // namespace KSpread

#endif
