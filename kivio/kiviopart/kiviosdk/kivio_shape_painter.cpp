/**
 * This class can be used to draw a KivioShape scaled to a given width and height
 * at a given point on a drawing surface.  Also planned is the ability to
 * transform this object, ie rotate/sheer.
 */
#include "kivio_painter.h"
#include "kivio_shape.h"
#include "kivio_shape_data.h"
#include "kivio_shape_painter.h"

#include <QColor>


KivioShapePainter::KivioShapePainter( KivioPainter *p )
{
    m_pPainter = p;
    m_fgColor = QColor(255,255,255);
    m_lineWidth = 1.0f;
    m_pShape = NULL;

    m_x = m_y = 0.0f;
    m_w = m_h = 72.0f;
}

KivioShapePainter::~KivioShapePainter()
{
}

void KivioShapePainter::drawShape( KivioShape *pShape, float x, float y, float w, float h )
{
    KivioShapeData *pShapeData;

    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;

    m_pShape = pShape;

    pShapeData = pShape->shapeData();

    switch( pShapeData->shapeType() )
    {
    case KivioShapeData::kstArc:
        drawArc();
        break;

    case KivioShapeData::kstPie:
        drawPie();
        break;

    case KivioShapeData::kstLineArray:
        drawLineArray();
        break;

    case KivioShapeData::kstPolyline:
        drawPolyline();
        break;

    case KivioShapeData::kstPolygon:
        drawPolygon();
        break;

    case KivioShapeData::kstBezier:
        drawBezier();
        break;

    case KivioShapeData::kstRectangle:
        drawRectangle();
        break;

    case KivioShapeData::kstRoundRectangle:
        drawRoundRectangle();
        break;

    case KivioShapeData::kstEllipse:
        drawEllipse();
        break;

    case KivioShapeData::kstOpenPath:
        drawOpenPath();
        break;

    case KivioShapeData::kstClosedPath:
        drawClosedPath();
        break;

    case KivioShapeData::kstTextBox:
        drawTextBox();
        break;


    case KivioShapeData::kstNone:
    default:
        break;
    }
}

void KivioShapePainter::drawArc()
{
}

void KivioShapePainter::drawBezier()
{
}

void KivioShapePainter::drawOpenPath()
{
}

void KivioShapePainter::drawClosedPath()
{
}

void KivioShapePainter::drawPie()
{
}

void KivioShapePainter::drawEllipse()
{
}

void KivioShapePainter::drawLineArray()
{
}

void KivioShapePainter::drawRectangle()
{
}

void KivioShapePainter::drawRoundRectangle()
{
}

void KivioShapePainter::drawPolygon()
{
}

void KivioShapePainter::drawPolyline()
{
}

void KivioShapePainter::drawTextBox()
{
}
