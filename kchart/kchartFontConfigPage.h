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

#include <QWidget>
#include <q3button.h>
#include <QCheckBox>
#include "kchartcolorarray.h"

#include "koChart.h"

class QLineEdit;
class Q3ListBox;
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
    Q3ListBox      *m_list;
    QPushButton   *m_fontButton;

    // Fonts for different things.
    QFont xTitle;
    QFont yTitle;
    QFont yAxis;
    QFont xAxis;
    QFont label;

    // Old stuff.  Remove?
	Qt::CheckState xTitleIsRelative;
	Qt::CheckState yTitleIsRelative;
	Qt::CheckState labelIsRelative;
	Qt::CheckState yAxisIsRelative;
	Qt::CheckState xAxisIsRelative;
	Qt::CheckState legendIsRelative;
    KDChartTableData *data;
};

}  //KChart namespace

#endif

