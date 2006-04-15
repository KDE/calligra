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


#ifndef __KCHARTLEGENDCONFIGPAGE_H__
#define __KCHARTLEGENDCONFIGPAGE_H__

#include <qwidget.h>
#include <q3button.h>
#include <qlayout.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3ButtonGroup>

class QRadioButton;
class QLineEdit;
class KColorButton;
class QPushButton;

namespace KChart
{

class KChartParams;

class KChartLegendConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartLegendConfigPage( KChartParams* params,
                                 QWidget* parent );
    void init();
    void apply();
public slots:
    void changeTitleLegendFont();
    void changeTextLegendFont();

private:
    KChartParams* _params;
    QPushButton *lTopLeft,    *lTop,     *lTopRight,
                *lLeft,       *noLegend, *lRight,
                *lBottomLeft, *lBottom,  *lBottomRight;
    QLineEdit *title;
    KColorButton *legendTitleColor, *legendTextColor;
    QFont titleLegend, textLegend;
    QPushButton *titleLegendFontButton, *textLegendFontButton;
	Qt::CheckState titleLegendIsRelative, textLegendIsRelative;

    QPushButton* addButton( Q3GridLayout* layout, Q3ButtonGroup* gb,
                            const QString &toolTipText, const QString &icon,
                            int posY, int posX );
};

}  //KChart namespace

#endif
