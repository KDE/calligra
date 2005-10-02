/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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
class KWDocument;
class KWCanvas;
class QWidget;
class QGridLayout;
class QLabel;
class QSpinBox;

/******************************************************************/
/* Class: KWDeleteDia                                             */
/******************************************************************/

class KWDeleteDia : public KDialogBase
{
    Q_OBJECT

public:
    enum DeleteType {ROW, COL};
    KWDeleteDia( QWidget *parent, const char *name, KWTableFrameSet *_table, KWDocument *_doc, DeleteType _type, KWCanvas *_canvas );

protected:
    void setupTab1();
    bool doDelete();

    QWidget *tab1;
    QGridLayout *grid1;
    QLabel *rc;
    QSpinBox *value;

    KWTableFrameSet *table;
    KWDocument *doc;
    DeleteType type;
    KWCanvas *canvas;
    
    QValueList<uint> m_toRemove; // not really sure i should hold this here but ok.

protected slots:
    virtual void slotOk();
};

#endif


