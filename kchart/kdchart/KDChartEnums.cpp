#include "KDChartEnums.h"

QString KDChartEnums::positionFlagToString( PositionFlag type )
{
    switch( type ) {
    case PosTopLeft:
        return "TopLeft";
    case PosTopCenter:
        return "TopCenter";
    case PosTopRight:
        return "TopRight";
    case PosCenterLeft:
        return "CenterLeft";
    case PosCenter:
        return "Center";
    case PosCenterRight:
        return "CenterRight";
    case PosBottomLeft:
        return "BottomLeft";
    case PosBottomCenter:
        return "BottomCenter";
    case PosBottomRight:
        return "BottomRight";
    default: // should not happen
        qDebug( "Unknown content position" );
        return "TopLeft";
    }
}

KDChartEnums::PositionFlag KDChartEnums::stringToPositionFlag( const QString& string )
{
    if( string ==      "TopLeft" )
        return PosTopLeft;
    else if( string == "TopCenter" )
        return PosTopCenter;
    else if( string == "TopRight" )
        return PosTopRight;
    else if( string == "CenterLeft" )
        return PosCenterLeft;
    else if( string == "Center" )
        return PosCenter;
    else if( string == "CenterRight" )
        return PosCenterRight;
    else if( string == "BottomLeft" )
        return PosBottomLeft;
    else if( string == "BottomCenter" )
        return PosBottomCenter;
    else if( string == "BottomRight" )
        return PosBottomRight;
    else // default, should not happen
        return PosTopLeft;
}

QPoint KDChartEnums::positionFlagToPoint( const QRect& rect,
                                          PositionFlag pos )
{
    QPoint pt;
    if( rect.isValid() ) {
        switch( pos ) {
        case KDChartEnums::PosTopLeft:
            pt = rect.topLeft();
            break;
        case KDChartEnums::PosTopCenter:
            pt.setY( rect.top() );
            pt.setX( rect.center().x() );
            break;
        case KDChartEnums::PosTopRight:
            pt = rect.topRight();
            break;
        case KDChartEnums::PosCenterLeft:
            pt.setY( rect.center().y() );
            pt.setX( rect.left() );
            break;
        case KDChartEnums::PosCenter:
            pt = rect.center();
            break;
        case KDChartEnums::PosCenterRight:
            pt.setY( rect.center().y() );
            pt.setX( rect.right() );
            break;
        case KDChartEnums::PosBottomLeft:
            pt = rect.bottomLeft();
            break;
        case KDChartEnums::PosBottomCenter:
            pt.setY( rect.bottom() );
            pt.setX( rect.center().x() );
            break;
        case KDChartEnums::PosBottomRight:
            pt = rect.bottomRight();
            break;
        }
    }
    return pt;
}

#include "KDChartEnums.moc"
