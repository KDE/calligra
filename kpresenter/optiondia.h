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

#include <qdialog.h>

#include <kcolorbutton.h>
#include <knuminput.h>

class QVBox;
class QLabel;
class QGroupBox;
class QPushButton;

/******************************************************************/
/* class OptionDia                                                */
/******************************************************************/

class OptionDia : public QDialog
{
    Q_OBJECT

public:

    // constructor - destructor
    OptionDia( QWidget *parent=0, const char *name=0 );
    ~OptionDia();

    // set values
    void setRastX( int rx ) { eRastX->setValue( rx ); }
    void setRastY( int ry ) { eRastY->setValue( ry ); }
    void setBackCol( const QColor &c ) { bBackCol->setColor( c ); }

    // get values
    int getRastX() { return eRastX->value(); }
    int getRastY() { return eRastY->value(); }
    QColor getBackCol() { return bBackCol->color(); }

protected slots:
    void slotOkClicked();

signals:
 void applyButtonPressed();
private:

    // dialog objects
    QVBox *general, *objects;
    QLabel *lRastX, *lRastY, *lBackCol;
    KIntNumInput *eRastX, *eRastY;
    QGroupBox *txtObj;
    KColorButton *bBackCol;
    QPushButton *okBut, *cancelBut;
    QGroupBox *gbObjects;
};
#endif
