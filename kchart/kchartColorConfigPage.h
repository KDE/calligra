/* This file is part of the KDE project
   Copyright (C) 1999 Matthias Kalle Dalheimer <kalle@kde.org>

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

#ifndef __KCHARTCOLORCONFIGPAGE_H__
#define __KCHARTCOLORCONFIGPAGE_H__


#include <qwidget.h>
#include <kcolorbutton.h>

#include "koChart.h"
#include "kchartcolorarray.h"

class KListBox;


namespace KChart
{

class KChartParams;

// PENDING(kalle) Make this dynamic.
#define NUMDATACOLORS 6

class KChartColorConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartColorConfigPage( KChartParams* params, QWidget* parent, KDChartTableData *dat );
    void apply();

    void setBackgroundColor( QColor color );
    QColor backgroundColor() const;
    void setGridColor( QColor color );
    QColor gridColor() const;
    void setLineColor( QColor color );
    QColor lineColor() const;
    void setXTitleColor( QColor color );
    QColor xTitleColor() const;
    void setYTitleColor( QColor color );
    QColor yTitleColor() const;
#if 0
    void setYTitle2Color( QColor color );
    QColor yTitle2Color() const;
#endif
    void setXLabelColor( QColor color );
    QColor xLabelColor() const;
    void setXLineColor( QColor color );
    QColor xLineColor() const;
    void setXZeroLineColor( QColor color );
    QColor xZeroLineColor() const;

    void setYLabelColor( QColor color );
    QColor yLabelColor() const;
    void setYLineColor( QColor color );
    QColor yLineColor() const;
    void setYZeroLineColor( QColor color );
    QColor yZeroLineColor() const;
#if 0
    void setYLabel2Color( QColor color );
    QColor yLabel2Color() const;
#endif
    /*void setEdgeColor( QColor color );
    QColor edgeColor() const;*/
/*     void setDataColor( uint dataset, QColor color ); */
/*     QColor dataColor( uint dataset ) const; */

private slots:
    void activeColorButton();
    void changeIndex( int );

private:
    void initDataColorList();

    KChartParams      *m_params;
    KDChartTableData  *m_data;

    KColorButton* _lineCB;
    KColorButton* _gridCB;
    KColorButton* _xtitleCB;
    KColorButton* _ytitleCB;
#if 0
    KColorButton* _ytitle2CB;
#endif
    KColorButton* _xlabelCB;
    KColorButton* _ylabelCB;
#if 0
    KColorButton* _ylabel2CB;
#endif
    KColorButton* _xlineCB;
    KColorButton* _ylineCB;
#if 0
    KColorButton* _yline2CB;
#endif
    KColorButton* _xzerolineCB;
    KColorButton* _yzerolineCB;
#if 0
    KColorButton* _yzeroline2CB;
#endif

//  KColorButton* _edgeCB;
//  KColorButton* _dataCB[NUMDATACOLORS];
    KListBox* _dataColorLB;
    KColorButton* _dataColorCB;
    KChartColorArray extColor;
    uint index;
};


inline void KChartColorConfigPage::setGridColor( QColor color )
{
    _gridCB->setColor( color );
}


inline QColor KChartColorConfigPage::gridColor() const
{
    return _gridCB->color();
}


inline void KChartColorConfigPage::setLineColor( QColor color )
{
    _lineCB->setColor( color );
}


inline QColor KChartColorConfigPage::lineColor() const
{
    return _lineCB->color();
}


inline void KChartColorConfigPage::setXTitleColor( QColor color )
{
    _xtitleCB->setColor( color );
}


inline QColor KChartColorConfigPage::xTitleColor() const
{
    return _xtitleCB->color();
}


inline void KChartColorConfigPage::setYTitleColor( QColor color )
{
    _ytitleCB->setColor( color );
}


inline QColor KChartColorConfigPage::yTitleColor() const
{
    return _ytitleCB->color();
}


#if 0
inline void KChartColorConfigPage::setYTitle2Color( QColor color )
{
    _ytitle2CB->setColor( color );
}


inline QColor KChartColorConfigPage::yTitle2Color() const
{
    return _ytitle2CB->color();
}
#endif

inline void KChartColorConfigPage::setXLabelColor( QColor color )
{
    _xlabelCB->setColor( color );
}


inline QColor KChartColorConfigPage::xLabelColor() const
{
    return _xlabelCB->color();
}


inline void KChartColorConfigPage::setYLabelColor( QColor color )
{
    _ylabelCB->setColor( color );
}


inline QColor KChartColorConfigPage::yLabelColor() const
{
    return _ylabelCB->color();
}


#if 0
inline void KChartColorConfigPage::setYLabel2Color( QColor color )
{
    _ylabel2CB->setColor( color );
}
inline QColor KChartColorConfigPage::yLabel2Color() const
{
    return _ylabel2CB->color();
}
#endif


inline void KChartColorConfigPage::setXLineColor( QColor color )
{
    _xlineCB->setColor( color );
}
inline QColor KChartColorConfigPage::xLineColor() const
{
    return _xlineCB->color();
}

inline void KChartColorConfigPage::setYLineColor( QColor color )
{
    _ylineCB->setColor( color );
}
inline QColor KChartColorConfigPage::yLineColor() const
{
    return _ylineCB->color();
}

#if 0
inline void KChartColorConfigPage::setYLine2Color( QColor color )
{
    _yline2CB->setColor( color );
}
inline QColor KChartColorConfigPage::yLine2Color() const
{
    return _yline2CB->color();
}
#endif


inline void KChartColorConfigPage::setXZeroLineColor( QColor color )
{
    _xzerolineCB->setColor( color );
}
inline QColor KChartColorConfigPage::xZeroLineColor() const
{
    return _xzerolineCB->color();
}

inline void KChartColorConfigPage::setYZeroLineColor( QColor color )
{
    _yzerolineCB->setColor( color );
}
inline QColor KChartColorConfigPage::yZeroLineColor() const
{
    return _yzerolineCB->color();
}

#if 0
inline void KChartColorConfigPage::setYZeroLine2Color( QColor color )
{
    _yzeroline2CB->setColor( color );
}
inline QColor KChartColorConfigPage::yZeroLine2Color() const
{
    return _yzeroline2CB->color();
}
#endif


/*inline void KChartColorConfigPage::setEdgeColor( QColor color )
{
    _edgeCB->setColor( color );
}


inline QColor KChartColorConfigPage::edgeColor() const
{
    return _edgeCB->color();
}*/

}  //KChart namespace

#endif
