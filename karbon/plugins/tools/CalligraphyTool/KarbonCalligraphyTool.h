/* This file is part of the KDE project
 * Copyright (C) 2008 Fela Winkelmolen <fela.kde@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _KARBONCALLIGRAPHYTOOL_H_
#define _KARBONCALLIGRAPHYTOOL_H_

#include <KoTool.h>
#include <KoPathShape.h>

class KoPathShape;
class KoShapeGroup;
class KoPathPoint;
class KarbonCalligraphicShape;

class KarbonCalligraphyTool : public KoTool
{
    Q_OBJECT
public:
    explicit KarbonCalligraphyTool(KoCanvasBase *canvas);
    ~KarbonCalligraphyTool();

    void paint( QPainter &painter, const KoViewConverter &converter );
    
    void mousePressEvent( KoPointerEvent *event ) ;
    void mouseMoveEvent( KoPointerEvent *event );
    void mouseReleaseEvent( KoPointerEvent *event );
    
    
    void activate ( bool temporary=false );
    void deactivate();

private:
    void addPoint( const QPointF &point );

    //QPointF m_point0;
    //QPointF m_point1;
    QPointF m_lastPoint;
    KarbonCalligraphicShape *m_path;
    //QList<KoPathShape *> m_pieces;
    
    double m_strokeWidth;
    double m_angle;
    double m_mass;
    
    bool m_isDrawing;
    
    
    // dinamic parameters
    QPointF m_speed;
};

#endif // _KARBONCALLIGRAPHYTOOL_H_
