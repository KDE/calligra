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

#ifndef ROTATEDIA_H
#define ROTATEDIA_H

#include <qdialog.h>
#include <qframe.h>

class QRadioButton;
class QWidget;
class QPainter;
class KDoubleNumInput;
class QGroupBox;
class QVButtonGroup;
class QPushButton;

/******************************************************************/
/* class RotatePreview                                            */
/******************************************************************/

class RotatePreview : public QFrame
{
    Q_OBJECT

public:
    // constructor - destructor
    RotatePreview( QWidget* parent, const char* );
    ~RotatePreview() {}

    void setAngle( float __angle ) {_angle = __angle; repaint( contentsRect(), true ); }

protected:
    void drawContents( QPainter* );

    float _angle;

};

/******************************************************************/
/* class RotateDia                                                */
/******************************************************************/

class RotateDia : public QDialog
{
    Q_OBJECT

public:
    // constructor - destructor
    RotateDia( QWidget* parent, const char* );
    ~RotateDia();

    void setAngle( float __angle );
    float getAngle() {return _angle; }

protected:
    QRadioButton *deg0, *deg90, *deg180, *deg270, *degCustom;
    KDoubleNumInput *custom;
    QGroupBox *preview;
    QVButtonGroup *angle;
    RotatePreview *rPreview;
    QPushButton *okBut, *applyBut, *cancelBut;

    float _angle;

protected slots:
    void deg0clicked();
    void deg90clicked();
    void deg180clicked();
    void deg270clicked();
    void degCustomclicked();
    void degCustomChanged( double );
    void Apply() {emit rotateDiaOk(); }

signals:
    void rotateDiaOk();

};

#endif
