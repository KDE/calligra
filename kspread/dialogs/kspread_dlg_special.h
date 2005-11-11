/* This file is part of the KDE project
   Copyright (C) 1999-2004 Laurent Montel <montel@kde.org>
             (C) 2003 Norbert Andres <nandres@web.de>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 John Dailey <dailey@vt.edu>
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

#ifndef __kspread_dlg_special__
#define __kspread_dlg_special__

#include <kdialogbase.h>

class QRadioButton;

namespace KSpread
{
class Sheet;
class View;

class SpecialDialog : public KDialogBase
{
    Q_OBJECT
public:
    SpecialDialog( View* parent, const char* name );

public slots:
    void slotOk();
    void slotToggled( bool );

private:
    View* m_pView;
    QRadioButton *rb1, *rb2, *rb3, *rb4, *rb5;
    QRadioButton *rb6, *rb7, *rb8;
    QRadioButton *rb9, *rb10, *rb11;
};

} // namespace KSpread

#endif
