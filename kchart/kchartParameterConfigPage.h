/* This file is part of the KDE project
   Copyright (C) 2001,2002,2003,2004 Laurent Montel <montel@kde.org>

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

#ifndef __KCHARTPARAMETERCONFIGPAGE_H__
#define __KCHARTPARAMETERCONFIGPAGE_H__

#include <qwidget.h>
#include <kcolorbutton.h>

class QSpinBox;
class QCheckBox;
class QLineEdit;

class KChartParams;

class KChartParameterConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartParameterConfigPage( KChartParams* params, QWidget* parent );
    void init();
    void apply();
public slots:
    void changeXaxisState( bool );
private:
    KChartParams* _params;
    QCheckBox *grid;
    QCheckBox *xaxis;
    QCheckBox *yaxis;
    QCheckBox *yaxis2;
    QCheckBox *xlabel;
    QCheckBox *lineMarker;
    QCheckBox *llabel;
    QLineEdit *xtitle;
    QLineEdit *ytitle;
    QLineEdit *ylabel_fmt;
    QLineEdit *ytitle2;
    QLineEdit *ylabel2_fmt;
    QLineEdit *annotation;
};

#endif
