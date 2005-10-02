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

#ifndef insdia_h
#define insdia_h

#include <kdialogbase.h>

class KWTableFrameSet;
class KWDocument;
class KWCanvas;
class QWidget;
class QGridLayout;
class QButtonGroup;
class QRadioButton;
class QLabel;
class QSpinBox;

/******************************************************************/
/* Class: KWInsertDia                                             */
/******************************************************************/

class KWInsertDia : public KDialogBase
{
    Q_OBJECT

public:
    enum InsertType {ROW, COL};
    KWInsertDia( QWidget *parent, const char *name, KWTableFrameSet *_grpMgr, KWDocument *_doc, InsertType _type, KWCanvas *_canvas );

protected:
    void setupTab1();
    bool doInsert();

    QWidget *tab1;
    QGridLayout *grid1, *grid2;
    QButtonGroup *grp;
    QRadioButton *rBefore, *rAfter;
    QLabel *rc;
    QSpinBox *value;

    KWTableFrameSet *table;
    KWDocument *doc;
    InsertType type;
    KWCanvas *canvas;

protected slots:
    virtual void slotOk();
};

#endif


