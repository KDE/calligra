/* This file is part of the KDE project
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2002 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 1999-2001 Laurent Montel <montel@kde.org>
   Copyright 1998-1999 Torben Weis <weis@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_ADD_NAMED_AREA_DIALOG
#define KSPREAD_ADD_NAMED_AREA_DIALOG

#include <kdialog.h>

class KLineEdit;

namespace KSpread
{
class Selection;

/**
 * \ingroup UI
 * Dialog to add a named area.
 */
class AddNamedAreaDialog : public KDialog
{
    Q_OBJECT

public:
    AddNamedAreaDialog(QWidget* parent, Selection* selection);

public slots:
    void slotOk();
    void slotAreaNameChanged(const QString& name);

protected:
    Selection*  m_selection;
    KLineEdit*  m_areaName;
};

} // namespace KSpread

#endif // KSPREAD_ADD_NAMED_AREA_DIALOG
