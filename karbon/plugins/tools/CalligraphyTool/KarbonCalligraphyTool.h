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

#ifndef KARBONCALLIGRAPHYTOOL_H
#define KARBONCALLIGRAPHYTOOL_H

#include <KoTool.h>
#include <KoPathShape.h>

class KoPathShape;
class KoShapeGroup;
class KoPathPoint;
class KarbonCalligraphicShape;
class KarbonCalligraphyOptionWidget;

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

signals:
    void pathSelectedChanged(bool selection);

private slots:
    void setUsePath( bool usePath );
    void setUsePressure( bool usePressure );
    void setUseAngle(bool useAngle );
    void setStrokeWidth( double width );
    void setThinning( double thinning );
    void setAngle( int angle ); // set theangle in degrees
    void setFixation( double fixation );
    void setCaps( double caps );
    void setMass( double mass );   // set the mass in user friendly format
    void setDrag( double drag );

    void updateSelectedPath();

private:
    void addPoint( KoPointerEvent *event );
    // auxiliary functions to calculate the dynamic parameters
    // returns the new point and sets speed to the speed
    QPointF calculateNewPoint(const QPointF &mousePos, QPointF *speed);
    double calculateWidth( double pressure );
    double calculateAngle( const QPointF &oldSpeed, const QPointF &newSpeed);

    KarbonCalligraphyOptionWidget *m_optionWidget;

    QPointF m_lastPoint;
    KarbonCalligraphicShape *m_shape;

    bool m_usePath;         // follow selected path
    bool m_usePressure;     // use tablet pressure
    bool m_useAngle;        // use tablet angle
    double m_strokeWidth;
    double m_lastWidth;
    double m_angle; // angle in radians
    double m_fixation;
    double m_thinning;
    double m_caps;
    double m_mass;  // in raw format (not user friendly)
    double m_drag;  // from 0.0 to 1.0

    KoPathShape *m_selectedPath;
    QPainterPath m_selectedPathOutline;
    double m_followPathPosition;
    bool m_endOfPath;
    QPointF m_lastMousePos;

    bool m_isDrawing;
    int m_pointCount;

    // dynamic parameters
    QPointF m_speed; // used as a vector

    // last calligraphic shape drawn, if any
    KarbonCalligraphicShape *m_lastShape;
};

#endif // KARBONCALLIGRAPHYTOOL_H
