// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

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

#include "global.h"

#include <qcombobox.h>
#include <knuminput.h>

class QBrush;
class QLabel;
class QGroupBox;
class QPushButton;
class QPen;
class QPainter;
class QComboBox;
class PiePreview;

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
    void resetConfigChangedValues();

    int getAngle() const { return angle; }
    int getLength() const { return len; }
    PieType getType() const { return type; }
    int getPieConfigChange() const;

protected:
    bool m_bTypeChanged, m_bAngleChanged, m_bLengthChanged;
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
