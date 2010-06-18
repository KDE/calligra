/* This file is part of the KDE project
   Copyright (C) 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
             (C) 1999-2004 Laurent Montel <montel@kde.org>
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
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_SPECIAL_PASTE_DIALOG
#define KSPREAD_SPECIAL_PASTE_DIALOG

#include <kdialog.h>

#include "ui_SpecialPasteWidget.h"


namespace KSpread
{
class Selection;

/**
 * \ingroup UI
 * Dialog for the special paste action.
 */
class SpecialPasteDialog : public KDialog, public Ui::SpecialPasteWidget
{
    Q_OBJECT
public:
    explicit SpecialPasteDialog(QWidget* parent, Selection* selection);

public slots:
    void slotOk();
    void slotToggled(bool);

private:
    Selection* m_selection;
};

} // namespace KSpread

#endif // KSPREAD_SPECIAL_PASTE_DIALOG
