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

#ifndef inspagedia_h
#define inspagedia_h

#include <qdialog.h>

#include "global.h"

class KPresenterDoc;
class QWidget;
class QSpinBox;
class QRadioButton;
class QPushButton;

/******************************************************************/
/* class InsPageDia                                               */
/******************************************************************/

class InsPageDia : public QDialog
{
    Q_OBJECT

public:
    InsPageDia( QWidget* parent, const char* name, KPresenterDoc *_doc, int _currPage );

    InsPageMode getInsPageMode() {
	return insPageMode;
    }
    InsertPos getInsertPos() {
	return insertPos;
    }
    int getPageNum() {
	return pageNum;
    }

protected:
    void uncheckAllPos();
    void uncheckAllMode();

    KPresenterDoc *doc;

    QSpinBox *spinBox;
    QRadioButton *before, *after, *leave, *_move;
    QPushButton *ok, *cancel;
    InsertPos insertPos;
    InsPageMode insPageMode;
    int pageNum;

protected slots:
    void leaveClicked();
    void moveClicked();
    void beforeClicked();
    void afterClicked();
    void okClicked();

signals:
    void insertPage( int, InsPageMode, InsertPos );

};

#endif
