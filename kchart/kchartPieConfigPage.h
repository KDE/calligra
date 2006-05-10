/* This file is part of the KDE libraries
    Copyright (C) 2000, 2001, 2002, 2003, 2004 Laurent Montel <montel@kde.org>

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



#ifndef __KCHARTPIECONFIGPAGE_H__
#define __KCHARTPIECONFIGPAGE_H__


#include <QWidget>
#include <q3listview.h>
//Added by qt3to4:
#include <Q3MemArray>

#include <koChart.h>


class QSpinBox;
class QLineEdit;
class QCheckBox;
class QPushButton;
class QFont;
class QRadioButton;


namespace KChart
{

class KChartParams;

class KChartPieConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartPieConfigPage( KChartParams* params, QWidget* parent, 
			 KDChartTableData* data);
    void init();
    void apply();
    void initList();
    
public slots:
    void changeValue(int);
    void slotselected(Q3ListViewItem *);
    
private:
    int col;
    KChartParams* _params;
    QSpinBox *dist;
    QSpinBox *column;
    Q3ListView *list;
    QSpinBox *explose;
    Q3MemArray<int> value;
    int pos;
};

}  //KChart namespace

#endif
