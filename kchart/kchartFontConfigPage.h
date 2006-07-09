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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __KCHARTFONTCONFIGPAGE_H__
#define __KCHARTFONTCONFIGPAGE_H__

#include <qwidget.h>
#include <qbutton.h>

#include "kchartcolorarray.h"

#include "koChart.h"

class QLineEdit;
class QListBox;
class QPushButton;

namespace KChart
{

class KChartParams;

class KChartFontConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartFontConfigPage( KChartParams* params,QWidget* parent, 
			  KDChartTableData *dat);
    void init();
    void apply();
    void initList();

public slots:
    void changeLabelFont();

private:
    KChartParams  *m_params;

    // Widgets
    QLineEdit     *m_font;
    QListBox      *m_list;
    QPushButton   *m_fontButton;

    // Fonts for different things.
    QFont xTitle;
    QFont yTitle;
    QFont yAxis;
    QFont xAxis;
    QFont label;

    // Old stuff.  Remove?
    QButton::ToggleState xTitleIsRelative;
    QButton::ToggleState yTitleIsRelative;
    QButton::ToggleState labelIsRelative;
    QButton::ToggleState yAxisIsRelative;
    QButton::ToggleState xAxisIsRelative;
    QButton::ToggleState legendIsRelative;
    KDChartTableData *data;
};

}  //KChart namespace

#endif

