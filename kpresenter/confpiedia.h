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

#ifndef confpiedia_h
#define confpiedia_h

#include <qframe.h>
#include <global.h>
#include <qcombobox.h>
#include <qpen.h>
#include <knuminput.h>
#include <qbrush.h>
class QLabel;
class QGroupBox;
class QPushButton;
class QPainter;
class QComboBox;

/******************************************************************/
/* class PiePreview                                               */
/******************************************************************/

class PiePreview : public QFrame
{
    Q_OBJECT

public:
    PiePreview( QWidget* parent, const char* );
    ~PiePreview() {}

    void setAngle( int _angle )
    { angle = _angle; repaint( true ); }
    void setLength( int _len )
    { len = _len; repaint( true ); }
    void setType( PieType _type )
    { type = _type; repaint( true ); }
    void setPenBrush( const QPen &_pen, const QBrush &_brush )
    { pen = _pen; brush = _brush; repaint( true ); }

protected:
    void drawContents( QPainter* );

    int angle, len;
    PieType type;
    QPen pen;
    QBrush brush;

};

/******************************************************************/
/* class ConfPieDia                                               */
/******************************************************************/

class ConfPieDia : public QWidget
{
    Q_OBJECT

public:
    ConfPieDia( QWidget* parent, const char* );
    ~ConfPieDia();

    void setAngle( int _angle );
    void setLength( int _len );
    void setType( PieType _type );
    void setPenBrush( const QPen &_pen, const QBrush &_brush );

    int getAngle() const
    { return angle; }
    int getLength() const
    { return len; }
    PieType getType() const
    { return type; }

protected:
    QLabel *lType, *lAngle, *lLen;
    KIntNumInput *eAngle, *eLen;
    QGroupBox *gSettings;
    PiePreview *piePreview;
    QComboBox *cType;

    PieType type;
    PieType oldType;

    int angle, len;
    int oldAngle, oldLen;

protected slots:
    void lengthChanged( int );
    void angleChanged( int );
    void typeChanged( int );
    void Apply() { emit confPieDiaOk(); }
    void slotReset();

signals:
    void confPieDiaOk();

};

#endif
