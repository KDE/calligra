/*
 * $Id$
 *
 * Copyright 1999 by Kalle Dalheimer, released under Artistic License.
 */

#ifndef __KCHARTCOLORCONFIGPAGE_H__
#define __KCHARTCOLORCONFIGPAGE_H__

#include <qwidget.h>
#include <kcolorbtn.h>
#include "kdchart/KDChartParams.h"

// PENDING(kalle) Make this dynamic.
#define NUMDATACOLORS 6

class KChartColorConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartColorConfigPage( QWidget* parent );

    void setBackgroundColor( QColor color );
    QColor backgroundColor() const;
    void setGridColor( QColor color );
    QColor gridColor() const;
    void setLineColor( QColor color );
    QColor lineColor() const;
    void setPlotColor( QColor color );
    QColor plotColor() const;
    void setVolColor( QColor color );
    QColor volColor() const;
    void setHdFtColor( uint pos, QColor color );
    QColor hdFtColor( uint pos ) const;
    void setXTitleColor( QColor color );
    QColor xTitleColor() const;
    void setYTitleColor( QColor color );
    QColor yTitleColor() const;
    void setYTitle2Color( QColor color );
    QColor yTitle2Color() const;
    void setXLabelColor( QColor color );
    QColor xLabelColor() const;
    void setYLabelColor( QColor color );
    QColor yLabelColor() const;
    void setYLabel2Color( QColor color );
    QColor yLabel2Color() const;
    /*void setEdgeColor( QColor color );
    QColor edgeColor() const;*/
/*     void setDataColor( uint dataset, QColor color ); */
/*     QColor dataColor( uint dataset ) const; */

private:
    KColorButton* _backgroundCB;
    KColorButton* _gridCB;
    KColorButton* _lineCB;
    KColorButton* _plotCB;
    KColorButton* _volCB;
    KColorButton* _headerCB;
    KColorButton* _header2CB;
    KColorButton* _footerCB;
    KColorButton* _xtitleCB;
    KColorButton* _ytitleCB;
    KColorButton* _ytitle2CB;
    KColorButton* _xlabelCB;
    KColorButton* _ylabelCB;
    KColorButton* _ylabel2CB;
    //KColorButton* _edgeCB;
//    KColorButton* _dataCB[NUMDATACOLORS];
};


inline void KChartColorConfigPage::setBackgroundColor( QColor color )
{
    _backgroundCB->setColor( color );
}


inline QColor KChartColorConfigPage::backgroundColor() const
{
    return _backgroundCB->color();
}


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


inline void KChartColorConfigPage::setPlotColor( QColor color )
{
    _plotCB->setColor( color );
}


inline QColor KChartColorConfigPage::plotColor() const
{
    return _plotCB->color();
}


inline void KChartColorConfigPage::setVolColor( QColor color )
{
    _volCB->setColor( color );
}


inline QColor KChartColorConfigPage::volColor() const
{
    return _volCB->color();
}


inline void KChartColorConfigPage::setHdFtColor( uint pos, QColor color )
{
    // NOTE: This will be replaced by sophisticated handling of KDChart headers/footers
    //                                                                 (khz, 9.12.2001)

    switch( pos ) {
        case KDChartParams::HdFtPosHeader:  _headerCB->setColor( color );
                break;
        case KDChartParams::HdFtPosHeader2: _header2CB->setColor( color );
                break;
        case KDChartParams::HdFtPosFooter:  _footerCB->setColor( color );
                break;
    }
}


inline QColor KChartColorConfigPage::hdFtColor( uint pos ) const
{
    // NOTE: This will be replaced by sophisticated handling of KDChart headers/footers
    //                                                                 (khz, 9.12.2001)

    switch( pos ) {
        case KDChartParams::HdFtPosHeader:  return _headerCB->color();
                break;
        case KDChartParams::HdFtPosHeader2: return _header2CB->color();
                break;
        case KDChartParams::HdFtPosFooter:  return _footerCB->color();
                break;
    }
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


inline void KChartColorConfigPage::setYTitle2Color( QColor color )
{
    _ytitle2CB->setColor( color );
}


inline QColor KChartColorConfigPage::yTitle2Color() const
{
    return _ytitle2CB->color();
}


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


inline void KChartColorConfigPage::setYLabel2Color( QColor color )
{
    _ylabel2CB->setColor( color );
}


inline QColor KChartColorConfigPage::yLabel2Color() const
{
    return _ylabel2CB->color();
}


/*inline void KChartColorConfigPage::setEdgeColor( QColor color )
{
    _edgeCB->setColor( color );
}


inline QColor KChartColorConfigPage::edgeColor() const
{
    return _edgeCB->color();
}*/


#endif
