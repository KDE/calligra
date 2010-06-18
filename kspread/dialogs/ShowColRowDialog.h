/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres <nandres@web.de>
             (C) 2000-2002 Laurent Montel <montel@kde.org>
             (C) 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 John Dailey <dailey@vt.edu>

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

#ifndef KSPREAD_SHOW_COL_ROW_DIALOG
#define KSPREAD_SHOW_COL_ROW_DIALOG

#include <QList>

#include <kdialog.h>

class QListWidget;
class QListWidgetItem;

namespace KSpread
{
class Selection;

/**
 * \ingroup UI
 * Dialog to show hidden columns/rows.
 */
class ShowColRow: public KDialog
{
    Q_OBJECT
public:
    enum Type { Column, Row };
    ShowColRow(QWidget* parent, Selection* selection, Type _type);

public slots:
    void slotOk();
    void slotDoubleClicked(QListWidgetItem *);

protected:
    Selection* m_selection;
    QListWidget * list;
    Type typeShow;
    QList<int> listInt;
};

} // namespace KSpread

#endif // KSPREAD_SHOW_COL_ROW_DIALOG
