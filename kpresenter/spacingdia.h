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

#ifndef spacingdia_h
#define spacingdia_h

#include <qdialog.h>

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

class QLabel;
class QLineEdit;
class QPushButton;
class QWidget;

/******************************************************************/
/* class SpacingDia                                               */
/******************************************************************/

class SpacingDia : public QDialog
{
    Q_OBJECT

public:
    SpacingDia( QWidget* parent, int _lineSpacing, int _distBefore, int _distAfter, int _gap );

protected:
    QLabel *lLineSpacing, *lDistBefore, *lDistAfter, *lGap;
    QLineEdit *eLineSpacing, *eDistBefore, *eDistAfter, *eGap;
    QPushButton *cancelBut, *okBut;

public slots:
    void slotSpacingDiaOk();

signals:
    void spacingDiaOk( int, int, int, int );

};

#endif


