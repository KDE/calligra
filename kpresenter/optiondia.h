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

#ifndef OPTIONDIA_H
#define OPTIONDIA_H

#include <qtabdialog.h>
#include <qcolor.h>
#include <qstring.h>

#include <kcolorbtn.h>
#include <qlineedit.h>

class QWidget;
class QLabel;
class QGroupBox;

/******************************************************************/
/* class OptionDia                                                */
/******************************************************************/

class OptionDia : public QTabDialog
{
    Q_OBJECT

public:

    // constructor - destrcutor
    OptionDia( QWidget *parent=0, const char *name=0 );
    ~OptionDia();

    // set values
    void setRastX( int rx ) { eRastX->setText( QString().setNum( rx ) ); }
    void setRastY( int ry ) { eRastY->setText( QString().setNum( ry ) ); }
    void setBackCol( const QColor &c ) { bBackCol->setColor( c ); }

    // get values
    int getRastX() { return QString( eRastX->text() ).toInt(); }
    int getRastY() { return QString( eRastY->text() ).toInt(); }
    QColor getBackCol() { return bBackCol->color(); }

private:

    // dialog objecsts
    QWidget *general, *objects;
    QLabel *lRastX, *lRastY, *lBackCol;
    QLineEdit *eRastX, *eRastY;
    QGroupBox *txtObj;
    KColorButton *bBackCol;

};
#endif
