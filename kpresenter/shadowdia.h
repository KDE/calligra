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

#ifndef SHADOWDIA_H
#define SHADOWDIA_H

#include <qdialog.h>
#include <qframe.h>
#include <qcolor.h>

#include "global.h"

class QWidget;
class QSpinBox;
class QGroupBox;
class QPushButton;
class KColorButton;
class QLabel;

/******************************************************************/
/* class ShadowPreview                                            */
/******************************************************************/

class ShadowPreview : public QFrame
{
    Q_OBJECT

public:
    // constructor - destructor
    ShadowPreview( QWidget* parent, const char* );
    ~ShadowPreview() {}

    void setShadowDirection( ShadowDirection sd ) { shadowDirection = sd; repaint( true ); }
    void setShadowDistance( int sd ) { shadowDistance = sd; repaint( true ); }
    void setShadowColor( QColor sc ) { shadowColor = sc; repaint( true ); }

protected:
    void drawContents( QPainter* );

    ShadowDirection shadowDirection;
    int shadowDistance;
    QColor shadowColor;

};

/******************************************************************/
/* class ShadowDia                                                */
/******************************************************************/

class ShadowDia : public QDialog
{
    Q_OBJECT

public:
    // constructor - destructor
    ShadowDia( QWidget* parent, const char* );
    ~ShadowDia();

    void setShadowDirection( ShadowDirection sd );
    void setShadowDistance( int sd );
    void setShadowColor( QColor sc );

    ShadowDirection getShadowDirection() { return shadowDirection; }
    int getShadowDistance() { return shadowDistance; }
    QColor getShadowColor() { return shadowColor; }

protected:
    QSpinBox *distance;
    QGroupBox *shadow, *preview;
    ShadowPreview *sPreview;
    QPushButton *lu, *u, *ru, *r, *rb, *b, *lb, *l;
    QPushButton *okBut, *applyBut, *cancelBut;
    KColorButton *color;
    QLabel *lcolor, *ldirection, *ldistance;

    ShadowDirection shadowDirection;
    int shadowDistance;
    QColor shadowColor;

protected slots:
    void luChanged();
    void uChanged();
    void ruChanged();
    void rChanged();
    void rbChanged();
    void bChanged();
    void lbChanged();
    void lChanged();
    void colorChanged( const QColor& );
    void distanceChanged( int );
    void Apply() { emit shadowDiaOk(); }

signals:
    void shadowDiaOk();

};

#endif
