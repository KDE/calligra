/* This file is part of the KDE project
   Copyright (C) 1999-2004 Laurent Montel <montel@kde.org>
             (C) 2002-2004 Ariya Hidayat <ariya@kde.org>
             (C) 2003 Norbert Andres <nandres@web.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
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

#ifndef KSPREAD_SHOW_DIALOG
#define KSPREAD_SHOW_DIALOG

#include <KDialog>

class QListWidget;

namespace KSpread
{
class Selection;

/**
 * \ingroup UI
 * Dialog to show a hidden sheet.
 * \todo Rename to ShowSheetDialog.
 */
class ShowDialog: public KDialog
{
    Q_OBJECT
public:
    ShowDialog(QWidget *parent, Selection *selection);

public Q_SLOTS:
    virtual void accept();

private:
    Selection* m_selection;
    QListWidget *m_listWidget;
};

} // namespace KSpread

#endif // KSPREAD_SHOW_DIALOG
