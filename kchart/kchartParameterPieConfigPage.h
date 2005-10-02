/* This file is part of the KDE project
   Copyright (C) 2000,2001,2002,2003,2004 Laurent Montel <montel@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#ifndef __KCHARTPARAMETERPIECONFIGPAGE_H__
#define __KCHARTPARAMETERPIECONFIGPAGE_H__

#include <qwidget.h>


class QCheckBox;
class QRadioButton;
class QLineEdit;
class QSpinBox;
class QCheckBox;


namespace KChart
{

class KChartParams;


class KChartParameterPieConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartParameterPieConfigPage( KChartParams* params,QWidget* parent );
    void init();
    void apply();

public slots:
    void active3DPie(bool b);

private:
    KChartParams* _params;
    QCheckBox *pie3d;
    QCheckBox *drawShadowColor;
    QSpinBox  *depth;
    QSpinBox  *angle;
    QSpinBox  *explode;

};

}  //KChart namespace

#endif
