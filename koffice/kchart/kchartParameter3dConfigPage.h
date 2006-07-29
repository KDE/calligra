/* This file is part of the KDE project
   Copyright (C) 2000,2002,2003,2004 Laurent Montel <montel@kde.org>

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


#ifndef __KCHARTPARAMETER3DCONFIGPAGE_H__
#define __KCHARTPARAMETER3DCONFIGPAGE_H__

#include <qwidget.h>
#include <knuminput.h>

class QCheckBox;

namespace KChart
{

class KChartParams;

class KChartParameter3dConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartParameter3dConfigPage( KChartParams* params,
                                 QWidget* parent );
    void init();
    void apply();

protected slots:
    void slotChange3DParameter(bool);

private:
    KChartParams     *m_params;

    // Widgets
    KIntNumInput     *angle3d;
    KDoubleNumInput  *depth;
    QCheckBox        *bar3d;
    QCheckBox        *drawShadowColor;
};

}  //KChart namespace

#endif
