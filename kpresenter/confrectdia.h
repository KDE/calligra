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

#ifndef confrectdia_h
#define confrectdia_h

#include <qdialog.h>
#include <qframe.h>

#include "global.h"

class QPainter;
class QLabel;
class QLineEdit;
class QGroupBox;
class QPushButton;

/******************************************************************/
/* class RectPreview                                               */
/******************************************************************/

class RectPreview : public QFrame
{
    Q_OBJECT

public:
    RectPreview( QWidget* parent, const char* );
    ~RectPreview() {}

    void setRnds( int _rx, int _ry )
    { xRnd = _rx; yRnd = _ry; repaint( true ); }

protected:
    void drawContents( QPainter* );

    int xRnd, yRnd;

};

/******************************************************************/
/* class ConfRectDia                                              */
/******************************************************************/

class ConfRectDia : public QDialog
{
    Q_OBJECT

public:
    ConfRectDia( QWidget* parent, const char* );
    ~ConfRectDia();

    void setRnds( int _rx, int _ry );

    int getRndX()
    { return xRnd; }
    int getRndY()
    { return yRnd; }

protected:
    QLabel *lRndX, *lRndY;
    QLineEdit *eRndX, *eRndY;
    QGroupBox *gSettings, *gPreview;
    RectPreview *rectPreview;
    QPushButton *okBut, *applyBut, *cancelBut;

    int xRnd, yRnd;

protected slots:
    void rndXChanged( const QString & );
    void rndYChanged( const QString & );
    void Apply() { emit confRectDiaOk(); }

signals:
    void confRectDiaOk();

};

#endif


