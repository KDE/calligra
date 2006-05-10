/* This file is part of the KDE project
   Copyright (C) 2001 Matthias Kalle Dalheimer <kalle@kde.org>

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

#ifndef __KCHARTLINE3DCONFIGPAGE_H__
#define __KCHARTLINE3DCONFIGPAGE_H__

#include <QWidget>
#include <knuminput.h>

class QCheckBox;

namespace KChart
{

class KChartParams;

class KChartLine3dConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartLine3dConfigPage( KChartParams* params,
                                 QWidget* parent );
    void init();
    void apply();
protected slots:
    void slotChange3DParameter(bool);
private:
    KChartParams* _params;
    KIntNumInput* angle3dX;
    KIntNumInput* angle3dY;
    KIntNumInput* lineWidth;
    KDoubleNumInput* depth;
    QCheckBox* line3d, *drawShadowColor;
    QCheckBox  *lineMarkers;
};


}  //KChart namespace

#endif
