/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998                   */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* KPresenter is under GNU GPL                                    */
/******************************************************************/
/* Module: KPresenter Utilities                                   */
/******************************************************************/

#include "kpresenter_utils.h"

#include <qpainter.h>
#include <qpntarry.h>

#include <qsize.h>

/*========================== draw a figure =======================*/
void drawFigure( LineEnd figure, QPainter* painter, QPoint coord, QColor color, int _w, float angle )
{
    painter->setPen( Qt::NoPen );
    painter->setBrush( Qt::NoBrush );

    switch ( figure )
    {
    case L_SQUARE:
    {
        int _h = _w;
        if ( _h % 2 == 0 ) _h--;
        painter->save();
        painter->translate( coord.x(), coord.y() );
        painter->rotate( angle );
        painter->scale( 1, 1 );
        painter->fillRect( -3 - _w / 2, -3 - _h / 2, 6 + _w, 6 + _h, color );
        painter->restore();
    } break;
    case L_CIRCLE:
    {
        painter->save();
        painter->translate( coord.x(), coord.y() );
        painter->setBrush( color );
        painter->drawEllipse( -3 - _w / 2, -3 - _w / 2, 6 + _w, 6 + _w );
        painter->restore();
    } break;
    case L_ARROW:
    {
        QPoint p1( -5 - _w / 2, -3 - _w / 2 );
        QPoint p2( 5 + _w / 2, 0 );
        QPoint p3( -5 - _w / 2, 3 + _w / 2 );
        QPointArray pArray( 3 );
        pArray.setPoint( 0, p1 );
        pArray.setPoint( 1, p2 );
        pArray.setPoint( 2, p3 );
    
        painter->save();
        painter->translate( coord.x(), coord.y() );
        painter->rotate( angle );
        painter->scale( 1, 1 );
        painter->setBrush( color );
        painter->drawPolygon( pArray );
        painter->restore();
    } break;
    default: break;
    }
}

/*================== get bounding with of figure =================*/
QSize getBoundingSize( LineEnd figure, int _w )
{
    switch ( figure )
    {
    case L_SQUARE:
    {
        int _h = _w;
        if ( _h % 2 == 0 ) _h--;
        return QSize( 6 + _w + 4, 6 + _h + 4 );
    } break;
    case L_CIRCLE:
        return QSize( 6 + _w + 4, 6 + _w + 4 );
        break;
    case L_ARROW:
        return QSize( 10 + _w + 4, 6 + _w + 4 );
        break;
    default: break;
    }

    return QSize( 0, 0 );
}
