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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef insdia_h
#define insdia_h

#include <qtabdialog.h>

class KWGroupManager;
class KWordDocument;
class KWPage;
class QWidget;
class QGridLayout;
class QButtonGroup;
class QRadioButton;
class QLabel;
class QSpinBox;

/******************************************************************/
/* Class: KWInsertDia                                             */
/******************************************************************/

class KWInsertDia : public QTabDialog
{
    Q_OBJECT

public:
    enum InsertType {ROW, COL};
    KWInsertDia( QWidget *parent, const char *name, KWGroupManager *_grpMgr, KWordDocument *_doc, InsertType _type, KWPage *_page );

protected:
    void setupTab1();
    void closeEvent( QCloseEvent * ) { emit cancelButtonPressed(); }

    QWidget *tab1;
    QGridLayout *grid1, *grid2;
    QButtonGroup *grp;
    QRadioButton *rBefore, *rAfter;
    QLabel *rc;
    QSpinBox *value;

    KWGroupManager *grpMgr;
    KWordDocument *doc;
    InsertType type;
    KWPage *page;

protected slots:
    void doInsert();

};

#endif


