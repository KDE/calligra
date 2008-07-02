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

    QWidget *createOptionWidget();
    void activate ( bool temporary=false );
    void deactivate();

private slots:
    void setStrokeWidth( double width );
    void setThinning( double thinning );
    void setAngle( int angle ); // set theangle in degrees
    void setFixation( double fixation );
    void setMass( double mass );   // set the mass in user friendly format
    void setDrag( double drag );

private:
    void addPoint( KoPointerEvent *event );
    // auxiliary functions to calculate the dynamic parameters
    double calculateWidth( double pressure );
    double calculateAngle();

    QPointF m_lastPoint;
    KarbonCalligraphicShape *m_shape;

    double m_strokeWidth;
    double m_angle; // angle in radians
    double m_fixation;
    double m_thinning;
    double m_mass; // in raw format (not user friendly)
    double m_drag; // from 0.0 to 1.0

    bool m_isDrawing;

    // dynamic parameters
    QPointF m_speed; // used as a vector
};

#endif // _KARBONCALLIGRAPHYTOOL_H_
