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

#ifndef __KCHARTCONFIGCOLORPAGE_H__
#define __KCHARTCONFIGCOLORPAGE_H__


#include <QWidget>
#include <kcolorbutton.h>

#include "koChart.h"
#include "kchartcolorarray.h"

class KListBox;


namespace KChart
{

class KChartParams;

// PENDING(kalle) Make this dynamic.
#define NUMDATACOLORS 6

class KCConfigColorPage : public QWidget
{
    Q_OBJECT

public:
    KCConfigColorPage( KChartParams* params, QWidget* parent, KDChartTableData *dat );
    void apply();

    void setBackgroundColor( QColor color );
    QColor backgroundColor() const;
    void setGridColor( QColor color );
    QColor gridColor() const;
    void setLineColor( QColor color );
    QColor lineColor() const;
    void setTitleColor( QColor color );
    QColor titleColor() const;
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
    void setYLabelColor( QColor color );
    QColor yLabelColor() const;
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
    //KColorButton* _edgeCB;
//    KColorButton* _dataCB[NUMDATACOLORS];
    KListBox* _dataColorLB;
    KColorButton* _dataColorCB;
    KChartColorArray extColor;
    uint index;
};


inline void KCConfigColorPage::setGridColor( QColor color )
{
    _gridCB->setColor( color );
}


inline QColor KCConfigColorPage::gridColor() const
{
    return _gridCB->color();
}


inline void KCConfigColorPage::setLineColor( QColor color )
{
    _lineCB->setColor( color );
}


inline QColor KCConfigColorPage::lineColor() const
{
    return _lineCB->color();
}


inline void KCConfigColorPage::setXTitleColor( QColor color )
{
    _xtitleCB->setColor( color );
}


inline QColor KCConfigColorPage::xTitleColor() const
{
    return _xtitleCB->color();
}


inline void KCConfigColorPage::setYTitleColor( QColor color )
{
    _ytitleCB->setColor( color );
}


inline QColor KCConfigColorPage::yTitleColor() const
{
    return _ytitleCB->color();
}


#if 0
inline void KCConfigColorPage::setYTitle2Color( QColor color )
{
    _ytitle2CB->setColor( color );
}


inline QColor KCConfigColorPage::yTitle2Color() const
{
    return _ytitle2CB->color();
}
#endif

inline void KCConfigColorPage::setXLabelColor( QColor color )
{
    _xlabelCB->setColor( color );
}


inline QColor KCConfigColorPage::xLabelColor() const
{
    return _xlabelCB->color();
}


inline void KCConfigColorPage::setYLabelColor( QColor color )
{
    _ylabelCB->setColor( color );
}


inline QColor KCConfigColorPage::yLabelColor() const
{
    return _ylabelCB->color();
}


#if 0
inline void KCConfigColorPage::setYLabel2Color( QColor color )
{
    _ylabel2CB->setColor( color );
}


inline QColor KCConfigColorPage::yLabel2Color() const
{
    return _ylabel2CB->color();
}
#endif

/*inline void KCConfigColorPage::setEdgeColor( QColor color )
{
    _edgeCB->setColor( color );
}


inline QColor KCConfigColorPage::edgeColor() const
{
    return _edgeCB->color();
}*/

}  //KChart namespace

#endif
