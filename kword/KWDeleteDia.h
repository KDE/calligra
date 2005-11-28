/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C)  2005 Thomas Zander <zander@kde.org>

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

#ifndef deldia_h
#define deldia_h

#include <kdialogbase.h>

class KWTableFrameSet;
class KWView;

/******************************************************************/
/* Class: KWDeleteDia                                             */
/******************************************************************/

class KWDeleteDia : public KDialogBase
{
    Q_OBJECT

public:
    enum DeleteType {deleteRow, deleteColumn};

    /**
     * Constructor for the ok-to-delete dialog.
     * @param parent the parent widget
     * @param name the internal name for the widget
     * @param table the table the rows or columns are to be deleted from
     * @param type the type of dialog this should become, either deleteRow or deleteColumn
     * @param remove a list of all the rows or columns that should be deleted.
     */
    KWDeleteDia( KWView *parent, KWTableFrameSet *table, DeleteType type, QValueList<uint> remove);

protected:
    void setupTab1();

    KWView *m_view;
    KWTableFrameSet *m_table;
    DeleteType m_type;

    QValueList<uint> m_toRemove;

protected slots:
    virtual void slotOk();
};

#endif


