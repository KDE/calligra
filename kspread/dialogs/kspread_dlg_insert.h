/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres <nandres@web.de>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2000-2001 Werner Trobin <trobin@kde.org>
             (C) 2000 David Faure <faure@kde.org>
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

#ifndef __kspread_dlg_insert__
#define __kspread_dlg_insert__

#include <kdialogbase.h>

class QCheckBox;
class QRadioButton;
class QRect;

namespace KSpread
{
class Sheet;
class View;

class InsertDialog : public KDialogBase
{
    Q_OBJECT
public:
    enum Mode { Insert, Remove };

    InsertDialog( View* parent, const char* name, const QRect &_rect, Mode _mode );

public slots:
    void slotOk();

private:
  View* m_pView;

  QRadioButton *rb1;
  QRadioButton *rb2;
  QRadioButton *rb3;
  QRadioButton *rb4;
  QRect  rect;
  Mode insRem;
};

} // namespace KSpread

#endif
