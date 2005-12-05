#ifndef __textpreview_h__
#define __textpreview_h__

#include <qframe.h>
#include "global.h"

class KPrTextPreview : public QFrame
{
    Q_OBJECT

public:
    KPrTextPreview( QWidget* parent, const char* name = 0 );
    ~KPrTextPreview() {}

    void setShadowDirection( ShadowDirection sd ) { shadowDirection = sd; repaint( true ); }
    void setShadowDistance( int sd ) { shadowDistance = sd; repaint( true ); }
    void setShadowColor( const QColor &sc ) { shadowColor = sc; repaint( true ); }
    void setAngle( double a ) { angle = a; repaint( true ); }

protected:
    void drawContents( QPainter* );

    ShadowDirection shadowDirection;
    int shadowDistance;
    QColor shadowColor;
    double angle;
};

#endif

