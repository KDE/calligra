#ifndef KIVIO_SHAPE_PAINTER_H
#define KIVIO_SHAPE_PAINTER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "kivio_fill_style.h"

class KivioPainter;
class KivioShape;

class KivioShapePainter
{
protected:
    KivioFillStyle m_fillStyle;
    KivioPainter *m_pPainter;
    KivioShape *m_pShape;
    QColor m_fgColor;
    float m_lineWidth;

    float m_x, m_y, m_w, m_h;

    void drawArc();
    void drawBezier();
    void drawOpenPath();
    void drawClosedPath();
    void drawPie();
    void drawEllipse();
    void drawLineArray();
    void drawRectangle();
    void drawRoundRectangle();
    void drawPolygon();
    void drawPolyline();
    void drawTextBox();

public:
    KivioShapePainter( KivioPainter * );
    virtual ~KivioShapePainter();


    void setFGColor( const QColor &c ) { m_fgColor = c; }
    void setBGColor( const QColor &c ) { m_fillStyle.setColor(c); }
    void setLineWidth( const float &f ) { m_lineWidth = f; }

    void drawShape( KivioShape *, float, float, float, float );
    void drawShapeOutline( KivioShape *, float, float, float, float );

};

#endif

