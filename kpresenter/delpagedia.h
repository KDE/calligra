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

#ifndef delpagedia_h
#define delpagedia_h

#include <qdialog.h>

#include "global.h"

class KPresenterDoc;
class QWidget;
class QGridLayout;
class QLabel;
class QSpinBox;
class QRadioButton;
class QPushButton;

/******************************************************************/
/* class DelPageDia                                               */
/******************************************************************/

class DelPageDia : public QDialog
{
    Q_OBJECT

public:
    DelPageDia( QWidget* parent, const char* name, KPresenterDoc *_doc, int currPageNum );

protected:
    void uncheckAll();

    KPresenterDoc *doc;

    QGridLayout *grid;
    QLabel *label;
    QSpinBox *spinBox;
    QRadioButton *leave, *_move, *del, *move_del;
    QPushButton *ok, *cancel;

protected slots:
    void leaveClicked();
    void moveClicked();
    void delClicked();
    void moveDelClicked();
    void okClicked();

signals:
    void deletePage( int, DelPageMode );

};

#endif
