// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Base code from Kontour.
   Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)


   Copyright (C) 2001 Toshitaka Fujioka <fujioka@kde.org>

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

#ifndef confpolygondia_h
#define confpolygondia_h

#include <qwidget.h>

class QLabel;
class KIntNumInput;
class QGroupBox;
class QPushButton;
class QRadioButton;
class PolygonPreview;

/******************************************************************/
/* class ConfPolygonDia                                           */
/******************************************************************/

class ConfPolygonDia : public QWidget
{
    Q_OBJECT

public:
    ConfPolygonDia( QWidget *parent, const char *name );
    ~ConfPolygonDia();

    bool getCheckConcavePolygon() const { return checkConcavePolygon; }
    int getCornersValue() const { return cornersValue; }
    int getSharpnessValue() const { return sharpnessValue; }
    int getPolygonConfigChange() const;

    void setCheckConcavePolygon(bool _concavePolygon);
    void setCornersValue(int _cornersValue);
    void setSharpnessValue(int _sharpnessValue);
    void setPenBrush( const QPen &_pen, const QBrush &_brush );
    void resetConfigChangedValues();

protected:
    QRadioButton *m_convexPolygon, *m_concavePolygon;
    KIntNumInput *m_corners, *m_sharpness;
    QGroupBox *gSettings;
    PolygonPreview *polygonPreview;

    int cornersValue, oldCornersValue;
    int sharpnessValue, oldSharpnessValue;
    bool checkConcavePolygon, oldCheckConcavePolygon;
    bool m_bCheckConcaveChanged, m_bCornersChanged, m_bSharpnessChanged;

protected slots:
    void slotConvexPolygon();
    void slotConcavePolygon();
    void slotCornersValue( int value );
    void slotSharpnessValue( int value );
    void Apply() { emit confPolygonDiaOk(); }
    void slotReset();

signals:
    void confPolygonDiaOk();
};

#endif
