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

#ifndef insdia_h
#define insdia_h

#include <kdialogbase.h>

class KWTableFrameSet;
class QRadioButton;
class QSpinBox;
class KWView;

class KWInsertDia : public KDialogBase
{
    Q_OBJECT

public:
    enum InsertType {insertRow, insertColumn};
    KWInsertDia( KWView *parent, KWTableFrameSet *table, InsertType type, int insertHint);

protected:
    void setupTab1(int insertHint);

    QRadioButton *m_rBefore;
    QSpinBox *m_value;

    KWTableFrameSet *m_table;
    InsertType m_type;
    KWView *m_view;

protected slots:
    virtual void slotOk();
};

#endif


